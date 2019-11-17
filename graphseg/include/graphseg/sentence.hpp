#ifndef GRAPHSEG_CPP_GRAPHSEG_SENTENCE_HPP
#define GRAPHSEG_CPP_GRAPHSEG_SENTENCE_HPP

#include "graphseg/internal/utils/mecab_helper.hpp"
#include "graphseg/language.hpp"

#include <codecvt>
#include <type_traits>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <mecab.h>

namespace GraphSeg
{
  template <Lang LangType = Lang::EN>
  class Sentence
  {
    using iterator = std::vector<std::string>::iterator;
    using const_iterator = std::vector<std::string>::const_iterator;

    std::vector<char> deliminator_set;

  public:
    Sentence(std::string&& s) : sentence(std::move(s))
    {
      deliminator_set.emplace_back(' ');
      CreateTerm();
    }

    Sentence(const std::string& s) : sentence(s)
    {
      deliminator_set.emplace_back(' ');
      CreateTerm();
    }

    inline const_iterator begin() const noexcept 
    { 
      return terms.cbegin(); 
    }
    
    inline const_iterator cbegin() const noexcept 
    { 
      return terms.cbegin(); 
    }
    
    inline const_iterator end() const noexcept 
    { 
      return terms.cend(); 
    }
    
    inline const_iterator cend() const noexcept 
    { 
      return terms.cend(); 
    }

    /// <summary>
    /// Get all term retrieved from sentences
    /// </summary>
    GRAPHSEG_INLINE_CONST std::vector<std::string>& GetTerms() const& 
    {
      return terms; 
    }

    GRAPHSEG_INLINE_CONST std::vector<std::string> GetTerms() &&
    {
      return std::move(terms);
    }

    /// <summary>
    /// Get term size
    /// </summary>
    GRAPHSEG_INLINE_CONST size_t GetSize()
    {
      return terms.size(); 
    }

    /// <summary>
    /// Get all sentence (rvalue & lvalue)
    /// </summary>
    GRAPHSEG_INLINE_CONST std::string& GetText() const& 
    { 
      return sentence; 
    }

    GRAPHSEG_INLINE_CONST std::string GetText() &&
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
    GRAPHSEG_INLINE_CONST std::string& operator[](size_t idx)
    { 
      return terms[idx]; 
    }

  private:
    void CreateTerm()
    {
      std::string item;
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

    std::string sentence;
    std::vector<std::string> terms;
  };
} // GraohSeg

#endif