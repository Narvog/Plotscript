#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"
#include "consumer.hpp"



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
	ThreadSafeQueue<std::string> * input = new ThreadSafeQueue<std::string>;
	ThreadSafeQueue<Expression> * output;
	Consumer cons(input);
	output = cons.getReturnQueue();
	std::thread t1(&Consumer::run, cons);
	while (!std::cin.eof()) {

		prompt();
		std::string line = readline();

		if (line.empty()) continue;

		input->push(line);

		Expression exp;
		output->wait_and_pop(exp);
		if (exp.isHeadSymbol())
		{
			if (exp.head().asSymbol() == "!!!ERROR!!!")
			{
				
			}
			else
			{
				std::cout << exp << std::endl;
			}
		}
		else
		{
			std::cout << exp << std::endl;
		}
	}
  cons.Exit = true;
  t1.join();
  delete input;
  delete output;
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
    repl();
  }
    
  return EXIT_SUCCESS;
}
