#ifndef GRAPHSEG_CPP_GRAPHSEG_LANG_H
#define GRAPHSEG_CPP_GRAPHSEG_LANG_H

#include "graphseg/internal/utils/mecab_helper.h"

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
  class I18NFactory
  {
  public:
    static std::function<const std::string(const std::string&)> SentenceTagger()
    {
      if constexpr (LangType == Lang::JP)
      {
        return [](const std::string& s) -> const std::string {
          const auto tagger = MeCab::createTagger("");
          auto parsed_sentence = tagger->parse(s.c_str());
          return GraphSeg::internal::utils::extractTerm(parsed_sentence);
        };
      }
      else
      {
        return [](const std::string& s) -> const std::string {
          return s;
        };
      }
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
