#include "Utils.h"
#include "debug/alert.h"

#include "AST.h"
#include "Parser.h"
#include "Error.h"

/**
 * @brief ステートメント
 *
 * @return AST::Base*
 */
AST::Base* Parser::stmt()
{
  //
  // Dict or Scope
  if (this->cur->str == "{")
    return this->parse_scope();

  //
  // if 文
  if (this->eat("if")) {
    auto ast = new AST::If(*this->ate);

    ast->condition = this->expr();

    ast->if_true = this->expect_scope();

    if (this->eat("else")) {
      if (this->cur->str == "if")
        ast->if_false = this->stmt();
      else
        ast->if_false = this->expect_scope();
    }

    return this->set_last_token(ast);
  }

  //
  // switch
  if (this->eat("switch")) {
    auto ast = new AST::Switch(*this->ate);

    ast->expr = this->expr();

    this->expect("{");

    while (this->eat("case")) {
      auto x = new AST::Case(*this->ate);

      x->cond = this->expr();

      this->expect(":");
      x->scope = this->expect_scope();

      x->end_token = x->scope->end_token;

      ast->cases.emplace_back(x);
    }

    ast->end_token = this->expect("}");

    if (ast->cases.empty()) {
      Error(ERR_EmptySwitch, ast->token,
            "empty switch-statement is not valid")
          .emit()
          .exit();
    }

    return ast;
  }

  //
  // loop
  if (this->eat("loop")) {
    return this->set_last_token(
        new AST::Loop(this->expect_scope()));
  }

  //
  // for
  if (this->eat("for")) {
    auto ast = new AST::For(*this->ate);

    ast->iter = this->expr();

    this->expect("in");
    ast->iterable = this->expr();

    ast->code = this->expect_scope();

    return this->set_last_token(ast);
  }

  //
  // while
  if (this->eat("while")) {
    auto ast = new AST::While(*this->ate);

    ast->cond = this->expr();
    ast->code = this->expect_scope();

    return this->set_last_token(ast);
  }

  //
  // do-while
  if (this->eat("do")) {
    auto ast = new AST::DoWhile(*this->ate);

    ast->code = this->expect_scope();

    this->expect("while");
    ast->cond = this->expr();

    this->expect_semi();

    return this->set_last_token(ast);
  }

  //
  // let 変数定義
  if (this->eat("let")) {
    auto ast = new AST::VariableDeclaration(*this->ate);

    ast->name = this->expect_identifier()->str;

    if (this->eat(":")) {
      ast->type = this->parse_typename();
    }

    if (this->eat("=")) {
      ast->init = this->expr();
    }

    // ast->end_token = this->expect_semi();

    return this->set_last_token(ast);
  }

  //
  // return 文
  if (this->eat("return")) {
    auto ast = new AST::Return(*this->ate);

    if (this->eat_semi())
      return ast;

    ast->expr = this->expr();
    // ast->end_token = this->expect_semi();

    return this->set_last_token(ast);
  }

  // break
  if (this->eat("break")) {
    auto ast = new AST::LoopController(*this->ate, AST_Break);

    // ast->end_token = this->expect_semi();

    return this->set_last_token(ast);
  }

  // continue
  if (this->eat("continue")) {
    auto ast = new AST::LoopController(*this->ate, AST_Continue);

    // ast->end_token = this->expect_semi();

    return this->set_last_token(ast);
  }

  return nullptr;
}
