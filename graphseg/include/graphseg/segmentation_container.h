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
    /// Set edge weight threshold in graph. If edge weight is higher than it, both vertex are connected each other (lvalue & rvalue)
    /// </summary>
    inline void SetNumberSegment(const size_t& n) noexcept 
    {
      assert(n > 0);
      numberSegment = n;
    }

    inline void SetNumberSegment(size_t&& n) noexcept 
    { 
      assert(n > 0);
      numberSegment = std::move(n);
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
    /// get minimum segment size
    /// </summary>
    size_t GetMinimumSegmentSize()
    {
      return segmentable.minimum_segment_size;
    }

    /// <summary>
    /// set minumum segmentation size
    /// </summary>
    void SetMinimumSegmentSize(size_t s)
    {
      segmentable.minimum_segment_size = s;
    }

    /// <summary>
    /// return segment
    /// </summary>
    inline const auto& GetSegment() const&
    {
      return segmentable.segments;
    }

    inline auto GetSegment() &&
    {
      return std::move(segmentable.segments);
    }

    void SetGraph()
    {
      SetVertices();
      SetEdges();
    }

    /// <summary>
    /// Execute segmentation
    /// </summary>
    void Segmentation()
    {
      segmentable = Segmentable<Graph, VectorDim, LangType>(graph);
      graph.SetMaximumClique();
      segmentable.ConstructSegment();
      graph.ConstructSegment(embedding);
    }

  private:
    /// <summary>
    /// Set sentence to graph vertex
    /// </summary>
    void SetVertices()
    {
      graph.SetNode();
    }

    /// <summary>
    /// Set weight calclated from sentence similarity by word embeddings
    /// </summary>
    void SetEdges()
    {
      const auto graph_size = graph.GetGraphSize();
      std::cout << graph_size << std::endl;
      assert(graph_size > 1);
      vector<vector<int>> memo(graph_size, vector<int>(graph_size, 0));
      for (int i = 0; i < graph_size; ++i)
      {
        for (int j = 0; j < graph_size; ++j)
        {
          if ((memo[i][j] == 1 && memo[j][i] == 1) || i == j) continue;
          const auto similarity = embedding.GetSimilarity(graph.GetSentence(i), graph.GetSentence(j));
          std::cout << similarity << std::endl;
          if (similarity > thereshold)
          {
            graph.SetEdge(i, j, similarity);
          }
          memo[i][j] = 1;
          memo[j][i] = 1;
        }
      }
    }

    /// <summary>
    /// thershold whether connect nodes each other
    /// if node similarity is lower than thershold, these are not connected
    /// </summary>
    double thereshold;

    /// <summary>
    /// number of segments
    /// </summary>
    size_t numberSegment = 0;

    /// <summary>
    /// sentence graph
    /// </summary>
    Graph graph;

    /// <summary>
    /// sentences
    /// </summary>    
    vector<SentenceType> sentences;

    /// <summary>
    /// Embedding information extracted from input sentences
    /// </summary>
    Embedding<VectorDim, LangType> embedding;

    /// <summary>
    /// entity of segmentation operation
    /// </summary>
    internal::Segmentable<Graph, VectorDim, LangType> segmentable;
  };
}

#endif