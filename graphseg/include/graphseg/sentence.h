#ifndef GRAPHSEG_CPP_GRAPHSEG_SENTENCE_H
#define GRAPHSEG_CPP_GRAPHSEG_SENTENCE_H

#include <type_traits>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <mecab.h>
#include <graphseg/lang.h>
#include <graphseg/internal/utils/mecab_helper.h>

namespace GraphSeg
{
  using std::string, std::vector, std::is_same_v;

  template <Lang LangType = Lang::EN>
  class Sentence
  {
    using iterator = vector<string>::iterator;
    using const_iterator = vector<string>::const_iterator;

    vector<char> deliminator_set;

  public:
    Sentence(string&& s)
    {
      deliminator_set.emplace_back(' ');
      sentence = I18NFactory<LangType>::SentenceTagger()(s);
      CreateTerm();
    }

    Sentence(const string& s)
    {
      deliminator_set.emplace_back(' ');
      sentence = I18NFactory<LangType>::SentenceTagger()(std::move(s));
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
    /// Get all term retrieved from sentences
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
    /// Get term size
    /// </summary>
    size_t GetSize()
    {
      return terms.size(); 
    }

    /// <summary>
    /// Get all sentence (rvalue & lvalue)
    /// </summary>
    const string& GetText() const& 
    { 
      return sentence; 
    }

    string GetText() &&
    { 
      return std::move(sentence); 
    }

    /// <summary>
    /// Add new deliminator
    /// </summary>
    void ApplyDeliminator(char ch) 
    {
      deliminator_set.emplace_back(ch);
    }

    /// <summary>
    /// Destroy deliminator
    /// <summary>
    void DestoryDeliminator(char ch)
    {
      if (std::find(deliminator_set.begin(), deliminator_set.end(), ch) != deliminator_set.end())
      {
        std::remove_if(deliminator_set.begin(), deliminator_set.end(), [&ch](auto delim){ return delim == ch; });
      }
    }

    /// <summary>
    /// Operator : Get term with index
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
        if (std::find(deliminator_set.begin(), deliminator_set.end(), *itr) != deliminator_set.end())
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