import json
from nltk import FreqDist
from nltk.corpus import knbc

cp = knbc.words()
dist = FreqDist(cp)
total_count = 0

for term, count in dist.items():
    total_count += count

if __name__ == "__main__":
    line = input()
    out = {"corpus_size": len(dist), "total_count": total_count} # |C|
    for word in line.split(" "):
        out[word] = dist[word]
    print(json.dumps(out))