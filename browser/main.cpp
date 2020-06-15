#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "rfs.h"

extern RFS::Entry _binary_entries_start;
RFS::Entry* root = &_binary_entries_start;

void print_tree(const RFS::Entry* entry, int depth = 0) {
  for (int i = 0; i < entry->files; i++) {
    std::cout << std::string(depth, ' ') << "- ";
    RFS::FileName file = RFS::get_file_info(entry, i);
    std::cout << std::string(file.name, file.size) << std::endl;
  }
  for (int i = 0; i < entry->dirs; i++) {
    std::cout << std::string(depth, ' ') << "+ ";
    RFS::Folder folder = RFS::get_folder_info(entry, i);
    std::cout << std::string(folder.name.name, folder.name.size) << std::endl;
    print_tree(folder.entry, depth + 1);
  }
}

int main() {
  std::cout << "Interactive browser testing. Type \"quit\" to exit."
            << std::endl;

  print_tree(root);

  while (true) {
    std::string path;
    std::cout << "Path? " << std::flush;
    std::cin >> path;
    if (path == "quit")
      return 0;

    RFS::File my_file = RFS::read_file_p(root, path.c_str());

    if (my_file.data) {
      std::cout << "File found. Size: " << my_file.size << std::endl;
      // std::cout << std::string(my_file.data, my_file.size) << std::endl;
    } else {
      std::cout << "File not found." << std::endl;
    }
  }

  return 0;
}