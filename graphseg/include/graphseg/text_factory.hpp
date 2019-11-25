#ifndef GRAPHSEG_CPP_GRAPHSEG_TEXT_FACTORY_HPP
#define GRAPHSEG_CPP_GRAPHSEG_TEXT_FACTORY_HPP

#include "graphseg/internal/utils/string.hpp"
#include "graphseg/language.hpp"
#include "graphseg/sentence.hpp"
#include "graphseg/text.hpp"

#include <algorithm>
#include <fstream>
#include <locale>
#include <sstream>
#include <vector>

#include <mecab.h>

namespace GraphSeg {
std::wstring ReadTextFile(const std::string &path, std::string current_locale) {
  std::wifstream wif(path);
  wif.imbue(std::locale(current_locale));
  std::wcout.imbue(std::locale(current_locale));
  std::wstringstream wss;
  wss << wif.rdbuf();
  return wss.str();
}

template <Lang LangType = Lang::EN>
class TextFactory : public Language<LangType> {
  using Base = Language<LangType>;

public:
  static std::vector<Text<LangType>>
  Execute(const std::vector<std::string> &paths) {
    std::vector<Text<LangType>> articles;
    for (auto &&path : paths) {
      auto text = ReadTextFile(path, Base::Locale());
      articles.emplace_back(Text<LangType>(text, path));
    }
    return articles;
  }

  static Text<LangType> Execute(const std::string &path) {
    auto text = ReadTextFile(path, Base::Locale());
    return Text<LangType>(text, path);
  }
};
} // namespace GraphSeg

#endif