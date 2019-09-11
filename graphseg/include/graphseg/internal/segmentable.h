#ifndef GRAPHSEG_CPP_GRAPHSEG_SEGMENTABLE_H
#define GRAPHSEG_CPP_GRAPHSEG_SEGMENTABLE_H

#include "graphseg/embedding.h"

#include <type_traits>
#include <list>
#include <vector>
#include <set>
#include <tuple>
#include <algorithm>
#include <bitset>
// #include <spdlog/spdlog.h>

namespace GraphSeg::internal
{
  using std::list, std::vector, std::set, std::tuple;

  using Vertex = unsigned int;
  using VertexSet = set<Vertex>;
  
  template <typename T>
  struct is_valid_iterable : std::disjunction<
    std::is_same<T, list<vector<Vertex>>>,
    std::is_same<T, set<VertexSet>>
  >
  {};

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
      // spdlog::info(vertex_node);
    }
  }

  /// <summary>
  /// セグメント化可能
  /// </summary>
  template <class T>
  class Segmentable
  {
  private:
    /// <summary>
    /// 最小セグメントサイズ
    /// </summary>
    size_t minimum_segment_size = 2;

    /// <summary>
    /// 計算済みセグメント
    /// <summary>
    vector<vector<Vertex>> segments;

  public:
    /// <summary>
    /// 最小セグメントサイズの左辺値参照を取得する
    /// </summary>
    size_t& GetMinimumSegmentSize(size_t s)
    {
      return minimum_segment_size;
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

private:
    /// <summary>
    /// ファーストセグメント中のあるノードを含む最大クリークが、セカンドセグメントに含まれている場合はマージしても良いとする。
    /// 最大クリークに含まれているということは、そのノードで示されているトピックがセカンドセグメントで言及されている可能性があるという事である。
    /// </summary>
    bool IsMergable(const vector<Vertex>& sg1, const vector<Vertex>& sg2)
    {
      int checker = 1;
      vector<Vertex> tmp;
      
      for (const auto& s: sg1)
      {
        for(auto& target_max_cliques: Derived().max_cliques_internal[s])
        {
          if (tmp.size() != 0) 
          {
            checker *= 0;
          }
          tmp.clear();
        }  
      }
      return checker == 0 ? true : false;
    }

    vector<Vertex> GetMergedSegment(const vector<Vertex>& first_itr, const vector<Vertex> second_itr)
    {
      vector<Vertex> merged_segment = first_itr;
      merged_segment.resize(first_itr.size() + second_itr.size());
      merged_segment.insert(merged_segment.end(), second_itr.begin(), second_itr.end());
      return merged_segment;
    }

public:
    /// <summary>
    /// 最大クリークからセグメントを構築する
    /// </summary>
    void ConstructSegment(const Embedding& embedding)
    {
      if (segments.size() == 0) 
        ConstructInitSegment();

      std::cout << "===== Init Segment =====" << std::endl;
      for(auto a: segments)
      {
        for(auto b: a)
        {
          std::cout << b << " ";
        }
        std::cout << std::endl;
      }
      std::cout << std::endl;

      vector<vector<Vertex>> next_segment;

      // マージできるか調べたセグメントを記録しておく。二重でセグメントが調べられるのを防ぐ
      vector<bool> segment_memo(segments.size(), false);

      for (size_t i = 0; i < segments.size() - 1; ++i)
      {
        const auto current_segment = segments[i];
        const auto adjacent_segment = segments[i+1];
       
        if (segment_memo[i] == true)
        {
          continue;
        }

        if (IsMergable(current_segment, adjacent_segment))
        {
          vector<Vertex> merged_segment = GetMergedSegment(current_segment, adjacent_segment);
          next_segment.emplace_back(merged_segment);
          segment_memo[i] = true;
          segment_memo[i+1] = true;
        }
        else
        {
          next_segment.emplace_back(current_segment);
          segment_memo[i] = true;
        }
      }

      if (segment_memo[segments.size() - 1] == false)
      {
        const auto segment_last_idx = segments.size() - 1;
        segment_memo[segment_last_idx] = true;
        next_segment.emplace_back(segments[segment_last_idx]);
      }

      segments.clear();
      segments = next_segment;

      std::cout << "===== Merged Segment =====" << std::endl;
      for(auto a: segments)
      {
        for(auto b: a)
        {
          std::cout << b << " ";
        }
        std::cout << std::endl;
      }
      std::cout << std::endl;

#ifdef DEBUG
      // spdlog::info("===== Merged Segment =====");
      // std::cout << segments << std::endl;
      // spdlog::info("===========================");
#endif

      ConstructInvalidSegment(embedding);
    }
  
  private:
    T& Derived() &
    {
      return static_cast<T&>(*this);
    }

    T&& Derived() &&
    {
      return static_cast<T&&>(*this);
    }

    /// <summary>
    /// 初期セグメントの構築
    /// </summary>
    void ConstructInitSegment()
    {
      vector<bool> check(Derived().GetGraphSize(), false);
      for (const auto& clique: Derived().max_cliques_set)
      {
        vector<Vertex> single_segment;
        for (const auto& node: clique)
        {
          if (check[node] == true)
          {
            continue;
          }          
          check[node] = true;
          if (single_segment.size() == 0)
          {
            single_segment.emplace_back(node);
          } 
          else 
          {
            if (node - single_segment[single_segment.size() - 1] > 1)
            {
              segments.emplace_back(single_segment);
              single_segment.clear();
              single_segment.emplace_back(node);
            }
            else
            {
              single_segment.emplace_back(node);
            }
          }
        }
        segments.emplace_back(single_segment);
      }
      std::sort(segments.begin(), segments.end());

#ifdef DEBUG
      // spdlog::info("===== Initial Segment =====");
      // std::cout << segments << std::endl;
      // spdlog::info("===========================");
#endif
    }

    /// <summary>
    /// 条件を満たしていない、つまり、長さの閾値を越えていないセグメントに関して前後のものとマージし、正しいセグメントを構築する
    /// </summary>
    void ConstructInvalidSegment(const Embedding& embedding)
    {
      const auto segment_relatedness = [&embedding, this](const auto& seg1, const auto& seg2)
      {
        double rel = 1.0;
        for (const auto& sent1: seg1)
        {
          for (const auto& sent2: seg2)
          {
            rel *= embedding.GetSimilarity(
              Derived().sentence_idx[sent1], 
              Derived().sentence_idx[sent2]
            );
          } 
        }
        return rel / seg1.size() * seg2.size();
      };

      vector<vector<Vertex>> next_segments;
      vector<bool> check_segment(segments.size(), false);

      for (size_t i = 0; i < segments.size() - 1; ++i)
      {
        const auto current_segment = segments[i];
        const auto next_segment = segments[i+1];

        if (check_segment[i] == true)
        {
          continue;
        }

        if (current_segment.size() < minimum_segment_size)
        {
          if (i == 0) // 最初のセグメントは一個後のものしかマージ対象にならない
          {
            auto merged_segment = GetMergedSegment(current_segment, next_segment);
            check_segment[i] = true;
            check_segment[i+1] = true;
            next_segments.emplace_back(merged_segment);
          }
          else // その他のセグメントは、セグメント関連度スコアが高いものとマージするようにする
          {
            const auto prev_segment = segments[i-1];
            auto before = segment_relatedness(current_segment, prev_segment);
            auto after = segment_relatedness(current_segment, next_segment);

            if (before > after)
            {
              auto merged_segment = GetMergedSegment(prev_segment, current_segment);
              check_segment[i-1] = true;
              check_segment[i] = true;
              next_segments.emplace_back(merged_segment);
            }
            else
            {
              auto merged_segment = GetMergedSegment(current_segment, next_segment);
              check_segment[i] = true;
              check_segment[i+1] = true;
              next_segments.emplace_back(merged_segment);
            }
          }
        }
        else
        {
          next_segments.emplace_back(current_segment);
        }
      }

      // 最後のセグメントがマージされなかった場合余るので追加
      if (check_segment[segments.size() - 1] == false)
      {
        const auto last_segment_idx = segments.size() - 1;
        check_segment[last_segment_idx] = true;
        next_segments.emplace_back(segments[last_segment_idx]);
      }

      segments.clear();
      segments = next_segments;

      // spdlog::info("===== Small Segment =====");
      // std::cout << segments << std::endl;
      // spdlog::info("===========================");
    }
  };
}

#endif