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

constexpr Lang LangType = Lang::JP;
constexpr int VectorDim = 50;

vector<Sentence<LangType>> s;
Embedding<VectorDim, LangType> em;

void PrepareSentenceJP()
{
  s.emplace_back(Sentence<LangType>(L"太郎は花子にプレゼントを渡した。"));
  s.emplace_back(Sentence<LangType>(L"プレゼントの中身は彼女のお気に入りの小説だった"));
}

void PrepareSentenceEN()
{
  s.emplace_back(Sentence<LangType>(L"I want to eat rabbit"));
  s.emplace_back(Sentence<LangType>(L"rabbit is easy to eat"));
  s.emplace_back(Sentence<LangType>(L"turtle is slower than rabbit"));
  s.emplace_back(Sentence<LangType>(L"turtle is yummy"));
  s.emplace_back(Sentence<LangType>(L"especially, turtle soup is delicious"));
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

  const std::string home = getenv("HOME");
  std::string dataPath = home + "/graphseg-cpp/data/article01.txt";
  auto stream = TextTransform<LangType>(dataPath);

  // if (dest[0] == L'あ')
  // {
  //   std::cout << "ああああああ" << std::endl;
  // }

  // for (int i = 0; i < stream.size(); ++i)
  // {
  //   std::wcout << stream[i] << std::endl;
  // }
  
  Transfer<LangType> transfer(stream);
  auto sentences = transfer.Transcode();

  for (auto&& sentnece : sentences)
  {
    std::cout << sentnece.GetText() << std::endl;
  }

  // Set texts
  // double thereshold = 35;
  // PrepareSentenceJP();
  // PrepareEmbedding();

  // SegmentationContainer<UndirectedGraph<LangType>, VectorDim, LangType> seg(s, em);

  // seg.SetThreshold(thereshold);
  // seg.SetGraph();
  // seg.Segmentation();
  
  // auto segments = sg.GetSegment();

  return 0;
}
