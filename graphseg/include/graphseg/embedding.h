#ifndef GRAPHSEG_CPP_GRAPHSEG_EMBEDDING_H
#define GRAPHSEG_CPP_GRAPHSEG_EMBEDDING_H

#include "graphseg/lang.h"
#include "graphseg/sentence.h"
#include "graphseg/internal/utils/exec.h"
#include "graphseg/internal/frequency.h"

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

  using std::unordered_map, std::array, std::min, std::sqrt, std::log, 
        std::pow, std::tuple, std::get, std::make_tuple, std::unique_ptr;

  template <int VectorDim, Lang LangType = Lang::EN>
  class Embedding
  {
  public:
    using SentenceType = Sentence<LangType>;
    using WordEmbedding = array<double, VectorDim>;

    Embedding() = default;

    /// <summary>
    /// Preprocess to retrieve embeddings from terms in sentences
    /// </summary>
    void AddSentenceWords(SentenceType& s)
    {
      for (const auto& term: s)
      {
        if (!exists(term))
        {
          ++termLength;
          InitWordEmbedding(term);
        }
        else
        {
          ++get<1>(words[term]);
        }
      }
    }

    void AddSentenceWords(SentenceType&& s)
    {
      for (auto&& term: std::move(s))
      {
        if (!exists(term))
        {
          ++termLength;
          InitWordEmbedding(term);
        }
        else
        {
          ++get<1>(words[term]);
        }
      }
    }

    /// <summary>
    /// Get all word embedding
    /// </summary>
    void GetWordEmbeddings()
    {
      int code;
      const string term_stream = GetTermStream();
      const string cmd = "echo " +  term_stream + " | " + I18NFactory<LangType>::CommandBaseExtractor() + "/vectorizer.py";
      auto result = utils::exec(cmd.c_str(), code);
      Document doc;
      const auto parse_result = doc.Parse(result.c_str()).HasParseError();
      assert(parse_result == false);
      for (auto itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr)
      {
        const auto term = itr->name.GetString();
        assert(doc.HasMember(term));
        const auto& get_vector = doc[term];
        for (SizeType i = 0; i < VectorDim; ++i)
        {
          get<0>(words[term])[static_cast<size_t>(i)] = get_vector[i].GetDouble();
        }
      }

      frequency = std::make_unique<Frequency<LangType>>(term_stream);
    }

    /// <summary>
    /// Get word vector
    /// </summary>
    inline const WordEmbedding& GetVector(const string& term) const
    { 
      return get<0>(words.at(term)); 
    }
    
    /// <summary>
    /// Get similarity based on Cosine Similarity between sentences 
    /// </summary>
    double GetSimilarity(const SentenceType& sg1, const SentenceType& sg2) const&
    {
      double result = 0.0;
      for(const auto& term: sg1.GetTerms())
      {
        for(const auto& target_term: sg2.GetTerms())
        {
          const auto& v1 = GetVector(term);
          const auto& v2 = GetVector(target_term);
          if (IsStopWord(v1) || IsStopWord(v2)) continue;
          assert(v1.size() == v2.size());
          auto sim = CosineSimilarity(v1.cbegin(), v1.cend(), v2.cbegin(), v2.cend());
          result += sim*min({InformationContent(term), InformationContent(target_term)});
        }
      }
      return -result;
    }

  private:
    template <class ForwardIterator>
    double CosineSimilarity(ForwardIterator _abegin, ForwardIterator _aend, ForwardIterator _bbegin, ForwardIterator _bend) const
    {
      double q = 0, d = 0, qd = 0;
      auto a = _abegin;
      auto b = _bbegin;
      while(a != _aend && b != _bend)
      {
        q += pow(*a, 2);
        d += pow(*b, 2);
        qd += (*a)*(*b);
        ++a;
        ++b;
      }
      return qd/(sqrt(q)*sqrt(d));
    }

    double InformationContent(const string& term) const
    {
      const auto numerator = frequency->GetSize() + frequency->GetSumFreq();
      const auto denominator = frequency->GetFrequency(term) + 1;
      return denominator / numerator;
    }

    string GetTermStream() const
    {
      string s;
      for(const auto& word: words)
      {
        s += word.first + " ";
      }
      return s;
    }

    bool exists(const string& term)
    {
      for(const auto& _w: words)
      {
        if (_w.first == term)
        { 
          return true; 
        }
      }
      return false;
    }

    void InitWordEmbedding(string term)
    {
      WordEmbedding wm;
      for (size_t j = 0; j < VectorDim; ++j)
      {
        wm[j] = 0.0;
      }
      words.insert({term, make_tuple(wm, 1)});
    }

    bool IsStopWord(const array<double, VectorDim>& d) const
    {
      // if d is zero-vector, reguard this term as stop-word
      for(size_t i = 0; i < VectorDim; ++i)
      {
        if (d[i] != 0.0) return false;
      }
      return true;
    }

    std::shared_ptr<Frequency<LangType>> frequency;
    unsigned int termLength;
    unordered_map<string, tuple<WordEmbedding, unsigned int>> words;
  };
}

#endif