#include "catch.hpp"

#include "consumer.hpp"

TEST_CASE("test1", "[consumer]")
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

TEST_CASE("test2", "[consumer]")
{
	ThreadSafeQueue<std::string>  input;
	ThreadSafeQueue<Expression> output;
	Consumer cons(&input, &output);
	std::thread t1(&Consumer::run, cons);

	std::string line = "(+ 1 2)";
	Expression exp;

	input.push(line);

	output.wait_and_pop(exp);
	CHECK(exp == Expression(3.));


	line = "%stop";
	input.push(line);

	output.wait_and_pop(exp);
	CHECK(exp.head().asSymbol() == "");
	t1.join();
}

TEST_CASE("test3", "[consumer]")
{
	ThreadSafeQueue<std::string>  input;
	ThreadSafeQueue<Expression> output;
	Consumer cons(&input, &output);
	std::thread t1(&Consumer::run, cons);

	std::string line = "(+ a 2)";
	Expression exp;

	input.push(line);

	output.wait_and_pop(exp);
	std::string t = "!!!ERROR!!!";
	CHECK(exp == Expression(t));


	line = "%stop";
	input.push(line);

	output.wait_and_pop(exp);
	CHECK(exp.head().asSymbol() == "");
	t1.join();
}

TEST_CASE("test4", "[consumer]")
{
	ThreadSafeQueue<std::string>  input;
	ThreadSafeQueue<Expression> output;
	Consumer cons(&input, &output);
	std::thread t1(&Consumer::run, cons);

	std::string line = "(+ 1 2";
	Expression exp;

	input.push(line);

	output.wait_and_pop(exp);
	std::string t = "!!!ERROR!!!";
	CHECK(exp == Expression(t));


	line = "%stop";
	input.push(line);

	output.wait_and_pop(exp);
	CHECK(exp.head().asSymbol() == "");
	t1.join();
}