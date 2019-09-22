#ifndef GRAPHSEG_CPP_GRAPHSEG_GRAPH_SEGMENT_GRAPH_H
#define GRAPHSEG_CPP_GRAPHSEG_GRAPH_SEGMENT_GRAPH_H 

#include "graphseg/sentence.h"
#include "graphseg/lang.h"

#include <vector>

namespace GraphSeg::graph
{
  using std::vector;
  using namespace GraphSeg;

  /// <summary>
  /// This abstruct class must be inherited if graph handles sentence
  /// </summary>
  template <class T, Lang LangType = Lang::EN>
  class SegmentGraph
  {
  public:
    using SentenceType = Sentence<LangType>;

  private:
    const T& Derived() const&
    {
      return static_cast<const T&>(*this);
    }

    T&& Derived() &&
    {
      return static_cast<T&&>(*this);
    }

  public:
    /// <summary>
    /// get specified sentence
    /// </summary>
    inline const SentenceType& GetSentence(size_t idx) const&
    { 
      return sentences[idx]; 
    }

  protected:
    SegmentGraph(const vector<SentenceType>& _sentences) : sentences(_sentences)
    {}

    SegmentGraph(vector<SentenceType>&& _sentences) : sentences(std::move(_sentences))
    {}

    /// <summary>
    /// all of sentences
    /// </summary>
    vector<SentenceType> sentences;
  };
}

#endif