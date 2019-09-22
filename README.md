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

## References
- Goran Glavaˇs, Federico Nanni, Simone Paolo Ponzetto, 2016, Unsupervised Text Segmentation Using Semantic Relatedness Graphs, 5th Joint Conference on Lexical and Computational Semantics, Proceedings, pp. 125-130

## LICENCE
MIT