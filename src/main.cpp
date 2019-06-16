#include <set>
#include <iostream>
#include <string>
#define DEBUG

using namespace std;


int main()
{
  
  SegmentGraph sg(3);
  sg.SetSentence("This is a pen");
  sg.SetSentence("This is a pen");
  sg.SetSentence("This is a pen");
  sg.SetSentence("This is a pen");
  sg.SetSentence("This is a pen");
  sg.SetSentence("This is a pen");
  sg.SetSentence("This is a pen");
  sg.SetSentence("This is a pen");
  sg.SetSentence("This is a pen");
  sg.SetEdge(1, 2, 30);
  sg.SetEdge(2, 3, 15);
  sg.SetEdge(3, 1, 15);
  sg.SetEdge(1, 2, 20);
  sg.SetEdge(1, 6, 10);
  sg.SetEdge(2, 6, 5);
  sg.SetEdge(2, 7, 20);
  sg.SetEdge(2, 4, 10);
  sg.SetEdge(7, 4, 15);
  sg.SetEdge(6, 3, 30);
  sg.SetEdge(3, 4, 8);
  sg.SetEdge(4, 5, 20);
  sg.SetEdge(3, 5, 18);
  sg.SetMaximumClique();

  for(const auto& v: sg.GetMaximumClique())
  {
    std::cout << "{";
    for(const auto& r: v)
    {
      std::cout << r << ",";
    }
    std::cout << "}" << std::endl;
  }
  return 0;
}