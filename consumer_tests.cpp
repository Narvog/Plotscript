#include "catch.hpp"

#include "consumer.hpp"

TEST_CASE("test1")
{
	ThreadSafeQueue<std::string>  input;
	ThreadSafeQueue<Expression> output;
	Consumer cons(&input, &output);
	std::thread t1(&Consumer::run, cons);
	std::string line = "%stop";
	input.push(line);

	Expression exp;
	output.wait_and_pop(exp);
	CHECK(exp.head().asSymbol() == "");
	t1.join();
}