#define DEBUG

#include "graphseg/graphseg.h"
#include <vector>
#include <iostream>
#include <string>
#include <list>
#include <set>
#include <mecab.h>

using namespace std;
using namespace GraphSeg;
using namespace GraphSeg::graph;

vector<Sentence<>> s;
Embedding em;

// class MeCab
// {

// };

void PrepareSentenceStream()
{
  s.emplace_back(Sentence("I want to eat rabbit"));
  s.emplace_back(Sentence("rabbit is easy to eat"));
  s.emplace_back(Sentence("turtle is slower than rabbit"));
  s.emplace_back(Sentence("turtle is yummy"));
  s.emplace_back(Sentence("especially, turtle soup is delicious"));
}

void PrepareSentenceGraph(UndirectedGraph& ud)
{
  ud.SetEdge(0, 7, 30);
  ud.SetEdge(0, 8, 15);
  ud.SetEdge(7, 8, 15);
  ud.SetEdge(0, 1, 20);
  ud.SetEdge(0, 6, 10);
  ud.SetEdge(1, 5, 5);
  ud.SetEdge(1, 6, 20);
  ud.SetEdge(1, 3, 10);
  ud.SetEdge(6, 3, 15);
  ud.SetEdge(5, 2, 30);
  ud.SetEdge(2, 3, 8);
  ud.SetEdge(3, 4, 20);
  ud.SetEdge(2, 4, 18);
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
  Lang lang = Lang::JP;

  s.emplace_back("太郎は花子にプレゼントを渡した。");
  s.emplace_back("プレゼントの中身は彼女のお気に入りの小説だった。");

  // Set texts
  double thereshold = 0.05;
  // PrepareSentenceStream();
  PrepareEmbedding();

  // UndirectedGraph ug(9);
  // PrepareSentenceGraph(ug);
  // SegmentationContainer<decltype(ug)> ctr(ug);

  // ctr.SetThreshold(thereshold);
  // ctr.SetVertices(s);
  // ctr.SetEdges(em);

  // auto& sg = ctr.GetGraph();
  // sg.SetMaximumClique();
  // sg.ConstructSegment(em);  
  // auto segments = sg.GetSegment();

  return 0;
}
