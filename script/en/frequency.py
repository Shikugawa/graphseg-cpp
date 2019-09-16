import json
from nltk import FreqDist
from nltk.corpus import gutenberg

emma = gutenberg.words("austen-emma.txt")
dist = FreqDist(emma)

if __name__ == "__main__":
    line = input()
    out = {"total_count": len(dist)}  # total_countという単語は間違いなく存在しないので可
    for word in line.split(" "):
        out[word] = dist[word]
    print(json.dumps(out))