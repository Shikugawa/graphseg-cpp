#ifndef GRAPHSEG_CPP_GRAPHSEG_SEGMENTABLE_HPP
#define GRAPHSEG_CPP_GRAPHSEG_SEGMENTABLE_HPP

#include "graphseg/internal/utils/custom_operator.hpp"
#include "graphseg/embedding.hpp"
#include "graphseg/segmentation_container.hpp"
#include "graphseg/lang.hpp"

#include <optional>
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
  using namespace GraphSeg::internal::utils;
  using std::list, std::vector, std::tuple, std::shared_ptr, std::optional;
  using Vertex = unsigned int;

  class SegmentChecker
  {
  private:
    vector<bool> segment_checker;

    void ClearSegmentChecker()
    {
      segment_checker.clear();
    }

  protected:
    SegmentChecker() = default;
    
    optional<bool> CheckSegment(size_t idx)
    {
      assert(idx < segment_checker.size());
      if (segment_checker.size() == 0)
      {
        return std::nullopt;
      }
      return segment_checker[idx];
    }

    void Mark(size_t idx) {
      assert(idx < segment_checker.size() && idx >= 0);
      segment_checker[idx] = true;
    }

    void MarkBackward(size_t idx)
    {
      assert(idx < segment_checker.size() && idx > 0);
      Mark(idx);
      Mark(idx-1);
    }

    void MarkForward(size_t idx)
    {
      assert(idx < segment_checker.size() - 1 && idx >= 0);
      Mark(idx);
      Mark(idx+1);
    }

    void InstantiateSegmentChecker(size_t size)
    {
      ClearSegmentChecker();
      segment_checker.resize(size);
    }
  };

  template <class Graph, int VectorDim, Lang LangType = Lang::EN>
  class Segmentable : public SegmentChecker
  {
  public:
    /// <summary>
    /// minimum segment size
    /// </summary>
    size_t minimum_segment_size = 2;

    /// <summary>
    /// calculated segments
    /// <summary>
    mutable vector<vector<Vertex>> segments;

    /// <summary>
    /// segment checked flag
    /// </summary>
    mutable vector<bool> segment_check;

  private:
    /// <summary>
    /// segment graph
    /// use shared_ptr to enable sharing original graph generated by segmentation_container
    /// </summary>
    shared_ptr<Graph> graph;

  public:
    explicit Segmentable() = default;

    explicit Segmentable(shared_ptr<Graph> g) : graph(g)
    {}

  private:
    /// <summary>
    /// Allow merging if the second segment includes one of maximum clique that includes some any node in first segment
    /// When one of sentence includes maximum clique, the topic indicated by the sentence should be referred by second segment
    /// </summary>
    bool IsMergable(const vector<Vertex>& sg1, const vector<Vertex>& sg2)
    {
      for (const auto& s: sg1)
      {
        for(const auto& maximum_cliques: graph->GetMaximumClique(s))
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
      merged_segment.insert(merged_segment.end(), second_itr.begin(), second_itr.end());
      return merged_segment;
    }

  public:
    /// <summary>
    /// construct segment from maximum clique
    /// </summary>
    void ConstructSegment(const Embedding<VectorDim, LangType>& embedding)
    {
      if (segments.size() == 0)
      {
        ConstructInitSegment();
      }

      // to avoid searching same segment twice, memory searched segment whether it can merge
      // vector<bool> segment_memo(segments.size(), false);
      vector<vector<Vertex>> next_segment;

      for (size_t i = 0; i < segments.size() - 1; ++i)
      {
        const auto current_segment = segments[i];
        const auto adjacent_segment = segments[i+1];

        if (CheckSegment(i) == true)
        {
          continue;
        }

        if (IsMergable(current_segment, adjacent_segment))
        {
          vector<Vertex> merged_segment = GetMergedSegment(current_segment, adjacent_segment);
          MarkForward(i);

          // merge if the segment can merge ahead segments
          for(size_t j = 2; i+j < segments.size() && IsMergable(merged_segment, segments[i+j]); ++j)
          {
            const auto offspring_segment = segments[i+j];
            merged_segment = GetMergedSegment(merged_segment, offspring_segment);
            Mark(i+j);
          }

          next_segment.emplace_back(merged_segment);
        }
        else
        {
          next_segment.emplace_back(current_segment);
          Mark(i);
        }
      }
      
      if (CheckSegment(segments.size() - 1) == false)
      {
        const auto segment_last_idx = segments.size() - 1;
        Mark(segment_last_idx);
        next_segment.emplace_back(segments[segment_last_idx]);
      }

      segments.clear();
      InstantiateSegmentChecker(next_segment.size());
      segments = next_segment;
     
#ifdef DEBUG
      std::cout << "===== Merged Segment =====" << std::endl; 
      std::cout << segments << std::endl;
#endif

      ConstructSmallSegment(embedding);
    }
  
  private:
    /// <summary>
    /// instantiate segment
    /// </summary>
    void ConstructInitSegment()
    {
      vector<bool> check(graph->GetGraphSize(), false);
      for (const auto& clique: graph->GetMaximumClique())
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
      
      InstantiateSegmentChecker(segments.size());
      std::sort(segments.begin(), segments.end());

#ifdef DEBUG
      std::cout << "===== Initial Segment =====" << std::endl;
      std::cout << segments << std::endl;
#endif
    }

    /// <summary>
    /// merge segments that don't have length higher than thereshold
    /// </summary>
    void ConstructSmallSegment(const Embedding<VectorDim, LangType>& embedding)
    {
      const auto segment_relatedness = [&embedding, this](const auto& seg1, const auto& seg2)
      {
        double rel = 1.0;
        for (const auto& sent1: seg1)
        {
          for (const auto& sent2: seg2)
          {
            rel *= embedding.GetSimilarity(
              graph->GetSentence(sent1),
              graph->GetSentence(sent2)
            );
          } 
        }
        return rel / seg1.size() * seg2.size();
      };

      vector<vector<Vertex>> next_segments;

      for (size_t i = 0; i < segments.size() - 1; ++i)
      {
        const auto current_segment = segments[i];
        const auto next_segment = segments[i+1];

        if (CheckSegment(i) == true)
        {
          continue;
        }

        if (current_segment.size() < minimum_segment_size)
        {
          if (i == 0) // 最初のセグメントは一個後のものしかマージ対象にならない
          {
            auto merged_segment = GetMergedSegment(current_segment, next_segment);
            MarkForward(i);
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
              MarkBackward(i);
              next_segments.emplace_back(merged_segment);
            }
            else
            {
              auto merged_segment = GetMergedSegment(current_segment, next_segment);
              MarkForward(i);
              next_segments.emplace_back(merged_segment);
            }
          }
        }
        else
        {
          next_segments.emplace_back(current_segment);
          Mark(i);
        }
      }

      // 最後のセグメントがマージされなかった場合余るので追加
      if (CheckSegment(segments.size() - 1) == false)
      {
        const auto last_segment_idx = segments.size() - 1;
        Mark(last_segment_idx);
        next_segments.emplace_back(segments[last_segment_idx]);
      }

      segments.clear();
      InstantiateSegmentChecker(next_segments.size());
      segments = next_segments;

#ifdef DEBUG
      std::cout << "===== Small Segment =====" << std::endl;
      std::cout << segments << std::endl;
#endif
    }
  };
}

#endif