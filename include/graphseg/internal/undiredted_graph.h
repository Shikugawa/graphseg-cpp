#include <iostream>
#include <set>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

namespace GraphSeg
{
  using std::vector, std::make_pair, std::set, std::string,  std::set_intersection, std::inserter;
  using Vertex = unsigned int;
  using Edge = std::pair<Vertex, double>;

  class UndirectedGraph
  {
  public:
    UndirectedGraph()
    {
    }

    void setSentence() 
    {
      ++node_idx;
      graph.emplace_back(vector<Edge>());
    }

    void setSimilarity(int src, int dst, double score)
    {
      graph[src].emplace_back(make_pair(dst, score));
      graph[dst].emplace_back(make_pair(src, score));
    }

    Vertex getGraphSize() const noexcept
    {
      return node_idx;
    }

  private:
    set<Vertex> bronKerbosch(std::set<Vertex> clique, set<Vertex> candidates, set<Vertex> excluded)
    {
      if (candidates.empty() && excluded.empty())
      {
        return clique;
      }

      for (Vertex v: candidates)
      {
        auto candidates_t = candidates & getNeighbors(v);
        auto excluded_t = excluded & getNeighbors(v);
        set<Vertex> clique_t;
        std::copy(clique.begin(), clique.end(), std::back_inserter(clique_t));
        bronKerbosch(clique_t, candidates_t, excluded_t);
        candidates.erase(v);
        excluded.emplace(v);
      }
    }

    set<Vertex> getNeighbors(Vertex idx)
    {
      set<Vertex> tmp;
      for(const auto& node: graph[idx])
      {
        tmp.emplace(node.first);
      }
      return tmp;
    }

    vector<vector<Edge>> graph;
    set<Vertex> max_clique;
    Vertex node_idx;
  };

  template <typename T>
  set<T> operator&(const set<T>& v1, const set<T>& v2)
  {
    set<T> result;
    set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), inserter(result));
    return result;
  }
} // namespace GraphSeg