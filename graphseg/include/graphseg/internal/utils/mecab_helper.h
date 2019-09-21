#ifndef GRAPHSEG_CPP_GRAPHSEG_INTERNAL_UTIL_MECAB_HELPER_H
#define GRAPHSEG_CPP_GRAPHSEG_INTERNAL_UTIL_MECAB_HELPER_H

#include <iostream>
#include <string>
#include <vector>

namespace GraphSeg::internal::utils
{
  using std::vector, std::string;

  /// <summary>
  /// MeCabのパース結果からもとの文章に含まれる単語を抽出する
  /// </summary>  
  const string extractTerm(const char* feature)
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
      if (feature[idx] == '\t')
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