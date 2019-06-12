#ifndef GRAPHSEG_CPP_GRAPHSEG_SENTENCE_H
#define GRAPHSEG_CPP_GRAPHSEG_SENTENCE_H
#ifdef DEBUG
  #include <iostream>
#endif
#include <simdjson/jsonparser.h>
#include <stdexcept>
#include <exception>
#include <string>
#include <algorithm>
#include <vector>
#include <cstdio>
#include <unordered_map>
#include <memory>
#include <cstdlib>

#define BUFFER_SIZE 256

namespace GraphSeg
{
  static constexpr auto home = getenv('HOME');
  static constexpr auto DIM = 300;

  using std::min, std::string, std::vector, std::unordered_map, std::array, std::pow, std::sqrt;

  class Sentence
  {
  public:
    Sentence(string&& s)
    {
    }

    double getSimilarity(const Sentence& sg)
    {
      double result = 0.0;
      for(const auto& term: this->terms)
      {
        for(const auto& target_term: sg->terms)
        {
          auto v1 = getVector(term);
          auto v2 = getVector(term);
          assert(v1.size() == v2.size());
          auto sim = CosineSimilarity(v1.cbegin(), v1.cend(), v2.cbegin(), v2.cend());
          result += sim*min({InformationContent(term), InformationContent(target_term)});
        }
      }
      return result;
    }

    vector<string>& getTerms() const& { return terms; }

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

    double InformationContent()
    {
      return 0.001;
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

    void addSentenceWords(const Sentence& s)
    {
      string chunk;
      for(size_t i = 0; i < s.size(); ++i)
      {
        if (s[i] == " "){
          if (!exists(s[i])) {
            words.insert({chunk, WordEmbedding(0.0)});
          }
          chunk.clear();
          continue;
        }
        chunk += s[i];
      }
      sentences.emplace_back(s);
    }

    void getWordEmbeddings()
    {
      string out;
      int code;
      // TODO: python環境を複数用意する
      auto wordvector = exec(home + '/.pyenv/shims/python', out, code);
      ParsedJson pj;
      pj.allocateCapacity(wordvector.size());
      if ((res = json_parse(wordvector, pj)) != 0) {
        throw std::runtime_error(simdjson::errorMsg(res));
      }
      for(const auto term: getjsonkeys(pj))
      {
        auto vec = tocppvector(pj[term]);
        term
      }
    }

  private:
    bool exists(const string& term)
    {
      for(const auto& _w: words)
      {
        if (_w.term == term)
        { return true; }
      }
      return false;
    }
    unordered_map<string, WordEmbedding> words;
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