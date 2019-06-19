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
  s.emplace_back(Sentence("I play"));
  s.emplace_back(Sentence("I love soccer very much"));
  EmbeddingManager em;
  for(auto& _s: s)
  {
    em.AddSentenceWords(_s);
  }
  em.GetWordEmbeddings();
  GraphManager gm(SegmentGraph(3));

  gm.SetVertices(s);
  gm.SetEdges(em);
  auto sg = gm.GetGraph();
  sg.SetMaximumClique();
  
  // SegmentGraph sg(3);
  // sg.SetSentence(s[0]);
  // sg.SetSentence(s[1]);
  // sg.SetSentence(s[2]);
  // sg.SetEdge(0, 1, -30.2);
  // sg.SetEdge(1, 2, -15.2);
  // sg.SetEdge(2, 0, -15.8);
  // sg.SetMaximumClique();
  std::cout << sg << std::endl;
  for(const auto& v: sg.GetMaximumClique())
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