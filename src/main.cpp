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

constexpr Lang LangType = Lang::EN;
constexpr int VectorDim = 300;

vector<Sentence<LangType>> s;
Embedding<VectorDim, LangType> em;

void PrepareSentenceJP()
{
  s.emplace_back(Sentence<LangType>("太郎は花子にプレゼントを渡した。"));
  s.emplace_back(Sentence<LangType>("プレゼントの中身は彼女のお気に入りの小説だった"));
}

void PrepareSentenceEN()
{
  s.emplace_back(Sentence<LangType>("I want to eat rabbit"));
  s.emplace_back(Sentence<LangType>("rabbit is easy to eat"));
  s.emplace_back(Sentence<LangType>("turtle is slower than rabbit"));
  s.emplace_back(Sentence<LangType>("turtle is yummy"));
  s.emplace_back(Sentence<LangType>("especially, turtle soup is delicious"));
}

void PrepareSentenceGraph(UndirectedGraph<LangType>& ud)
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
  double thereshold = 35;
  PrepareSentenceEN();
  PrepareEmbedding();

  SegmentationContainer<UndirectedGraph<LangType>, VectorDim, LangType> seg(s, em);

  seg.SetThreshold(thereshold);
  seg.SetGraph();
  seg.Segmentation();
  
  // auto segments = sg.GetSegment();

  return 0;
}
