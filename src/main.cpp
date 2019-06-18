#include "graphseg/graphseg.h"
#include <vector>
#include <iostream>
#define DEBUG

using namespace std;
using namespace GraphSeg;

int main()
{
  // Set texts
  vector<Sentence> s;
  s.emplace_back(Sentence("This is a pen"));
  s.emplace_back(Sentence("I want to play soccer"));
  s.emplace_back(Sentence("soccer is very fun"));

  EmbeddingManager em;
  GraphManager gm(SegmentGraph(3));

  gm.SetVertices(s);
  gm.SetEdges(em);
  auto graph = gm.GetGraph();

  for(const auto& v: graph.GetMaximumClique())
  {
    std::cout << "{";
    for(const auto& r: v)
    {
      std::cout << r << ",";
    }
    std::cout << "}" << std::endl;
  }
  return 0;
}