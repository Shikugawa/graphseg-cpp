#ifndef GRAPHSEG_CPP_GRAPHSEG_GRAPH_SEGMENT_GRAPH_HPP
#define GRAPHSEG_CPP_GRAPHSEG_GRAPH_SEGMENT_GRAPH_HPP

#include "graphseg/language.hpp"
#include "graphseg/sentence.hpp"

#include <vector>

namespace GraphSeg::graph {
using namespace GraphSeg;

/// <summary>
/// This abstruct class must be inherited if graph handles sentence
/// </summary>
template <class T, Lang LangType = Lang::EN> class SegmentGraph {
public:
  using SentenceType = Sentence<LangType>;

private:
  GRAPHSEG_INLINE_CONST T &Derived() const & {
    return static_cast<const T &>(*this);
  }

  T &&Derived() && { return static_cast<T &&>(*this); }

public:
  /// <summary>
  /// get specified sentence
  /// </summary>
  GRAPHSEG_INLINE_CONST SentenceType &GetSentence(size_t idx) const & {
    return sentences[idx];
  }

protected:
  SegmentGraph(const std::vector<SentenceType> &_sentences)
      : sentences(_sentences) {}

  SegmentGraph(std::vector<SentenceType> &&_sentences)
      : sentences(std::move(_sentences)) {}

  /// <summary>
  /// all of sentences
  /// </summary>
  std::vector<SentenceType> sentences;
};
} // namespace GraphSeg::graph

#endif