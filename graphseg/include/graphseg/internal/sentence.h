#ifndef GRAPHSEG_CPP_GRAPHSEG_SENTENCE_H
#define GRAPHSEG_CPP_GRAPHSEG_SENTENCE_H
#include <string>
#include <vector>
#include <iostream>

namespace GraphSeg
{
  using std::string, std::vector;

  class Sentence
  {
    using iterator = vector<string>::iterator;
    using const_iterator = vector<string>::const_iterator;

    const char delim = ' ';

  public:
    Sentence(string&& s) : sentence(std::move(s))
    {
      string item;
      for (auto itr = sentence.begin(); itr != sentence.end(); ++itr)
      {
        if (*itr == delim)
        {
          terms.emplace_back(item);
          item.clear();
          continue;
        }
        item += *itr;
      }
      terms.emplace_back(item);
    }

    /// <summary>
    /// apply Iterable
    /// </summary>
    inline iterator begin() noexcept { return terms.begin(); }
    
    inline const_iterator cbegin() const noexcept { return terms.cbegin(); }
    
    inline iterator end() noexcept { return terms.end(); }
    
    inline const_iterator cend() const noexcept { return terms.cend(); }

    /// <summary>
    /// get all terms in a sentence
    /// </summary>
    const vector<string>& GetTerms() const& { return terms; }

    /// <summary>
    /// get sentence length
    /// </summary>
    size_t GetSize() const& { return terms.size(); }

    /// <summary>
    /// get term from index
    /// </summary>
    const string& operator[](size_t idx) const& { return terms[idx]; }

    const string& GetText() const& { return sentence; }
  
  private:
    string sentence;
    vector<string> terms;
  };
} // GraohSeg

#endif