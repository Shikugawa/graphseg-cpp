import gensim
import sys

model = gensim.models.KeyedVectors.load_word2vec_format('GoogleNews-vectors-negative300.bin', binary=True)

if __name__ == "__main__":
  for line in sys.stdin:
    out = {}
    words = line.split(' ')
    for word in words:
      vector = model.wv[word]
      out[word] = list(vector)
    print(out)