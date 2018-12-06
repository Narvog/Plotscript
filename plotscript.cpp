#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <csignal>
#include <cstdlib>

#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"
#include "consumer.hpp"


// *****************************************************************************
// install a signal handler for Cntl-C on Windows
// *****************************************************************************
#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>

// this function is called when a signal is sent to the process
BOOL WINAPI interrupt_handler(DWORD fdwCtrlType) {

	switch (fdwCtrlType) {
	case CTRL_C_EVENT: // handle Cnrtl-C
					   // if not reset since last call, exit
		interupt = true;
		return TRUE;

	default:
		return FALSE;
	}
}

// install the signal handler
inline void install_handler() { SetConsoleCtrlHandler(interrupt_handler, TRUE); }
// *****************************************************************************

// *****************************************************************************
// install a signal handler for Cntl-C on Unix/Posix
// *****************************************************************************
#elif defined(__APPLE__) || defined(__linux) || defined(__unix) ||             \
    defined(__posix)
#include <unistd.h>

// this function is called when a signal is sent to the process
void interrupt_handler(int signal_num) {

	if (signal_num == SIGINT) { // handle Cnrtl-C
		interupt = true;
	}
}

// install the signal handler
inline void install_handler() {

	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = interrupt_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);
}
#endif





void prompt(){
  std::cout << "\nplotscript> ";
}

std::string readline(){
  std::string line;
  std::getline(std::cin, line);

  return line;
}

void error(const std::string & err_str){
  std::cerr << "Error: " << err_str << std::endl;
}

void info(const std::string & err_str){
  std::cout << "Info: " << err_str << std::endl;
}

int eval_from_stream(std::istream & stream){

  Interpreter interp;
  std::ifstream ifs(STARTUP_FILE);
  interp.parseStream(ifs);
  Expression exp = interp.evaluate();
  
  if(!interp.parseStream(stream)){
    error("Invalid Program. Could not parse.");
    return EXIT_FAILURE;
  }
  else{
    try{
      Expression exp = interp.evaluate();
      std::cout << exp << std::endl;
    }
    catch(const SemanticError & ex){
      std::cerr << ex.what() << std::endl;
      return EXIT_FAILURE;
    }	
  }

  return EXIT_SUCCESS;
}

int eval_from_file(std::string filename){
      
  std::ifstream ifs(filename);
  
  if(!ifs){
    error("Could not open file for reading.");
    return EXIT_FAILURE;
  }
  
  return eval_from_stream(ifs);
}

int eval_from_command(std::string argexp){

  std::istringstream expression(argexp);

  return eval_from_stream(expression);
}

// A REPL is a repeated read-eval-print loop
void repl(){
	enum State {RUNNING, STOPPED};
	State currentS = RUNNING;
	ThreadSafeQueue<std::string>  input;
	ThreadSafeQueue<Expression>  output;
	Consumer cons(&input, &output);
	std::thread t1(&Consumer::run, cons);
	while (!std::cin.eof()) {

		prompt();
		std::string line = readline();

		if (line.empty()) continue;

		
		Expression exp;
		switch (currentS) {
		case RUNNING:
			if (line == "%stop")
			{
				currentS = STOPPED;
				input.push(line);
				
				output.wait_and_pop(exp);
				t1.join();
			}
			else if (line == "%exit")
			{
				currentS = STOPPED;
				input.push(line);

				output.wait_and_pop(exp);
				t1.join();
			}
			else if (line == "%reset")
			{
				currentS = RUNNING;
				input.push(line);

				output.wait_and_pop(exp);
				t1.join();
				Consumer cons(&input, &output);
				std::thread t2(&Consumer::run, cons);
				t1 = std::move(t2);
			}
			else if (line == "%start")
			{
				std::cerr << "Error: interpreter kernal already running" << std::endl;
			}
			else
			{
				input.push(line);
				output.wait_and_pop(exp);

				if (exp.isError())
				{
					std::cerr << exp.head().asSymbol() << std::endl;
				}
				else
				{
					std::cout << exp << std::endl;
				}
			}
			break;
		case STOPPED:
			if (line == "%start")
			{
				currentS = RUNNING;
				Consumer cons(&input, &output);
				std::thread t2(&Consumer::run, cons);
				t1 = std::move(t2);
			}
			else if (line == "%exit")
			{

			}
			else if (line == "%reset")
			{
				std::cerr << "Error: interpreter kernal is stopped no reason to reset" << std::endl;
			}
			else if (line == "%stop")
			{
				std::cerr << "Error: interpreter kernal already stopped" << std::endl;
			}
			else
			{
				std::cerr << "Error: interpreter kernal not running" << std::endl;
			}
			break;
		default:
			break;
		}
		if (line == "%exit")
		{
			return;
		}
	}
}

int main(int argc, char *argv[])
{  
	
  if(argc == 2){
    return eval_from_file(argv[1]);
  }
  else if(argc == 3){
    if(std::string(argv[1]) == "-e"){
      return eval_from_command(argv[2]);
    }
    else{
      error("Incorrect number of command line arguments.");
    }
  }
  else{
	  install_handler();
    repl();
  }
    
  return EXIT_SUCCESS;
}
