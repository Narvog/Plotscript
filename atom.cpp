#include "atom.hpp"
#include "semantic_error.hpp"

#include <sstream>
#include <cctype>
#include <cmath>
#include <limits>

Atom::Atom(): m_type(NoneKind) {}

Atom::Atom(double value){

  setNumber(value);
}

Atom::Atom(complex<double> value) {

  setComplex(value);
}

Atom::Atom(const Token & token): Atom(){
  
  // is token a number?
  double temp;
  std::istringstream iss(token.asString());
  if(iss >> temp){
    // check for trailing characters if >> succeeds
    if(iss.rdbuf()->in_avail() == 0){
      setNumber(temp);
    }
  }

  else{ // else assume symbol
    // make sure does not start with number
    if(!std::isdigit(token.asString()[0])){
		if (!token.getisStringL())
		{
			setSymbol(token.asString());
		}
		else
		{
			setString(token.asString());
		}
    }
  }
}

Atom::Atom(const std::string & value): Atom() {
  
  setSymbol(value);
}

//complex added to the copy constructor
Atom::Atom(const Atom & x): Atom(){
  if(x.isNumber()){
	  if (m_type == SymbolKind || m_type == StringKind) {
		  stringValue.~basic_string();
	  }
    setNumber(x.numberValue);
  }
  else if(x.isSymbol()){
    setSymbol(x.stringValue);
  }
  else if (x.isComplex()) {
	  if (m_type == SymbolKind || m_type == StringKind) {
		  stringValue.~basic_string();
	  }
	setComplex(x.complexValue);
  }
  else if (x.m_type == StringKind) {
	  setString(x.stringValue);
  }
}

//complex added to the assignment opperator
Atom & Atom::operator=(const Atom & x){

  if(this != &x){
    if(x.m_type == NoneKind){
		if (m_type == SymbolKind || m_type == StringKind) {
			stringValue.~basic_string();
		}
      m_type = NoneKind;
    }
    else if(x.m_type == NumberKind){
		if (m_type == SymbolKind || m_type == StringKind) {
			stringValue.~basic_string();
		}
      setNumber(x.numberValue);
    }
    else if(x.m_type == SymbolKind){
      setSymbol(x.stringValue);
    }
	else if (x.m_type == ComplexKind) {
		if (m_type == SymbolKind || m_type == StringKind) {
			stringValue.~basic_string();
		}
		setComplex(x.complexValue);
	}
	else if (x.m_type == StringKind) {
		setString(x.stringValue);
	}
  }
  return *this;
}
  
Atom::~Atom(){

  // we need to ensure the destructor of the symbol string is called
  if(m_type == SymbolKind || m_type == StringKind){
    stringValue.~basic_string();
  }
}

bool Atom::isNone() const noexcept{
  return m_type == NoneKind;
}

bool Atom::isNumber() const noexcept{
  return m_type == NumberKind;
}

bool Atom::isSymbol() const noexcept{
  return m_type == SymbolKind;
} 

bool Atom::isComplex() const noexcept {
	return m_type == ComplexKind;
}

bool Atom::isString() const noexcept {
	return m_type == StringKind;
}


void Atom::setNumber(double value){

  m_type = NumberKind;
  numberValue = value;
}

void Atom::setSymbol(const std::string & value){

  // we need to ensure the destructor of the symbol string is called
  if(m_type == SymbolKind || m_type == StringKind){
    stringValue.~basic_string();
  }
    
  m_type = SymbolKind;

  // copy construct in place
  new (&stringValue) std::string(value);
}

void Atom::setComplex(const complex<double> value) {
	m_type = ComplexKind;
	complexValue = value;
}

void Atom::setString(const std::string & value) {

	// we need to ensure the destructor of the symbol string is called
	setSymbol(value);

	m_type = StringKind;

	// copy construct in place
}

double Atom::asNumber() const noexcept{

  return (m_type == NumberKind) ? numberValue : 0.0;  
}


std::string Atom::asSymbol() const noexcept{

  std::string result;

  if(m_type == SymbolKind){
    result = stringValue;
  }

  return result;
}

complex<double> Atom::asComplex() const noexcept {

	return (m_type == ComplexKind) ? complexValue : complex<double> (0.0, 0.0);
}

std::string Atom::asString() const noexcept {

	std::string result;

	if (m_type == StringKind) {
		result = stringValue;
	}

	return result;
}

void Atom::setString()
{
	m_type = StringKind;
}

bool Atom::operator==(const Atom & right) const noexcept{
  
  if(m_type != right.m_type) return false;

  switch(m_type){
  case NoneKind:
    if(right.m_type != NoneKind) return false;
    break;
  case NumberKind:
    {
      if(right.m_type != NumberKind) return false;
      double dleft = numberValue;
      double dright = right.numberValue;
      double diff = fabs(dleft - dright);
      if(std::isnan(diff) ||
	 (diff > std::numeric_limits<double>::epsilon()*250)) return false;
    }
    break;
  case SymbolKind:
    {
      if(right.m_type != SymbolKind) return false;

      return stringValue == right.stringValue;
    }
    break;
	//added the case for ComplexKind
  case ComplexKind:
  {
	  if (right.m_type != ComplexKind) return false;
	  double dleftr = complexValue.real();
	  double dlefti = complexValue.imag();
	  double drightr = right.complexValue.real();
	  double drighti = right.complexValue.imag();
	  double diffr = fabs(dleftr - drightr);
	  double diffi = fabs(dlefti - drighti);
	  if (std::isnan(diffr) || (diffr > std::numeric_limits<double>::epsilon()) || std::isnan(diffi) || (diffi > std::numeric_limits<double>::epsilon()))
	  {
		  return false;
	  }
	  
  }
  break;
  case StringKind:
  {
	  if (right.m_type != StringKind) return false;

	  return stringValue == right.stringValue;
  }
  break;
  default:
    return false;
  }

  return true;
}

bool operator!=(const Atom & left, const Atom & right) noexcept{
  
  return !(left == right);
}


std::ostream & operator<<(std::ostream & out, const Atom & a){

  if (a.isNone()) {
	  out << "NONE";
  }
  if(a.isNumber()){
    out << a.asNumber();
  }
  if(a.isSymbol()){
    out << a.asSymbol();
  }
  //added the case for complex
  if (a.isComplex()) {
	  out << a.asComplex();
  }
  if (a.isString()) {
	  out << a.asString();
  }

  return out;
}



