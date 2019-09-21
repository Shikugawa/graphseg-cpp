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
  /// 与えられた文章から構築された無向グラフ
  /// </summary>
  template <Lang LangType = Lang::EN>
  class UndirectedGraph
  {
  public:
    using Vertex = unsigned int;
    using VertexSet = set<Vertex>;
    using SentenceType = Sentence<LangType>;
    using Edge = std::pair<Vertex, double>;

    /// <summary>
    /// グラフに文章を与えない場合
    /// </summary>
    explicit UndirectedGraph(uint32_t _graph_size) : graph_size(_graph_size)
    {
      graph.resize(graph_size*graph_size);
    }

    explicit UndirectedGraph() = default;

    /// <summary>
    /// セグメントグラフにノードを与える
    /// </summary>
    void SetSentence(const SentenceType& s)
    {
      sentence_idx.emplace_back(s);
      graph.emplace_back(vector<Edge>());
    }

    void SetSentence(SentenceType&& s)
    {
      sentence_idx.emplace_back(std::move(s));
      graph.emplace_back(vector<Edge>());
    }

    /// <summary>
    /// セグメントグラフにエッジを張る
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
    /// 最大クリークを計算する
    /// </summary>
    void SetMaximumClique()
    {
      vector<Vertex> tmp(graph_size);
      int i = -1;
      std::generate(tmp.begin(), tmp.end(), [&i](){ ++i; return i; });
      BronKerbosch(set<Vertex>(), set<Vertex>(tmp.begin(), tmp.end()), set<Vertex>());
      ConstructMaximumCliqueArrayContainer();
#ifdef DEBUG
      // spdlog::info("===== Retrieved Maximum Cliques =====");
      // std::cout << max_cliques_set << std::endl;
      // spdlog::info("=====================================");
#endif
    }

    /// <summary>
    /// 計算した最大クリークを取得する
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
    /// 文章を取得する
    /// </summary>
    inline const SentenceType& GetSentence(size_t idx) const&
    { 
      return sentence_idx[idx]; 
    }

    /// <summary>
    /// 指定したノードに隣接しているノードの番号と重みを取得する
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

    vector<vector<Edge>> graph;

    /// <summary>
    /// グラフのノードIDとセンテンスの対応付
    /// </summary
    vector<SentenceType> sentence_idx;

    /// <summary>
    /// 最大クリーク
    /// </summary>
    set<VertexSet> max_cliques_set;

    /// <summary>
    /// 定数時間で計算済み最大クリークを取得出来る
    /// <summary>
    vector<vector<vector<Vertex>>> max_cliques_internal;

    /// <summary>
    /// 現在のグラフサイズ
    /// </summary>
    Vertex graph_size = 0;
  };
} // namespace GraphSeg

#endif