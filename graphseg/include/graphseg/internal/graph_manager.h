#ifndef GRAPHSEG_INTERNAL_GRAPHSEG_MANAGER_H
#define GRAPHSEG_INTERNAL_GRAPHSEG_MANAGER_H

#include "segment_graph.h"
#include "embedding_manager.h"
#include "sentence.h"
#include <vector>
#include <iostream>

namespace GraphSeg
{
  using std::vector;

  class GraphManager
  {
  public:
    GraphManager(SegmentGraph& sg) : graph(sg)
    {}

    GraphManager(SegmentGraph&& sg) : graph(std::move(sg))
    {}

    void SetVertices(const vector<Sentence>& ss)
    {
      for (const auto& s : ss)
      {
        graph.SetSentence(s);
        sentences.emplace_back(s);
      }
    }

    void SetEdges(EmbeddingManager& em)
    {
      const auto graph_size = graph.GetGraphSize();
      assert(graph_size > 1);
      vector<vector<int>> memo(graph_size, vector<int>(graph_size, 0));
      for (int i = 0; i < graph_size; ++i)
      {
        for (int j = 0; j < graph_size; ++j)
        {
          if ((memo[i][j] == 1 && memo[j][i] == 1) || i == j) continue;
          graph.SetEdge(i, j, em.GetSimilarity(sentences[i], sentences[j]));
          memo[i][j] = 1;
          memo[j][i] = 1;
        }
      }
    }

    const SegmentGraph& GetGraph() const& { return graph; }

  private:
    SegmentGraph graph;
    vector<Sentence> sentences;
  };
}

#endif