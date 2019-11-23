#ifndef GRAPHSEG_CPP_GRAPHSEG_EMBEDDING_HPP
#define GRAPHSEG_CPP_GRAPHSEG_EMBEDDING_HPP

#include "graphseg/language.hpp"
#include "graphseg/sentence.hpp"
#include "graphseg/internal/utils/exec.hpp"
#include "graphseg/internal/frequency.hpp"

#include <rapidjson/document.h>
#include <unordered_map>
#include <tuple>
#include <type_traits>
#include <array>
#include <memory>

namespace GraphSeg
{
using namespace rapidjson;
using namespace internal;

template <int VectorDim, Lang LangType = Lang::EN>
class Embedding : public Executable<LangType>
{
public:
  using Base = Executable<LangType>;
  using SentenceType = Sentence<LangType>;
  using WordEmbedding = std::array<double, VectorDim>;

  Embedding() = default;

  /// <summary>
  /// Preprocess to retrieve embeddings from terms in sentences
  /// </summary>
  template <class T, std::enable_if_t<std::is_same_v<std::remove_reference_t<T>, const SentenceType>> * = nullptr>
  void AddSentenceWords(T &&s)
  {
    auto tmp = std::forward<T>(s);
    for (const auto &term : tmp)
    {
      if (!exists(term))
      {
        ++termLength;
        InitWordEmbedding(term);
      }
      else
      {
        ++std::get<1>(words[term]);
      }
    }
  }

  /// <summary>
  /// Get all word embedding
  /// </summary>
  void GetWordEmbeddings()
  {
    const std::string term_stream = GetTermStream();
    auto result = Base::Execute("vectorizer.py", term_stream);
    Document doc;
    const auto parse_result = doc.Parse(result.c_str()).HasParseError();
    assert(parse_result == false);
    for (auto itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr)
    {
      const auto term = itr->name.GetString();
      assert(doc.HasMember(term));
      const auto &get_vector = doc[term];
      for (SizeType i = 0; i < VectorDim; ++i)
      {
        std::get<0>(words[term])[static_cast<size_t>(i)] = get_vector[i].GetDouble();
      }
    }

    frequency = std::make_unique<Frequency<LangType>>(term_stream);
  }

  /// <summary>
  /// Get word vector
  /// </summary>
  GRAPHSEG_INLINE_CONST WordEmbedding &GetVector(const std::string &term) const
  {
    return std::get<0>(words.at(term));
  }

  /// <summary>
  /// Get similarity based on Cosine Similarity between sentences
  /// </summary>
  double GetSimilarity(const SentenceType &sg1, const SentenceType &sg2) const &
  {
    double result = 0.0;
    for (const auto &term : sg1.GetTerms())
    {
      for (const auto &target_term : sg2.GetTerms())
      {
        const auto &v1 = GetVector(term);
        const auto &v2 = GetVector(target_term);
        if (IsStopWord(v1) || IsStopWord(v2))
        {
          continue;
        }
        assert(v1.size() == v2.size());
        auto sim = CosineSimilarity(v1.cbegin(), v1.cend(), v2.cbegin(), v2.cend());
        result += sim * std::min({InformationContent(term), InformationContent(target_term)});
      }
    }
    return result;
  }

  /// <summary>
  /// not to aware sentence length similarity caluculation
  /// </summary>
  double NormalizedSimilarity(const SentenceType &sg1, const SentenceType &sg2) const &
  {
    const auto sim = GetSimilarity(sg1, sg2);
    const auto normalized_rel_1 = sim / sg1.GetSize();
    const auto normalized_rel_2 = sim / sg2.GetSize();
    return (normalized_rel_1 + normalized_rel_2) / 2;
  }

private:
  template <class ForwardIterator>
  double CosineSimilarity(ForwardIterator _abegin, ForwardIterator _aend, ForwardIterator _bbegin, ForwardIterator _bend) const
  {
    double q = 0, d = 0, qd = 0;
    auto a = _abegin;
    auto b = _bbegin;
    while (a != _aend && b != _bend)
    {
      q += pow(*a, 2);
      d += pow(*b, 2);
      qd += (*a) * (*b);
      ++a;
      ++b;
    }
    return qd / (std::sqrt(q) * std::sqrt(d));
  }

  double InformationContent(const std::string &term) const
  {
    const double denominator = frequency->GetFrequency(term) + 1;
    const double numerator = frequency->GetCorpusSize() + frequency->GetTotalCount();
    return -std::log(denominator / numerator);
  }

  std::string GetTermStream() const
  {
    std::string s;
    for (const auto &word : words)
    {
      s += word.first + " ";
    }
    return s;
  }

  bool exists(const std::string &term)
  {
    for (const auto &_w : words)
    {
      if (_w.first == term)
      {
        return true;
      }
    }
    return false;
  }

  void InitWordEmbedding(std::string term)
  {
    WordEmbedding wm;
    for (size_t j = 0; j < VectorDim; ++j)
    {
      wm[j] = 0.0;
    }
    words.insert({term, std::make_tuple(wm, 1)});
  }

  bool IsStopWord(const WordEmbedding &d) const
  {
    // if d is zero-vector, reguard this term as stop-word
    for (size_t i = 0; i < VectorDim; ++i)
    {
      if (d[i] != 0.0)
        return false;
    }
    return true;
  }

  std::shared_ptr<Frequency<LangType>> frequency;
  unsigned int termLength;
  std::unordered_map<std::string, std::tuple<WordEmbedding, unsigned int>> words;
};
} // namespace GraphSeg

#endif