#ifndef GRAPHSEG_CPP_GRAPHSEG_LANG_HPP
#define GRAPHSEG_CPP_GRAPHSEG_LANG_HPP

#include "graphseg/internal/utils/mecab_helper.hpp"
#include "graphseg/sentence.hpp"

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

  template <Lang LangType>
  std::string Locale()
  {
    if constexpr (LangType == Lang::JP)
    {
      return "ja_JP.UTF-8";
    }
  }

  template <Lang LangType = Lang::EN>
  class I18NFactory
  {
  public:
    static std::function<const std::string(const std::string&)> SentenceTagger()
    {
      return [](const std::string& s) -> const std::string {
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
      };
    }

    static std::string CommandBaseExtractor()
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
