#ifndef GRAPHSEG_CPP_GRAPHSEG_INTERNAL_FREQUENCY_H
#define GRAPHSEG_CPP_GRAPHSEG_INTERNAL_FREQUENCY_H

#include <string>
#include <unordered_map>

namespace GraphSeg
{
  using std::unordered_map, std::string;

  class Frequency
  {
  public:
    /// <summary>
    /// n-gram corpus parh
    /// </summary>
    Frequency(string& path)
    {

    }

    Frequency(const Frequency&) = delete;

    Frequency& operator=(const Frequency&)
    {}

    double GetFrequency(const string& term)
    {

    }
  private:
    unordered_map<string, unsigned int> frequency;
  }
}

#endif