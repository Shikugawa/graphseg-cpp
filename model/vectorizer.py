import os, sys
import gensim

# import argparse
from gensim.models.word2vec import Word2Vec

model_path = os.path.join(
    os.environ["HOME"], "graphseg-cpp/model/GoogleNews-vectors-negative300.bin"
)

model = gensim.models.KeyedVectors.load_word2vec_format(
    model_path, binary=True, limit=50000
)

line = input()


def GetVector(word):
    try:
        if word[-1] == ".":
            vector = model.wv[word[:-1]]
        else:
            vector = model.wv[word]
        return list(vector)
    except Exception:
        return []


out = {word: GetVector(word) for word in line[:-1].split(" ")}
print(out)
