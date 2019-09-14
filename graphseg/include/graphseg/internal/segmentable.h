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

#ifdef DEBUG
#include <spdlog/spdlog.h>
#endif

namespace GraphSeg::internal
{
  using std::list, std::vector, std::set, std::tuple, std::inserter;

  using Vertex = unsigned int;
  using VertexSet = set<Vertex>;

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
    mutable vector<vector<Vertex>> segments;

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
      for (const auto& s: sg1)
      {
        for(const auto& maximum_cliques: Derived().max_cliques_internal[s])
        {
          const auto& duplicated = sg2 & maximum_cliques;
          if (duplicated.size() != 0)
          {
            return true;
          }
        }  
      }
      return false;
    }

    vector<Vertex> GetMergedSegment(const vector<Vertex>& first_itr, const vector<Vertex> second_itr)
    {
      vector<Vertex> merged_segment = first_itr;
      merged_segment.resize(first_itr.size() + second_itr.size() - 1);
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
      {
        ConstructInitSegment();
      }
      
      // マージできるか調べたセグメントを記録しておく。二重でセグメントが調べられるのを防ぐ
      vector<bool> segment_memo(segments.size(), false);
      vector<vector<Vertex>> next_segment;

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
          segment_memo[i] = true;
          segment_memo[i+1] = true;

          // セグメントを先読みしてマージしきれるならマージする
          for(size_t j = 2; IsMergable(merged_segment, segments[i+j]) && i+j < segments.size(); ++j)
          {
            const auto offspring_segment = segments[i+j];
            merged_segment = GetMergedSegment(merged_segment, offspring_segment);
            segment_memo[i+j] = true;
          }

          next_segment.emplace_back(merged_segment);
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
     
#ifdef DEBUG
      std::cout << "===== Merged Segment =====" << std::endl; 
      std::cout << segments;
      std::cout << "===========================" << std::endl;
#endif

      ConstructInvalidSegment(embedding);
    }
  
  private:
    const T& Derived() const&
    {
      return static_cast<const T&>(*this);
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
              continue;
            }
            else
            {
              single_segment.emplace_back(node);
            }
          }
        }
        if (single_segment.size() == 0)
        {
          continue;
        }
        segments.emplace_back(single_segment);
      }
      std::sort(segments.begin(), segments.end());

#ifdef DEBUG
      std::cout << "===== Initial Segment =====" << std::endl;
      std::cout << segments;
      std::cout << "===========================" << std::endl;
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
          check_segment[i] = true;
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

#ifdef DEBUG
      std::cout << "===== Small Segment =====" << std::endl;
      std::cout << segments;
      std::cout << "===========================" << std::endl;
#endif
    }
  };
}

#endif