#include "expression.hpp"

#include <sstream>
#include <string>
#include <list>

#include "environment.hpp"
#include "semantic_error.hpp"

Expression::Expression(){}

Expression::Expression(const Atom & a){

  m_head = a;
}

// recursive copy
Expression::Expression(const Expression & a){

  m_head = a.m_head;
  isList = a.isList;
  islambda = a.islambda;
  for(auto e : a.m_tail){
    m_tail.push_back(e);
  }
}

Expression & Expression::operator=(const Expression & a){

  // prevent self-assignment
  if(this != &a){
    m_head = a.m_head;
	isList = a.isList;
	islambda = a.islambda;
    m_tail.clear();
    for(auto e : a.m_tail){
      m_tail.push_back(e);
    } 
  }
  
  return *this;
}


Atom & Expression::head(){
  return m_head;
}

const Atom & Expression::head() const{
  return m_head;
}

std::vector<Expression> & Expression::rTail() {
	return m_tail;
}

const std::vector<Expression> & Expression::rTail() const {
	return m_tail;
}

bool Expression::isHeadNumber() const noexcept{
  return m_head.isNumber();
}

bool Expression::isHeadSymbol() const noexcept{
  return m_head.isSymbol();
}  

bool Expression::isHeadComplex() const noexcept {
	return m_head.isComplex();
}

bool Expression::isLList() const noexcept {
	return isList;
}

void Expression::setLList(bool set)
{
	isList = set;
}

bool Expression::isLLambda() const noexcept {
	return islambda;
}

void Expression::setLLambda(bool set)
{
	islambda = set;
}


void Expression::append(const Atom & a){
  m_tail.emplace_back(a);
}


Expression * Expression::tail(){
  Expression * ptr = nullptr;
  
  if(m_tail.size() > 0){
    ptr = &m_tail.back();
  }

  return ptr;
}

Expression::ConstIteratorType Expression::tailConstBegin() const noexcept{
  return m_tail.cbegin();
}

Expression::ConstIteratorType Expression::tailConstEnd() const noexcept{
  return m_tail.cend();
}

Expression apply(const Atom & op, const std::vector<Expression> & args, const Environment & env){

  // head must be a symbol
  if(!op.isSymbol()){
    throw SemanticError("Error during evaluation: procedure name not symbol");
  }
  
  // must map to a proc
  if(!env.is_proc(op)){
    throw SemanticError("Error during evaluation: symbol does not name a procedure");
  }
  
  // map from symbol to proc
  Procedure proc = env.get_proc(op);
  
  // call proc with args
  return proc(args);
}

Expression Expression::handle_lookup(const Atom & head, const Environment & env){
    if(head.isSymbol()){ // if symbol is in env return value
		if (env.is_lambda_exp(head))
		{
			return env.get_lambda_exp(head);
		}
		else if (env.is_exp(head))
		{
			return env.get_exp(head);
		}
		else{
		throw SemanticError("Error during evaluation: unknown symbol");
		}
    }
    else if(head.isNumber()){
      return Expression(head);
    }
	else if (head.isComplex()) {
		return Expression(head);
	}
    else{
      throw SemanticError("Error during evaluation: Invalid type in terminal expression");
    }
}

Expression Expression::handle_begin(Environment & env){
  
  if(m_tail.size() == 0){
    throw SemanticError("Error during evaluation: zero arguments to begin");
  }

  // evaluate each arg from tail, return the last
  Expression result;
  for(Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it){
    result = it->eval(env);
  }
  
  return result;
}


Expression Expression::handle_define(Environment & env){

  // tail must have size 3 or error
  if(m_tail.size() != 2){
    throw SemanticError("Error during evaluation: invalid number of arguments to define");
  }
  
  // tail[0] must be symbol
  if(!m_tail[0].isHeadSymbol()){
    throw SemanticError("Error during evaluation: first argument to define not symbol");
  }

  // but tail[0] must not be a special-form or procedure
  std::string s = m_tail[0].head().asSymbol();
  if((s == "define") || (s == "begin")){
    throw SemanticError("Error during evaluation: attempt to redefine a special-form");
  }
  
  if(env.is_proc(m_head)){
    throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
  }
  //if (env.is_lambda(m_head)) {
	//  throw SemanticError("Error during evaluation: attempt to redefine a lambda procedure");
  //}

  // eval tail[1]
  Expression result = m_tail[1].eval(env);
  if (result.isLLambda())
  {
	  islambda = true;
  }
  if (result.islambdaexp)
  {
	  islambdaexp = true;
  }

  //and add to env
  if (!islambda)
  {
	  if (!islambdaexp)
	  {
		  //throw SemanticError("also incorrect");
		  if (env.is_exp(m_head)) {
			  throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
		  }
		  env.add_exp(m_tail[0].head(), result);
	  }
	  else
	  {
		  env.add_lambda_exp(m_tail[0].head(), result);
	  }
  }
  else
  {
	 env.add_lambda(m_tail[0].head(), result);
  }

  return result;
}


Expression Expression::handle_list(Environment & env)
{
	Expression result;
	result.isList = true;
	if (m_tail.empty())
	{
		result.m_tail.push_back(Atom(""));

	}
	else
	{
		for (Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it) {
			result.m_tail.push_back(it->eval(env));
		}
	}
	return result;
}


Expression Expression::handle_lambda(Environment & env)
{
	if (m_tail.size() == 0)
	{
		throw SemanticError("Error during evaluation: zero arguments to lambda");
	}
	Expression s1;
	Expression s2;
	//lists
	Expression result;
	Expression arguments;
	Expression procedure;
	std::size_t length = m_tail.size();
	if (length == 2)
	{
		s1 = m_tail[0];
		s2 = m_tail[1];
		std::size_t s1Length = s1.m_tail.size();
		std::size_t s2Length = s2.m_tail.size();
		arguments.rTail().push_back(s1.m_head);
		for (std::size_t i = 0; i < s1Length; i++)
		{
			arguments.rTail().push_back(s1.m_tail[i]);
		}

		procedure = s2;
		arguments.setLList(true);
		result.rTail().push_back(arguments);
		result.rTail().push_back(procedure);
	}
	else
	{
		throw SemanticError("Error during evaluation: incorrect amout of arguments to lambda");
	}
	result.islambda = true;
	return result;
}

Expression Expression::handle_lambda_lookup(const Atom & head, Environment & env)
{
	Expression result;
	Expression stage1 = env.get_lambda(head);

	Expression arguments = stage1.m_tail[0];
	Expression operations = stage1.m_tail[1];
	result = operations;

	std::size_t lengtharg = arguments.m_tail.size();
	if (m_tail.size() == lengtharg)
	{
		for (std::size_t i = 0; i < lengtharg; i++)
		{
			Expression args(Atom("define"));
			args.append(arguments.m_tail[i].head());
			Expression argi = m_tail[i].eval(env);
			args.append(argi.head());
			args.islambdaexp = true;
			args.handle_define(env);
		}
		result = operations.eval(env);
	}
	else
	{
		throw SemanticError("Error in call to procedure: invalid number of arguments.");
	}
	return result;
}

Expression Expression::handle_apply(Environment & env)
{
	//check the head/tail for relevent vars
	Expression result;

	if ((env.is_proc(m_tail[0].head()) || env.is_lambda(m_tail[0].head())) && (m_tail[0].m_tail.empty()))
	{
		Expression list = m_tail[1].eval(env);
		if (list.isLList())
		{
			Expression expr(m_tail[0].head());
			expr.m_tail = list.m_tail;
			try
			{
			result = expr.eval(env);
			}
			catch (SemanticError e)
			{
				std::string t = e.what();
				std::string output = "Error: during apply: " + t;
				throw SemanticError(output);
			}
		}
		else
		{
			throw SemanticError("Error: second argument to apply not a list");
		}
	}
	else
	{
		throw SemanticError("Error: first argument to apply not a procedure");
	}
	return result;
}

Expression Expression::handle_map(Environment & env)
{
	//check the head/tail for relevent vars
	Expression resultf;

	if ((env.is_proc(m_tail[0].head()) || env.is_lambda(m_tail[0].head())) && (m_tail[0].m_tail.empty()))
	{
		Expression list = m_tail[1].eval(env);
		if (list.isLList())
		{

			
			std::size_t listL = list.m_tail.size();
			for (std::size_t i = 0; i < listL; i++)
			{
				Expression expr(m_tail[0].head());
				Expression result1;
				Expression temp = list.m_tail[i].eval(env);

				expr.append(temp.head());
				try
				{
					 result1 = expr.eval(env);
				}
				catch (SemanticError e)
				{
					std::string t = e.what();
					std::string output = "Error: during map: " + t;
					throw SemanticError(output);
				}
				resultf.append(result1.head());
				resultf.setLList(true);
			}
		}
		else
		{
			throw SemanticError("Error: second argument to map not a list");
		}
	}
	else
	{
		throw SemanticError("Error: first argument to map not a procedure");
	}
	return resultf;
}

// this is a simple recursive version. the iterative version is more
// difficult with the ast data structure used (no parent pointer).
// this limits the practical depth of our AST
Expression Expression::eval(Environment & env){
  if (m_head.isSymbol() && m_head.asSymbol() == "list") {
	  return handle_list(env);
  }
  else if (m_tail.empty() && env.is_lambda(m_head)) {
	  //fix this as errors are occuring
	  return env.get_lambda(m_head.asSymbol());
  }
  else if(m_tail.empty()){
    return handle_lookup(m_head, env);
  }
  // handle begin special-form
  else if(m_head.isSymbol() && m_head.asSymbol() == "begin"){
    return handle_begin(env);
  }
  // handle define special-form
  else if(m_head.isSymbol() && m_head.asSymbol() == "define"){
    return handle_define(env);
  }
  else if (m_head.isSymbol() && m_head.asSymbol() == "apply") {
	  return handle_apply(env);
  }
  else if (m_head.isSymbol() && m_head.asSymbol() == "map") {
	  return handle_map(env);
  }
  else if (m_head.isSymbol() && m_head.asSymbol() == "lambda") {
	  islambda = true;
	  return handle_lambda(env);
  }
  else if (m_head.isSymbol() && env.is_lambda(m_head))
  {
	  return handle_lambda_lookup(m_head, env);
  }
  // else attempt to treat as procedure
  else{ 
    std::vector<Expression> results;
    for(Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it){
      results.push_back(it->eval(env));
    }
    return apply(m_head, results, env);
  }
}


std::ostream & operator<<(std::ostream & out, const Expression & exp){
	//added the if statement to allow for the proper outputing of complex numbers
	if (!exp.head().isComplex())
	{
		out << "(";
	}
		
  out << exp.head();
  std::size_t tailL = exp.rTail().size();
  std::size_t i = 0;
  if ((tailL != 0) && !exp.isLList() && !exp.isLLambda())
  {
	  out << " ";
  }

  for(auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e){
	  if (e->head().isSymbol() && e->head().asSymbol() == "")
	  {

	  }
	  else
	  {
		  out << *e;
		  if ((i < tailL - 1))
		  {
			  out << " ";
		  }
		  i++;
	  }
  }
  //added the if statement to allow for the proper outputing of complex numbers
  if (!exp.head().isComplex())
  {
	  out << ")";
  }

  return out;
}

bool Expression::operator==(const Expression & exp) const noexcept{

  bool result = (m_head == exp.m_head);

  result = result && (m_tail.size() == exp.m_tail.size());

  if(result){
    for(auto lefte = m_tail.begin(), righte = exp.m_tail.begin();
	(lefte != m_tail.end()) && (righte != exp.m_tail.end());
	++lefte, ++righte){
      result = result && (*lefte == *righte);
    }
  }

  return result;
}

bool operator!=(const Expression & left, const Expression & right) noexcept{

  return !(left == right);
}
