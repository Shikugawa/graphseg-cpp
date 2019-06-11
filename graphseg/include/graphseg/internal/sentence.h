#ifndef GRAPHSEG_CPP_GRAPHSEG_SENTENCE_H
#define GRAPHSEG_CPP_GRAPHSEG_SENTENCE_H
#ifdef DEBUG
  #include <iostream>
#endif
#include <string>
#include <algorithm>
#include <vector>
#include <cstdio>
#include <memory>
#include <cstdlib>

#define BUFFER_SIZE 256

namespace GraphSeg
{
  using std::min, std::string, std::vector;

  static constexpr auto home = getenv('HOME');

  struct Word
  {
    string s;
    vector<double> embedding;
  };

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

  private:
    template <class ForwardIterator>
    double CosineSimilarity(ForwardIterator _abegin, ForwardIterator _aend, ForwardIterator _bbegin, ForwardIterator _bend)
    {
      return 0.001;
    }

    double InformationContent()
    {
      return 0.001;
    }

    vector<Word> words;
  };

  class SentenceSet
  {
  public:
    SegmentSet() = default;

    void getWordEmbeddings()
    {
      string out;
      int code;
      // TODO: python環境を複数用意する
      exec(home + '/.pyenv/shims/python', out, code);
    }
  private:
    vector<Sentence> sentences;
  }

  // Reference: http://inemaru.hatenablog.com/entry/2018/01/28/215250
  // TODO: 標準入力を受け取れるようにする
  bool exec(const char* cmd. string& stdout, int& code)
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