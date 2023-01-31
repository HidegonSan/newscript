#pragma once

#include <string>
#include <vector>
#include <list>
#include <functional>

enum TokenKind {
  TOK_Int,
  TOK_Float,
  TOK_Char,
  TOK_String,
  TOK_Ident,
  TOK_Punctuater,
  TOK_End
};

struct Token {
  TokenKind kind;
  
  std::string_view str;

  

  explicit Token(TokenKind kind)
    : kind(TOK_Int)
  {
  }
};

enum ASTKind {
  AST_Value,
  AST_Variable,

  AST_Add,
  AST_Sub,

  AST_Function
};

namespace AST {

struct Base {
  ASTKind kind;
  Token const& token;

  virtual ~Base() { }

protected:
  Base(ASTKind kind, Token const& token)
    : kind(kind),
      token(token)
  {
  }
};

}

class Lexer {
public:
  Lexer(std::string const& source);
  ~Lexer();

  std::list<Token> lex();


private:
  bool check();
  char peek();
  size_t pass_while(std::function<bool(char)> cond);
  
  size_t pass_space() {
    return this->pass_while([] (char c) { return isspace(c); });
  }

  std::string const& source;
  size_t position;
};

class Parser {
public:
  Parser(std::list<Token> const& token_list);
  ~Parser();


private:



};

