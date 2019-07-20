#ifndef GRAPHSEG_CPP_GRAPHSEG_SEGMENT_GRAPH_H
#define GRAPHSEG_CPP_GRAPHSEG_SEGMENT_GRAPH_H 
#ifdef DEBUG
  #include <iostream>
#endif
#include "sentence.h"
#include "nameof.hpp"
#include <set>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <ostream>
#include <iterator>
#include <unordered_map>

namespace GraphSeg
{
  using std::unordered_map, std::vector, std::make_pair, std::set, std::string, std::set_intersection, 
        std::set_union, std::set_difference, std::inserter, std::basic_ostream;
  using Vertex = unsigned int;
  using VertexSet = set<Vertex>;
  using Edge = std::pair<Vertex, double>;

  enum class GraphType
  {
    DIRECTED_GRAPH,
    UNDIRECTED_GRAPH
  };

  struct FormattedGraph
  {
    struct FormattedNode
    {
      struct FormattedTargetNode 
      {
        string id;
        double edge_weight;

        FormattedTargetNode(string _id, double _weight) : id(_id), edge_weight(_weight)
        {}
      };

      double score;
      vector<FormattedTargetNode> targets;
    };

    GraphType graph_type;
    unordered_map<string, FormattedNode> value;
  };

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

  template <typename T>
  set<T> operator-(const set<T>& v1, const set<T>& v2)
  {
    set<T> result;
    set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), inserter(result, result.end()));
    return result;
  }

  class SegmentGraph
  {
  public:
    size_t minimum_segment_size = 2;

    SegmentGraph(Vertex _max_sentence_size) : max_sentence_size(_max_sentence_size)
    {
    }

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

    /// <summary>
    /// 最大クリークからセグメントを構築する
    /// </summary>
    set<VertexSet> ConstructSegment()
    {
      if (segments.size == 0) 
        ConstructInitSegment();

      while (!flag) {
        vector<vector<Vertex>> next_segment;
        vector<int> segment_memo(segment,size(), 0); // next_segmentに加えられたものは1とする

        for (size_t i = 0; i < segment.size(); ++i)
        {
          if (segment_memo[i] == 1)
            continue;

          if (IsMergable(segment[i], segment[i+1]))
          {
            next_segment.emplace_back(std::copy(segment[i+1].begin(), segment[i+1].end(), std::back_inserter(segment[i])));
            segment_memo[i] = 1;
            segment_memo[i+1] = 1;
            continue;
          }

          segment_memo.emplace_back(segment[i]);
          segment_memo[i] = 1;
        }

        segment.clear();
        segment = next_segment;

        
        if (IsValidSegment()) break;
      }
    }

    set<VertexSet> GetAdjacentNodes(const size_t idx)
    {
      set<VertexSet> adjacents;
      for (const auto& [adjacent_node_id, edge_weight]: graph[idx])
      {
        adjacents.emplace_back(adjacent_node_id);
      }
      return adjacent_node_id;
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

      max_cliques.resize(max_sentence_size);
      for (const auto& max_clique_: max_cliques_set)
      {
        for (const auto& clique_vertex: max_clique_)
        {
          const auto rem = max_clique_ - set(clique_vertex);
          max_cliques[clique_vertex].emplace_back(vector<Vertex>(rem.begin(), rem.end()));
        }
      }
    }

    /// <summary>
    /// ファーストセグメント中のあるノードを含む最大クリークが、セカンドセグメントに含まれている場合はマージしても良いとする。
    /// 最大クリークに含まれているということは、そのノードで示されているトピックがセカンドセグメントで言及されている可能性があるという事である。
    /// </summary>
    bool IsMergable(const vector<Vertex>& sg1, const vector<Vertex>& sg2)
    {
      for (const auto& s: sg1)
      {
        for (const auto& max_clique_nodes: max_cliques[s])
        { 
          vector<Vertex> tmp;
          set_intersection(max_clique_nodes.begin(), max_clique_nodes.end(), sg2.begin(), sg2.end(), inserter(tmp, tmp.end()));
          if (tmp.size() != 0) 
          {
            return true;
          }
        }
      }
      return false;
    }

    bool IsValidSegment()
    {
        int mem = 1;
        for (const auto& seg: segment)
        {
          if (seg.size() < minimum_segment_size)
            mem *= 0;
        }
        if (mem == 0) return false;
        return true;
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

    void ConstructInitSegment()
    {
      for (const auto& clique: max_cliques_set)
      {
        vector<int> check(graph_size, 0); // TODO: ビットで管理したい
        for (const auto& seg_base: clique)
        {
          VertexSet segment;
          
          if (check[seg_base] == 1) 
            continue;

          check[seg_base] = 1;
          segment.emplace_back(seg_base);
          const auto candidates = GetNeighbors(seg_base) & clique;
          for (const auto& c: candidates)
          {
            segment.emplace_back(c);
            check[c] = 1;
          }

          segments.insert(segment);
        }
      }
    }
    
    vector<vector<Edge>> graph;
    vector<Sentence> sentence_idx;
    set<VertexSet> max_cliques_set;
    vector<vector<VertexSet>> max_cliques;
    vector<vector<Vertex>> segments;
    Vertex node_idx = 0;
    Vertex max_sentence_size;
  };

  FormattedGraph format(const SegmentGraph& sg)
  {
    using FormattedNode = FormattedGraph::FormattedNode;
    using FormattedTargetNode = FormattedNode::FormattedTargetNode;

    FormattedGraph fg;
    fg.graph_type = GraphType::UNDIRECTED_GRAPH;
    for (size_t i = 0; i < sg.GetGraphSize(); ++i)
    {
      const auto& adjacent_nodes = sg[i];
      FormattedNode fn;
      fn.score = 0; // この文脈ではノードの重みは必要ない
      for (const auto& adjacent_node: adjacent_nodes)
      {
        fn.targets.emplace_back(FormattedTargetNode(std::to_string(adjacent_node.first), adjacent_node.second));
      }
      fg.value[std::to_string(i)] = fn;
    }
    return fg;
  }

  template <typename CharT, typename Traits>
  basic_ostream<CharT, Traits>& operator<<(basic_ostream<CharT, Traits>& os, const FormattedGraph& fg)
  {
    os << "{\n";
    os << "  \"graph_type\": " << NAMEOF_ENUM(fg.graph_type) << ",\n";
    os << "  \"value\": [\n";
    for (const auto& node: fg.value)
    {
      const auto& formatted_node = node.second;
      os << "    {\n";
      os << "      \"score\": " << formatted_node.score << ",\n";
      os << "      \"targets: [\"\n";
      for (const auto& target: formatted_node.targets)
      {
        os << "         {\n";
        os << "           \"id\"" << target.id << "\n";
        os << "           \"weight\"" << target.edge_weight << "\n"; 
        os << "         },\n";
      }
      os << "      ]\n";
      os << "    },\n";
    }
    os << "  ]\n";
    os << "}\n";
    return os;
  }

  template <typename CharT, typename Traits>
  basic_ostream<CharT, Traits>& operator<<(basic_ostream<CharT, Traits>& os, const SegmentGraph& sg)
  {
    for(size_t i = 0; i < sg.GetGraphSize(); ++i)
    {
      os << i << ": " << sg.GetSentence(i).GetText() << " => " << "{";
      for(const auto& p: sg[i])
      {
        os << "{" << p.first << ", " << p.second << "},";
      }
      os << "}" << std::endl;
    }
    return os;
  }
} // namespace GraphSeg

#endif