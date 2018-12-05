#include "consumer.hpp"

Consumer::Consumer(ThreadSafeQueue<std::string> *inputQ, ThreadSafeQueue<Expression> *outputQ)
{
	OperadQ = inputQ;
	resultQ = outputQ;
	std::ifstream ifs(STARTUP_FILE);
	interp.parseStream(ifs);
	Expression exp = interp.evaluate();
}

Consumer::~Consumer()
{

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
				std::string t = "Invalid Expression.Could not parse.";
				Expression result(t);
				result.setError();
				//std::cerr << "Invalid Expression. Could not parse." << std::endl;
				resultQ->push(result);
			}
			else
			{
				try
				{
					Expression exp = interp.evaluate();
					resultQ->push(exp);
				}
				catch (const SemanticError & ex) {
					std::string t = ex.what();
					Expression result(t);
					result.setError();
					//std::cerr << ex.what() << std::endl;
					resultQ->push(result);
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
