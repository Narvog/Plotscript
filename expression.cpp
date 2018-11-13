#include "expression.hpp"

#include <sstream>
#include <string>
#include <list>
#include <iomanip>

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
				resultf.rTail().emplace_back(result1);
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
	//test change should be set to 0.5 but is 0.1 to prove a point with 0.1 turns up clear for some reason.
	double P = 0.5;
	double scale = 1;

	Expression resultf;
	Expression resultb;
	Expression resultTM;
	Expression resultTitles;
	Expression resultAxes;
	Expression resultplot;
	bool areTitles = false;
	bool hasAxes = false;
	Expression stage1;
	Atom list("list");
	Atom thickness("\"thickness\"");
	thickness.setString();
	m_head = list;
	stage1 = eval(env);
	

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







			double midX = (maxX + minX) / 2;
			double midY = (maxY + minY) / 2;

			double scaleX = (N / (maxX - minX));
			double scaleY = -1 * (N / (maxY - minY));
			

			if (stage1.rTail().size() == 2)
			{
				Expression titles = stage1.rTail()[1];

				if (titles.isLList())
				{
					//find text scale if applicable;
					for (std::size_t i = 0; i < titles.rTail().size(); i++)
					{
						if (titles.rTail()[i].rTail().size() == 2)
						{
							if (titles.rTail()[i].rTail()[0].head().isString())
							{
								if (titles.rTail()[i].rTail()[0].head().asString() == "\"text-scale\"")
								{
									if (titles.rTail()[i].rTail()[1].isHeadNumber())
									{
										scale = titles.rTail()[i].rTail()[1].head().asNumber();
									}
								}
							}
						}
						else
						{
							//error
						}

					}

					Atom list("list");
					Expression stage2(list);
					for (std::size_t i = 0; i < titles.rTail().size(); i++)
					{
						if (titles.rTail()[i].rTail().size() == 2)
						{

							if (titles.rTail()[i].rTail()[0].head().isString())
							{

								if (titles.rTail()[i].rTail()[0].head().asString() == "\"title\"")
								{
									if (titles.rTail()[i].rTail()[1].head().isString())
									{
										areTitles = true;
										Expression textO = helper_make_text(env, titles.rTail()[i].rTail()[1].head().asString(), midX * scaleX, maxY * scaleY, 0, -A, scale);
										stage2.rTail().push_back(textO);
									}
								}
								else if (titles.rTail()[i].rTail()[0].head().asString() == "\"abscissa-label\"")
								{
									if (titles.rTail()[i].rTail()[1].head().isString())
									{
										areTitles = true;
										Expression textO = helper_make_text(env, titles.rTail()[i].rTail()[1].head().asString(), midX * scaleX, minY * scaleY, 0, A, scale);
										stage2.rTail().push_back(textO);
									}
								}
								else if (titles.rTail()[i].rTail()[0].head().asString() == "\"ordinate-label\"")
								{
									if (titles.rTail()[i].rTail()[1].head().isString())
									{
										areTitles = true;
										Expression textO = helper_make_text(env, titles.rTail()[i].rTail()[1].head().asString(), minX * scaleX, midY * scaleY, -B, 0, scale);
										Atom rot("\"text-rotation\"");
										rot.setString();
										textO.add_prop(rot, Expression(Atom(std::atan2(0, -1) * -1 / 2)));
										stage2.rTail().push_back(textO);
									}
								}
							}
						}
						else
						{
							//error
						}

					}
					if (areTitles)
					{
						resultTitles = stage2.eval(env);
					}
				}
				else
				{
					//error
				}
			}

			resultb = make_box(env, minX * scaleX, minY * scaleY, maxX * scaleX, maxY * scaleY);

			resultTM = make_pos_labels(env, D, C, scale, minX, maxX, minY, maxY, scaleX, scaleY);

			double Xaxis = minY*scaleY;
			double Yaxis = minX;
			bool hasXaxis = false;

			Expression stage2(list);
			if (minY < 0 && maxY > 0)
			{
			hasAxes = true;
			hasXaxis = true;
			Xaxis = 0;
			Expression XA = helper_make_line(env, maxX * scaleX, Xaxis, minX * scaleX, Xaxis, 0);
			stage2.rTail().push_back(XA);
			}

			if (minX < 0 && maxX > 0)
			{
			hasAxes = true;
			Yaxis = 0;
			Expression YA = helper_make_line(env, Yaxis, maxY * scaleY, Yaxis, minY * scaleY, 0);
			stage2.rTail().push_back(YA);
			}

			if (hasAxes)
			{
			resultAxes = stage2.eval(env);
			}

			Expression stage3(list);
			for (size_t i = 0; i < points.rTail().size(); i++)
			{
			double x;
			double y;
			Expression point = points.rTail()[i];
			if (point.rTail().size() == 2)
			{
			if (point.rTail()[0].isHeadNumber())
			{
			x = ((point.rTail()[0].head().asNumber()) * scaleX);
			}
			else
			{
			//error
			}

			if (point.rTail()[1].isHeadNumber())
			{
			y = ((point.rTail()[1].head().asNumber()) * scaleY);
			}
			else
			{
			//error
			}
			}
			Expression dot = helper_make_point(env, x, y, P);
			Expression line = helper_make_line(env, x, y, x, Xaxis, 0);
			stage3.rTail().push_back(dot);
			stage3.rTail().push_back(line);
			}
			resultplot = stage3.eval(env);
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
	
	Expression join1(Atom("join"));
	join1.rTail().emplace_back(resultb);
	join1.rTail().emplace_back(resultTM);
	Expression result1 = join1.eval(env);
	
	//implement join2 here
	
	if (hasAxes)
	{
		Expression join2(Atom("join"));
		join2.rTail().push_back(result1);
		join2.rTail().emplace_back(resultAxes);
		result1 = join2.eval(env);
	}
	//implement join3 here
	Expression join3(Atom("join"));
	join3.rTail().push_back(result1);
	join3.rTail().emplace_back(resultplot);
	Expression result2 = join3.eval(env);
	if (!areTitles)
	{
		return result2;
	}
	else
	{
		Expression join4(Atom("join"));
		join4.rTail().emplace_back(result2);
		join4.rTail().emplace_back(resultTitles);
		resultf = join4.eval(env);
	}
	return resultf;
}

Expression Expression::handle_contplot(Environment & env)
{
	double N = 20;
	double A = 3;
	double B = 3;
	double C = 2;
	double D = 2;
	double P = 0.5;
	double scale = 1;
	bool areTitles = false;
	bool hasAxes = false;

	Expression resultdata;
	Expression stage1;
	Expression func;

	Expression resultb;
	Expression resultTM;
	Expression resultTitles;
	Expression resultAxes;
	Expression resultplot;

	Expression resultf;

	Atom range("range");
	Expression stage2(range);

	Atom map("map");
	Expression stage3(map);

	Atom list("list");


	if (rTail().size() > 2)
	{
		func = rTail()[0];
		stage1 = rTail()[1].eval(env);
			if (stage1.rTail()[0].isHeadNumber() && stage1.rTail()[1].isHeadNumber())
			{
				double b1 = stage1.rTail()[0].head().asNumber();
				double b2 = stage1.rTail()[1].head().asNumber();
				double dist = b2 - b1;
				double seg = dist / 50;

				std::stringstream stream;
				stream << std::setprecision(4) << seg;
				std::string segd;
				stream >> segd;
				double seg2 = std::stod(segd, nullptr);
				double seg3 = 0.08;
				stage2.append(Atom(b1));
				stage2.append(Atom(b2));
				stage2.append(Atom(seg2));

				stage3.rTail().emplace_back(func);

				Expression Xcord = stage2.eval(env);

				stage3.rTail().emplace_back(Xcord);
				Expression Ycord = stage3.eval(env);

				for (int i = 0; i < 9; i++)
				{
					Expression nextX(list);
					Expression nextY(map);
					nextY.rTail().push_back(func);
					int z = 0;
					std::size_t max = Xcord.rTail().size();
					for (size_t j = 1; j < Xcord.rTail().size() - 1; j++)
					{
						z++;
						double x1 = Xcord.rTail()[j-1].head().asNumber();
						double y1 = Ycord.rTail()[j-1].head().asNumber();

						double x2 = Xcord.rTail()[j].head().asNumber();
						double y2 = Ycord.rTail()[j].head().asNumber();

						double x3 = Xcord.rTail()[j+1].head().asNumber();
						double y3 = Ycord.rTail()[j+1].head().asNumber();

						if (checkline(x1, y1, x2, y2, x3, y3))
						{
							if (nextX.rTail().empty())
							{
								nextX.rTail().emplace_back(Expression(x1));
							}
							else
							{
								if (!(nextX.rTail()[j - 2] == Expression(x1)))
								{
									nextX.rTail().emplace_back(Expression(x1));
								}
								if (j == max - 2)
								{
									nextX.rTail().emplace_back(Expression(x2));
									nextX.rTail().emplace_back(Expression(x3));
								}
							}
						}
						else
						{
							double midx1 = (x2 + x1) / 2;
							double midx2 = (x3 + x2) / 2;
							if (nextX.rTail().empty())
							{
								nextX.rTail().emplace_back(Expression(x1));
								nextX.rTail().emplace_back(Expression(midx1));
								nextX.rTail().emplace_back(Expression(x2));
								nextX.rTail().emplace_back(Expression(midx2));
							}
							else
							{
								if (!(nextX.rTail()[j - 2] == Expression(x1)))
								{
									nextX.rTail().emplace_back(Expression(x1));
									nextX.rTail().emplace_back(Expression(midx1));
									nextX.rTail().emplace_back(Expression(x2));
									nextX.rTail().emplace_back(Expression(midx2));
								}
								else
								{
									nextX.rTail().emplace_back(Expression(x2));
									nextX.rTail().emplace_back(Expression(midx2));
								}

								if (j == max - 2)
								{
									nextX.rTail().emplace_back(Expression(x3));
								}
							}
							
							
						}
					}
					nextX = nextX.eval(env);
					if (nextX.rTail().size() > Xcord.rTail().size())
					{
						Xcord = nextX;
						nextY.rTail().push_back(Xcord);
						Ycord = nextY.eval(env);
					}
					else
					{
						break;
					}
				}

				std::size_t numpoints = Xcord.rTail().size();

				double maxX = -10000;
				double minX = 100000;
				double maxY = -10000;
				double minY = 100000;

				for (size_t i = 0; i < numpoints; i++)
				{
					if (Xcord.rTail()[i].head().asNumber() > maxX)
					{
						maxX = Xcord.rTail()[i].head().asNumber();
					}
					if (Xcord.rTail()[i].head().asNumber() < minX)
					{
						minX = Xcord.rTail()[i].head().asNumber();
					}
				}

				for (size_t i = 0; i < numpoints; i++)
				{
					if (Ycord.rTail()[i].head().asNumber() > maxY)
					{
						maxY = Ycord.rTail()[i].head().asNumber();
					}
					if (Ycord.rTail()[i].head().asNumber() < minY)
					{
						minY = Ycord.rTail()[i].head().asNumber();
					}
				}

				double midX = (maxX + minX) / 2;
				double midY = (maxY + minY) / 2;

				double scaleX = (N / (maxX - minX));
				double scaleY = -1 * (N / (maxY - minY));

				resultb = make_box(env, minX * scaleX, minY * scaleY, maxX * scaleX, maxY * scaleY);

				resultTM = make_pos_labels(env, D, C, scale, minX, maxX, minY, maxY, scaleX, scaleY);

				if (rTail().size() == 3)
				{
					Expression t1(list);
					for (std::size_t i = 0; i < rTail()[2].rTail().size(); i++)
					{
						t1.rTail().push_back(rTail()[2].rTail()[i]);
					}
					Expression titles = t1.eval(env);

					if (titles.isLList())
					{
						//find text scale if applicable;
						for (std::size_t i = 0; i < titles.rTail().size(); i++)
						{
							if (titles.rTail()[i].rTail().size() == 2)
							{
								if (titles.rTail()[i].rTail()[0].head().isString())
								{
									if (titles.rTail()[i].rTail()[0].head().asString() == "\"text-scale\"")
									{
										if (titles.rTail()[i].rTail()[1].isHeadNumber())
										{
											scale = titles.rTail()[i].rTail()[1].head().asNumber();
										}
									}
								}
							}
							else
							{
								//error
							}

						}

						Atom list("list");
						Expression stage4(list);
						for (std::size_t i = 0; i < titles.rTail().size(); i++)
						{
							if (titles.rTail()[i].rTail().size() == 2)
							{

								if (titles.rTail()[i].rTail()[0].head().isString())
								{

									if (titles.rTail()[i].rTail()[0].head().asString() == "\"title\"")
									{
										if (titles.rTail()[i].rTail()[1].head().isString())
										{
											areTitles = true;
											Expression textO = helper_make_text(env, titles.rTail()[i].rTail()[1].head().asString(), midX * scaleX, maxY * scaleY, 0, -A, scale);
											stage4.rTail().push_back(textO);
										}
									}
									else if (titles.rTail()[i].rTail()[0].head().asString() == "\"abscissa-label\"")
									{
										if (titles.rTail()[i].rTail()[1].head().isString())
										{
											areTitles = true;
											Expression textO = helper_make_text(env, titles.rTail()[i].rTail()[1].head().asString(), midX * scaleX, minY * scaleY, 0, A, scale);
											stage4.rTail().push_back(textO);
										}
									}
									else if (titles.rTail()[i].rTail()[0].head().asString() == "\"ordinate-label\"")
									{
										if (titles.rTail()[i].rTail()[1].head().isString())
										{
											areTitles = true;
											Expression textO = helper_make_text(env, titles.rTail()[i].rTail()[1].head().asString(), minX * scaleX, midY * scaleY, -B, 0, scale);
											//Atom rot("\"text-rotation\"");
											//rot.setString();
											//textO.add_prop(rot, Expression(Atom(std::atan2(0, -1) * -1 / 2)));
											stage4.rTail().push_back(textO);
										}
									}
								}
							}
							else
							{
								//error
							}

						}
						if (areTitles)
						{
							resultTitles = stage4.eval(env);
						}
					}
					else
					{
						//error
					}
				}

				double Xaxis = minY*scaleY;
				double Yaxis = minX;
				bool hasXaxis = false;

				Expression stage5(list);
				if (minY < 0 && maxY > 0)
				{
					hasAxes = true;
					hasXaxis = true;
					Xaxis = 0;
					Expression XA = helper_make_line(env, maxX * scaleX, Xaxis, minX * scaleX, Xaxis, 0);
					stage5.rTail().push_back(XA);
				}

				if (minX < 0 && maxX > 0)
				{
					hasAxes = true;
					Yaxis = 0;
					Expression YA = helper_make_line(env, Yaxis, maxY * scaleY, Yaxis, minY * scaleY, 0);
					stage5.rTail().push_back(YA);
				}

				if (hasAxes)
				{
					resultAxes = stage5.eval(env);
				}

				Expression stage6(list);
				for (size_t i = 1; i < numpoints; i++)
				{
					double x1 = Xcord.rTail()[i-1].head().asNumber() * scaleX;
					double y1 = Ycord.rTail()[i-1].head().asNumber() * scaleY;
					double x2 = Xcord.rTail()[i].head().asNumber() * scaleX;
					double y2 = Ycord.rTail()[i].head().asNumber() * scaleY;
					Expression line = helper_make_line(env, x1, y1, x2, y2, 0);
					stage6.rTail().push_back(line);
				}
				resultplot = stage6.eval(env);
			}
			else
			{
				//throw error
			}
	}
	else
	{
		//throw error
	}

	Expression join1(Atom("join"));
	join1.rTail().emplace_back(resultb);
	join1.rTail().emplace_back(resultTM);
	Expression result1 = join1.eval(env);

	//implement join2 here
	if (hasAxes)
	{
		Expression join2(Atom("join"));
		join2.rTail().push_back(result1);
		join2.rTail().emplace_back(resultAxes);
		result1 = join2.eval(env);
	}
	//implement join3 here
	Expression join3(Atom("join"));
	join3.rTail().push_back(result1);
	join3.rTail().emplace_back(resultplot);
	Expression result2 = join3.eval(env);
	if (!areTitles)
	{
		return result2;
	}
	else
	{
		Expression join4(Atom("join"));
		join4.rTail().emplace_back(result2);
		join4.rTail().emplace_back(resultTitles);
		resultf = join4.eval(env);
	}
	return resultf;

}

bool Expression::checkline(const double x1, const double y1, const double x2, const double y2, const double x3, const double y3)
{
	double b = sqrt(((x3 - x2)*(x3 - x2) + (y3 - y2)*(y3 - y2)));
	double a = sqrt(((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)));
	double c = sqrt(((x1 - x3)*(x1 - x3) + (y1 - y3)*(y1 - y3)));
	double nat = ((a*a) + (b*b) - (c*c)) / (2 * a*b);
	if(nat < -1)
	{
		double diff = nat + 1;
		nat = -1 - diff;
	}
	double angle = acos(nat);

	double check1 = 185 * (std::atan2(0, -1) / 180.0);
	double check2 = 175 * (std::atan2(0, -1) / 180.0);
	return (angle > check2 && angle < check1);
}

Expression Expression::make_box(Environment & env, const double minX, const double minY, const double maxX, const double maxY)
{
	Expression resultb;
	Atom list("list");
	
	Expression stage2(list);
	Expression boxu = helper_make_line(env, minX, maxY, maxX, maxY, 0);
	stage2.rTail().push_back(boxu);

	Expression boxd = helper_make_line(env, minX, minY, maxX, minY, 0);
	stage2.rTail().push_back(boxd);

	Expression boxr = helper_make_line(env, maxX, maxY, maxX, minY, 0);
	stage2.rTail().push_back(boxr);

	Expression boxl = helper_make_line(env, minX, minY, minX, maxY, 0);
	stage2.rTail().push_back(boxl);

	resultb = stage2.eval(env);
	return resultb;
}

Expression Expression::make_pos_labels(Environment & env, const double C, const double D, const double scale, const double minX, const double maxX, const double minY, const double maxY, const double Xscale, const double Yscale)
{
	Atom list("list");
	Expression stage2(list);
	std::string textConOU;
	std::stringstream stream;
	stream << "\"" << std::setprecision(2) << maxY << "\"";
	stream >> textConOU;
	Expression OUF = helper_make_text(env, textConOU, minX * Xscale, maxY * Yscale, -D, 0, scale);
	stage2.rTail().push_back(OUF);

	stream.clear();
	std::string textConOL;
	stream << "\"" << std::setprecision(2) << minY << "\"";
	stream >> textConOL;
	Expression OLF = helper_make_text(env, textConOL, minX * Xscale, minY * Yscale, -D, 0, scale);	
	stage2.rTail().push_back(OLF);

	stream.clear();
	std::string textConAL;
	stream << "\"" << std::setprecision(2) << minX << "\"";
	stream >> textConAL;
	Expression ALF = helper_make_text(env, textConAL, minX * Xscale, minY * Yscale, 0, C, scale);
	stage2.rTail().push_back(ALF);

	stream.clear();
	std::string textConAU;
	stream << "\"" << std::setprecision(2) << maxX << "\"";
	stream >> textConAU;
	Expression AUF = helper_make_text(env, textConAU, maxX * Xscale, minY * Yscale, 0, C, scale);
	stage2.rTail().push_back(AUF);
	Expression resultTM = stage2.eval(env);
	return resultTM;
}

Expression Expression::helper_make_text(Environment & env, const std::string cont, const double XN, const double YN, const double X, const double Y, const double scale)
{
	

	Atom text("make-text");
	Atom position("\"position\"");
	position.setString();

	Expression OU(text);

	Atom OUCon(cont);
	OUCon.setString();
	OU.append(OUCon);

	Expression OUF = OU.eval(env);

	Expression OUpos = helper_make_point(env, ((XN)+X), ((YN)+Y), 0);

	Atom scaleT("\"text-scale\"");
	scaleT.setString();

	OUF.add_prop(position, OUpos);
	OUF.add_prop(scaleT, Expression(Atom(scale)));
	return OUF;
}

Expression Expression::helper_make_line(Environment & env, const double x1, const double y1, const double x2, const double y2, const double thickness)
{
	Atom thicknessA("\"thickness\"");
	thicknessA.setString();
	Expression boxu(Atom("make-line"));
	Expression fpu = helper_make_point(env, x1, y1, 0);
	Expression spu = helper_make_point(env, x2, y2, 0);
	boxu.rTail().push_back(fpu);
	boxu.rTail().push_back(spu);
	boxu = boxu.eval(env);

	boxu.add_prop(thicknessA, Expression(thickness));
	return boxu;
}

Expression Expression::helper_make_point(Environment & env, const double x, const double y, const double size)
{
	Atom sizeA("\"size\"");
	sizeA.setString();
	Expression fpu(Atom("make-point"));
	fpu.append(Atom(x));
	fpu.append(Atom(y));
	fpu = fpu.eval(env);
	fpu.add_prop(sizeA, Expression(Atom(size)));
	return fpu;
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
  else if (m_head.isSymbol() && m_head.asSymbol() == "continuous-plot") {
	  return handle_contplot(env);
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
