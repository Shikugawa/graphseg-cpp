#ifndef GRAPHSEG_CPP_GRAPHSEG_INTERNAL_UTIL_EXEC_HPP
#define GRAPHSEG_CPP_GRAPHSEG_INTERNAL_UTIL_EXEC_HPP

#include <string>
#include <memory>
#include <array>
#include <cstdio>
#include <cstdlib>

namespace GraphSeg::internal::utils
{
static constexpr auto BUFFER_SIZE = 256;

std::string exec(const char *cmd, int &code)
{
  std::string stdout;
  std::shared_ptr<FILE> pipe(popen(cmd, "r"), [&](FILE *p) { code = pclose(p); });
  if (!pipe)
  {
    return stdout;
  }
  std::array<char, BUFFER_SIZE> buf;
  while (!feof(pipe.get()))
  {
    if (fgets(buf.data(), buf.size(), pipe.get()) != nullptr)
    {
      stdout += buf.data();
    }
  }
  return stdout;
}
} // namespace GraphSeg::internal::utils

#endif