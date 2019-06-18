#ifndef GRAPHSEG_CPP_GRAPHSEG_INTERNAL_EMBEDDING_MANAGER_H
#define GRAPHSEG_CPP_GRAPHSEG_INTERNAL_EMBEDDING_MANAGER_H

#include "sentence.h"
#include <rapidjson/document.h>
#include <unordered_map>
#include <cstdlib>
#include <cstdio>
#include <tuple>
#include <type_traits>
#include <memory>
#include <array>

namespace GraphSeg {
  using namespace rapidjson;
  using std::unordered_map, std::shared_ptr, std::array, std::min, std::sqrt, std::log, 
        std::pow, std::tuple, std::get, std::make_tuple;

  static constexpr auto BUFFER_SIZE = 256;
  static constexpr auto DIM = 300;

  const string home = getenv("HOME");
  const string COMMAND = home + "/.pyenv/shims/python" + " " + home + "/graphseg-cpp/model/vectorizer.py";
  
  string exec(const char* cmd, int& code)
  {
    string stdout;
    shared_ptr<FILE> pipe(popen(cmd, "r"), [&](FILE* p) {code = pclose(p);});
    if(!pipe)
    {
      return stdout;
    }
    array<char, BUFFER_SIZE> buf;
    while(!feof(pipe.get()))
    {
      if(fgets(buf.data(), buf.size(), pipe.get()) != nullptr)
      {
        stdout += buf.data();
      }
    }
    return stdout;
  }

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
      doc.Parse(result.c_str());
      for (auto itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr)
      {
        const auto term = itr->name.GetString();
        assert(doc.HasMember(term));
        const auto& get_vector = doc[term];
        array<double, DIM> word_vector;
        for (SizeType i = 0; i < DIM; ++i)
        {
          word_vector[static_cast<size_t>(i)] = get_vector[i].GetDouble();
        }
        get<0>(words[term]) = word_vector;
      }
    }

    /// <summary>
    /// TODO: get word vector
    /// </summary>
    inline WordEmbedding& GetVector(const string& term)
    { return get<0>(words[term]); }

    /// <summary>
    /// TODO: get word frequency from all sentences
    /// </summary>
    double GetFrequency(const string& term)
    {
      // return words[term][1] / termLength;
    }


    /// <summary>
    /// get sentence similarity with cosine similarity
    /// </summary>
    double GetSimilarity(const Sentence& sg1, const Sentence& sg2)
    {
      double result = 0.0;
      for(const auto& term: sg1.GetTerms())
      {
        for(const auto& target_term: sg2.GetTerms())
        {
          auto& v1 = this->GetVector(term);
          auto& v2 = this->GetVector(term);
          assert(v1.size() == v2.size());
          auto sim = CosineSimilarity(v1.cbegin(), v1.cend(), v2.cbegin(), v2.cend());
          result += sim*min({InformationContent(term), InformationContent(target_term)});
        }
      }
      return result;
    }

  private:
    template <class ForwardIterator>
    double CosineSimilarity(ForwardIterator _abegin, ForwardIterator _aend, ForwardIterator _bbegin, ForwardIterator _bend)
    {
      double q, d, qd;
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

    // TODO
    double InformationContent(const string& term)
    {
      // double result;
      // result += wvm.GetFrequency(term);
      // return result;
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

    unsigned int termLength;
    unordered_map<string, tuple<WordEmbedding, unsigned int>> words;
  };
}

#endif