#ifndef GRAPHSEG_CPP_GRAPHSEG_INTERNAL_UTIL_STRING_HPP
#define GRAPHSEG_CPP_GRAPHSEG_INTERNAL_UTIL_STRING_HPP

#include <type_traits>
#include <locale>
#include <codecvt>
#include <string>
#include <iostream>
#include <vector>

namespace GraphSeg::internal::utils
{
std::string ConvertString(const std::wstring &wstr)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
  return cv.to_bytes(wstr);
}

std::wstring ConvertWstring(std::string str, std::string localeStr)
{
  std::wcout.imbue(std::locale(localeStr));
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
  return cv.from_bytes(str);
}

// TODO: use template specialization
std::vector<std::wstring> Split(const std::wstring &str, wchar_t delim = L' ')
{
  std::vector<std::wstring> v;
  std::wstring ch;
  for (auto symbol : str)
  {
    if (symbol == delim)
    {
      v.emplace_back(ch);
      ch.clear();
    }
    else
    {
      ch += symbol;
    }
  }
  if (ch.size() != 0)
  {
    v.emplace_back(ch);
  }
  return v;
}

std::vector<std::string> SplitString(const std::string &str, char delim = ' ')
{
  std::vector<std::string> v;
  std::string ch;
  for (auto symbol : str)
  {
    if (symbol == delim)
    {
      v.emplace_back(ch);
      ch.clear();
    }
    else
    {
      ch += symbol;
    }
  }
  if (ch.size() != 0)
  {
    v.emplace_back(ch);
  }
  return v;
}

template <class T>
T Join(const std::vector<T> &ary)
{
  T s;
  for (const auto &a : ary)
  {
    s += a;
  }
  return s;
}

template <class T>
T FindReplace(T s, T s1, T s2)
{
  auto pos = s.find(s1);
  auto len = s1.length();
  if (pos != std::string::npos)
  {
    s.replace(pos, len, s2);
  }
  return s;
}
} // namespace GraphSeg::internal::utils

#endif
