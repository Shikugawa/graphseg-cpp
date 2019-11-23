# GraphSeg-cpp

re-implementation of GraphSeg for text segmentation with undirected graph falling into maximum clique problem.

## Dependencies
### C++ Dependencies
- boost
- rapidjson
- mecab

### Python Dependencies
- gensim >= 3.8
- nltk

### Depencenty Corpus
In japanese, knbc corpus is needed to calculate word frequency

check whether that corpus is installed to your `nltk_data` path
`nltk_data` path can be got to run under script in REPL

```
import nltk
nltk.data.path
```

### Example
```PY_SCRIPT_PATH``` and ```PYTHON_PATH``` is needed

```sh
PY_SCRIPT_PATH=${HOME}/graphseg-cpp/script PYTHON_PATH=${HOME}/.pyenv/shims/python ./graphseg_main
```

```cpp
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

```

## How to Build
CMake is used as build config generator, and vcpkg is employed as package management system

```
cmake -DCMAKE_TOOLCHAIN_FILE=$(cat ~/.vcpkg/vcpkg.path.txt)/scripts/buildsystems/vcpkg.cmake -DCMAKE_CXX_COMPILER=clang++ ..
```

## References
- Goran Glavaˇs, Federico Nanni, Simone Paolo Ponzetto, 2016, Unsupervised Text Segmentation Using Semantic Relatedness Graphs, 5th Joint Conference on Lexical and Computational Semantics, Proceedings, pp. 125-130

## LICENCE
MIT