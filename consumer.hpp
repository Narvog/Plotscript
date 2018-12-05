#ifndef CONSUMER_HPP
#define CONSUMER_HPP
#include "threadsafequeue.hpp"
#include "interpreter.hpp"
#include "startup_config.hpp"
#include "semantic_error.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

class Consumer
{
public:
	Consumer();
	Consumer(ThreadSafeQueue<std::string> *inputQ, ThreadSafeQueue<Expression> *outputQ);
	~Consumer();


	void run();

	bool Exit = false;
private:
	ThreadSafeQueue<Expression> * resultQ;
	ThreadSafeQueue<std::string> * OperadQ;
	Interpreter interp;
};

#endif