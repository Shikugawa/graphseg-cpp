#ifndef GRAPHSEG_CPP_GRAPHSEG_SEGMENTABLE_H
#define GRAPHSEG_CPP_GRAPHSEG_SEGMENTABLE_H

#include <list>
#include <vector>
#include <set>
#include <tuple>
#include <spdlog/spdlog.h>

namespace GraphSeg::internal
{
  using std::list, std::vector, std::set, std::tuple;

  using Vertex = unsigned int;

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
      spdlog::info(vertex_node);
    }
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

  /// <summary>
  /// セグメント化可能
  /// </summary>
  template <class T>
  class Segmentable
  {
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

    /// <summary>
    /// 最大クリークからセグメントを構築する
    /// </summary>
    void ConstructSegment()
    {
      if (segments.size() == 0) 
        ConstructInitSegment();

      list<vector<Vertex>> next_segment;

      // マージできるか調べたセグメントを記録しておく。二重でセグメントが調べられるのを防ぐ
      // TODO：bitsetで管理したい。next_segmentに加えられたものは1とする
      vector<int> segment_memo(segments.size(), 0);

      size_t i = 0;
      for (auto itr = segments.begin(); itr != segments.end(); ++itr)
      {
        if (segment_memo[i] == 1)
        {
          continue;
        }

        if (std::next(itr) == segments.end())
        {
          segment_memo[i] = 1;
          next_segment.emplace_back(*itr);
          continue;
        }

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
        }
        else
        {
          next_segment.emplace_back(current_segment);
          segment_memo[i] = 1;
        }

        ++i;
      }
      
      segments.clear();
      segments = next_segment;

#ifdef DEBUG
      spdlog::info("===== Merged Segment =====");
      std::cout << segments << std::endl;
      spdlog::info("===========================");
#endif

      ConstructInvalidSegment();
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
      for (const auto& clique: Derived().max_cliques_set)
      {
        vector<int> check(Derived().GetGraphSize(), 0); // TODO: ビットで管理したい
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

#ifdef DEBUG
      spdlog::info("===== Initial Segment =====");
      std::cout << segments << std::endl;
      spdlog::info("===========================");
#endif
    }

    /// <summary>
    /// 条件を満たしていない、つまり、長さの閾値を越えていないセグメントに関して前後のものとマージし、正しいセグメントを構築する
    /// </summary>
    void ConstructInvalidSegment()
    {
      constexpr auto get_merged_segment = [](auto first_itr, auto second_itr)
      {
        vector<Vertex> merged_segment = *first_itr;
        merged_segment.resize(first_itr->size() + second_itr->size());
        merged_segment.insert(merged_segment.end(), second_itr->begin(), second_itr->end());
        return merged_segment;
      };

      const auto segment_relatedness = [this](auto itr_seg1, auto itr_seg2)
      {
        double rel = 1.0;
        for (const auto& sent1: *itr_seg1)
        {
          std::cout << rel << std::endl;
          for (const auto& sent2: *itr_seg2)
          {
            rel *= Derived().em->GetSimilarity(
              Derived().sentence_idx[sent1], 
              Derived().sentence_idx[sent2]
            );
          } 
        }
        return rel / itr_seg1->size() * itr_seg2->size();
      };

      for (auto itr = segments.begin(); itr != segments.end(); ++itr)
      {
        if (std::next(itr) == segments.end())
        {
          break;
        }

        if (itr->size() < minimum_segment_size)
        {
          if (itr == segments.begin()) // 最初のセグメントは一個後のものしかマージ対象にならない
          {
            auto next_itr = std::next(itr);
            auto merged_segment = get_merged_segment(itr, next_itr);
            replace_list(segments, merged_segment, itr, next_itr);
          } 
          else if (itr == segments.end()) // 最後のセグメントは一個前のものしかマージ対象にならない
          {
            auto prev_itr = std::prev(itr);
            auto merged_segment = get_merged_segment(prev_itr, itr);
            replace_list(segments, merged_segment, prev_itr, itr);
          }
          else // その他のセグメントは、セグメント関連度スコアが高いものとマージするようにする
          {
            auto prev_itr = std::prev(itr);
            auto next_itr = std::next(itr);
            auto before = segment_relatedness(itr, prev_itr);
            auto after = segment_relatedness(itr, next_itr);

            if (before > after)
            {
              auto prev_itr = std::prev(itr);
              auto merged_segment = get_merged_segment(prev_itr, itr);
              replace_list(segments, merged_segment, prev_itr, itr);
            }
            else
            {
              auto next_itr = std::next(itr);
              auto merged_segment = get_merged_segment(itr, next_itr);
              replace_list(segments, merged_segment, itr, next_itr);
            }
          }
        }
      }

      spdlog::info("===== Small Segment =====");
      for (const auto& segment : segments)
      {
        std::string vertex_node;
        for (const auto& segment_vertex : segment)
        {
          vertex_node += std::to_string(segment_vertex);
          vertex_node += " ";
        }
        spdlog::info(vertex_node);
      }
      spdlog::info("===========================");
    }

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

    /// <summary>
    /// 最小セグメントサイズ
    /// </summary>
    size_t minimum_segment_size = 2;

    /// <summary>
    /// 計算済みセグメント
    /// <summary>
    list<vector<Vertex>> segments;
  };
}

#endif