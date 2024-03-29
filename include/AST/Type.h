#pragma once

namespace AST {

struct Type : Base {
  std::string_view name;
  std::vector<Type*> parameters;
  bool is_const;

  Type(Token const& token)
      : Base(AST_Type, token),
        name(token.str),
        is_const(false)
  {
  }

  ~Type()
  {
    for (auto&& p : this->parameters)
      delete p;
  }
};

}  // namespace AST