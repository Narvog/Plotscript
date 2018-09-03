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

  // check all aruments are numbers, while adding
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
 
  // check all aruments are numbers, while multiplying
	double result = 1.0;
	bool hasImag = false;
	complex<double> Ires(1.0, 1.0);
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

	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			if ((abs(args[0].head().asNumber())) == args[0].head().asNumber())
			{
				result = std::sqrt(args[0].head().asNumber());
			}
			else
				throw SemanticError("Error in call to sqrt: negative argument.");
			
		}
		else {
			throw SemanticError("Error in call to sqrt: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to sqrt: invalid number of arguments.");
	}
	return Expression(result);
};

Expression pow(const std::vector<Expression> & args) {

	double result = 0;

	if (nargs_equal(args, 2)) {
		if ((args[0].isHeadNumber()) && (args[1].isHeadNumber())) {
			result = std::pow(args[0].head().asNumber(), args[1].head().asNumber());
		}
		else {
			throw SemanticError("Error in call to power: invalid argument.");
		}
	}
	else {
		throw SemanticError("Error in call to power: invalid number of arguments.");
	}
	return Expression(result);
};

Expression logn(const std::vector<Expression> & args) {

	double result = 0;

	if (nargs_equal(args, 1)) {
		if (args[0].isHeadNumber()) {
			if ((abs(args[0].head().asNumber())) == args[0].head().asNumber())
			{
				result = log(args[0].head().asNumber());
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
}
