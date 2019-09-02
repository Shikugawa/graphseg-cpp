#ifndef GRAPHSEG_CPP_GRAPHSEG_INTERNAL_EMBEDDING_H
#define GRAPHSEG_CPP_GRAPHSEG_INTERNAL_EMBEDDING_H

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

  static constexpr auto DIM = 300;
  const string COMMAND_VECTORIZER = home + "/.pyenv/shims/python" + " " + home + "/graphseg-cpp/script/vectorizer.py";

  class Embedding
  {
  public:
    using WordEmbedding = array<double, DIM>;

    Embedding() = default;
    
    Embedding(const Embedding&) = delete;

    Embedding& operator=(const Embedding&) = delete;

    /// <summary>
    /// センテンスの単語から埋め込みを得る際の前処理
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

    void AddSentenceWords(Sentence&& s)
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
    /// 全ての単語埋め込みを得る
    /// </summary>
    void GetWordEmbeddings()
    {
      int code;
      const string term_stream = GetTermStream();
      const string cmd = "echo " +  term_stream + " | " + COMMAND_VECTORIZER;
      auto result = utils::exec(cmd.c_str(), code);
      Document doc;
      const auto parse_result = doc.Parse(result.c_str()).HasParseError();
      assert(parse_result == false);
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

      frequency = std::make_unique<Frequency>(term_stream);
    }

    /// <summary>
    /// 単語ベクトルの取得
    /// </summary>
    inline const WordEmbedding& GetVector(const string& term) const
    { 
      return get<0>(words.at(term)); 
    }
    
    /// <summary>
    /// コサイン類似度に基づくセンテンス間の類似度を得る
    /// 負数で小さいほど類似度が高いので符号を入れ替える
    /// </summary>
    double GetSimilarity(const Sentence& sg1, const Sentence& sg2) const&
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
      return result;
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
      for (size_t j = 0; j < DIM; ++j)
      {
        wm[j] = 0.0;
      }
      words.insert({term, make_tuple(wm, 1)});
    }

    /// <summary>
    /// 零ベクトルならStop Wordであるとみなす
    /// </summary>
    bool IsStopWord(const array<double, DIM>& d) const
    {
      for(size_t i = 0; i < DIM; ++i)
      {
        if (d[i] != 0.0) return false;
      }
      return true;
    }

    unique_ptr<Frequency> frequency;
    unsigned int termLength;
    unordered_map<string, tuple<WordEmbedding, unsigned int>> words;
  };
}

#endif