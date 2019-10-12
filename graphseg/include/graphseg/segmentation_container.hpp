#ifndef GRAPHSEG_INTERNAL_GRAPHSEG_SEGMENTATION_CONTAINER_HPP
#define GRAPHSEG_INTERNAL_GRAPHSEG_SEGMENTATION_CONTAINER_HPP

#include "graphseg/graph/undirected_graph.hpp"
#include "graphseg/embedding.hpp"
#include "graphseg/sentence.hpp"
#include "graphseg/language.hpp"
#include "graphseg/internal/segmentable.hpp"

namespace GraphSeg
{
  using namespace graph;
  using std::vector, std::make_unique, std::shared_ptr, std::make_shared;

  template <class Graph, int VectorDim, Lang LangType = Lang::EN>
  class SegmentOperator
  {
  public:
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

  protected:
    /// <summary>
    /// entity of segmentation operation 
    /// </summary>
    unique_ptr<internal::Segmentable<Graph, VectorDim, LangType>> segmentable;

    /// <summary>
    /// number of segments
    /// </summary>
    size_t numberSegment = 0;
  };
  
  template <int VectorDim, Lang LangType = Lang::EN>
  class EmbeddingOperator
  {
  protected:
    EmbeddingOperator(const Embedding<VectorDim, LangType>& _embedding) : embedding(_embedding)
    {}

    EmbeddingOperator(Embedding<VectorDim, LangType>&& _embedding) : embedding(std::move(_embedding))
    {}

    /// <summary>
    /// Embedding information extracted from input sentences
    /// </summary>
    Embedding<VectorDim, LangType> embedding;
  };

  template <class T, class Graph>
  class GraphOperator
  {
  public:
    /// <summary>
    /// Sentence graph instantiation
    /// </summary>
    void SetGraph()
    {
      SetVertices();
      SetEdges();
    }

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
      return *graph; 
    }

    inline Graph&& GetGraph() &&
    {
      return std::move(*graph);
    }

  private:
    const T& Derived() const& 
    {
      return static_cast<const T&>(*this);
    }

    T& Derived() & 
    {
      return static_cast<T&>(*this);
    }

    T&& Derived() && 
    {
      return static_cast<T&&>(*this);
    }

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
          const auto similarity = Derived().GetEmbedding().GetSimilarity(graph->GetSentence(i), graph->GetSentence(j));
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

  protected:
    GraphOperator(shared_ptr<Graph> _graph) : graph(_graph)
    {}

    /// <summary>
    /// sentence graph
    /// </summary>
    shared_ptr<Graph> graph;

    /// <summary>
    /// thershold whether connect nodes each other
    /// if node similarity is lower than thershold, these are not connected
    /// </summary>
    double thereshold;
  };

  template <class Graph, int VectorDim, Lang LangType = Lang::EN>
  class SegmentationContainer : public SegmentOperator<Graph, VectorDim, LangType>,
                                public EmbeddingOperator<VectorDim, LangType>,
                                public GraphOperator<SegmentationContainer<Graph, VectorDim, LangType>, Graph>
  {
    using SegmentOpr = SegmentOperator<Graph, VectorDim, LangType>;
    using GraphOpr = GraphOperator<SegmentationContainer<Graph, VectorDim, LangType>, Graph>;
    using EmbeddingOpr = EmbeddingOperator<VectorDim, LangType>;
    using SentenceType = Sentence<LangType>;

  public:
    SegmentationContainer(const vector<SentenceType>& sentences, const Embedding<VectorDim, LangType>& em)
      : GraphOpr(make_shared<Graph>(sentences)), EmbeddingOpr(em)
    {}

    SegmentationContainer(vector<SentenceType>&& sentences, const Embedding<VectorDim, LangType>& em) 
      : GraphOpr(make_shared<Graph>(sentences)), EmbeddingOpr(em)
    {}

    /// <summary>
    /// Execute segmentation
    /// </summary>
    void Segmentation()
    {
      SegmentOpr::segmentable = make_unique<internal::Segmentable<Graph, VectorDim, LangType>>(GraphOpr::graph);
      GraphOpr::graph->SetMaximumClique();
      SegmentOpr::segmentable->ConstructSegment(GetEmbedding());
    }

  private:
    const Embedding<VectorDim, LangType>& GetEmbedding()
    {
      return EmbeddingOpr::embedding;
    }

    friend GraphOpr;
  };
}

#endif