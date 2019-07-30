#ifndef GRAPHSEG_CPP_GRAPHSEG_SEGMENT_GRAPH_H
#define GRAPHSEG_CPP_GRAPHSEG_SEGMENT_GRAPH_H 

#include "sentence.h"
#include "embedding_manager.h"
#include "nameof.hpp"

#include <set>
#include <list>
#include <vector>
#include <string>
#include <iterator>
#include <memory>
#include <algorithm>
#include <iostream>
#include <tuple>
#include <ostream>
#include <iterator>
#include <unordered_map>

namespace GraphSeg
{
  using std::unordered_map, std::vector, std::make_pair, std::set, std::string, std::set_intersection, std::tuple,
        std::set_union, std::set_difference, std::inserter, std::basic_ostream, std::list;
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

  /// <summary>
  /// Listコンテナに対して、2つののリスト要素と与えた値を置換する
  /// TODO: 任意数のリスト要素を置換出来るように拡張したい
  /// </summary>
  template <class T, class... It>
  void replace_list(list<T>& l, T value, It&... rest)
  {
    tuple<It...> expand_rest = { rest... };
    *std::get<0>(expand_rest) = value;
    l.erase(std::get<1>(expand_rest));
  }

  class SegmentGraph
  {
  public:
    /// <summary>
    /// 最小セグメントサイズ
    /// </summary>
    size_t minimum_segment_size = 2;

    /// <summary>
    /// セグメント構築時にセンテンスの類似度を計算する必要があるため
    /// </summary>
    std::unique_ptr<EmbeddingManager> em;

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
    /// セグメントを返す
    /// </summary>
    inline const auto& GetSegment() const&
    {
      return segments;
    }

    inline auto GetSegment() &&
    {
      return std::move(segments);
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
    void ConstructSegment()
    {
      if (segments.size() == 0) 
        ConstructInitSegment();

      list<vector<Vertex>> next_segment;
      vector<int> segment_memo(segments.size(), 0); // TODO：bitsetで管理したい。next_segmentに加えられたものは1とする

      size_t i = 0;
      for (auto itr = segments.begin(); itr != segments.end(); ++itr)
      {
        if (segment_memo[i] == 1)
          continue;

        auto current_segment = *itr;
        auto adjacent_segment = *std::next(itr);
        
        if (IsMergable(current_segment, adjacent_segment))
        {
          vector<Vertex> merged_segment;
          std::copy(current_segment.begin(), current_segment.end(), merged_segment.begin());
          current_segment.insert(current_segment.end(), adjacent_segment.begin(), adjacent_segment.end());
          next_segment.emplace_back(merged_segment);
          segment_memo[i] = 1;
          segment_memo[i+1] = 1;
          continue;
        }

        segment_memo[i] = 1;
        ++i;
      }

      segments.clear();
      segments = next_segment;

      ConstructInvalidSegment();
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

      // TODO: 一度Setで最大クリークを構築するが、定数オーダーで最大クリークが欲しいのでベクターに変換しているが、メモリ効率が悪いし、変換処理も無駄
      max_cliques_internal.resize(max_sentence_size);
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

    /// <summary>
    /// ファーストセグメント中のあるノードを含む最大クリークが、セカンドセグメントに含まれている場合はマージしても良いとする。
    /// 最大クリークに含まれているということは、そのノードで示されているトピックがセカンドセグメントで言及されている可能性があるという事である。
    /// </summary>
    bool IsMergable(const vector<Vertex>& sg1, const vector<Vertex>& sg2)
    {
      int checker = 1;
      for (const auto& s: sg1)
      {
        for(auto& target_max_cliques: max_cliques_internal[s])
        {
          vector<Vertex> tmp;
          set_intersection(target_max_cliques.begin(), target_max_cliques.end(), sg2.begin(), sg2.end(), inserter(tmp, tmp.end()));
          if (tmp.size() != 0) 
          {
            checker *= 0;
          }
        }
      }
      return checker == 0 ? true : false;
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
        vector<int> check(GetGraphSize(), 0); // TODO: ビットで管理したい
        for (const auto& seg_base: clique)
        {
          vector<Vertex> segment;
          
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

          segments.emplace_back(segment);
        }
      }
    }
    
    void ConstructInvalidSegment()
    {
      const auto segment_relatedness = [this](auto itr_seg1, auto itr_seg2)
      {
        double rel = 1.0;
        for (const auto& sent1: *itr_seg1)
        {
          for (const auto& sent2: *itr_seg2)
          {
            rel *= em->GetSimilarity(sentence_idx[sent1], sentence_idx[sent2]);
          }
        }
        return rel / itr_seg1->size() * itr_seg2->size();
      };

      const auto get_merged_segment = [](auto first_itr, auto second_itr)
      {
        vector<Vertex> merged_segment;
        std::copy(first_itr->begin(), first_itr->end(), merged_segment.begin());
        merged_segment.insert(merged_segment.end(), second_itr->begin(), second_itr->end());
        return merged_segment;
      };

      // セグメントセットを順方向に走査し、条件を満たしていないセグメントは前後のいずれかのセグメントとマージ
      for (auto itr = segments.begin(); itr != segments.end(); ++itr)
      {
        if (itr->size() < minimum_segment_size)
        {
          if (itr == segments.begin())
          {
            auto next_itr = std::next(itr);
            auto merged_segment = get_merged_segment(itr, next_itr);
            replace_list<
              decltype(merged_segment), decltype(itr), decltype(next_itr)
            >(segments, merged_segment, itr, next_itr);
          } 
          else if (itr == segments.end()) 
          {
            auto prev_itr = std::prev(itr);
            auto merged_segment = get_merged_segment(prev_itr, itr);
            replace_list<
              decltype(merged_segment), decltype(prev_itr), decltype(itr)
            >(segments, merged_segment, prev_itr, itr);
          }
          else 
          { 
            auto prev_itr = std::prev(itr);
            auto next_itr = std::next(itr);
            auto before = segment_relatedness(itr, prev_itr);
            auto after = segment_relatedness(itr, next_itr);

            if (before > after)
            {
              auto prev_itr = std::prev(itr);
              auto merged_segment = get_merged_segment(prev_itr, itr);
              replace_list<
                decltype(merged_segment), decltype(prev_itr), decltype(itr)
              >(segments, merged_segment, prev_itr, itr);
            }
            else
            {
              auto next_itr = std::next(itr);
              auto merged_segment = get_merged_segment(itr, next_itr);
              replace_list<
                decltype(merged_segment), decltype(itr), decltype(next_itr)
              >(segments, merged_segment, itr, next_itr);
            }
          }
        }
      }
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
    /// 計算済みセグメント
    /// <summary>
    list<vector<Vertex>> segments;

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