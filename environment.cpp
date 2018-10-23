#include "environment.hpp"

#include <cassert>
#include <cmath>

#include "environment.hpp"
#include "semantic_error.hpp"

/*********************************************************************** 
Helper Functions
**********************************************************************/

// predicate, the number of args is nargs
bool nargs_equal(const std::vector<Expression> & args, unsigned nargs){
  return args.size() == nargs;
}

/*********************************************************************** 
Each of the functions below have the signature that corresponds to the
typedef'd Procedure function pointer.
**********************************************************************/

// the default procedure always returns an expresison of type None
Expression default_proc(const std::vector<Expression> & args){
  args.size(); // make compiler happy we used this parameter
  return Expression();
};

Expression add(const std::vector<Expression> & args){

  // check all aruments are numbers, or complex, while adding
  double result = 0.0;
  bool hasImag = false;
  complex<double> Ires(0.0, 0.0);
  for( auto & a :args){
    if(a.isHeadNumber()){
      Ires = Ires + a.head().asNumber();      
    }
	else if (a.isHeadComplex())
	{
		hasImag = true;
		Ires = Ires + a.head().asComplex();
	}
    else{
      throw SemanticError("Error in call to add, argument not a number or a complex number");
    }
  }
  if (!hasImag)
  {
	  result = Ires.real();
	  return Expression(result);
  }
  else
  {
	  return Expression(Ires);
  }
};


Expression mul(const std::vector<Expression> & args){
 
  // check all aruments are numbers, or complex, while multiplying
	double result = 1.0;
	bool hasImag = false;
	complex<double> Ires(1.0, 0.0);
  for( auto & a :args){
    if(a.isHeadNumber()){
      Ires = Ires * a.head().asNumber();      
    }
	else if (a.isHeadComplex()) {
		hasImag = true;
		Ires = Ires * a.head().asComplex();
	}
    else{
      throw SemanticError("Error in call to mul, argument not a number or a complex");
    }
  }
  if (!hasImag)
  {
	  result = Ires.real();
	  return Expression(result);
  }
  else
  {
	  return Expression(Ires);
  }
  
};

Expression subneg(const std::vector<Expression> & args){
	//returns a number, or complex, * -1 if one argument is given
	//else return argument[0] - argument[1] for any combinmation of numbers or complex numbers
  double result = 0.0;
  bool hasImag = false;
  complex<double> Ires(0.0,0.0);

  // preconditions
  if(nargs_equal(args,1)){
    if(args[0].isHeadNumber()){
      Ires = -args[0].head().asNumber();
    }
	else if (args[0].isHeadComplex()) {
		hasImag = true;
		Ires = -args[0].head().asComplex();
	}
    else{
      throw SemanticError("Error in call to negate: invalid argument.");
    }
  }
  //performs subtraction for any combination of numbers and complex numbers
  else if(nargs_equal(args,2)){
    if( (args[0].isHeadNumber()) && (args[1].isHeadNumber()) ){
      Ires = args[0].head().asNumber() - args[1].head().asNumber();
    }
	else if ((args[0].isHeadComplex()) && (args[1].isHeadNumber()))
	{
		hasImag = true;
		Ires = args[0].head().asComplex() - args[1].head().asNumber();
	}
	else if ((args[0].isHeadNumber()) && (args[1].isHeadComplex()))
	{
		hasImag = true;
		Ires = args[0].head().asNumber() - args[1].head().asComplex();
	}
	else if ((args[0].isHeadComplex()) && (args[1].isHeadComplex()))
	{
		hasImag = true;
		Ires = args[0].head().asComplex() - args[1].head().asComplex();
	}
    else{      
      throw SemanticError("Error in call to subtraction: invalid argument.");
    }
  }
  else{
    throw SemanticError("Error in call to subtraction or negation: invalid number of arguments.");
  }

  if (!hasImag)
  {
	  result = Ires.real();
	  return Expression(result);
  }
  else
  {
	  return Expression(Ires);
  }
};

Expression div(const std::vector<Expression> & args){
	//performs division for any combnation of complex numbers and numbers
	//at least two numbers/complex are requred or an error is thown
	double result = 0.0;
	bool hasImag = false;
	complex<double> Ires(0.0, 0.0);


  if(nargs_equal(args,2)){
    if( (args[0].isHeadNumber()) && (args[1].isHeadNumber()) ){
      result = args[0].head().asNumber() / args[1].head().asNumber();
    }
	else if ((args[0].isHeadComplex()) && (args[1].isHeadNumber()))
	{
		hasImag = true;
		Ires = args[0].head().asComplex() / args[1].head().asNumber();
	}
	else if ((args[0].isHeadNumber()) && (args[1].isHeadComplex()))
	{
		hasImag = true;
		Ires = args[0].head().asNumber() / args[1].head().asComplex();
	}
	else if ((args[0].isHeadComplex()) && (args[1].isHeadComplex()))
	{
		hasImag = true;
		Ires = args[0].head().asComplex() / args[1].head().asComplex();
	}
    else{      
      throw SemanticError("Error in call to division: invalid argument.");
    }
  }
  else if (nargs_equal(args, 1))
  {
	  if (args[0].isHeadNumber())
	  {
		  result = 1.0 / args[0].head().asNumber();
	  }
	  else if (args[0].isHeadComplex())
	  {
		  hasImag = true;
		  Ires = 1.0 / args[0].head().asComplex();
	  }
	  else
	  {
		  throw SemanticError("Error in call to division: invalid argument.");
	  }
  }
  else{
    throw SemanticError("Error in call to division: invalid number of arguments.");
  }
  if (!hasImag)
  {
	  return Expression(result);
  }
  else
  {
	  return Expression(Ires);
  }
};

Expression sq(const std::vector<Expression> & args) {
	//performs sqrt on numbers and complex values. returns a complex value if the input is complex or neagtive
	double result = 0;
	complex<double> Ires(0.0, 0.0);
	complex<double> temp(0.0, 0.0);
	bool hasImag = false;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			if ((args[0].head().asNumber()) >= 0)
			{
				result = std::sqrt(args[0].head().asNumber());
			}
			else {
				hasImag = true;
				temp = temp + args[0].head().asNumber();
				Ires = std::sqrt(temp);
			}
		}
		else if (args[0].isHeadComplex()) {
			hasImag = true;
			Ires = std::sqrt(args[0].head().asComplex());
		}
		else {
			throw SemanticError("Error in call to sqrt: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to sqrt: invalid number of arguments.");
	}
	if (!hasImag)
	{
		return Expression(result);
	}
	else
	{
		return Expression(Ires);
	}
	
};

Expression pow(const std::vector<Expression> & args) {
	//perfroms the power function on complex or real numbers as either argument returns complex if a complexnumber is
	//used in either slot
	double result = 0.0;
	bool hasImag = false;
	complex<double> Ires(0.0, 0.0);

	if (nargs_equal(args, 2)) {
		if ((args[0].isHeadNumber()) && (args[1].isHeadNumber())) {
			result = std::pow(args[0].head().asNumber(), args[1].head().asNumber());
		}
		else if ((args[0].isHeadComplex()) && (args[1].isHeadNumber()))
		{
			hasImag = true;
			Ires = std::pow(args[0].head().asComplex(), args[1].head().asNumber());
		}
		else if ((args[0].isHeadNumber()) && (args[1].isHeadComplex()))
		{
			hasImag = true;
			Ires = std::pow(args[0].head().asNumber(), args[1].head().asComplex());
		}
		else if ((args[0].isHeadComplex()) && (args[1].isHeadComplex()))
		{
			hasImag = true;
			Ires = std::pow(args[0].head().asComplex(), args[1].head().asComplex());
		}
		else {
			throw SemanticError("Error in call to power: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to power: invalid number of arguments.");
	}
	if (!hasImag)
	{
		return Expression(result);
	}
	else
	{
		return Expression(Ires);
	}
};

Expression logn(const std::vector<Expression> & args) {
	//performs the ln operation in real numbers
	//thows an error in ln is called on 0 or a negative number
	//can't be called on a complex value
	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			if ((args[0].head().asNumber()) > 0)
			{
				result = log(args[0].head().asNumber());
			}
			else if ((args[0].head().asNumber()) == 0)
			{
				throw SemanticError("Error in call to ln: ln called  on 0.");
			}
			else
				throw SemanticError("Error in call to ln: negative argument.");

		}
		else {
			throw SemanticError("Error in call to ln: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to ln: invalid number of arguments.");
	}
	return Expression(result);
};

Expression sn(const std::vector<Expression> & args) {
	//performs sin on numbers. Treats the number argument as radians.
	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			result = sin(args[0].head().asNumber());
		}
		else {
			throw SemanticError("Error in call to sin: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to sin: invalid number of arguments.");
	}
	return Expression(result);
};

Expression cn(const std::vector<Expression> & args) {
	//performs cos on numbers. Treats the number argument as radians.
	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			result = cos(args[0].head().asNumber());
		}
		else {
			throw SemanticError("Error in call to cos: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to cos: invalid number of arguments.");
	}
	return Expression(result);
};

Expression tn(const std::vector<Expression> & args) {
	//performs tan on numbers. Treats the number argument as radians.
	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			result = tan(args[0].head().asNumber());
		}
		else {
			throw SemanticError("Error in call to tan: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to tan: invalid number of arguments.");
	}
	return Expression(result);
};

Expression IMreal(const std::vector<Expression> & args) {
	//returns the real component of a complex number. throws an error if called on more than one number.
	//also throws an error if called on a non-complex number
	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = args[0].head().asComplex().real();
		}
		else {
			throw SemanticError("Error in call to real: real called on non-Complex number.");
		}
	}
	else {
		throw SemanticError("Error in call to real: invalid number of arguments.");
	}
	return Expression(result);
};

Expression IMimag(const std::vector<Expression> & args) {
	//returns the imaginary component of a complex number. throws an error if called on more than one number.
	//also throws an error if called on a non-complex number
	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = args[0].head().asComplex().imag();
		}
		else {
			throw SemanticError("Error in call to imag: imag called on non-Complex number.");
		}
	}
	else {
		throw SemanticError("Error in call to imag: invalid number of arguments.");
	}
	return Expression(result);
};

Expression IMmag(const std::vector<Expression> & args) {
	//returns the magnitude of a complex number. throws an error if called on more than one number.
	//also throws an error if called on a non-complex number
	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = std::abs(args[0].head().asComplex());
		}
		else {
			throw SemanticError("Error in call to mag: mag called on non-Complex number.");
		}
	}
	else {
		throw SemanticError("Error in call to mag: invalid number of arguments.");
	}
	return Expression(result);
};

Expression IMarg(const std::vector<Expression> & args) {
	//returns the angle of a complex number. throws an error if called on more than one number.
	//also throws an error if called on a non-complex number
	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			result = std::arg(args[0].head().asComplex());
		}
		else {
			throw SemanticError("Error in call to arg: arg called on non-Complex number.");
		}
	}
	else {
		throw SemanticError("Error in call to arg: invalid number of arguments.");
	}
	return Expression(result);
};

Expression IMconj(const std::vector<Expression> & args) {
	//returns the conjugate of a complex number. throws an error if called on more than one number.
	//also throws an error if called on a non-complex number
	complex<double> Ires;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadComplex()) {
			Ires = std::conj(args[0].head().asComplex());
		}
		else {
			throw SemanticError("Error in call to conj: conj called on non-Complex number.");
		}
	}
	else {
		throw SemanticError("Error in call to conj: invalid number of arguments.");
	}
	return Expression(Ires);
};

Expression Lfirst(const std::vector<Expression> & args) {
	Expression result;
	if (nargs_equal(args, 1))
	{
		if (args[0].isLList())
		{
			if (!args[0].rTail()[0].head().isSymbol())
			{
				result = args[0].rTail()[0];
			}
			else
			{
				throw SemanticError("Error: argument to first is an empty list");
			}
			
		}
		else
		{
			throw SemanticError("Error: argument to first is not a list");
		}
	}
	else
	{
		throw SemanticError("Error: more than one argument in call to first");
	}
	return result;
};

Expression Lrest(const std::vector<Expression> & args) {
	Expression result;
	if (nargs_equal(args, 1))
	{
		if (args[0].isLList())
		{
			if (!args[0].rTail()[0].head().isSymbol())
			{
				result.setLList(true);
				std::size_t listL = args[0].rTail().size();
				for (std::size_t i = 1; i < listL; i++)
				{
					result.rTail().push_back(args[0].rTail()[i]);
				}
				if (listL == 1)
				{
					result.rTail().push_back(Atom(""));
				}
			}
			else
			{
				throw SemanticError("Error: argument to rest is an empty list");
			}
			
		}
		else
		{
			throw SemanticError("Error: argument to rest is not a list");
		}
	}
	else
	{
		throw SemanticError("Error: more than one argument in call to rest");
	}
	return result;
};

Expression Llength(const std::vector<Expression> & args) {
	double result = 0.0;
	if (nargs_equal(args, 1))
	{
		if (args[0].isLList())
		{
			if (!args[0].rTail()[0].isHeadSymbol())
			{
				result = args[0].rTail().size();
			}
			else
			{
				result = 0.0;
			}
		}
		else
		{
			throw SemanticError("Error: argument to length is not a list");
		}
	}
	else
	{
		throw SemanticError("Error: more than one argument in call to length");
	}
	return Expression(result);
};

Expression Lappend(const std::vector<Expression> & args) {
	Expression result;
	if (nargs_equal(args, 2))
	{
		if (args[0].isLList())
		{
			std::size_t length = args[0].rTail().size();
			std::size_t i;
			for (i = 0; i < length; i++)
			{
				result.rTail().push_back(args[0].rTail()[i]);
			}
			result.rTail().push_back(args[1].head());
			result.setLList(true);
		}
		else
		{
			throw SemanticError("Error: first argument to append is not a list");
		}
	}
	else
	{
		throw SemanticError("Error: more than two argument in call to length");
	}
	return result;
};

Expression Ljoin(const std::vector<Expression> & args) {
	Expression result;
	if (nargs_equal(args, 2))
	{
		if (args[0].isLList() && args[1].isLList())
		{
			std::size_t length = args[0].rTail().size();
			std::size_t i;
			for (i = 0; i < length; i++)
			{
				result.rTail().push_back(args[0].rTail().at(i));
			}
			std::size_t length2 = args[1].rTail().size();
			std::size_t j;
			for (j = 0; j < length2; j++)
			{
				result.rTail().push_back(args[1].rTail().at(j));
			}
			result.setLList(true);
		}
		else
		{
			throw SemanticError("Error:  argument to join is not a list");
		}
	}
	else
	{
		throw SemanticError("Error: more than two argument in call to join");
	}
	return result;
};

Expression Lrange(const std::vector<Expression> & args) {
	Expression result;
	if (nargs_equal(args, 3))
	{
		if (args[0].head().isNumber() && args[1].head().isNumber() && args[2].head().isNumber())
		{
			if (args[0].head().asNumber() < args[1].head().asNumber())
			{
				if (args[2].head().asNumber() > 0.0)
				{
					double num = args[0].head().asNumber();
					while (num <= args[1].head().asNumber()) //change base upon behavior of (range 3 3 1)/ ect.
					{
						result.rTail().push_back(Expression(num));
						num += args[2].head().asNumber();
					}
					result.setLList(true);
				}
				else
				{
					throw SemanticError("Error: negative or zero increment in range");
				}
			}
			else
			{
				throw SemanticError("Error: begin greater than end in range");
			}
		}
		else
		{
			throw SemanticError("Error: non-number argument is called with range");
		}
	}
	else
	{
		throw SemanticError("Error: either more than, or less than three argument in call to ");
	}
	return result;
};

//Variables set up for pi, e, and I
const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);
const complex<double> IMI(0.0, 1.0);

Environment::Environment(){

  reset();
}

bool Environment::is_known(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  return envmap.find(sym.asSymbol()) != envmap.end();
}

bool Environment::is_exp(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ExpressionType);
}

Expression Environment::get_exp(const Atom & sym) const{

  Expression exp;
  
  if(sym.isSymbol()){
    auto result = envmap.find(sym.asSymbol());
    if((result != envmap.end()) && (result->second.type == ExpressionType)){
      exp = result->second.exp;
    }
  }

  return exp;
}

void Environment::add_exp(const Atom & sym, const Expression & exp){

  if(!sym.isSymbol()){
    throw SemanticError("Attempt to add non-symbol to environment");
  }
    
  // error if overwriting symbol map
  if(envmap.find(sym.asSymbol()) != envmap.end()){
    throw SemanticError("Attempt to overwrite symbol in environemnt");
  }

  envmap.emplace(sym.asSymbol(), EnvResult(ExpressionType, exp)); 
}

bool Environment::is_lambda_exp(const Atom & sym) const {
	if (!sym.isSymbol()) return false;

	auto result = envmapLambda.find(sym.asSymbol());
	return (result != envmapLambda.end()) && (result->second.type == ExpressionType);
}

Expression Environment::get_lambda_exp(const Atom & sym) const {

	Expression exp;

	if (sym.isSymbol()) {
		auto result = envmapLambda.find(sym.asSymbol());
		if ((result != envmapLambda.end()) && (result->second.type == ExpressionType)) {
			exp = result->second.exp;
		}
	}

	return exp;
}

void Environment::add_lambda_exp(const Atom & sym, const Expression & exp) {

	if (!sym.isSymbol()) {
		throw SemanticError("Attempt to add non-symbol to environment");
	}
	if (envmapLambda.find(sym.asSymbol()) != envmapLambda.end()) {
		envmapLambda.erase(sym.asSymbol());
	}

	envmapLambda.emplace(sym.asSymbol(), EnvResult(ExpressionType, exp));
}

bool Environment::is_lambda(const Atom & sym) const {
	if (!sym.isSymbol()) return false;

	auto result = envmap.find(sym.asSymbol());
	return (result != envmap.end()) && (result->second.type == LambdaType);
}

Expression Environment::get_lambda(const Atom & sym) const {

	Expression exp;

	if (sym.isSymbol()) {
		auto result = envmap.find(sym.asSymbol());
		if ((result != envmap.end()) && (result->second.type == LambdaType)) {
			exp = result->second.exp;
		}
	}

	return exp;
}

void Environment::add_lambda(const Atom & sym, const Expression & exp) {

	if (!sym.isSymbol()) {
		throw SemanticError("Attempt to add non-symbol to environment");
	}

	// error if overwriting symbol map
	if (envmap.find(sym.asSymbol()) != envmap.end()) {
		throw SemanticError("Attempt to overwrite symbol in environemnt");
	}

	envmap.emplace(sym.asSymbol(), EnvResult(LambdaType, exp));
}

bool Environment::is_proc(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ProcedureType);
}

Procedure Environment::get_proc(const Atom & sym) const{

  //Procedure proc = default_proc;

  if(sym.isSymbol()){
    auto result = envmap.find(sym.asSymbol());
    if((result != envmap.end()) && (result->second.type == ProcedureType)){
      return result->second.proc;
    }
  }

  return default_proc;
}

/*
Reset the environment to the default state. First remove all entries and
then re-add the default ones.
 */
void Environment::reset(){

  envmap.clear();
  envmapLambda.clear();
  
  // Built-In value of pi
  envmap.emplace("pi", EnvResult(ExpressionType, Expression(PI)));

  // Built-In value of e
  envmap.emplace("e", EnvResult(ExpressionType, Expression(EXP)));

  // Built-In value of I
  envmap.emplace("I", EnvResult(ExpressionType, Expression(IMI)));

  // Procedure: add;
  envmap.emplace("+", EnvResult(ProcedureType, add)); 

  // Procedure: subneg;
  envmap.emplace("-", EnvResult(ProcedureType, subneg)); 

  // Procedure: mul;
  envmap.emplace("*", EnvResult(ProcedureType, mul)); 

  // Procedure: div;
  envmap.emplace("/", EnvResult(ProcedureType, div));

  // Procedure: sqrt;
  envmap.emplace("sqrt", EnvResult(ProcedureType, sq));

  // Procedure: pow;
  envmap.emplace("^", EnvResult(ProcedureType, pow));

  // Procedure: logn;
  envmap.emplace("ln", EnvResult(ProcedureType, logn));

  // Procedure: sin;
  envmap.emplace("sin", EnvResult(ProcedureType, sn));

  // Procedure: cos;
  envmap.emplace("cos", EnvResult(ProcedureType, cn));

  // Procedure: tan;
  envmap.emplace("tan", EnvResult(ProcedureType, tn));

  // Procedure: real;
  envmap.emplace("real", EnvResult(ProcedureType, IMreal));

  // Procedure: imag;
  envmap.emplace("imag", EnvResult(ProcedureType, IMimag));

  // Procedure: mag;
  envmap.emplace("mag", EnvResult(ProcedureType, IMmag));

  // Procedure: arg;
  envmap.emplace("arg", EnvResult(ProcedureType, IMarg));

  // Procedure: conj;
  envmap.emplace("conj", EnvResult(ProcedureType, IMconj));

  // Procedure: first;
  envmap.emplace("first", EnvResult(ProcedureType, Lfirst));

  // Procedure: rest;
  envmap.emplace("rest", EnvResult(ProcedureType, Lrest));

  // Procedure: length;
  envmap.emplace("length", EnvResult(ProcedureType, Llength));

  // Procedure: append;
  envmap.emplace("append", EnvResult(ProcedureType, Lappend));

  // Procedure: join;
  envmap.emplace("join", EnvResult(ProcedureType, Ljoin));

  // Procedure: range;
  envmap.emplace("range", EnvResult(ProcedureType, Lrange));
}
