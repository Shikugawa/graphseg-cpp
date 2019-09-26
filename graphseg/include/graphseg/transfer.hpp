#ifndef GRAPHSEG_CPP_GRAPHSEG_TRANSFER_HPP
#define GRAPHSEG_CPP_GRAPHSEG_TRANSFER_HPP

#include "graphseg/lang.hpp"
#include "graphseg/sentence.hpp"
#include "graphseg/text_stream.hpp"

#include <cstring>

namespace GraphSeg
{
  using std::vector, std::wstring;

  template <Lang LangType>
  class Transfer
  {
    template <Lang T>
    static constexpr auto false_v = false;
    static_assert(false_v<LangType>, "Specified Transfer is not implemented");
  };

  template <>
  class Transfer<Lang::JP>
  {
  public:
    Transfer(const wstring& _article) : article(_article)
    {
    }

  private:
    void ParseBlackBracket()
    {
      while (article.at(current_idx) != L'】')
      {
        ++current_idx;
      }
    }

    void ParseBracket()
    {
      while (article.at(current_idx) != L'）')
      {
        ++current_idx;
      }
    }

    void ParseAngleBracket()
    {
      while (article.at(current_idx) != L'」')
      {
        current_sentence += article.at(current_idx);
        ++current_idx;
      }
    }

  public:
    const vector<Sentence<Lang::JP>> Transcode()
    {
      vector<Sentence<Lang::JP>> sentences;
      while (current_idx < article.size())
      {
        // カッコは基本的に全角
        if (article.at(current_idx) == L'【')
        {
          ParseBlackBracket();
        } 
        else if (article.at(current_idx) == L'（')
        {
          ParseBracket();
        }
        else if (article.at(current_idx) == L'「')
        {
          ++current_idx; // Prevent "「」" insertion to sentence
          ParseAngleBracket();
        }
        else if (article.at(current_idx) == L'。') // Sentence deliminator
        {
          sentences.emplace_back(Sentence<Lang::JP>(current_sentence));
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
  class Transfer<Lang::EN>
  {};
} // namespace GraphSeg


#endif