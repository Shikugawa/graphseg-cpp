#ifndef GRAPHSEG_INTERNAL_GRAPHSEG_SEGMENTATION_CONTAINER_H
#define GRAPHSEG_INTERNAL_GRAPHSEG_SEGMENTATION_CONTAINER_H

#include "graphseg/graph/undirected_graph.h"
#include "graphseg/embedding.h"
#include "graphseg/sentence.h"

namespace GraphSeg
{
  using namespace graph;
  using std::vector;

  template <class Graph>
  class SegmentationContainer
  {
  public:
    SegmentationContainer(Graph&& ud) : graph(std::move(ud))
    {}

    SegmentationContainer(const Graph& ud) : graph(ud)
    {}

    /// <summary>
    /// Set edge weight threshold in graph. If edge weight is higher than it, both vertex are connected each other (lvalue & rvalue)
    /// </summary>
    inline void SetThreshold(const double& thd) noexcept 
    { 
      thereshold = thd; 
    }

    inline void SetThreshold(double&& thd) noexcept 
    { 
      thereshold = std::move(thd); 
    }

    /// <summary>
    /// Get graph (lvalue & rvalue)
    /// </summary>
    inline Graph& GetGraph() &
    { 
      return graph; 
    }

    inline Graph&& GetGraph() &&
    {
      return std::move(graph);
    }

    /// <summary>
    /// Set sentence to graph vertex (lvalue & rvalue)
    /// </summary>
    void SetVertices(const vector<Sentence>& ss)
    {
      for (const auto& s : ss)
      {
        graph.SetSentence(s);
        sentences.emplace_back(s);
      }
    }

    void SetVertices(vector<Sentence>&& ss)
    {
      for (auto&& s : std::move(ss))
      {
        graph.SetSentence(s);
        sentences.emplace_back(s);
      }
    }

    /// <summary>
    /// Set weight calclated from sentence similarity by word embeddings
    /// </summary>
    void SetEdges(Embedding& em)
    {
      const auto graph_size = graph.GetGraphSize();
      assert(graph_size > 1);
      vector<vector<int>> memo(graph_size, vector<int>(graph_size, 0));
      for (int i = 0; i < graph_size; ++i)
      {
        for (int j = 0; j < graph_size; ++j)
        {
          if ((memo[i][j] == 1 && memo[j][i] == 1) || i == j) continue;
          const auto similarity = em.GetSimilarity(sentences[i], sentences[j]);
          if (similarity > thereshold)
          {
            graph.SetEdge(i, j, similarity);
          }
          memo[i][j] = 1;
          memo[j][i] = 1;
        }
      }
    }

  private:
    double thereshold;
    UndirectedGraph graph;
    vector<Sentence> sentences;
  };
}

#endif