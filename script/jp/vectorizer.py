import os, sys
import gensim
import json
import numpy
from gensim.models.word2vec import Word2Vec


class MyEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, numpy.integer):
            return int(obj)
        elif isinstance(obj, numpy.floating):
            return float(obj)
        elif isinstance(obj, numpy.ndarray):
            return obj.tolist()
        else:
            return super(MyEncoder, self).default(obj)


model_path = os.path.join(
    os.environ["HOME"], "graphseg-cpp/script/jp/word2vec.gensim.model"
)

model = Word2Vec.load(model_path)

line = input()

out = {}
for word in line.split(" "):
    vector = None
    try:
        if word[-1] == ".":
            vector = model.wv[word[:-1]]
        else:
            vector = model.wv[word]
    except:
        continue
    if len(vector) != 0:      
        out[word] = vector

print(json.dumps(out, cls=MyEncoder))
