#include <iostream>
#include <cassert>

#include "Utils.h"
#include "debug/alert.h"

#include "AST.h"
#include "Error.h"
#include "ScriptFileContext.h"
#include "Application.h"

Error& Error::emit(ErrorLevel level)
{
  auto line_num = this->_loc._line_num;

  // レベルによって最初の表示を変える
  switch (level) {
      // エラー
    case EL_Error:
      std::cout << COL_BOLD COL_RED "error: " << COL_WHITE
                << this->_msg;
      break;

      // 警告
    case EL_Warning:
      std::cout << COL_BOLD COL_MAGENTA "warning: " << COL_WHITE
                << this->_msg;
      break;

      // ヒント、ヘルプなど
    case EL_Note:
      std::cout << COL_BOLD COL_CYAN "note: " << COL_WHITE
                << this->_msg;
      break;
  }

  // エラーが起きたファイルと行番号
  std::cerr << std::endl
            << COL_GREEN "    --> " << _RGB(0, 255, 255)
            << this->_pContext->get_path() << ":" << line_num
            << COL_DEFAULT << std::endl;

  // エラーが起きた行
  this->show_error_lines();

  std::cerr << std::endl << std::endl;

  if (level == EL_Error) {
    _count++;
  }

  return *this;
}

std::pair<Token const*, Token const*> Error::get_token_range()
    const
{
  Token const* begin = nullptr;
  Token const* end = nullptr;

  // ソースコード上の位置を取得
  switch (this->_loc.loc_kind) {
    case ERRLOC_AST: {
      begin = &this->_loc.ast->token;
      end = &*this->_loc.ast->end_token;
      break;
    }

    case ERRLOC_Token:
      begin = end = this->_loc.token;
      break;
  }

  assert(begin);
  assert(end);

  return {begin, end};
}

void Error::show_error_lines()
{
  auto [tbegin, tend] = this->get_token_range();

  // line indexes
  auto const begin = tbegin->src_loc.line_num - 1;
  auto const end = tend->src_loc.line_num - 1;

  auto const& src_data = this->_pContext->_srcdata;

  std::vector<std::string> lines;

  if (this->_is_single_line || begin == end) {
    lines.emplace_back(src_data._lines[begin].str_view);
  }
  else {
    for (auto i = begin; i <= end; i++)
      lines.emplace_back(src_data._lines[i].str_view);

    lines.begin()->insert(
        tbegin->src_loc.position - src_data._lines[begin].begin,
        "\033[4m");

    lines.rbegin()->insert(
        tend->src_loc.get_end_pos() - src_data._lines[end].begin,
        COL_DEFAULT);

    lines.rbegin()->insert(0, "\033[4m");
  }

  std::cerr << "     |" << std::endl;

  for (auto line_num = this->_loc._line_num;
       auto&& line : lines) {
    std::cerr << COL_DEFAULT
              << Utils::format("%4d | ", line_num++) << line
              << std::endl;
  }

  std::cerr << "     | ";

  if (lines.size() == 1) {
    std::cerr << std::string(tbegin->src_loc.position -
                                 src_data._lines[begin].begin,
                             ' ')
              << std::string(std::max<size_t>(
                                 tend->src_loc.get_end_pos() -
                                     tbegin->src_loc.position,
                                 1),
                             '^');
  }
}
