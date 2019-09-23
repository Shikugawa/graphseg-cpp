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

constexpr Lang ltype = Lang::JP;
constexpr int VectorDim = 50;

vector<Sentence<ltype>> s;
Embedding<VectorDim, ltype> em;

void PrepareSentenceJP()
{
  s.emplace_back(Sentence<ltype>("太郎は花子にプレゼントを渡した。"));
  s.emplace_back(Sentence<ltype>("プレゼントの中身は彼女のお気に入りの小説だった"));
}

void PrepareSentenceEN()
{
  s.emplace_back(Sentence<ltype>("I want to eat rabbit"));
  s.emplace_back(Sentence<ltype>("rabbit is easy to eat"));
  s.emplace_back(Sentence<ltype>("turtle is slower than rabbit"));
  s.emplace_back(Sentence<ltype>("turtle is yummy"));
  s.emplace_back(Sentence<ltype>("especially, turtle soup is delicious"));
}

void PrepareSentenceGraph(UndirectedGraph<ltype>& ud)
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
  // Set texts
  double thereshold = 0.05;
  PrepareSentenceJP();
  PrepareEmbedding();

  UndirectedGraph<ltype> ug(s);
  SegmentationContainer<UndirectedGraph<ltype>, VectorDim, ltype> seg(ug, em);

  seg.SetThreshold(thereshold);
  seg.SetGraph();
  seg.Segmentation();
  
  // auto segments = sg.GetSegment();

  return 0;
}
