#ifndef GRAPHSEG_INTERNAL_GRAPHSEG_GRAPH_CONTROLLER_H
#define GRAPHSEG_INTERNAL_GRAPHSEG_GRAPH_CONTROLLER_H

#include "segment_graph.h"
#include "embedding.h"
#include "sentence.h"
#include <vector>
#include <iostream>

namespace GraphSeg::internal
{
  using std::vector;

  class GraphController
  {
  public:
    GraphController(SegmentGraph&& sg) : graph(std::move(sg))
    {}

    /// <summary>
    /// エッジ重みの閾値を設定（右辺値&左辺値）
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
    /// グラフを取得する
    /// </summary>
    inline SegmentGraph& GetGraph() &
    { 
      return graph; 
    }

    inline SegmentGraph&& GetGraph() &&
    {
      return std::move(graph);
    }

    /// <summary>
    /// 与えられた文章をグラフの頂点に設定する
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
    /// 文章の類似度を計算して重みを設定する
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
    SegmentGraph graph;
    vector<Sentence> sentences;
  };
}

#endif