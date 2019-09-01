#define DEBUG

#include "graphseg/graphseg.h"
#include <vector>
#include <iostream>
#include <string>

using namespace std;
using namespace GraphSeg;

int main()
{
  // Set texts
  double thereshold = 0.05;
  vector<Sentence> s;
  s.emplace_back(Sentence("I want to eat rabbit"));
  s.emplace_back(Sentence("rabbit is easy to eat"));
  s.emplace_back(Sentence("turtle is slower than rabbit"));
  s.emplace_back(Sentence("turtle is yummy"));
  s.emplace_back(Sentence("especially, turtle soup is delicious"));
  Embedding em;

  for(auto& _s: s)
  {
    em.AddSentenceWords(_s);
  }
  em.GetWordEmbeddings();
  
  GraphController gm(SegmentGraph(s.size()));
  gm.SetThreshold(thereshold);
  gm.SetVertices(s);
  gm.SetEdges(em);

  auto& sg = gm.GetGraph();
  sg.SetMaximumClique();

  std::cout << "======== maximum clique set =========" << std::endl;
  for(const auto& v: sg.GetMaximumClique())
  {
    std::cout << "{";
    for(const auto& r: v)
    {
      std::cout << r << ",";
    }
    std::cout << "}" << std::endl;
  }
  std::cout << "====================================" << std::endl;

  sg.ConstructSegment();
  
  return 0;
}
