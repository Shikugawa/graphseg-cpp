#define DEBUG

#include "graphseg/graphseg.hpp"

#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <list>
#include <set>

#include <codecvt>
#include <clocale>
#include <cstdio>
#include <locale>

using namespace std;
using namespace GraphSeg;
using namespace GraphSeg::graph;

int main()
{
  constexpr Lang LangType = Lang::JP;
  constexpr int VectorDim = 50;

  const std::string home = getenv("HOME");
  std::string dataPath = home + "/graphseg-cpp/data/article01.txt";
  
  TextProcessor<LangType> tp(dataPath);
  Embedding<VectorDim, LangType> em;
  
  for(auto& sentence: tp.GetSentences())
  {
    em.AddSentenceWords(sentence);
  }
  em.GetWordEmbeddings();

  for (const auto& sentnece : tp.GetSentences())
  {
    std::cout << sentnece.GetText() << std::endl;
  }

  // Set texts
  double thereshold = 300;
  
  SegmentationContainer<UndirectedGraph<LangType>, VectorDim, LangType> seg(
    tp.GetSentences(), em);

  seg.SetThreshold(thereshold);
  seg.SetGraph();
  seg.Segmentation();
  
  for (const auto& segment : seg.GetSegment())
  {
    for (const auto& sentence_idx : segment)
    {
      std::cout << tp.GetSentences().at(sentence_idx).GetText() << std::endl;
    }
  }
  return 0;
}
