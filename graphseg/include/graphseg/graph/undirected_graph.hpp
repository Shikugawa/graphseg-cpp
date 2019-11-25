#ifndef GRAPHSEG_CPP_GRAPHSEG_GRAPH_UNDIRECTED_GRAPH_HPP
#define GRAPHSEG_CPP_GRAPHSEG_GRAPH_UNDIRECTED_GRAPH_HPP

#include "graphseg/graph/segment_graph.hpp"
#include "graphseg/internal/utils/custom_operator.hpp"
#include "graphseg/internal/utils/nameof.hpp"
#include "graphseg/language.hpp"

#include <iostream>
#include <iterator>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <unordered_map>

namespace GraphSeg::graph {
using namespace GraphSeg::internal::utils;

/// <summary>
/// Undirected graph based on passed sentences
/// </summary>
template <Lang LangType = Lang::EN>
class UndirectedGraph
    : public SegmentGraph<UndirectedGraph<LangType>, LangType> {
public:
  using Vertex = unsigned int;
  using VertexSet = std::set<Vertex>;
  using Edge = std::pair<Vertex, double>;
  using Base = SegmentGraph<UndirectedGraph<LangType>, LangType>;

  explicit UndirectedGraph() = default;

  explicit UndirectedGraph(
      const std::vector<typename Base::SentenceType> &_sentences)
      : Base(_sentences), graph_size(_sentences.size()) {
    graph.resize(graph_size * graph_size);
  }

  explicit UndirectedGraph(
      std::vector<typename Base::SentenceType> &&_sentences)
      : Base(std::move(_sentences)), graph_size(_sentences.size()) {
    graph.resize(graph_size * graph_size);
  }

  /// <summary>
  /// Add node to segment graph
  /// </summary>
  void SetNode() {
    for (size_t i = 0; i < graph_size; ++i) {
      graph.emplace_back(std::vector<Edge>());
    }
  }

  /// <summary>
  /// pass edges to nodes
  /// </summary>
  void SetEdge(int src, int dst, double score) {
    assert(src < graph_size && dst < graph_size);
    SetArc(src, dst, score);
    SetArc(dst, src, score);
  }

  /// <summary>
  /// get graph size
  /// </summary>
  GRAPHSEG_INLINE_CONST Vertex &GetGraphSize() const & { return graph_size; }

  GRAPHSEG_INLINE_CONST Vertex GetGraphSize() && {
    return std::move(graph_size);
  }

  /// <summary>
  /// calculate maximum clique
  /// </summary>
  void SetMaximumClique() {
    std::vector<Vertex> tmp(graph_size);
    int i = -1;
    std::generate(tmp.begin(), tmp.end(), [&i]() {
      ++i;
      return i;
    });
    BronKerbosch(std::set<Vertex>(), std::set<Vertex>(tmp.begin(), tmp.end()),
                 std::set<Vertex>());
    ConstructMaximumCliqueArrayContainer();
#ifdef DEBUG
    std::cout << "===== Retrieved Maximum Cliques =====" << std::endl;
    std::cout << max_cliques_set << std::endl;
#endif
  }

  /// <summary>
  /// get caluclated all of maximum cliques
  /// </summary>
  GRAPHSEG_INLINE_CONST std::set<VertexSet> &GetMaximumClique() const & {
    return max_cliques_set;
  }

  GRAPHSEG_INLINE_CONST std::set<VertexSet> GetMaximumClique() && {
    return std::move(max_cliques_set);
  }

  /// <summary>
  /// get caluclated maximum clique from internal efficient data structure
  /// </summary>
  GRAPHSEG_INLINE_CONST std::vector<std::vector<Vertex>> &
  GetMaximumClique(size_t idx) {
    return max_cliques_internal[idx];
  }

  /// <summary>
  /// get node number and weight that passed adjacent nodes
  /// </summary>
  GRAPHSEG_INLINE_CONST std::vector<Edge> &operator[](size_t idx) const & {
    return graph[idx];
  }

  VertexSet GetAdjacentNodes(const size_t idx) {
    VertexSet adjacents;
    for (const auto &[adjacent_node_id, edge_weight] : graph[idx]) {
      adjacents.insert(adjacent_node_id);
    }
    return adjacents;
  }

private:
  void SetArc(int src, int dst, double score) {
    auto pair = std::make_pair(dst, score);
    graph[src].emplace_back(pair);
  }

  void BronKerbosch(std::set<Vertex> clique, std::set<Vertex> candidates,
                    std::set<Vertex> excluded) {
    if (candidates.empty() && excluded.empty()) {
      max_cliques_set.insert(clique);
      return;
    }

    // TODO(rei.shimizu): It may be invalid solution to resolve iterator
    // breakdown problem
    std::set<Vertex> candidates_tmp;
    for (auto itr2 = candidates.begin(); itr2 != candidates.end(); ++itr2) {
      if (candidates_tmp.find(*itr2) != candidates_tmp.end())
        break;
      auto v = *itr2;
      auto clique_t = clique + std::set<Vertex>({v});
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

  void ConstructMaximumCliqueArrayContainer() {
    max_cliques_internal.resize(graph_size);
    for (auto &max_clique : max_cliques_set) {
      for (auto &clique_vertex : max_clique) {
        max_cliques_internal[clique_vertex].emplace_back(
            std::vector<Vertex>(max_clique.begin(), max_clique.end()));
      }
    }
  }

  std::set<Vertex> GetNeighbors(Vertex idx) {
    std::set<Vertex> tmp;
    for (const auto &node : graph[idx]) {
      tmp.emplace(node.first);
    }
    return tmp;
  }

  /// <summary>
  /// Base graph
  /// </summary>
  std::vector<std::vector<Edge>> graph;

  /// <summary>
  /// std::set of maximum clique
  /// </summary>
  std::set<VertexSet> max_cliques_set;

  /// <summary>
  /// Data structure to get maximum clique with O(N)
  /// <summary>
  std::vector<std::vector<std::vector<Vertex>>> max_cliques_internal;

  /// <summary>
  /// current graph size
  /// </summary>
  Vertex graph_size = 0;
};
} // namespace GraphSeg::graph

#endif