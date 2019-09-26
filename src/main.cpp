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
  auto stream = TextTransform<LangType>(dataPath);

  Embedding<VectorDim, LangType> em;

  Transfer<LangType> transfer(stream);
  auto sentences = transfer.Transcode();

  for(auto& sentence: sentences)
  {
    em.AddSentenceWords(sentence);
  }
  em.GetWordEmbeddings();

  for (auto&& sentnece : sentences)
  {
    std::cout << sentnece.GetText() << std::endl;
  }

  // Set texts
  double thereshold = 300;
  
  SegmentationContainer<UndirectedGraph<LangType>, VectorDim, LangType> seg(sentences, em);

  seg.SetThreshold(thereshold);
  seg.SetGraph();
  seg.Segmentation();
  
  return 0;
}
