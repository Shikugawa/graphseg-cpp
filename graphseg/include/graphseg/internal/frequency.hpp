#ifndef GRAPHSEG_CPP_GRAPHSEG_INTERNAL_FREQUENCY_HPP
#define GRAPHSEG_CPP_GRAPHSEG_INTERNAL_FREQUENCY_HPP

#include "graphseg/language.hpp"

#include <string>
#include <utility>
#include <type_traits>
#include <unordered_map>
#include <rapidjson/document.h>
#include <iostream>
#include <cstring>

namespace GraphSeg::internal
{
using namespace rapidjson;

template <Lang LangType = Lang::EN>
class Frequency : public Executable<LangType>
{
  using Base = Executable<LangType>;

public:
  explicit Frequency(const std::string &stream)
  {
    AddFrequency(stream);
  }

  explicit Frequency(std::string &&stream)
  {
    AddFrequency(std::move(stream));
  }

  /// <summary>
  /// get term frequency ratio
  /// </summary>
  GRAPHSEG_INLINE_CONST unsigned int GetFrequency(const std::string &term)
  {
    return frequency_count[term];
  }

  /// <summary>
  /// number of terms
  /// </summary>
  GRAPHSEG_INLINE_CONST unsigned int &GetTotalCount() noexcept
  {
    return total_count;
  }

  /// <summary>
  /// get corpus size
  /// </summary>
  GRAPHSEG_INLINE_CONST unsigned int &GetCorpusSize() noexcept
  {
    return corpus_size;
  }

private:
  template <typename T, std::enable_if_t<std::is_same_v<std::string, std::decay_t<T>>> * = nullptr>
  void AddFrequency(T &&stream)
  {
    auto result = Base::Execute("frequency.py", std::forward<T>(stream));
    Document doc;
    const auto parse_result = doc.Parse(result.c_str()).HasParseError();
    assert(parse_result == false);
    for (auto itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr)
    {
      const auto &term = itr->name.GetString();
      assert(doc.HasMember(term));
      if (std::string(term) == "corpus_size")
      {
        corpus_size = doc[term].GetUint();
        continue;
      }
      if (std::string(term) == "total_count")
      {
        total_count = doc[term].GetUint();
        continue;
      }
      frequency_count[term] = doc[term].GetUint();
    }
  }

  /// <summary>
  /// Σ_{w'∈C}freq(w')
  /// </summary>
  unsigned int total_count;

  /// <summary>
  /// |C|
  /// </summary>
  unsigned int corpus_size;

  /// <summary>
  /// term count
  /// </summary>
  std::unordered_map<std::string, unsigned int> frequency_count;
};
} // namespace GraphSeg::internal

#endif
