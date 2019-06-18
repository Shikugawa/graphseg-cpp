#include "graphseg/graphseg.h"
#include <vector>
#include <iostream>
#include <string>
#define DEBUG

using namespace std;
using namespace GraphSeg;

int main()
{
  // Set texts
  vector<Sentence> s;
  s.emplace_back(Sentence("a very large"));
  s.emplace_back(Sentence("a very large"));
  s.emplace_back(Sentence("I play"));

  EmbeddingManager em;
  for(auto& _s: s)
  {
    em.AddSentenceWords(_s);
  }
  em.GetWordEmbeddings();
  GraphManager gm(SegmentGraph(3));

  gm.SetVertices(s);
  gm.SetEdges(em);
  auto graph = gm.GetGraph();
  std::cout << graph << std::endl;
  graph.SetMaximumClique();
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