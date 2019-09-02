#define DEBUG

#include "graphseg/graphseg.h"
#include <vector>
#include <iostream>
#include <string>

using namespace std;
using namespace GraphSeg;
using namespace GraphSeg::graph;

vector<Sentence> s;
Embedding em;

void PrepareSentenceStream()
{
  s.emplace_back(Sentence("I want to eat rabbit"));
  s.emplace_back(Sentence("rabbit is easy to eat"));
  s.emplace_back(Sentence("turtle is slower than rabbit"));
  s.emplace_back(Sentence("turtle is yummy"));
  s.emplace_back(Sentence("especially, turtle soup is delicious"));
}

void PrepareEmbedding()
{  
  for(auto& _s: s)
  {
    em.AddSentenceWords(_s);
  }
  em.GetWordEmbeddings();
}

int main()
{
  // Set texts
  double thereshold = 0.05;

  PrepareSentenceStream();
  PrepareEmbedding();

  UndirectedGraph ug;

  GraphContainer ctr(ug);
  ctr.SetThreshold(thereshold);
  ctr.SetVertices(s);
  ctr.SetEdges(em);

  auto& sg = ctr.GetGraph();

  sg.SetMaximumClique();
  sg.ConstructSegment(em);
  
  return 0;
}
