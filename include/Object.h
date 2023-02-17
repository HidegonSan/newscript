#pragma once

#include "TypeInfo.h"

// ---------------------------------------------
//  Object
// ---------------------------------------------

class Application;
struct ObjNone;
struct Object {
  TypeInfo type;
  size_t ref_count;

  static ObjNone* obj_none;

  static void initialize();

  virtual std::string to_string() const = 0;

  virtual Object* clone() const = 0;

  virtual bool is_numeric() const {
    return false;
  }

  virtual ~Object();

protected:
  Object(TypeInfo type);

  friend class Application;
};

struct ObjNone : Object {
  std::string to_string() const;
  ObjNone* clone() const;

  explicit ObjNone()
    : Object(TYPE_None)
  {
  }
};

struct ObjLong : Object {
  int64_t value;

  std::string to_string() const;
  ObjLong* clone() const;

  bool is_numeric() const {
    return true;
  }

  explicit ObjLong(int64_t value)
    : Object(TYPE_Int),
      value(value)
  {
  }
};

struct ObjFloat : Object {
  float value;

  std::string to_string() const;
  ObjFloat* clone() const;

  bool is_numeric() const {
    return true;
  }

  explicit ObjFloat(float value)
    : Object(TYPE_Float),
      value(value)
  {
  }
};

struct ObjBool : Object {
  bool value;

  std::string to_string() const {
    return value ? "true" : "false";
  }

  ObjBool* clone() const {
    return new ObjBool(this->value);
  }

  explicit ObjBool(bool value)
    : Object(TYPE_Bool),
      value(value)
  {
  }
};

struct ObjString : Object {
  std::wstring value;

  std::string to_string() const;
  ObjString* clone() const;

  explicit ObjString(std::wstring value = L"")
    : Object(TYPE_String),
      value(value)
  {
  }
};