#ifndef GRAPHSEG_CPP_GRAPHSEG_GRAPH_UNDIRECTED_GRAPH_H
#define GRAPHSEG_CPP_GRAPHSEG_GRAPH_UNDIRECTED_GRAPH_H 

#include "graphseg/internal/utils/nameof.hpp"
#include "graphseg/lang.h"
#include "graphseg/sentence.h"

#include <set>
#include <string>
#include <iterator>
#include <memory>
#include <iostream>
#include <ostream>
#include <iterator>
#include <unordered_map>

namespace GraphSeg::graph
{
  using std::set, std::unordered_map, std::make_pair, std::string, std::set_intersection,
        std::set_union, std::set_difference, std::inserter, std::basic_ostream;

  template <typename T, typename = void>
  struct is_iterable : std::false_type
  {};

  template <typename T>
  struct is_iterable<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>> : std::true_type
  {};

  template <typename T, bool = is_iterable<typename T::reference>::value>
  struct is_valid_iterable : std::false_type
  {};

  template <typename T>
  struct is_valid_iterable<T, true> : std::true_type
  {};
  
  template <typename T, typename = std::enable_if_t<is_iterable<T>::value>*>
  T operator&(const T& v1, const T& v2)
  {
    T result;
    set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), inserter(result, result.end()));
    return result;
  }

  template <typename T, typename = std::enable_if_t<is_iterable<T>::value>*>
  T operator+(const T& v1, const T& v2)
  {
    T result;
    set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), inserter(result, result.end()));
    return result;
  }

  template <typename T, typename = std::enable_if_t<is_iterable<T>::value>*>
  T operator-(const T& v1, const T& v2)
  {
    set<T> result;
    set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), inserter(result, result.end()));
    return result;
  }
  
  template <typename T, typename = std::enable_if_t<is_valid_iterable<T>::value>*>
  std::ostream& operator<<(std::ostream& os, const T& segments)
  {
    for (const auto& segment : segments)
    {
      std::string vertex_node;
      for (const auto& segment_vertex : segment)
      {
        vertex_node += std::to_string(segment_vertex);
        vertex_node += " ";
      }
      os << vertex_node << std::endl;
    }
    return os;
  }
  
  /// <summary>
  /// Undirected graph based on passed sentences
  /// </summary>
  template <Lang LangType = Lang::EN>
  class UndirectedGraph
  {
  public:
    using Vertex = unsigned int;
    using VertexSet = set<Vertex>;
    using SentenceType = Sentence<LangType>;
    using Edge = std::pair<Vertex, double>;

    explicit UndirectedGraph() = default;

    explicit UndirectedGraph(const vector<SentenceType>& _sentences)
      : graph_size(_sentences.size()), sentences(_sentences)
    {
      graph.resize(graph_size*graph_size);
    }

    explicit UndirectedGraph(SentenceType&& _sentences)
      : graph_size(_sentences.size()), sentences(std::move(_sentences))
    {
      graph.resize(graph_size*graph_size);
    }

    /// <summary>
    /// Add node to segment graph
    /// </summary>
    void SetSentence()
    {
      for (size_t i = 0; i < sentences.size(); ++i)
      {
        graph.emplace_back(vector<Edge>());
      }      
    }

    /// <summary>
    /// pass edges to nodes
    /// </summary>
    void SetEdge(int src, int dst, double score)
    {
      assert(src < graph_size && dst < graph_size);
      SetArc(src, dst, score);
      SetArc(dst, src, score);
    }

    /// <summary>
    /// グラフの大きさを取得する
    /// </summary>
    inline const Vertex& GetGraphSize() const&
    {
      return graph_size;
    }

    inline Vertex GetGraphSize() &&
    {
      return std::move(graph_size);
    }

    /// <summary>
    /// calculate maximum clique
    /// </summary>
    void SetMaximumClique()
    {
      vector<Vertex> tmp(graph_size);
      int i = -1;
      std::generate(tmp.begin(), tmp.end(), [&i](){ ++i; return i; });
      BronKerbosch(set<Vertex>(), set<Vertex>(tmp.begin(), tmp.end()), set<Vertex>());
      ConstructMaximumCliqueArrayContainer();
#ifdef DEBUG
      std::cout << "===== Retrieved Maximum Cliques =====" << std::endl;
      std::cout << max_cliques_set << std::endl;
      std::cout << "=====================================" << std::endl;
#endif
    }

    /// <summary>
    /// get caluclated maximum clique
    /// </summary>
    inline const set<VertexSet>& GetMaximumClique() const&
    {
      return max_cliques_set;
    }

    inline set<VertexSet> GetMaximumClique() &&
    {
      return std::move(max_cliques_set);
    }

    /// <summary>
    /// get sentence
    /// </summary>
    inline const SentenceType& GetSentence(size_t idx) const&
    { 
      return sentences[idx]; 
    }

    /// <summary>
    /// get node number and weight that passed adjacent nodes
    /// </summary>
    inline const vector<Edge>& operator[](size_t idx) const& 
    { 
      return graph[idx]; 
    }

    VertexSet GetAdjacentNodes(const size_t idx)
    {
      VertexSet adjacents;
      for (const auto& [adjacent_node_id, edge_weight]: graph[idx])
      {
        adjacents.insert(adjacent_node_id);
      }
      return adjacents;
    }
    
  private:
    void SetArc(int src, int dst, double score)
    {
      auto pair = make_pair(dst, score);
      graph[src].emplace_back(pair);
    }

    void BronKerbosch(set<Vertex> clique, set<Vertex> candidates, set<Vertex> excluded)
    {
      if (candidates.empty() && excluded.empty())
      { 
        max_cliques_set.insert(clique);
        return;
      }

      // TODO(rei.shimizu): It may be invalid solution to resolve iterator breakdown problem
      set<Vertex> candidates_tmp;
      for (auto itr2 = candidates.begin(); itr2 != candidates.end(); ++itr2)
      {
        if (candidates_tmp.find(*itr2) != candidates_tmp.end()) break;
        auto v = *itr2;
        auto clique_t = clique + set<Vertex>({v});
        auto candidates_t = candidates & GetNeighbors(v);
        auto excluded_t = excluded & GetNeighbors(v);
        
        assert(clique_t.size() > clique.size());
        assert(candidates_t.size() <= candidates.size());
        assert(excluded_t.size() <= excluded.size());

        BronKerbosch(clique_t, candidates_t, excluded_t);

        candidates_tmp.insert(v);  
        excluded.insert(v);
      }      
    }

    void ConstructMaximumCliqueArrayContainer()
    {
      // TODO: 一度Setで最大クリークを構築するが、定数オーダーで最大クリークが欲しいのでベクターに変換しているが、メモリ効率が悪いし、変換処理も無駄
      max_cliques_internal.resize(graph_size);
      for (auto& max_clique: max_cliques_set)
      {
        for (auto& clique_vertex: max_clique)
        {
          max_cliques_internal[clique_vertex].emplace_back(
            vector<Vertex>(max_clique.begin(), max_clique.end())
          );
        }
      }
    }
    
    set<Vertex> GetNeighbors(Vertex idx)
    {
      set<Vertex> tmp;
      for(const auto& node: graph[idx])
      {
        tmp.emplace(node.first);
      }
      return tmp;
    }

    /// <summary>
    /// Base graph
    /// </summary>
    vector<vector<Edge>> graph;

    /// <summary>
    /// all of sentences
    /// </summary>
    vector<SentenceType> sentences;

    /// <summary>
    /// set of maximum clique
    /// </summary>
    set<VertexSet> max_cliques_set;

    /// <summary>
    /// Data structure to get maximum clique with O(N)
    /// <summary>
    vector<vector<vector<Vertex>>> max_cliques_internal;

    /// <summary>
    /// current graph size
    /// </summary>
    Vertex graph_size = 0;
  };
} // namespace GraphSeg

#endif