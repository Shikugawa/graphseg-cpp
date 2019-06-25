#ifndef GRAPHSEG_CPP_GRAPHSEG_INTERNAL_UTIL_H
#define GRAPHSEG_CPP_GRAPHSEG_INTERNAL_UTIL_H

#include <memory>
#include <array>
#include <cstdio>
#include <cstdlib>

namespace GraphSeg
{
  using std::array, std::shared_ptr;

  static constexpr auto BUFFER_SIZE = 256;
  
  string exec(const char* cmd, int& code)
  {
    string stdout;
    shared_ptr<FILE> pipe(popen(cmd, "r"), [&](FILE* p) {code = pclose(p);});
    if(!pipe)
    {
      return stdout;
    }
    array<char, BUFFER_SIZE> buf;
    while(!feof(pipe.get()))
    {
      if(fgets(buf.data(), buf.size(), pipe.get()) != nullptr)
      {
        stdout += buf.data();
      }
    }
    return stdout;
  }
}

#endif