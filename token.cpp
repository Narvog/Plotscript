#include "token.hpp"
#include "semantic_error.hpp"
// system includes
#include <cctype>
#include <iostream>

// define constants for special characters
const char OPENCHAR = '(';
const char CLOSECHAR = ')';
const char COMMENTCHAR = ';';
const char QUOTECHAR = '"';

Token::Token(TokenType t): m_type(t), isStringL(false) {}

Token::Token(const std::string & str): m_type(STRING), value(str), isStringL(false) {}

Token::Token(const Token & Tok)
{
	m_type = Tok.m_type;
	value = Tok.value;
	isStringL = Tok.isStringL;
}

Token::TokenType Token::type() const{
  return m_type;
}

std::string Token::asString() const{
  switch(m_type){
  case OPEN:
    return "(";
  case CLOSE:
    return ")";
  case STRING:
    return value;
  case STRINGQs:
	return "\"";
  case STRINGQe:
	  return "\"";
  }
  return "";
}

void Token::setisStringL(bool Q)
{
	isStringL = Q;
}

bool Token::getisStringL() const
{
	return isStringL;
}


// add token to sequence unless it is empty, clears token
void store_ifnot_empty(std::string & token, TokenSequenceType & seq){
  if(!token.empty()){
    seq.emplace_back(token);
    token.clear();
  }
}

TokenSequenceType tokenize(std::istream & seq){
  TokenSequenceType tokens;
  std::string token;
  bool inQuotes = false;
  while(true){
    char c = seq.get();

	if (seq.eof() && !inQuotes)
	{
		break;
	}
	else if (seq.eof() && inQuotes)
	{
		throw SemanticError("error unmatched \"");
	}
    
    if(c == COMMENTCHAR && !inQuotes){
      // chomp until the end of the line
      while((!seq.eof()) && (c != '\n')){
	c = seq.get();
      }
      if(seq.eof()) break;
    }
    else if(c == OPENCHAR && !inQuotes){
      store_ifnot_empty(token, tokens);
      tokens.push_back(Token::TokenType::OPEN);
    }
    else if(c == CLOSECHAR && !inQuotes){
      store_ifnot_empty(token, tokens);
      tokens.push_back(Token::TokenType::CLOSE);
    }
	else if (c == QUOTECHAR)
	{
		if (inQuotes)
		{
			inQuotes = false;
			token.push_back(c);
			store_ifnot_empty(token, tokens);
			Token temp = tokens.back();
			temp.setisStringL(true);
			tokens.pop_back();
			tokens.emplace_back(temp);
			tokens.push_back(Token::TokenType::STRINGQe);
		}
		else
		{
			inQuotes = true;
			store_ifnot_empty(token, tokens);
			tokens.push_back(Token::TokenType::STRINGQs);
			token.push_back(c);
		}
	}
    else if(isspace(c) && !inQuotes){
      store_ifnot_empty(token, tokens);
    }
    else{
      token.push_back(c);
    }
  }
  store_ifnot_empty(token, tokens);

  return tokens;
}
