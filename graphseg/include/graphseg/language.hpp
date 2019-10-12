#ifndef GRAPHSEG_CPP_GRAPHSEG_LANG_HPP
#define GRAPHSEG_CPP_GRAPHSEG_LANG_HPP

#include "graphseg/internal/utils/mecab_helper.hpp"

#include <iostream>
#include <type_traits>
#include <functional>
#include <string>
#include <mecab.h>

namespace GraphSeg
{
  enum class Lang
  {
    EN,
    JP
  };

  template <Lang LangType = Lang::EN>
  class Language
  {
  protected:
    std::string Locale() const
    {
      if constexpr (LangType == Lang::JP)
      {
        return "ja_JP.UTF-8";
      }
    }

    const std::string SentenceTagger(const std::string& s)
    {
      if constexpr (LangType == Lang::JP)
      {
        const auto tagger = MeCab::createTagger("");
        auto parsed_sentence = tagger->parse(s.c_str());
        return GraphSeg::internal::utils::ExtractTerm(parsed_sentence);
      }
      else
      {
        return s;
      }
    }
  };

  template <Lang LangType = Lang::EN>
  class Executable
  {
  protected:
    std::string CommandBaseExtractor()
    {
      const std::string home = getenv("HOME");
      if constexpr (LangType == Lang::JP)
      {
        return home + "/.pyenv/shims/python" + " " + home + "/graphseg-cpp/script/jp";
      }
      else
      {
        return home + "/.pyenv/shims/python" + " " + home + "/graphseg-cpp/script/en";
      }
    }
  };
}

#endif
