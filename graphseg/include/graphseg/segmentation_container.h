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
  using std::vector, std::make_unique, std::shared_ptr, std::make_shared;

  template <class Graph, int VectorDim, Lang LangType = Lang::EN>
  class SegmentationContainer
  {
    using SentenceType = Sentence<LangType>;

  public:
    SegmentationContainer(const vector<SentenceType>& sentences, const Embedding<VectorDim, LangType>& em) 
      : graph(make_shared<Graph>(sentences)), embedding(em)
    {}

    SegmentationContainer(vector<SentenceType>&& sentences, const Embedding<VectorDim, LangType>& em) 
      : graph(make_shared<Graph>(sentences)), embedding(em)
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
      return *graph; 
    }

    inline Graph&& GetGraph() &&
    {
      return std::move(*graph);
    }

    /// <summary>
    /// get minimum segment size
    /// </summary>
    size_t GetMinimumSegmentSize()
    {
      assert(segmentable);
      return segmentable->minimum_segment_size;
    }

    /// <summary>
    /// set minumum segmentation size
    /// </summary>
    void SetMinimumSegmentSize(size_t s)
    {
      assert(segmentable);
      segmentable->releaseminimum_segment_size = s;
    }

    /// <summary>
    /// return segment
    /// </summary>
    inline const auto& GetSegment() const&
    {
      assert(segmentable);
      return segmentable->segments;
    }

    inline auto GetSegment() &&
    {
      assert(segmentable);
      return std::move(segmentable->segments);
    }

    /// <summary>
    /// Sentence graph instantiation
    /// </summary>
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
      segmentable = make_unique<internal::Segmentable<Graph, VectorDim, LangType>>(graph);
      graph->SetMaximumClique();
      segmentable->ConstructSegment(embedding);
    }

  private:
    /// <summary>
    /// Set sentence to graph vertex
    /// </summary>
    void SetVertices()
    {
      graph->SetNode();
    }

    /// <summary>
    /// Set weight calclated from sentence similarity by word embeddings
    /// </summary>
    void SetEdges()
    {
      const auto graph_size = graph->GetGraphSize();
      assert(graph_size > 1);
      vector<vector<int>> memo(graph_size, vector<int>(graph_size, 0));
      for (int i = 0; i < graph_size; ++i)
      {
        for (int j = 0; j < graph_size; ++j)
        {
          if ((memo[i][j] == 1 && memo[j][i] == 1) || i == j)
          {
            continue;
          }
          const auto similarity = embedding.GetSimilarity(graph->GetSentence(i), graph->GetSentence(j));
          #ifdef DEBUG
            std::cout << "sentence 1: " << graph->GetSentence(i).GetText() << std::endl;
            std::cout << "sentence 2: " << graph->GetSentence(j).GetText() << std::endl;
            std::cout << "similarity: " << similarity << std::endl;
            std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
          #endif
          if (similarity > thereshold)
          {
            graph->SetEdge(i, j, similarity);
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
    /// sentences
    /// </summary>    
    vector<SentenceType> sentences;

    /// <summary>
    /// Embedding information extracted from input sentences
    /// </summary>
    Embedding<VectorDim, LangType> embedding;

    /// <summary>
    /// sentence graph
    /// </summary>
    shared_ptr<Graph> graph;

    /// <summary>
    /// entity of segmentation operation 
    /// </summary>
    unique_ptr<internal::Segmentable<Graph, VectorDim, LangType>> segmentable;
  };
}

#endif