#ifndef GRAPHSEG_CPP_GRAPHSEG_INTERNAL_FREQUENCY_H
#define GRAPHSEG_CPP_GRAPHSEG_INTERNAL_FREQUENCY_H

#include "utils/exec.h"

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

  const string home = getenv("HOME");
  const string COMMAND_FREQUENCY = home + "/.pyenv/shims/python" + " " + home + "/graphseg-cpp/script/frequency.py";

  class Frequency
  {
  public:
    Frequency(const string& stream)
    {
      AddFrequency(std::forward<const string>(stream));
    }

    Frequency(string&& stream)
    {
      AddFrequency(std::forward<string>(stream));
    }

    Frequency(const Frequency&) = delete;

    Frequency& operator=(const Frequency&) = delete;

    /// <summary>
    /// 単語の頻度を得る
    /// </summary>
    double GetFrequency(const string& term)
    {
      return frequency[term];
    }

    /// <summary>
    /// 総単語数
    /// </summary>
    inline const uint64_t& GetSize() noexcept
    { 
      return total_count; 
    }

    /// <summary>
    /// 頻度の合計を与える
    /// </summary>
    inline const uint64_t& GetSumFreq() noexcept 
    { 
      return sum_frequency; 
    }

  private:
    template <typename T, typename = enable_if_t<is_same_v<string, std::remove_cv_t<T>>>*>
    void AddFrequency(T&& stream)
    {
      int code;
      const string cmd = "echo " + stream + " | " + COMMAND_FREQUENCY;
      auto result = utils::exec(cmd.c_str(), code);
      Document doc;
      const auto parse_result = doc.Parse(result.c_str()).HasParseError();
      assert(parse_result == false);
      for (auto itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr)
      {
        const auto term = itr->name.GetString();
        assert(doc.HasMember(term));
        if (!std::strcmp("total_count", term))
        {
          total_count = doc[term].GetInt();
          continue;
        }
        const auto& freq = doc[term].GetDouble();
        sum_frequency += freq;
        frequency[term] = freq;
      }
    }

    unsigned int total_count;
    double sum_frequency;
    unordered_map<string, double> frequency;
  };
}

#endif
