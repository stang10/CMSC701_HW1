#include "libsais64.h"
#include <iostream>
#include <memory>

int main(int argc, char* argv[]) {
  
  std::string test_string("banana$");
  std::unique_ptr<int64_t[]> sa(new int64_t[test_string.size()]);
  int64_t n = test_string.size();
  assert(n > 0);
  int64_t fs = 0;
  int64_t* freq = nullptr;
  
  int64_t ret = libsais64(reinterpret_cast<uint8_t*>(test_string.data()), sa.get(), n, fs, freq);
  if (ret == 0) {
    std::cout << "suffix array construction successful!\n";
    std::cout  << "[ ";
    for (size_t i = 0; i < n; ++i) {
      std::cout << sa[i] << ((i < n-1) ? ", " : " ");
    }
    std::cout << "]\n";
  } else {
    std::cout << "suffix array construction failed; exit code = " << ret << "\n";
  }

  return 0;
}
