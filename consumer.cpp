#include "consumer.hpp"

Consumer::Consumer(ThreadSafeQueue<std::string>* inputQ)
{
	OperadQ = inputQ;

	std::ifstream ifs(STARTUP_FILE);
	interp.parseStream(ifs);
	Expression exp = interp.evaluate();
}

Consumer::~Consumer()
{

}

ThreadSafeQueue<Expression>* Consumer::getReturnQueue()
{
	return resultQ;
}

void Consumer::run()
{
	while (!Exit)
	{
		std::string input;
		OperadQ->wait_and_pop(input);
		if (input != "%stop" && input != "%reset" && input != "%exit")
		{
			std::istringstream expression(input);
			if (!interp.parseStream(expression)) {
				std::string t = "!!!ERROR!!!";
				std::cerr << "Invalid Expression. Could not parse." << std::endl;
				resultQ->push(Expression(t));
			}
			else
			{
				try
				{
					Expression exp = interp.evaluate();
					resultQ->push(exp);
				}
				catch (const SemanticError & ex) {
					std::string t = "!!!ERROR!!!";
					std::cerr << ex.what() << std::endl;
					resultQ->push(Expression(t));
				}
			}
		}
		else
		{
			Exit = true;
			resultQ->push(Expression());
		}
		
	}
}
