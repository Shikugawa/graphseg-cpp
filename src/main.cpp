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
  WordVectorManager wvm;
  for(const auto& _s : s)
  {
    wvm.AddSentenceWords(_s);
  }
  wvm.GetWordEmbeddings();

  GraphManager gm(sg(3));
  gm.SetVertices(s)
  gm.SetEdges(wvm);
  auto graph = sg.GetGraph()
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