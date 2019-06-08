#ifndef GRAPHSEG_CPP_GRAPHSEG_SEGMENT_GRAPH_H
#define GRAPHSEG_CPP_GRAPHSEG_SEGMENT_GRAPH_H 
// #ifdef DEBUG
#include <iostream>
// #endif
#include <set>
#include <vector>
#include <string>
#include <memory>
#include <iterator>
#include <algorithm>
#include <unordered_map>

namespace GraphSeg
{
  using std::vector, std::make_pair, std::set, std::string, std::set_intersection, std::set_union, std::inserter;
  using Vertex = unsigned int;
  using VertexSet = set<Vertex>;
  using Edge = std::pair<Vertex, double>;

  template <typename T>
  set<T> operator&(const set<T>& v1, const set<T>& v2)
  {
    set<T> result;
    set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), inserter(result, result.end()));
    return result;
  }

  template <typename T>
  set<T> operator+(const set<T>& v1, const set<T>& v2)
  {
    set<T> result;
    set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), inserter(result, result.end()));
    return result;
  }

  template <class ForwardIterator>
  void unique(ForwardIterator _begin, ForwardIterator _last)
  {
    auto prev = *_begin;
    vector<decltype(_begin)> deletable;
    _begin++;
    while(_begin != _last)
    {
      if (prev == *_begin)
      {
        deletable.emplace_back(_begin);
      }
      prev = *_begin;
      _begin++;      
    }
  }

  class SegmentGraph
  {
  public:
    SegmentGraph(Vertex _max_sentence_size) : max_sentence_size(_max_sentence_size)
    {
    }

    /// <summary>
    /// set sentence graph (lvalue)
    /// </summary>
    void SetSentence(const string& s)
    {
      sentence_idx[node_idx] = s;
      ++node_idx;
      graph.emplace_back(vector<Edge>());
    }

    /// <summary>
    /// set sentence graph (rvalue)
    /// </summary>
    void SetSentence(string&& s)
    {
      ++node_idx;
      sentence_idx.emplace_back(s);
      graph.emplace_back(vector<Edge>());
    }

    /// <summary>
    /// set segment graph edge
    /// </summary>
    void SetEdge(int src, int dst, double score)
    {
      graph[src].emplace_back(make_pair(dst, score));
      graph[dst].emplace_back(make_pair(src, score));
    }

    /// <summary>
    /// get graph size
    /// </summary>
    Vertex GetGraphSize() const noexcept
    {
      return node_idx;
    }

    /// <summary>
    /// calclate maximum clique
    /// </summary>
    void SetMaximumClique()
    {
      vector<Vertex> tmp(node_idx);
      int i = 0;
      std::generate(tmp.begin(), tmp.end(), [&i](){ ++i; return i; });
      BronKerbosch(set<Vertex>(), set<Vertex>(tmp.begin(), tmp.end()), set<Vertex>());
      std::sort(max_cliques.begin(), max_cliques.end());
      auto result = std::unique(max_cliques.begin(), max_cliques.end());
      max_cliques.erase(result, max_cliques.end());
    }

    /// <summary>
    /// get maximum clique
    /// </summary>
    set<VertexSet> GetMaximumClique()
    {
      return max_cliques;
    }

  private:
    void BronKerbosch(set<Vertex> clique, set<Vertex> candidates, set<Vertex> excluded)
    {
      if (candidates.empty() && excluded.empty())
      {
        max_cliques.insert(clique);
        return;
      }
      for (const auto& v: candidates)
      {
        auto clique_t = clique + set<Vertex>({v});
        auto candidates_t = candidates & GetNeighbors(v);
        auto excluded_t = excluded & GetNeighbors(v);
        BronKerbosch(clique_t, candidates_t, excluded_t);
        deleteWithVal(candidates, v);
        excluded.insert(v);
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

    void deleteWithVal(set<Vertex>& target, Vertex value)
    {
      for(auto itr = target.begin(); itr != target.end(); ++itr)
      {
        if (*itr == value)
        {
          target.erase(itr);
          return;
        }
      }
    }

    vector<vector<Edge>> graph;
    vector<string> sentence_idx;
    set<VertexSet> max_cliques;
    Vertex node_idx = 0;
    Vertex max_sentence_size;
  };
} // namespace GraphSeg

#endif