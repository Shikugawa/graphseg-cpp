#ifndef GRAPHSEG_CPP_GRAPHSEG_ARTICLEPROCESSOR_HPP
#define GRAPHSEG_CPP_GRAPHSEG_ARTICLEPROCESSOR_HPP

#include "graphseg/language.hpp"

#include <cstring>

namespace GraphSeg
{
  using std::vector, std::wstring;

  template <Lang LangType>
  class ArticleProcessor
  {
    template <Lang T>
    static constexpr auto false_v = false;
    static_assert(false_v<LangType>, "Specified ArticleProcessor is not implemented");
  };

  template <>
  class ArticleProcessor<Lang::JP>
  {
  public:
    ArticleProcessor(const wstring& _article) : article(_article)
    {
    }

  private:
    void BlockBracket()
    {
      while (article.at(current_idx) != L'】')
      {
        ++current_idx;
      }
    }

    void RoundBracket()
    {
      while (article.at(current_idx) != L'）')
      {
        ++current_idx;
      }
    }

    void AngleBracket()
    {
      while (article.at(current_idx) != L'」')
      {
        current_sentence += article.at(current_idx);
        ++current_idx;
      }
    }

  public:
    const vector<std::wstring> Execute()
    {
      vector<std::wstring> sentences;
      while (current_idx < article.size())
      {
        // カッコは基本的に全角
        if (article.at(current_idx) == L'【')
        {
          BlockBracket();
        } 
        else if (article.at(current_idx) == L'（')
        {
          RoundBracket();
        }
        else if (article.at(current_idx) == L'「')
        {
          ++current_idx; // Prevent "「」" insertion to sentence
          AngleBracket();
        }
        else if (article.at(current_idx) == L'。') // Sentence deliminator
        {
          sentences.emplace_back(current_sentence);
          current_sentence.clear();
        }
        else 
        {
          current_sentence += article.at(current_idx);
        }
        current_idx++;
      }
      return sentences;
    }

    std::wstring current_sentence;
    std::wstring article;
    size_t current_idx = 0;
  };

  template <>
  class ArticleProcessor<Lang::EN>
  {};
} // namespace GraphSeg


#endif