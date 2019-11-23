#define DEBUG

#include "graphseg/graphseg.hpp"

#include <vector>
#include <iostream>
#include <string>

using namespace std;
using namespace GraphSeg;
using namespace GraphSeg::graph;

int main()
{
  constexpr Lang LangType = Lang::JP;
  constexpr int VectorDim = 50;

  std::string dataPath = "/Users/shimizurei/graphseg-cpp/data/article01.txt";

  auto text = TextFactory<LangType>::Execute(dataPath);

  Embedding<VectorDim, LangType> em;

  for (auto &sentence : text.GetSentences())
  {
    em.AddSentenceWords(sentence);
  }
  em.GetWordEmbeddings();

  for (const auto &sentnece : text.GetSentences())
  {
    std::cout << sentnece.GetText() << std::endl;
  }

  // Set texts
  double thereshold = 300;

  SegmentationContainer<UndirectedGraph<LangType>, VectorDim, LangType> seg(
      text.GetSentences(), em);

  seg.SetThreshold(thereshold);
  seg.SetGraph();
  seg.Segmentation();

  for (const auto &segment : seg.GetSegment())
  {
    for (const auto &sentence_idx : segment)
    {
      std::cout << text.GetSentences().at(sentence_idx).GetText() << std::endl;
    }
  }
  return 0;
}
