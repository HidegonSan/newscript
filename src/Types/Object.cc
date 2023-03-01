#include <cassert>
#include <iostream>

#include "common.h"

#include "Token.h"
#include "Object.h"

#include "debug/alert.h"

// --------------------------------------------------------
//  Object
// --------------------------------------------------------

static bool nested=0;

ObjNone* Object::obj_none;

Object::Object(TypeInfo type)
  : type(type),
    ref_count(0)
{
  alert_ctor;
}

Object::~Object() {
  alert_dtor;
}

void Object::initialize() {


}

std::string ObjNone::to_string() const {
  return "none";
}

std::string ObjLong::to_string() const {
  return std::to_string(this->value);
}

std::string ObjFloat::to_string() const {
  // 処理まったく上と同じだからどうにかしろ
  return std::to_string(this->value);
}

std::string ObjString::to_string() const {
  if(nested)
    return '"'+
    Utils::String::to_str(this->value);
      +'"';

  return Utils::String::to_str(this->value);
}

std::string ObjDict::to_string() const {
  std::string s = "{ ";

  auto nss = nested;
  nested=1;

  for(auto&&x:this->items){
    s+=x.key->to_string()+": "+x.value->to_string();
    if(&x!=&*this->items.rbegin())
      s+=", ";
  }

  nested=nss;

  return s + "}";
}

ObjNone* ObjNone::clone() const {
  return new ObjNone; // ???
}

ObjLong* ObjLong::clone() const {
  return new ObjLong(this->value);
}

ObjFloat* ObjFloat::clone() const {
  return new ObjFloat(this->value);
}

ObjString* ObjString::clone() const {
  return new ObjString(this->value);
}

ObjDict* ObjDict::clone() const {
  auto ret = new ObjDict;
  
  for( auto&& item : this->items ) {
    ret->items.emplace_back(
      item.key->clone(),
      item.value->clone()
    );
  }

  return ret;
}

