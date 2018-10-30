#include "catch.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "semantic_error.hpp"
#include "interpreter.hpp"
#include "expression.hpp"

Expression run(const std::string & program){
  
  std::istringstream iss(program);
    
  Interpreter interp;
    
  bool ok = interp.parseStream(iss);
  if(!ok){
    std::cerr << "Failed to parse: " << program << std::endl; 
  }
  REQUIRE(ok == true);

  Expression result;
  REQUIRE_NOTHROW(result = interp.evaluate());

  return result;
}

TEST_CASE( "Test Interpreter parser with expected input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r)))";

  std::istringstream iss(program);
 
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == true);
}

TEST_CASE( "Test Interpreter parser with numerical literals", "[interpreter]" ) {

  std::vector<std::string> programs = {"(1)", "(+1)", "(+1e+0)", "(1e-0)"};
  
  for(auto program : programs){
    std::istringstream iss(program);
 
    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == true);
  }

  {
    std::istringstream iss("(define x 1abc)");
    
    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with truncated input", "[interpreter]" ) {

  {
    std::string program = "(f";
    std::istringstream iss(program);
  
    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
  
  {
    std::string program = "(begin (define r 10) (* pi (* r r";
    std::istringstream iss(program);

    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with extra input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r))) )";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with single non-keyword", "[interpreter]" ) {

  std::string program = "hello";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty input", "[interpreter]" ) {

  std::string program;
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty expression", "[interpreter]" ) {

  std::string program = "( )";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with bad number string", "[interpreter]" ) {

  std::string program = "(1abc)";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with incorrect input. Regression Test", "[interpreter]" ) {

  std::string program = "(+ 1 2) (+ 3 4)";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter result with literal expressions", "[interpreter]" ) {
  
  { // Number
    std::string program = "(4)";
    Expression result = run(program);
    REQUIRE(result == Expression(4.));
  }

  { // Symbol
    std::string program = "(pi)";
    Expression result = run(program);
    REQUIRE(result == Expression(atan2(0, -1)));
  }

}

TEST_CASE( "Test Interpreter result with simple procedures (add)", "[interpreter]" ) {

  { // add, binary case
    std::string program = "(+ 1 2)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(3.));
  }
  
  { // add, 3-ary case
    std::string program = "(+ 1 2 3)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(6.));
  }

  { // add, 6-ary case
    std::string program = "(+ 1 2 3 4 5 6)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(21.));
  }
}
  
TEST_CASE( "Test Interpreter special forms: begin and define", "[interpreter]" ) {

  {
    std::string program = "(define answer 42)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }

  {
    std::string program = "(begin (define answer 42)\n(answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }
  
  {
    std::string program = "(begin (define answer (+ 9 11)) (answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(20.));
  }

  {
    std::string program = "(begin (define a 1) (define b 1) (+ a b))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(2.));
  }
}

TEST_CASE( "Test a medium-sized expression", "[interpreter]" ) {

  {
    std::string program = "(+ (+ 10 1) (+ 30 (+ 1 1)))";
    Expression result = run(program);
    REQUIRE(result == Expression(43.));
  }
}

TEST_CASE( "Test arithmetic procedures", "[interpreter]" ) {

  {
    std::vector<std::string> programs = {"(+ 1 -2)",
					 "(+ -3 1 1)",
					 "(- 1)",
					 "(- 1 2)",
					 "(* 1 -1)",
					 "(* 1 1 -1)",
					 "(/ -1 1)",
					 "(/ 1 -1)"};

    for(auto s : programs){
      Expression result = run(s);
      REQUIRE(result == Expression(-1.));
    }
  }
}


TEST_CASE( "Test some semantically invalid expresions", "[interpreter]" ) {
  
  std::vector<std::string> programs = {"(@ none)", // so such procedure
				       "(- 1 1 2)", // too many arguments
				       "(define begin 1)", // redefine special form
				       "(define pi 3.14)"}; // redefine builtin symbol
    for(auto s : programs){
      Interpreter interp;

      std::istringstream iss(s);
      
      bool ok = interp.parseStream(iss);
      REQUIRE(ok == true);
      
      REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
    }
}

TEST_CASE( "Test for exceptions from semantically incorrect input", "[interpreter]" ) {

  std::string input = R"(
(+ 1 a)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test malformed define", "[interpreter]" ) {

    std::string input = R"(
(define a 1 2)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test using number as procedure", "[interpreter]" ) {
    std::string input = R"(
(1 2 3)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 1", "[interpreter]") {
	std::string input = R"((begin
    (define a 1)
    (define b 2)
    (define c (* 2 b))
    (- c (+ (/ 1 a) b))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(1.));
}

TEST_CASE("Test Prersonal 2", "[interpreter]") {
	std::string input = R"((begin
(^ e 0)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(1.));
}

TEST_CASE("Test Prersonal 3", "[interpreter]") {
	std::string input = R"((begin
(- I (sqrt -1))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(std::complex<double>(0.,0.)));
}

TEST_CASE("Test Prersonal 4", "[interpreter]") {
	std::string input = R"((begin
(sqrt (^ 2 2))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(2.));
}

TEST_CASE("Test Prersonal 5", "[interpreter]") {
	std::string input = R"((begin
(ln (^ e 2))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(2.));
}

TEST_CASE("Test Prersonal 6", "[interpreter]") {
	std::string input = R"((begin
(cos 0)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(1.));
}

TEST_CASE("Test Prersonal 7", "[interpreter]") {
	std::string input = R"((begin
(sin 0)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(0.));
}

TEST_CASE("Test Prersonal 8", "[interpreter]") {
	std::string input = R"((begin
(tan 0)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(0.));
}

TEST_CASE("Test Prersonal 9", "[interpreter]") {
	std::string input = R"((begin
(- I)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(std::complex<double>(0., -1.)));
}

TEST_CASE("Test Prersonal 10", "[interpreter]") {
	std::string input = R"((begin
(- I I)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(std::complex<double>(0., 0.)));
}

TEST_CASE("Test Prersonal 11", "[interpreter]") {
	std::string input = R"((begin
(+ (+ I I) (+ 1 I) (+ I 1) (+ 1 1))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(std::complex<double>(4., 4.)));
}

TEST_CASE("Test Prersonal 12", "[interpreter]") {
	std::string input = R"((begin
(* (* I I) (* 1 I) (* I 1) (* 1 1))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(std::complex<double>(1., -0.)));
}

TEST_CASE("Test Prersonal 13", "[interpreter]") {
	std::string input = R"((begin
(* (/ I I) (/ 1 I) (/ I 1) (/ 1 1) (/ I))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(std::complex<double>(0., -1.)));
}

TEST_CASE("Test Prersonal 14", "[interpreter]") {
	std::string input = R"((begin
(imag I)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(1.));
}

TEST_CASE("Test Prersonal 15", "[interpreter]") {
	std::string input = R"((begin
(real I)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(0.));
}

TEST_CASE("Test Prersonal 16", "[interpreter]") {
	std::string input = R"((begin
(mag I)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(1.));
}

TEST_CASE("Test Prersonal 17", "[interpreter]") {
	std::string input = R"((begin
(arg (+ 0 (* 0 I)))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(0.));
}

TEST_CASE("Test Prersonal 18", "[interpreter]") {
	std::string input = R"((begin
(conj I)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(std::complex<double>(0., -1.)));
}

TEST_CASE("Test Prersonal 19", "[interpreter]") {
	std::string input = R"((begin
(sqrt (^ I 2))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(std::complex<double>(0., 1.)));
}

TEST_CASE("Test Prersonal 20", "[interpreter]") {
	std::string input = R"((begin
(^ (^ 2 I) (/ I))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(std::complex<double>(2., 0.)));
}

TEST_CASE("Test Prersonal 21", "[interpreter]") {
	std::string input = R"((begin
(define mylist (list 1 (+ 1 I) (list 5 4 3 2 1) (list)))
(mylist)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result.isLList());
}

TEST_CASE("Test Prersonal 22", "[interpreter]") {
	std::string input = R"((begin
(first (list 1 2 3))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(1.));
}

TEST_CASE("Test Prersonal 23", "[interpreter]") {
	std::string input = R"((begin
(first (1))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 24", "[interpreter]") {
	std::string input = R"((begin
(first (list))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 25", "[interpreter]") {
	std::string input = R"((begin
(first (list 1 2) (list 3 4))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 26", "[interpreter]") {
	std::string input = R"((begin
(rest (list 1 2 3))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result.isLList());
}

TEST_CASE("Test Prersonal 27", "[interpreter]") {
	std::string input = R"((begin
(rest (1))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 28", "[interpreter]") {
	std::string input = R"((begin
(rest (list))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 29", "[interpreter]") {
	std::string input = R"((begin
(rest (list 1 2) (list 3 4))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 30", "[interpreter]") {
	std::string input = R"((begin
(rest (list 1))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result.isLList());
}

TEST_CASE("Test Prersonal 31", "[interpreter]") {
	std::string input = R"((begin
(define a (list))
(define b (list 1 2 3 4))
plotscript> (length a)
plotscript> (length b)
plotscript> (length 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 32", "[interpreter]") {
	std::string input = R"((begin
(define x (list 0 1 2 3))
(define y (append x (+ 3 (* 4 I))))
(x)
(y)
(append 3 x)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 33", "[interpreter]") {
	std::string input = R"((begin
(define x (list 0 1 2 3))
(define y (list (+ 3 I) 100 110))
(define z (join x y))
(list (length x) (length y) (length z))
(join (list 1 2) 10)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 34", "[interpreter]") {
	std::string input = R"((begin
(range 0 5 1)
(range -3 3 1)
(range 0 1 0.11)
(range 3 -1 1)
(range 0 5 -1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 35", "[interpreter]") {
	std::string input = R"((begin
(begin
  (define a 1)
  (define x 100)
  (define f (lambda (x) (begin
                          (define b 12)
                          (+ a b x))))
  (f 2)
)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(15.));
}

TEST_CASE("Test Prersonal 36", "[interpreter]") {
	std::string input = R"((begin
(lambda (x) (* 2 x))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result.isLLambda());
}

TEST_CASE("Test Prersonal 37", "[interpreter]") {
	std::string input = R"((begin
(define f1 (lambda (x y) (/ x y)))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result.isLLambda());
}

TEST_CASE("Test Prersonal 38", "[interpreter]") {
	std::string input = R"((begin
(apply + (list 1 2 3 4))
(define complexAsList (lambda (x) (list (real x) (imag x))))
(apply complexAsList (list (+ 1 (* 3 I))))
(define linear (lambda (a b x) (+ (* a x) b)))
(apply linear (list 3 4 5))
(apply + 3)
(apply (+ z I) (list 0))
(apply / (list 1 2 4))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 39", "[interpreter]") {
	std::string input = R"((begin
(define f (lambda (x) (sin x)))
(map f (list (- pi) (/ (- pi) 2) 0 (/ pi 2) pi))
(map / (list 1 2 4)) ; compare to above - see Note below
(map + 3)
(map 3 (list 1 2 3))
(define addtwo (lambda (x y) (+ x y)))
(map addtwo (list 1 2 3))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 40", "[interpreter]") {
	std::string input = R"((begin
("foo")
("a string with spaces")
(define x ("foo"))
(x)

(define mylist (list "eggs" "bread" "milk" "bacon"))
(first mylist)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result.head().asString() == "\"eggs\"");
}

TEST_CASE("Test Prersonal 41", "[interpreter]") {
	std::string input = R"((begin
(set-property "number" "three" (3))
(set-property "number" (3) "three")
(set-property "number" (+ 1 2) "three")
(set-property "label" "foo" (lambda (x) (* 3 (+ 2 x))))
(set-property (+ 1 2) "number" "three")("a string with spaces")
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 42", "[interpreter]") {
	std::string input = R"((begin
(define a (+ 1 I))
(define c "testing")
(c)
(define b (set-property "note" "a complex number" a))
(get-property "note" b)
(get-property "foo" b)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	//REQUIRE(result.head().isNone());
}

TEST_CASE("Test Prersonal 43", "[interpreter]") {
	std::string input = R"(
(+ 1 "test")
)";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 44", "[interpreter]") {
	std::string input = R"(
(* 1 "test")
)";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 45", "[interpreter]") {
	std::string input = R"((begin
(- I 1)
(- 1 I)
(- 1 "text")
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);

}

TEST_CASE("Test Prersonal 46", "[interpreter]") {
	std::string input = R"((begin
(- "p")
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 47", "[interpreter]") {
	std::string input = R"((begin
(/ "p")
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 48", "[interpreter]") {
	std::string input = R"((begin
(/ 1 "p")
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 49", "[interpreter]") {
	std::string input = R"((begin
(/ 1 2 "p")
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 50", "[interpreter]") {
	std::string input = R"((begin
(sqrt 1 2 3)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 51", "[interpreter]") {
	std::string input = R"((begin
(sqrt "p")
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 52", "[interpreter]") {
	std::string input = R"((begin
(^ "p")
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 53", "[interpreter]") {
	std::string input = R"((begin
(^ 2 "p")
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 54", "[interpreter]") {
	std::string input = R"((begin
(ln 0)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 55", "[interpreter]") {
	std::string input = R"((begin
(ln -1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}
TEST_CASE("Test Prersonal 56", "[interpreter]") {
	std::string input = R"((begin
(ln (+ I 1))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 57", "[interpreter]") {
	std::string input = R"((begin
(ln 1 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 58", "[interpreter]") {
	std::string input = R"((begin
(sin "q")
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 59", "[interpreter]") {
	std::string input = R"((begin
(sin 1 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 60", "[interpreter]") {
	std::string input = R"((begin
(cos 1 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 61", "[interpreter]") {
	std::string input = R"((begin
(cos "a")
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 62", "[interpreter]") {
	std::string input = R"((begin
(tan 1 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 63", "[interpreter]") {
	std::string input = R"((begin
(tan "a")
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 64", "[interpreter]") {
	std::string input = R"((begin
(apply + (list 1 2 "a"))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 65", "[interpreter]") {
	std::string input = R"((begin
(map + (list 1 2 "a"))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 66", "[interpreter]") {
	std::string input = R"((begin
(real 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 67", "[interpreter]") {
	std::string input = R"((begin
(real 1 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 68", "[interpreter]") {
	std::string input = R"((begin
(imag 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 69", "[interpreter]") {
	std::string input = R"((begin
(real 1 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 70", "[interpreter]") {
	std::string input = R"((begin
(mag 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 71", "[interpreter]") {
	std::string input = R"((begin
(real 1 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 72", "[interpreter]") {
	std::string input = R"((begin
(arg 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 73", "[interpreter]") {
	std::string input = R"((begin
(arg 1 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 74", "[interpreter]") {
	std::string input = R"((begin
(conj 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 75", "[interpreter]") {
	std::string input = R"((begin
(conj 1 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 76", "[interpreter]") {
	std::string input = R"((begin
(mag 1 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 77", "[interpreter]") {
	std::string input = R"((begin
(length (list))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	Expression result = run(input);
	REQUIRE(result == Expression(0.));
}

TEST_CASE("Test Prersonal 78", "[interpreter]") {
	std::string input = R"((begin
(length 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 79", "[interpreter]") {
	std::string input = R"((begin
(length (list 1 2) (list 1 2))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 80", "[interpreter]") {
	std::string input = R"((begin
(append (list 1 2) (list 1 2) 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 81", "[interpreter]") {
	std::string input = R"((begin
(join (list 1 2) (list 1 2) 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 82", "[interpreter]") {
	std::string input = R"((begin
(range 0 0 0)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 83", "[interpreter]") {
	std::string input = R"((begin
(range "a" "b" "c")
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 84", "[interpreter]") {
	std::string input = R"((begin
(range "a" "b" "c" "d")
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 85", "[interpreter]") {
	std::string input = R"((begin
(define x (lambda (1) (+ x 1)))
(x 2)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 86", "[interpreter]") {
	std::string input = R"((begin
(define a (lambda (x) (+ x 1)))
(define a (lambda (x) (+ x 2)))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 87", "[interpreter]") {
	std::string input = R"((begin
(define 1 (lambda (x) (+ x 1)))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 88", "[interpreter]") {
	std::string input = R"((begin
(define + (lambda (x) (+ x 1)))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}


TEST_CASE("Test Prersonal 89", "[interpreter]") {
	std::string input = R"((begin
(define a (lambda (x) (+ x 1)))
(a 1 2)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 90", "[interpreter]") {
	std::string input = R"((begin
(apply 1 1 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 91", "[interpreter]") {
	std::string input = R"((begin
(set-property 123 1 45)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 92", "[interpreter]") {
	std::string input = R"((begin
(define a 1)
(define b (set-property "hello" 1 a))
(define b (set-property "hello" 2 a))
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 93", "[interpreter]") {
	std::string input = R"((begin
(set-property 123 1 45 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 94", "[interpreter]") {
	std::string input = R"((begin
(get-property 123 1 45 1)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Prersonal 95", "[interpreter]") {
	std::string input = R"((begin
(get-property 123 1 45)
))";

	Interpreter interp;

	std::istringstream iss(input);

	bool ok = interp.parseStream(iss);
	REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}