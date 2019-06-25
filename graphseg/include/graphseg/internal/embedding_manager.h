#ifndef GRAPHSEG_CPP_GRAPHSEG_INTERNAL_EMBEDDING_MANAGER_H
#define GRAPHSEG_CPP_GRAPHSEG_INTERNAL_EMBEDDING_MANAGER_H

#include "sentence.h"
#include "util.h"
#include "frequency.h"
#include <rapidjson/document.h>
#include <unordered_map>
#include <tuple>
#include <type_traits>
#include <array>

#include <iostream>

namespace GraphSeg
{
  using namespace rapidjson;
  using std::unordered_map, std::array, std::min, std::sqrt, std::log, 
        std::pow, std::tuple, std::get, std::make_tuple;

  static constexpr auto DIM = 300;

  const string home = getenv("HOME");
  const string COMMAND = home + "/.pyenv/shims/python" + " " + home + "/graphseg-cpp/script/vectorizer.py";

  class EmbeddingManager
  {
  public:
    using WordEmbedding = array<double, DIM>;

    EmbeddingManager() = default;
    
    EmbeddingManager(const EmbeddingManager&) = delete;

    // disable copy
    EmbeddingManager& operator=(const EmbeddingManager&)
    {}

    /// <summary>
    /// append word to word list from one sentence
    /// TODO: sentence should be const
    /// </summary>
    void AddSentenceWords(Sentence& s)
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

    /// <summary>
    /// get all word embedding from trained word data (Google News Dataset)
    /// </summary>
    void GetWordEmbeddings()
    {
      int code;
      const string term_stream = GetTermStream();
      const string cmd = "echo " +  term_stream + " | " + COMMAND;
      auto result = exec(cmd.c_str(), code);
      Document doc;
      const auto parse_result = doc.Parse(result.c_str()).HasParseError();
      for (auto itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr)
      {
        const auto term = itr->name.GetString();
        assert(doc.HasMember(term));
        const auto& get_vector = doc[term];
        for (SizeType i = 0; i < DIM; ++i)
        {
          get<0>(words[term])[static_cast<size_t>(i)] = get_vector[i].GetDouble();
        }
      }
    }

    /// <summary>
    /// TODO: get word vector
    /// </summary>
    inline WordEmbedding& GetVector(const string& term)
    { return get<0>(words[term]); }
    
    /// <summary>
    /// get sentence similarity with cosine similarity
    /// 負数で小さいほど類似度が高いので符号を入れ替える
    /// </summary>
    double GetSimilarity(const Sentence& sg1, const Sentence& sg2)
    {
      double result = 0.0;
      for(const auto& term: sg1.GetTerms())
      {
        for(const auto& target_term: sg2.GetTerms())
        {
          const auto& v1 = this->GetVector(term);
          const auto& v2 = this->GetVector(target_term);
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
    double CosineSimilarity(ForwardIterator _abegin, ForwardIterator _aend, ForwardIterator _bbegin, ForwardIterator _bend)
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

    double InformationContent(const string& term)
    {
      const auto numerator = frequency.GetSize() + frequency.GetSumFreq();
      const auto denominator = frequency.GetFrequency(term) + 1;
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
      for (size_t j = 0; j < DIM; ++j)
      {
        wm[j] = 0.0;
      }
      words.insert({term, make_tuple(wm, 1)});
    }

    /// <summary>
    /// 零ベクトルならStop Wordであるとみなす
    /// </summary>
    bool IsStopWord(const array<double, DIM>& d)
    {
      for(size_t i = 0; i < DIM; ++i)
      {
        if (d[i] != 0.0) return false;
      }
      return true;
    }

    Frequency frequency;
    unsigned int termLength;
    unordered_map<string, tuple<WordEmbedding, unsigned int>> words;
  };
}

#endif