#include "graphseg/graphseg.h"
#include <vector>
#include <iostream>
#define DEBUG

using namespace std;
using namespace GraphSeg;

int main()
{
  // Set texts
  vector<Sentence> s = {
   Sentence("This is a pen"),
   Sentence("I want to play soccer"),
   Sentence("soccer is very fun")
  }

  // Word vector instantiation
  EmbeddingManager em;
  for(const auto& _s : s)
  {
    em.AddSentenceWords(_s);
  }
  em.GetWordEmbeddings();

  GraphManager gm(SegmentGraph(3));
  gm.SetVertices(s)
  gm.SetEdges(wvm);
  auto graph = gm.GetGraph()

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