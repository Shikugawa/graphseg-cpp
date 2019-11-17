#ifndef GRAPHSEG_CPP_GRAPHSEG_TEXT_PROCESSOR_HPP
#define GRAPHSEG_CPP_GRAPHSEG_TEXT_PROCESSOR_HPP

#include "graphseg/language.hpp"
#include "graphseg/sentence.hpp"
#include "graphseg/article_processor.hpp"

#include <fstream>
#include <sstream>
#include <locale>
#include <vector>

namespace GraphSeg
{
  template <Lang LangType = Lang::EN>  
  class TextProcessor : public Language<LangType>
  {
    using Base = Language<LangType>;
  public:
    explicit TextProcessor(const std::string& path) : wText(ReadTextFile(path))
    {
      auto wstringSentences = TextTranscoder();
      for (const auto& sentence : wstringSentences)
      {
        sentences.emplace_back(
          Sentence<LangType>(Base::SentenceTagger(BasicStringConverter(sentence))));
      }
    }

    GRAPHSEG_INLINE_CONST std::vector<Sentence<LangType>>& GetSentences()
    {
      return sentences;
    }

  private:
    const std::wstring wText;
    std::vector<Sentence<LangType>> sentences;

    string BasicStringConverter(const std::wstring& wstr)
    {
      std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
      return cv.to_bytes(wstr);
    }

    wstring ReadTextFile(const std::string& path)
    {
      const auto localeStr = Base::Locale();
      std::wifstream wif(path);
      wif.imbue(locale(localeStr));
      std::wcout.imbue(locale(localeStr));
      std::wstringstream wss;
      wss << wif.rdbuf();
      return wss.str();
    }

    std::vector<std::wstring> TextTranscoder()
    {
      auto processor = ArticleProcessor<LangType>(wText);
      return processor.Execute();
    }
  };
}

#endif