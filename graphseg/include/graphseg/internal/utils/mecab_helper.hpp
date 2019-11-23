#ifndef GRAPHSEG_CPP_GRAPHSEG_INTERNAL_UTIL_MECAB_HPPELPER_HPP
#define GRAPHSEG_CPP_GRAPHSEG_INTERNAL_UTIL_MECAB_HPPELPER_HPP

#include <iostream>
#include <string>
#include <vector>

namespace GraphSeg::internal::utils
{
/// <summary>
/// detect if it is symbol or not from MeCab parse result
/// Currently, I've not use this because it is not needed
/// But if including symbol cause decline segmentation accuracy, Use it to avoid this problem.
/// </summary>
bool IsSymbol(const char *feature, int pointer)
{
  std::string term;
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
/// Detect terms from MeCab parse result
/// </summary>
const std::string ExtractTerm(const char *feature)
{
  std::string sentence;
  std::string term;
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
} // namespace GraphSeg::internal::utils

#endif