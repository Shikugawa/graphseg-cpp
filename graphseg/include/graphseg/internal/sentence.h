#ifndef GRAPHSEG_CPP_GRAPHSEG_SENTENCE_H
#define GRAPHSEG_CPP_GRAPHSEG_SENTENCE_H
#include <string>
#include <vector>
#include <iostream>

namespace GraphSeg
{
  using std::string, std::vector;

  class Sentence
  {
    using iterator = vector<string>::iterator;
    using const_iterator = vector<string>::const_iterator;

    const char delim = ' ';

  public:
    Sentence(string&& s) : sentence(std::move(s))
    {
      CreateTerm();
    }

    Sentence(const string& s) : sentence(s)
    {
      CreateTerm();
    }

    inline iterator begin() noexcept 
    { 
      return terms.begin(); 
    }
    
    inline const_iterator cbegin() const noexcept 
    { 
      return terms.cbegin(); 
    }
    
    inline iterator end() noexcept 
    { 
      return terms.end(); 
    }
    
    inline const_iterator cend() const noexcept 
    { 
      return terms.cend(); 
    }

    /// <summary>
    /// 文章中の全タームを取得する
    /// </summary>
    const vector<string>& GetTerms() const& 
    { 
      return terms; 
    }

    vector<string> GetTerms() &&
    {
      return std::move(terms);
    }

    /// <summary>
    /// 文章の全ターム数を取得する
    /// </summary>
    size_t GetSize()
    {
      return terms.size(); 
    }

    /// <summary>
    /// 文章の内容を取得する
    /// </summary>
    const string& GetText() const& 
    { 
      return sentence; 
    }

    /// <summary>
    /// 文章の内容を取得する
    /// </summary>
    string GetText() &&
    { 
      return std::move(sentence); 
    }

    /// <summary>
    /// タームを取得する
    /// </summary>
    const string& operator[](size_t idx)
    { 
      return terms[idx]; 
    }

  private:
    void CreateTerm()
    {
      string item;
      for (auto itr = sentence.begin(); itr != sentence.end(); ++itr)
      {
        if (*itr == delim)
        {
          terms.emplace_back(item);
          item.clear();
          continue;
        }
        item += *itr;
      }
      terms.emplace_back(item);
    }

    string sentence;
    vector<string> terms;
  };
} // GraohSeg

#endif