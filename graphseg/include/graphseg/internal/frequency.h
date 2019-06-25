#ifndef GRAPHSEG_CPP_GRAPHSEG_INTERNAL_FREQUENCY_H
#define GRAPHSEG_CPP_GRAPHSEG_INTERNAL_FREQUENCY_H

#include "util.h"
#include <string>
#include <unordered_map>
#include <rapidjson/document.h>

namespace GraphSeg
{
  using std::unordered_map, std::string;
  using namespace rapidjson;

  const string home = getenv("HOME");
  const string COMMAND = home + "/.pyenv/shims/python" + " " + home + "/graphseg-cpp/script/frequency.py";

  class Frequency
  {
  public:
    /// <summary>
    /// { total_count: <total number of word>, word1: ... }
    /// </summary>
    Frequency(const string& stream)
    {
      int code;
      const string cmd = "echo " + stream + " | " + COMMAND;
      auto result = exec(cmd.c_str(), code);
      Document doc;
      const auto parse_result = doc.Parse(result.c_str()).HasParseError();
      for (auto itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr)
      {
        const auto term = itr->name.GetString();
        assert(doc.HasMember(term));
        if (term == "total_count")
        {
          total_count = doc[term].GetInt();
          continue;
        }
        const auto& freq = doc[term].GetDouble();
        sum_frequency += freq;
        frequency[term] = freq;
      }
    }

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
    unsigned int GetSize() noexcept { return total_count; }

    /// <summary>
    /// 頻度の合計を与える
    /// </summary>
    unsigned int GetSumFreq() noexcept { return sum_frequency; }

  private:
    unsigned int total_count;
    unsigned int sum_frequency;
    unordered_map<string, double> frequency;
  }
}

#endif
