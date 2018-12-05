/*! \file expression.hpp
Defines the Expression type and assiciated functions.
 */



#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <vector>

#include "token.hpp"
#include "atom.hpp"


#include <atomic>

static std::atomic_bool interupt = false;

// forward declare Environment
class Environment;

/*! \class Expression
\brief An expression is a tree of Atoms.

An expression is an atom called the head followed by a (possibly empty) 
list of expressions called the tail.
 */
class Expression {
public:

  typedef std::vector<Expression>::const_iterator ConstIteratorType;

  /// Default construct and Expression, whose type in NoneType
  Expression();

  /*! Construct an Expression with given Atom as head an empty tail
    \param atom the atom to make the head
  */
  Expression(const Atom & a);

  /// deep-copy construct an expression (recursive)
  Expression(const Expression & a);

  /// deep-copy assign an expression  (recursive)
  Expression & operator=(const Expression & a);

  /// return a reference to the head Atom
  Atom & head();

  /// return a const-reference to the head Atom
  const Atom & head() const;

  std::vector<Expression>& rTail();

  /// return a const-reference to the head Atom
  const std::vector<Expression>& rTail() const;

  /// append Atom to tail of the expression
  void append(const Atom & a);

  /// return a pointer to the last expression in the tail, or nullptr
  Expression * tail();

  /// return a const-iterator to the beginning of tail
  ConstIteratorType tailConstBegin() const noexcept;

  /// return a const-iterator to the tail end
  ConstIteratorType tailConstEnd() const noexcept;

  /// convienience member to determine if head atom is a number
  bool isHeadNumber() const noexcept;

  /// convienience member to determine if head atom is a symbol
  bool isHeadSymbol() const noexcept;

  /// convienience member to determine if head atom is a complex
  bool isHeadComplex() const noexcept;

  bool isLList() const noexcept;

  void setLList(bool set);

  bool isLLambda() const noexcept;

  void setLLambda(bool set);

  /// Evaluate expression using a post-order traversal (recursive)
  Expression eval(Environment & env);

  /// equality comparison for two expressions (recursive)
  bool operator==(const Expression & exp) const noexcept;
  
  bool inLambda = false;
  void helperinL(Expression & oper);

  bool is_prop(const Atom &key) const;
  Expression get_prop(const Atom &key) const;
  void add_prop(const Atom &key, const Expression &prop);
  

  void setError();
  bool isError();

private:
  // the head of the expression
  Atom m_head;
  std::map<std::string, Expression> propMap;
  
  bool error = false;
  bool isList = false;
  bool islambda = false;
  bool islambdaexp = false;
  // the tail list is expressed as a vector for access efficiency
  // and cache coherence, at the cost of wasted memory.
  std::vector<Expression> m_tail;

  // convenience typedef
  typedef std::vector<Expression>::iterator IteratorType;
  
  // internal helper methods
  Expression handle_lookup(const Atom & head, const Environment & env);
  Expression handle_define(Environment & env);
  Expression handle_begin(Environment & env);
  Expression handle_list(Environment & env);

  Expression handle_lambda();
  Expression handle_lambda_lookup(const Atom & head, Environment & env);
  

  Expression handle_apply(Environment & env);
  Expression handle_map(Environment & env);
  Expression handle_setprop(Environment & env);
  Expression handle_getprop(Environment & env);
  Expression handle_discplot(Environment & env);
  Expression handle_contplot(Environment & env);

  bool checkline(const double x1, const double y1, const double x2, const double y2, const double x3, const double y3);

  Expression make_box(Environment & env, const double minX, const double minY, const double maxX, const double maxY);
  Expression make_pos_labels(Environment & env, const double C, const double D, const double scale, const double minX, const double maxX, const double minY, const double maxY, const double Xscale, const double Yscale);
  Expression helper_make_text(Environment & env, const std::string cont, const double XN, const double YN, const double X, const double Y, const double scale);
  Expression helper_make_line(Environment & env, const double x1, const double y1, const double x2, const double y2, const double thickness);
  Expression helper_make_point(Environment & env, const double x, const double y, const double size);
};

/// Render expression to output stream
std::ostream & operator<<(std::ostream & out, const Expression & exp);

/// inequality comparison for two expressions (recursive)
bool operator!=(const Expression & left, const Expression & right) noexcept;
  
#endif
