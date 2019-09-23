#ifndef GRAPHSEG_CPP_GRAPHSEG_INTERNAL_FREQUENCY_H
#define GRAPHSEG_CPP_GRAPHSEG_INTERNAL_FREQUENCY_H

#include "graphseg/internal/utils/exec.h"
#include "graphseg/lang.h"

#include <string>
#include <utility>
#include <type_traits>
#include <unordered_map>
#include <rapidjson/document.h>
#include <iostream>
#include <cstring>

namespace GraphSeg::internal
{
  using std::unordered_map, std::string, std::enable_if_t, std::is_same_v;
  using namespace rapidjson;

  template <Lang LangType = Lang::EN>
  class Frequency
  {
  public:
    Frequency(const string& stream)
    {
      AddFrequency(std::forward<const string>(stream));
    }

    Frequency(string&& stream)
    {
      AddFrequency(std::forward<string&&>(stream));
    }

    /// <summary>
    /// get term frequency ratio
    /// </summary>
    inline const unsigned int GetFrequency(const string& term)
    {
      return frequency_count[term];
    }

    /// <summary>
    /// number of terms
    /// </summary>
    inline const unsigned int& GetTotalCount() noexcept
    { 
      return total_count; 
    }

    /// <summary>
    /// get corpus size
    /// </summary>
    inline const unsigned int& GetCorpusSize() noexcept
    { 
      return corpus_size; 
    }

  private:
    template <typename T, typename = enable_if_t<is_same_v<string, std::remove_cv_t<T>>>*>
    void AddFrequency(T&& stream)
    {
      int code;
      const string cmd = "echo " + stream + " | " + I18NFactory<LangType>::CommandBaseExtractor() + "/frequency.py";
      auto result = utils::exec(cmd.c_str(), code);
      Document doc;
      const auto parse_result = doc.Parse(result.c_str()).HasParseError();
      assert(parse_result == false);
      for (auto itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr)
      {
        const auto& term = itr->name.GetString();
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
    unordered_map<string, unsigned int> frequency_count;
  };
}

#endif
