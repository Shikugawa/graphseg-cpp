#ifndef GRAPHSEG_CPP_GRAPHSEG_TEXT_STREAM_HPP
#define GRAPHSEG_CPP_GRAPHSEG_TEXT_STREAM_HPP

#include "graphseg/lang.hpp"

#include <fstream>
#include <sstream>
#include <locale>

namespace GraphSeg
{
  using std::wstring, std::string, std::wifstream, std::locale, std::wstringstream;

  template <Lang LangType = Lang::EN>
  wstring TextTransform(const string& path)
  {
    auto localeStr = Locale<LangType>();
    wifstream wif(path);
    wif.imbue(locale(localeStr));
    std::wcout.imbue(locale(localeStr));
    wstringstream wss;
    wss << wif.rdbuf();
    return wss.str();
  }
}

#endif