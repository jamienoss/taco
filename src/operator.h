#ifndef TACO_OPERATOR_H
#define TACO_OPERATOR_H

#include <iostream>
#include <string>

#include "expr.h"
#include "error.h"
#include "internal_tensor.h"
#include "util/strings.h"

namespace taco {

namespace internal {

template <typename T>
std::vector<Expr> mergeOperands(const Expr&, const Expr&);

}

struct NaryExpr;
struct Add;
struct Sub;
struct Mul;
struct Div;

struct Read;

class ReadNode : public internal::TENode {
  friend struct Read;

  ReadNode(internal::Tensor tensor, const std::vector<Var>& indices) :
      tensor(tensor), indices(indices) {}

  virtual void print(std::ostream& os) const {
    os << tensor.getName() << "(" << util::join(indices) << ")";
  }

  internal::Tensor tensor;
  std::vector<Var> indices;
};

struct Read : public Expr {
  typedef ReadNode Node;

  Read() = default;
  Read(const Node* n) : Expr(n) {}
  Read(internal::Tensor tensor, const std::vector<Var>& indices) :
      Read(new Node(tensor, indices)) {}

  const Node* getPtr() const {
    return static_cast<const Node*>(Read::ptr);
  }

  internal::Tensor getTensor() const {
    return getPtr()->tensor;
  }

  const std::vector<Var>& getIndexVars() const {
    return getPtr()->indices;
  }

  void operator=(const Expr& source) {
    assign(source);
  }
  
  void operator=(const Read& source) {
    assign(source);
  }

private:
  void assign(Expr);
};

class NaryExprNode : public internal::TENode {
  friend struct NaryExpr;

  template <typename T>
  friend std::vector<Expr> mergeOperands(const Expr&, const Expr&);

  // Syntactic sugar for arithmetic operations.
  friend Add operator+(const Expr&, const Expr&);
  friend Mul operator*(const Expr&, const Expr&);

protected:
  NaryExprNode(const std::vector<Expr>& operands) : operands(operands) {}

  void printNary(std::ostream& os, const std::string& op) const {
    os << util::join(operands, op);
  }

  std::vector<Expr> operands;
};

struct NaryExpr : public Expr {
  typedef NaryExprNode Node;

  NaryExpr() = default;
  NaryExpr(const Node* n) : Expr(n) {}
  NaryExpr(const std::vector<Expr>& operands) : 
      NaryExpr(new NaryExprNode(operands)) {}

  const Node* getPtr() const { return static_cast<const Node*>(Expr::ptr); }

  // Retrieve specified operand (casted to type E).
  template <typename E = Expr>
  E getOperand(size_t idx) const { return to<E>(getPtr()->operands[idx]); }
};

class BinaryExprNode : public internal::TENode {
  friend struct BinaryExpr;

  // Syntactic sugar for arithmetic operations.
  friend Sub operator-(const Expr&, const Expr&);
  friend Div operator/(const Expr&, const Expr&);

protected:
  BinaryExprNode(Expr lhs, Expr rhs) : lhs(lhs), rhs(rhs) {}

  void printBinary(std::ostream& os, const std::string& op) const {
    os << lhs << op << rhs;
  }

  Expr lhs;
  Expr rhs;
};

struct BinaryExpr : public Expr {
  typedef BinaryExprNode Node;

  BinaryExpr() = default;
  BinaryExpr(const Node* n) : Expr(n) {}
  BinaryExpr(Expr lhs, Expr rhs) : BinaryExpr(new BinaryExprNode(lhs, rhs)) {}

  const Node* getPtr() const {
    return static_cast<const Node*>(Expr::ptr);
  }

  // Retrieve left operand (casted to type E).
  template <typename E = Expr>
  E getLhs() const {
    return to<E>(getPtr()->lhs);
  }

  // Retrieve right operand (casted to type E).
  template <typename E = Expr>
  E getRhs() const {
    return to<E>(getPtr()->rhs);
  }
};

class AddNode : public NaryExprNode {
  friend struct Add;

  AddNode(const std::vector<Expr>& operands) : NaryExprNode(operands) {}
  
  virtual void print(std::ostream& os) const {
    printNary(os, " + ");
  }
};

struct Add : public NaryExpr {
  typedef AddNode Node;

  Add() = default;
  Add(const Node* n) : NaryExpr(n) {}
  Add(const std::vector<Expr>& operands) : Add(new AddNode(operands)) {}
};

class SubNode : public BinaryExprNode {
  friend struct Sub;

  SubNode(Expr lhs, Expr rhs) : BinaryExprNode(lhs, rhs) {}
  
  virtual void print(std::ostream& os) const {
    printBinary(os, " - ");
  }
};

struct Sub : public BinaryExpr {
  typedef SubNode Node;

  Sub() = default;
  Sub(const Node* n) : BinaryExpr(n) {}
  Sub(Expr lhs, Expr rhs) : Sub(new SubNode(lhs, rhs)) {}
};

class MulNode : public NaryExprNode {
  friend struct Mul;

  MulNode(const std::vector<Expr>& operands) : NaryExprNode(operands) {}
  
  virtual void print(std::ostream& os) const {
    printNary(os, " * ");
  }
};

struct Mul : public NaryExpr {
  typedef MulNode Node;

  Mul() = default;
  Mul(const Node* n) : NaryExpr(n) {}
  Mul(const std::vector<Expr>& operands) : Mul(new MulNode(operands)) {}
};

class DivNode : public BinaryExprNode {
  friend struct Div;

  DivNode(Expr lhs, Expr rhs) : BinaryExprNode(lhs, rhs) {}
  
  virtual void print(std::ostream& os) const {
    printBinary(os, " / ");
  }
};

struct Div : public BinaryExpr {
  typedef DivNode Node;

  Div() = default;
  Div(const Node* n) : BinaryExpr(n) {}
  Div(Expr lhs, Expr rhs) : Div(new DivNode(lhs, rhs)) {}
};

Add operator+(const Expr&, const Expr&);
Sub operator-(const Expr&, const Expr&);
Mul operator*(const Expr&, const Expr&);
Div operator/(const Expr&, const Expr&);

}

#endif