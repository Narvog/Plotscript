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
  inLambda = a.inLambda;
  propMap = a.propMap;
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
	inLambda = a.inLambda;
	propMap = a.propMap;
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
	else if (head.isString())
	{
		return *this;
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
  if (env.is_lambda(m_head)) {
	  throw SemanticError("Error during evaluation: attempt to redefine a lambda procedure");
  }

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
		  if (!inLambda)
		  {
			  env.add_exp(m_tail[0].head(), result);
		  }
		  else
		  {
			  env.add_lambda_exp(m_tail[0].head(), result);
		  }
		  
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


Expression Expression::handle_lambda()
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
			args.rTail().emplace_back(argi);
			args.islambdaexp = true;
			args.handle_define(env);
		}
		helperinL(operations);
		result = operations.eval(env);
	}
	else
	{
		throw SemanticError("Error in call to procedure: invalid number of arguments.");
	}
	return result;
}

void Expression::helperinL(Expression & oper)
{
	for (size_t i = 0; i < oper.rTail().size(); i++)
	{
		oper.rTail()[i].inLambda = true;
		helperinL(oper.rTail()[i]);
	}
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

Expression Expression::handle_setprop(Environment & env)
{
	if (m_tail.size() == 3)
	{
		for (int i = 0; i < 3; i++)
		{
			m_tail[i] = m_tail[i].eval(env);
		}
		if (m_tail[0].head().isString())
		{
			m_tail[2].add_prop(m_tail[0].head(), m_tail[1]);
		}
		else
		{
			throw SemanticError("Error: first argument to set-property not a string");
		}
	}
	else
	{
		throw SemanticError("Improper number of arguments in set-property");
	}
	return m_tail[2];
}

Expression Expression::handle_getprop(Environment & env)
{
	Expression result;
	if (m_tail.size() == 2)
	{
		for (int i = 0; i < 2; i++)
		{
			m_tail[i] = m_tail[i].eval(env);
		}

		if (m_tail[0].head().isString())
		{
			result = m_tail[1].get_prop(m_tail[0].head());
		}
		else
		{
			throw SemanticError("Error: first argument to set-property not a string");
		}
	}
	else
	{
		throw SemanticError("Improper number of arguments in get-property");
	}
	return result;
}

Expression Expression::handle_discplot(Environment & env)
{
	double N = 20;
	double A = 3;
	double B = 3;
	double C = 2;
	double D = 2;
	double P = 0.5;


	Expression resultf;
	Expression resultb;
	Expression resultTM;
	Expression stage1;
	Atom list("list");
	Atom thickness("\"thickness\"");
	thickness.setString();
	m_head = list;
	stage1 = eval(env);
	resultb = make_box(env, N);

	if (stage1.rTail().size() > 0)
	{
		Expression points = stage1.rTail()[0];
		if (points.rTail().size() > 0)
		{
			double maxX = -10000;
			double minX = 100000;
			double maxY = -10000;
			double minY = 100000;
			for (std::size_t i = 0; i < points.rTail().size(); i++)
			{
				Expression point = points.rTail()[i];
				if (point.rTail().size() == 2)
				{
					if (point.rTail()[0].isHeadNumber())
					{
						if (maxX < point.rTail()[0].head().asNumber())
						{
							maxX = point.rTail()[0].head().asNumber();
						}

						if (minX > point.rTail()[0].head().asNumber())
						{
							minX = point.rTail()[0].head().asNumber();
						}
					}
					else
					{
						//error
					}

					if (point.rTail()[1].isHeadNumber())
					{
						if (maxY < point.rTail()[1].head().asNumber())
						{
							maxY = point.rTail()[1].head().asNumber();
						}

						if (minY > point.rTail()[1].head().asNumber())
						{
							minY = point.rTail()[1].head().asNumber();
						}
					}
					else
					{
						//error
					}
				}
				else
				{
					//error
				}
			}
			
			resultTM = make_pos_labels(env, N, C, D, minX, maxX, minY, maxY);

			double midX = (maxX + minX)/2;
			double midY = (maxY + minY)/2;

			double scaleX = ((abs(maxX) + abs(minX)) / N);
			double scaleY = -1*((abs(maxY) + abs(minY)) / N);
		}
		else
		{
			//error
		}
	}
	else
	{
		//error
	}
	Expression join(Atom("join"));
	join.rTail().emplace_back(resultb);
	join.rTail().emplace_back(resultTM);
	resultf = join.eval(env);
	return resultf;
}

Expression Expression::make_box(Environment & env, const double N)
{
	Expression resultb;
	Atom list("list");
	Atom thickness("\"thickness\"");
	thickness.setString();
	Expression stage2(list);
	Expression boxu(Atom("make-line"));
	Expression fpu(Atom("make-point"));
	fpu.append(Atom(-N/2));
	fpu.append(Atom(-N/2));
	Expression spu(Atom("make-point"));
	spu.append(Atom(N/2));
	spu.append(Atom(-N/2));
	boxu.rTail().push_back(fpu);
	boxu.rTail().push_back(spu);
	boxu = boxu.eval(env);

	boxu.add_prop(thickness, Expression(0));
	
	stage2.rTail().push_back(boxu);

	Expression boxd(Atom("make-line"));
	Expression fpd(Atom("make-point"));
	fpd.append(Atom(N/2));
	fpd.append(Atom(N/2));
	Expression spd(Atom("make-point"));
	spd.append(Atom(-N/2));
	spd.append(Atom(N/2));
	boxd.rTail().push_back(fpd);
	boxd.rTail().push_back(spd);
	boxd = boxd.eval(env);

	boxd.add_prop(thickness, Expression(0));
	
	stage2.rTail().push_back(boxd);

	Expression boxr(Atom("make-line"));
	Expression fpr(Atom("make-point"));
	fpr.append(Atom(N/2));
	fpr.append(Atom(-N/2));
	Expression spr(Atom("make-point"));
	spr.append(Atom(N/2));
	spr.append(Atom(N/2));
	boxr.rTail().push_back(fpr);
	boxr.rTail().push_back(spr);
	boxr = boxr.eval(env);

	boxr.add_prop(thickness, Expression(0));
	stage2.rTail().push_back(boxr);

	Expression boxl(Atom("make-line"));
	Expression fpl(Atom("make-point"));
	fpl.append(Atom(-N/2));
	fpl.append(Atom(-N/2));
	Expression spl(Atom("make-point"));
	spl.append(Atom(-N/2));
	spl.append(Atom(N/2));
	boxl.rTail().push_back(fpl);
	boxl.rTail().push_back(spl);
	boxl = boxl.eval(env);

	boxl.add_prop(thickness, Expression(0));
	stage2.rTail().push_back(boxl);

	resultb = stage2.eval(env);
	return resultb;
}

Expression Expression::make_pos_labels(Environment & env, const double N, const double C, const double D, const double minX, const double maxX, const double minY, const double maxY)
{
	Atom list("list");
	Expression stage2(list);

	Atom text("make-text");
	Atom position("\"position\"");
	position.setString();

	Expression OU(text);
	std::string textConOU;
	textConOU = "\"" + std::to_string((int)std::round(maxY)) + "\"";
	Atom OUCon(textConOU);
	OUCon.setString();
	OU.append(OUCon);

	Expression OUF = OU.eval(env);


	Expression OUpos(Atom("make-point"));
	OUpos.append(Atom(((-N / 2) - D)));
	OUpos.append(Atom(((-N / 2))));

	OUpos = OUpos.eval(env);

	OUF.add_prop(position, OUpos);
	stage2.rTail().push_back(OUF);


	Expression OL(text);
	std::string textConOL;
	textConOL = "\"" + std::to_string((int)std::round(minY)) + "\"";
	Atom OLCon(textConOL);
	OLCon.setString();
	OL.append(OLCon);

	Expression OLF = OL.eval(env);


	Expression OLpos(Atom("make-point"));
	OLpos.append(Atom(((-N / 2) - D)));
	OLpos.append(Atom(((N / 2))));

	OLpos = OLpos.eval(env);

	OLF.add_prop(position, OLpos);
	stage2.rTail().push_back(OLF);

	Expression AL(text);
	std::string textConAL;
	textConAL = "\"" + std::to_string((int)std::round(minX)) + "\"";
	Atom ALCon(textConAL);
	ALCon.setString();
	AL.append(ALCon);

	Expression ALF = AL.eval(env);


	Expression ALpos(Atom("make-point"));
	ALpos.append(Atom(((-N / 2))));
	ALpos.append(Atom(((N / 2) + C)));

	ALpos = ALpos.eval(env);

	ALF.add_prop(position, ALpos);
	stage2.rTail().push_back(ALF);


	Expression AU(text);
	std::string textConAU;
	textConAU = "\"" + std::to_string((int)std::round(maxX)) + "\"";
	Atom AUCon(textConAU);
	AUCon.setString();
	AU.append(AUCon);

	Expression AUF = AU.eval(env);


	Expression AUpos(Atom("make-point"));
	AUpos.append(Atom(((N / 2))));
	AUpos.append(Atom(((N / 2) + C)));

	AUpos = AUpos.eval(env);

	AUF.add_prop(position, AUpos);
	stage2.rTail().push_back(AUF);
	Expression resultTM = stage2.eval(env);
	return resultTM;
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
	  Expression test = handle_lookup(m_head, env);
	  return test;
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
  else if (m_head.isSymbol() && m_head.asSymbol() == "set-property") {
	  Expression test = handle_setprop(env);
	  return test;
  }
  else if (m_head.isSymbol() && m_head.asSymbol() == "get-property") {
	  return handle_getprop(env);
  }
  else if (m_head.isSymbol() && m_head.asSymbol() == "discrete-plot") {
	  return handle_discplot(env);
  }
  else if (m_head.isSymbol() && m_head.asSymbol() == "lambda") {
	  islambda = true;
	  return handle_lambda();
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
	if (!isList)
	{
		return apply(m_head, results, env);
	}
	return *this;
  }
}


std::ostream & operator<<(std::ostream & out, const Expression & exp){
	//added the if statement to allow for the proper outputing of complex numbers
	if (!exp.head().isComplex())
	{
		if (!exp.head().isNone() || exp.isLLambda() || exp.isLList())
		{
			out << "(";
		}
		
	}
	if (!exp.isLLambda() && !exp.isLList())
	{
		out << exp.head();
	}
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
	  if (!exp.head().isNone() || exp.isLLambda() || exp.isLList())
	  {
		  out << ")";
	  }
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


bool Expression::is_prop(const Atom & key) const {
	if (!key.isString()) return false;
	auto result = propMap.find(key.asString());
	return (result != propMap.end());
}


Expression Expression::get_prop(const Atom & key) const {
	Expression exp;
	if (key.isString()) {
		auto result = propMap.find(key.asString());
		if ((result != propMap.end())) {
			exp = Expression(result->second);
		}
	}
	return exp;
}


void Expression::add_prop(const Atom & key, const Expression & prop) {

	if (!key.isString()) {
		throw SemanticError("Attempt to add non-string to the property list.");
	}
	if (propMap.find(key.asString()) != propMap.end()) {
		propMap.erase(key.asString());
	}

	propMap.emplace(key.asString(), prop);
}
