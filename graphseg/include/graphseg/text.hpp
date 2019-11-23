#ifndef GRAPHSEG_CPP_GRAPHSEG_TEXT_HPP
#define GRAPHSEG_CPP_GRAPHSEG_TEXT_HPP

#include "graphseg/sentence.hpp"
#include "graphseg/language.hpp"
#include "graphseg/internal/utils/string.hpp"

#include <unordered_map>
#include <string>
#include <vector>

namespace GraphSeg
{
template <Lang LangType>
struct TextData
{
  std::vector<Sentence<LangType>> sentences;
  std::string origin_path;
};

template <Lang LangType>
class TextProcessor : public TextData<LangType>, public Language<LangType>
{
  template <Lang T>
  static constexpr auto false_v = false;
  static_assert(false_v<LangType>, "Specified TextProcessor is not implemented");
};

template <>
class TextProcessor<Lang::JP> : public TextData<Lang::JP>, public Language<Lang::JP>
{
public:
  TextProcessor(const std::wstring &_article, std::string _origin_path) : article(_article)
  {
    origin_path = _origin_path;
    this->Execute();
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

  void SmallRoundBracket()
  {
    while (article.at(current_idx) != L')')
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

  void Execute()
  {
    while (current_idx < article.size())
    {
      if (article.at(current_idx) == L'【')
      {
        ++current_idx;
        BlockBracket();
      }
      else if (article.at(current_idx) == L'（')
      {
        RoundBracket();
      }
      else if (article.at(current_idx) == L'(')
      {
        SmallRoundBracket();
      }
      else if (article.at(current_idx) == L'「')
      {
        ++current_idx; // Prevent "「」" insertion to sentence
        AngleBracket();
      }
      else if (article.at(current_idx) == L'。') // Sentence deliminator
      {
        wstring_sentences.emplace_back(current_sentence);
        current_sentence.clear();
      }
      else
      {
        current_sentence += article.at(current_idx);
      }
      current_idx++;
    }
    for (auto &&sentence : wstring_sentences)
    {
      auto raw_sentence = Sentence<Lang::JP>(Language<Lang::JP>::SentenceTagger(internal::utils::ConvertString(sentence)));
      sentences.emplace_back(raw_sentence);
    }
  }

  std::vector<std::wstring> wstring_sentences;
  std::wstring current_sentence;
  std::wstring article;
  size_t current_idx = 0;
};

template <Lang LangType>
class Text final : public TextProcessor<LangType>
{
public:
  using Base = TextProcessor<LangType>;

  explicit Text(const std::wstring &_article, std::string _origin_path) : Base(_article, _origin_path)
  {
  }

  GRAPHSEG_INLINE_CONST std::vector<Sentence<LangType>> &GetSentences() const
  {
    return Base::sentences;
  }
};
} // namespace GraphSeg

#endif