#include <cassert>
#include "metro.h"

Evaluator::Evaluator(GarbageCollector const& gc)
  : gc(gc)
{
}

Evaluator::~Evaluator() {
  
}

/**
 * @brief 即値・リテラルの AST からオブジェクトを作成する
 * 
 * @note すでに作成済みのものであれば、既存のものを返す
 * 
 * @param ast 
 * @return 作成されたオブジェクト (Object*)
 */
Object* Evaluator::create_object(AST::Value* ast) {
  auto type = Checker::value_type_cache[ast];

  auto& obj = this->immediate_objects[ast];

  if( obj )
    return obj;

  switch( type.kind ) {
    case TYPE_Int:
      obj = new ObjLong(std::stoi(ast->token.str.data()));
      break;

    case TYPE_String:
      obj = new ObjString(Utils::String::to_wstr(
        std::string(ast->token.str)
      ));

      break;

    default:
      debug(
        std::cout << type.to_string() << std::endl
      );

      todo_impl;
  }

  assert(obj != nullptr);

  return obj;
}

Evaluator::FunctionStack& Evaluator::enter_function(AST::Function* func) {
  auto& stack = this->call_stack.emplace_back(func);

  return stack;
}

void Evaluator::leave_function(AST::Function* func) {
  auto& stack = *this->call_stack.rbegin();

  debug(assert(stack.ast == func));

  this->call_stack.pop_back();
}

Evaluator::FunctionStack& Evaluator::get_current_func_stack() {
  return *this->call_stack.rbegin();
}

Object* Evaluator::evaluate(AST::Base* _ast) {
  if( !_ast )
    return new ObjNone;

  switch( _ast->kind ) {
    case AST_None:
    case AST_Function:
      break;

    case AST_Value: {
      return Evaluator::create_object((AST::Value*)_ast);
    }

    //
    // 関数呼び出し
    case AST_CallFunc: {
      auto ast = (AST::CallFunc*)_ast;

      std::vector<Object*> args;

      // 引数
      for( auto&& arg : ast->args ) {
        args.emplace_back(this->evaluate(arg));
      }

      // 組み込み関数
      if( ast->is_builtin ) {
        return ast->builtin_func->impl(args);
      }

      // ユーザー定義関数
      auto func = ast->callee;

      // コールスタック作成
      this->enter_function(func);

      // 引数をスタックに追加
      for( auto&& obj : args ) {
        this->push_object(obj);
      }

      // 関数実行
      this->evaluate(func->code);

      // 戻り値を取得
      auto result =
        this->get_current_func_stack().result;

      // コールスタック削除
      this->leave_function(func);

      // スタックからオブジェクトを削除
      this->pop_object_with_count(args.size());

      // 戻り値を返す
      return result;
    }

    //
    // 式
    case AST_Expr: {
      auto x = (AST::Expr*)_ast;

      auto ret = this->evaluate(x->first);

      for( auto&& elem : x->elements ) {
        auto item = this->evaluate(elem.ast);

        switch( elem.kind ) {
          case AST::Expr::EX_Add:
            ret = Evaluator::add_object(ret, item);
            break;
          
          case AST::Expr::EX_Sub:
            ret = Evaluator::sub_object(ret, item);
            break;

          case AST::Expr::EX_Mul:
            ret = Evaluator::mul_object(ret, item);
            break;

          case AST::Expr::EX_Mul:
            ret = Evaluator::mul_object(ret, item);
            break;
        }
      }

      return ret;
    }

    // scope
    case AST_Scope: {
      auto ast = (AST::Scope*)_ast;

      for( auto&& item : ast->list ) {
        this->evaluate(item);
      }

      break;
    }

    case AST_Let: {
      auto ast = (AST::VariableDeclaration*)_ast;

      if( ast->init ) {
        
      }


      break;
    }

    case AST_Return: {
      auto ast = (AST::Return*)_ast;

      auto& func_stack = this->get_current_func_stack();

      func_stack.result = this->evaluate(ast->expr);

      break;
    }

    default:
      todo_impl;
  }

  return new ObjNone;
}

Object* Evaluator::add_object(Object* left, Object* right) {
  // todo: if left is vector

  auto ret = left->clone();

  switch( left->type.kind ) {
    case TYPE_Int:
      ((ObjLong*)ret)->value += ((ObjLong*)right)->value;
      break;

    default:
      todo_impl;
  }

  return ret;
}

Object* Evaluator::sub_object(Object* left, Object* right) {
  auto ret = left->clone();

  switch( left->type.kind ) {
    case TYPE_Int:
      ((ObjLong*)ret)->value -= ((ObjLong*)right)->value;
      break;

    default:
      todo_impl;
  }

  return ret;
}

Object*& Evaluator::push_object(Object* obj) {
  return this->object_stack.emplace_back(obj);
}

Object* Evaluator::pop_object() {
  auto obj = *this->object_stack.rbegin();

  this->object_stack.pop_back();

  return obj;
}

void Evaluator::pop_object_with_count(size_t count) {
  for( size_t i = 0; i < count; i++ ) {
    this->pop_object();
  }
}
