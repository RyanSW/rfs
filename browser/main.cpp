#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "rfs.h"

extern struct Entry _binary_entries_start;
struct Entry* root = &_binary_entries_start;

int main() {
  std::cout << "Interactive browser testing. Type \"quit\" to exit."
            << std::endl;

  while (true) {
    std::string path;
    std::cout << "Path? " << std::flush;
    std::cin >> path;
    if (path == "quit")
      return 0;

    std::istringstream iss(path);
    std::vector<std::string> split;

    for (std::string s; std::getline(iss, s, '/');)
      split.push_back(s);

    std::vector<const char*> c_strs;
    for (auto item = split.begin(); item != split.end(); item++) {
      c_strs.push_back((*item).c_str());
    }

    struct File my_file = read_file(root, &c_strs[0], c_strs.size() - 1);

    if (my_file.data) {
      std::cout << "File found. Size: " << my_file.size << std::endl;
      std::cout << std::string(my_file.data, my_file.size) << std::endl;
    } else {
      std::cout << "File not found." << std::endl;
    }
  }

  return 0;
}