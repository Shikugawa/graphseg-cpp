#ifndef GRAPHSEG_INTERNAL_GRAPHSEG_SEGMENTATION_CONTAINER_H
#define GRAPHSEG_INTERNAL_GRAPHSEG_SEGMENTATION_CONTAINER_H

#include "graphseg/graph/undirected_graph.h"
#include "graphseg/embedding.h"
#include "graphseg/sentence.h"
#include "graphseg/lang.h"
#include "graphseg/internal/segmentable.h"

namespace GraphSeg
{
  using namespace graph;
  using std::vector;

  template <class Graph, int VectorDim, Lang LangType = Lang::EN>
  class SegmentationContainer
  {
    using SentenceType = Sentence<LangType>;

  public:
    SegmentationContainer(Graph&& ud, const Embedding<VectorDim, LangType>& em) 
      : graph(std::move(ud)), embedding(em)
    {}

    SegmentationContainer(const Graph& ud, const Embedding<VectorDim, LangType>& em) 
      : graph(ud), embedding(em)
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
    /// 最小セグメントサイズの左辺値参照を取得する
    /// </summary>
    size_t GetMinimumSegmentSize()
    {
      return segmentable.minimum_segment_size;
    }

    /// <summary>
    /// 最小セグメントサイズの左辺値参照を取得する
    /// </summary>
    void SetMinimumSegmentSize(size_t s)
    {
      segmentable.minimum_segment_size = s;
    }

    /// <summary>
    /// セグメントを返す
    /// </summary>
    inline const auto& GetSegment() const&
    {
      return segmentable.segments;
    }

    inline auto GetSegment() &&
    {
      return std::move(segmentable.segments);
    }

    /// <summary>
    /// Set sentence to graph vertex (lvalue & rvalue)
    /// </summary>
    void SetVertices(const vector<SentenceType>& ss)
    {
      for (const auto& s : ss)
      {
        graph.SetSentence(s);
        sentences.emplace_back(s);
      }
    }

    void SetVertices(vector<SentenceType>&& ss)
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
    void SetEdges()
    {
      const auto graph_size = graph.GetGraphSize();
      assert(graph_size > 1);
      vector<vector<int>> memo(graph_size, vector<int>(graph_size, 0));
      for (int i = 0; i < graph_size; ++i)
      {
        for (int j = 0; j < graph_size; ++j)
        {
          if ((memo[i][j] == 1 && memo[j][i] == 1) || i == j) continue;
          const auto similarity = embedding.GetSimilarity(sentences[i], sentences[j]);
          if (similarity > thereshold)
          {
            graph.SetEdge(i, j, similarity);
          }
          memo[i][j] = 1;
          memo[j][i] = 1;
        }
      }
    }

    void Segmentation()
    {
      segmentable = Segmentable<Graph, VectorDim, LangType>(graph);
      graph.SetMaximumClique();
      segmentable.ConstructSegment();
      graph.ConstructSegment(embedding);
    }

  private:
    /// <summary>
    /// ノードを連結するかどうかを決める閾値
    /// </summary>
    double thereshold;

    /// <summary>
    /// センテンスグラフ
    /// </summary>
    Graph graph;

    /// <summary>
    /// 文章
    /// </summary>    
    vector<SentenceType> sentences;

    /// <summary>
    /// 文章から収集した埋め込み情報
    /// </summary>
    Embedding<VectorDim, LangType> embedding;

    /// <summary>
    /// セグメントを行う機能
    /// </summary>
    internal::Segmentable<Graph, VectorDim, LangType> segmentable;
  };
}

#endif