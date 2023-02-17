#pragma once

#include <string>

class Application {


public:
  Application();
  ~Application();

  int main(int argc, char** argv);

  std::string const& get_source_code();

  static void initialize(); // これいるのか？

  static Application& get_current_instance();

private:


  std::string source_code;

};
