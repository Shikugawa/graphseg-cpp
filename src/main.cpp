#define DEBUG

#include <iostream>
#include <absl/strings/string_view.h>

int main()
{
  absl::string_view sample{"testtest"};
  sample = sample.substr(1, 3);
  std::cout << sample << std::endl;
  std::cout << sample.data() << std::endl;
  return 0;
}
