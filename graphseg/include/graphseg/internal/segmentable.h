#ifndef GRAPHSEG_CPP_GRAPHSEG_SEGMENTABLE_H
#define GRAPHSEG_CPP_GRAPHSEG_SEGMENTABLE_H

#include "graphseg/internal/utils/custom_operator.h"
#include "graphseg/embedding.h"
#include "graphseg/segmentation_container.h"
#include "graphseg/lang.h"

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
  using std::list, std::vector, std::tuple;
  using Vertex = unsigned int;

  template <class Graph, int VectorDim, Lang LangType = Lang::EN>
  class Segmentable
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

  private:
    /// <summary>
    /// segment graph
    /// </summary>
    Graph graph;

  public:
    Segmentable() = default;

    Segmentable(const Graph& g) : graph(g)
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
        for(const auto& maximum_cliques: graph.GetMaximumClique(s))
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
    /// construct segment from maximum clique
    /// </summary>
    void ConstructSegment(const Embedding<VectorDim, LangType>& embedding)
    {
      if (segments.size() == 0)
      {
        ConstructInitSegment();
      }

      // to avoid searching same segment twice, memory searched segment whether it can merge
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

          // merge if the segment can merge ahead segments
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
      std::cout << segments << std::endl;
#endif

      ConstructInvalidSegment(embedding);
    }
  
  private:
    /// <summary>
    /// instantiate segment
    /// </summary>
    void ConstructInitSegment()
    {
      vector<bool> check(graph.GetGraphSize(), false);
      for (const auto& clique: graph.GetMaximumClique())
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
      std::cout << segments << std::endl;
#endif
    }

    /// <summary>
    /// merge segments that don't have length higher than thereshold
    /// </summary>
    void ConstructInvalidSegment(const Embedding<VectorDim, LangType>& embedding)
    {
      const auto segment_relatedness = [&embedding, this](const auto& seg1, const auto& seg2)
      {
        double rel = 1.0;
        for (const auto& sent1: seg1)
        {
          for (const auto& sent2: seg2)
          {
            rel *= embedding.GetSimilarity(
              graph.GetSentence(sent1),
              graph.GetSentence(sent2)
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
      std::cout << segments << std::endl;
#endif
    }
  };
}

#endif