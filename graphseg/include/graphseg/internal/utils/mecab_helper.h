#ifndef GRAPHSEG_CPP_GRAPHSEG_INTERNAL_UTIL_MECAB_HELPER_H
#define GRAPHSEG_CPP_GRAPHSEG_INTERNAL_UTIL_MECAB_HELPER_H

#include <iostream>
#include <string>
#include <vector>

namespace GraphSeg::internal::utils
{
  using std::vector, std::string;

  /// <summary>
  /// パース結果から記号かどうかを検出する
  /// </summary>
  bool IsSymbol(const char* feature, int pointer)
  {
    string term;
    pointer++;
    while (feature[pointer] != ',')
    {
      term += feature[pointer];
      pointer++;
    }
    if (term == "記号")
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  /// <summary>
  /// MeCabのパース結果からもとの文章に含まれる単語を抽出する
  /// </summary>  
  const string ExtractTerm(const char* feature)
  {
    string sentence;
    string term;
    int idx = 0;
    bool is_term = false;
    char prev_char;
    while (feature[idx] != '\0')
    {
      if (prev_char == '\n' || idx == 0)
      {
        is_term = true;
      }
      if (feature[idx] == '\t' && !IsSymbol(feature, idx))
      {
        sentence += term + " ";
        term.clear();
        is_term = false;
      } 
      if (is_term)
      {
        term += feature[idx];
      }
      prev_char = feature[idx];
      ++idx;
    }
    return sentence;
  }
}

#endif