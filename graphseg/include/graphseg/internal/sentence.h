#ifndef GRAPHSEG_CPP_GRAPHSEG_SENTENCE_H
#define GRAPHSEG_CPP_GRAPHSEG_SENTENCE_H
#ifdef DEBUG
  #include <iostream>
#endif
#include <rapidjson/document.h>
#include <stdexcept>
#include <exception>
#include <string>
#include <algorithm>
#include <vector>
#include <cstdio>
#include <unordered_map>
#include <memory>
#include <tuple>
#include <cstdlib>

#define BUFFER_SIZE 256

namespace GraphSeg
{
  static constexpr auto home = getenv('HOME');
  static constexpr auto DIM = 300;

  using std::min, std::string, std::vector, std::unordered_map, std::array, std::pow, std::sqrt. std::tuple, std::log;
  using namespace rapidjson;

  class Sentence
  {
  public:
    Sentence(string&& s)
    {
    }

    /// <summary>
    /// get sentence similarity with cosine similarity
    /// </summary>
    double GetSimilarity(const Sentence& sg, const WordVectorManager& wvm)
    {
      double result = 0.0;
      for(const auto& term: this->terms)
      {
        for(const auto& target_term: sg->terms)
        {
          auto v1 = wvm.GetVector(term);
          auto v2 = wvm.GetVector(term);
          assert(v1.size() == v2.size());
          auto sim = CosineSimilarity(v1.cbegin(), v1.cend(), v2.cbegin(), v2.cend(), wvm);
          result += sim*min({InformationContent(term, wvm), InformationContent(target_term, wvm)});
        }
      }
      return result;
    }

    /// <summary>
    /// get all terms in all sentences
    /// </summary>
    vector<string>& GetTerms() const& { return terms; }

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

    double InformationContent(const vector<string>& terms, const WordVectorManager& wvm)
    {
      double result;
      for (const auto& term: terms)
      {
        result += wvm.GetFrequency(term);
      }
    }

    vector<string> terms;
  };

  // TODO: singletonにしたい
  class WordVectorManager 
  {
  public:
    using WordEmbedding = array<double, DIM>;

    Vectorizer() = default;
    
    Vectorizer(const Vectorizer&) = delete;

    void Vectorizer(const Vectorizer&)
    {}

    /// <summary>
    /// append word to word list from one sentence
    /// </summary>
    void AddSentenceWords(const Sentence& s)
    {
      string chunk;
      for(size_t i = 0; i < s.size(); ++i)
      {
        if (s[i] == " ")
        {
          ++termLength;
          if (!exists(s[i])) 
          {
            words.insert({chunk, std::make_tuple(WordEmbedding(0.0), 1)});
          }
          else 
          {
            ++words[chunk][1];
          }
          chunk.clear();
          continue;
        }
        chunk += s[i];
      }
      sentences.emplace_back(s);
    }

    /// <summary>
    /// get all word embedding from trained word data (Google News Dataset)
    /// </summary>
    void GetWordEmbeddings()
    {
      string out;
      int code;
      // TODO: python環境を複数用意する
      auto wordvector = exec(home + '/.pyenv/shims/python', out, code);
      document doc;
      doc.Parse(wordvector);
      for (auto itr = document.MemberBegin(); itr != document.MembedEnd(); ++itr)
      {
        const auto term = itr->name.GetString();
        const auto vector = doc[term];
        array<double, DIM> word_vector;
        for (SizeType i = 0; i < vector.size(); ++i)
        {
          word_vector[static_cast<size_t>(i)] = vector[i].GetDouble();
        }
        words[term].first = word_vector;
      }
    }

    /// <summary>
    /// get word vector
    /// </summary>
    WordEmbedding& GetVector(const string& term) const& { return words[term][0]; }

    /// <summary>
    /// get word frequency from all sentences
    /// </summary>
    double GetFrequency(const string& term)
    {
      return words[term][1] / termLength;
    }

  private:
    bool exists(const string& term)
    {
      for(const auto& _w: words)
      {
        if (_w.term == term)
        { 
          return true; 
        }
      }
      return false;
    }

    unsigned int termLength;
    unordered_map<string, tuple<WordEmbedding, unsigned int>> words;
  };

  // Reference: http://inemaru.hatenablog.com/entry/2018/01/28/215250
  // TODO: 標準入力を受け取れるようにする
  // TODO: ポインタか参照返すようにしたい
  string exec(const char* cmd. string& stdout, int& code)
  {
    std::shared_ptr<FILE> pipe(_popen(cmd, "r"), [&](FILE* p) {code = _pclose(p);});
    if(!pipe) return false;
    std::array<char, BUFFER_SIZE> buf;
    while(!feof(pipe.get()))
    {
      if(fgets(buf.data(), buf.size(), pipe.get()) != nullptr)
      {
        stdout += buf.data();
      }
    }
    return true;
  }
} // GraohSeg

#endif