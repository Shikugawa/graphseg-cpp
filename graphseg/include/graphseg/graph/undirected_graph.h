#ifndef GRAPHSEG_CPP_GRAPHSEG_GRAPH_UNDIRECTED_GRAPH_H
#define GRAPHSEG_CPP_GRAPHSEG_GRAPH_UNDIRECTED_GRAPH_H 

#include "graphseg/internal/utils/nameof.hpp"
#include "graphseg/internal/segmentable.h"
#include "graphseg/sentence.h"

#include <string>
#include <iterator>
#include <memory>
#include <iostream>
#include <ostream>
#include <iterator>
#include <unordered_map>

namespace GraphSeg::graph
{
  using std::unordered_map, std::make_pair, std::string, std::set_intersection,
        std::set_union, std::set_difference, std::inserter, std::basic_ostream;

  /// <summary>
  /// 与えられた文章から構築された無向グラフ
  /// </summary>
  class UndirectedGraph : public Segmentable<UndirectedGraph>
  {
  public:
    using Edge = std::pair<Vertex, double>;
    using Base = Segmentable<UndirectedGraph>;

    explicit UndirectedGraph() = default;

    /// <summary>
    /// セグメントグラフにノードを与える
    /// </summary>
    void SetSentence(const Sentence& s)
    {
      ++node_idx;
      sentence_idx.emplace_back(s);
      graph.emplace_back(vector<Edge>());
    }

    void SetSentence(Sentence&& s)
    {
      ++node_idx;
      sentence_idx.emplace_back(std::move(s));
      graph.emplace_back(vector<Edge>());
    }

    void SetArc(int src, int dst, double score)
    {
      graph[src].emplace_back(make_pair(dst, score));
    }

    /// <summary>
    /// セグメントグラフにエッジを張る
    /// </summary>
    void SetEdge(int src, int dst, double score)
    {
      SetArc(src, dst, score);
      SetArc(dst, src, score);
    }

    /// <summary>
    /// グラフの大きさを取得する
    /// </summary>
    inline const Vertex& GetGraphSize() const&
    {
      return node_idx;
    }

    inline Vertex GetGraphSize() &&
    {
      return std::move(node_idx);
    }

    /// <summary>
    /// 最大クリークを計算する
    /// </summary>
    void SetMaximumClique()
    {
      vector<Vertex> tmp(node_idx);
      int i = -1;
      std::generate(tmp.begin(), tmp.end(), [&i](){ ++i; return i; });
      BronKerbosch(set<Vertex>(), set<Vertex>(tmp.begin(), tmp.end()), set<Vertex>());

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
    inline const Sentence& GetSentence(size_t idx) const&
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

    void ConstuctMaximumCliqueArrayContainer()
    {
      // TODO: 一度Setで最大クリークを構築するが、定数オーダーで最大クリークが欲しいのでベクターに変換しているが、メモリ効率が悪いし、変換処理も無駄
      max_cliques_internal.resize(node_idx);
      for (auto& max_clique: max_cliques_set)
      {
        for (auto& clique_vertex: max_clique)
        {
          std::cout << clique_vertex << std::endl;
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
    vector<Sentence> sentence_idx;

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
    Vertex node_idx = 0;

    friend Base;
  };
} // namespace GraphSeg

#endif