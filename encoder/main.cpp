#include <deque>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <new>
#include <utility>
#include <vector>

#include "rfs.h"

const char Error_NoFolder[] = "Error, please provide a valid folder";
const char Error_NoFile[] = "Error, please provide an output file";

typedef std::pair<struct Entry*, int> EntryP;
namespace fs = std::filesystem;

EntryP get_entry(fs::path file_path) {
  int dirs = 0;
  int dlen = 0;
  int files = 0;
  int flen = 0;
  int sizes = 0;

  auto d_entries = std::vector<EntryP>();

  for (auto file : std::filesystem::directory_iterator(file_path)) {
    if (file.is_directory()) {
      dirs++;
      dlen += file.path().filename().string().size();
      d_entries.push_back(get_entry(file.path()));
    } else if (file.is_regular_file()) {
      files++;
      flen += file.path().filename().string().size();
      sizes += std::filesystem::file_size(file);
    } else {
      std::cerr << "Unsupported file type: " << file.path().string()
                << std::endl;
    }
  }

  int total_size = sizeof(struct Entry) +
                   sizeof(struct NameEntry) * (dirs + files) + dlen + flen +
                   sizes;

  int adjust = (4 - (total_size % 4)) % 4;  // Allign entry to 4 bytes
  total_size += adjust;

  int folder_curoff = total_size / 4;

  for (auto entry : d_entries) {
    total_size += entry.second;
  }

  struct Entry* this_entry = (struct Entry*)::operator new(total_size);
  this_entry->dirs = dirs;
  this_entry->dlen = dlen;
  this_entry->files = files;
  this_entry->flen = flen;

  int name_i = 0;
  char* name_ptr = (char*)((uint8_t*)this_entry + sizeof(struct Entry) +
                           sizeof(struct NameEntry) * (dirs + files));

  char* file_ptr = (char*)((uint8_t*)name_ptr + dlen + flen);

  int folder_i = 0;
  uint8_t* folder_ptr = (uint8_t*)file_ptr + sizes + adjust;

  for (auto file : std::filesystem::directory_iterator(file_path)) {
    if (!file.is_directory())
      continue;

    auto name = file.path().filename().string();
    auto subdir_entry = d_entries[folder_i++];

    this_entry->names[name_i].length = name.length();
    this_entry->names[name_i++].offset = folder_curoff;

    std::copy(name.begin(), name.end(), name_ptr);
    name_ptr += name.size();

    std::copy((uint8_t*)subdir_entry.first,
              (uint8_t*)subdir_entry.first + subdir_entry.second, folder_ptr);
    folder_ptr += subdir_entry.second;
    folder_curoff += subdir_entry.second / 4;
  }

  for (auto file : std::filesystem::directory_iterator(file_path)) {
    if (!file.is_regular_file())
      continue;

    auto name = file.path().filename().string();
    auto size = std::filesystem::file_size(file);

    this_entry->names[name_i].length = name.length();
    this_entry->names[name_i++].offset = size;

    std::copy(name.begin(), name.end(), name_ptr);
    name_ptr += name.size();

    std::ifstream ifile(file.path(), std::ios::binary);
    ifile.read(file_ptr, size);
    file_ptr += size;
  }

  for (auto entry : d_entries) {
    free(entry.first);
  }

  return std::make_pair(this_entry, total_size);
}

int main(int argn, const char** argc) {
  if (argn < 2) {
    std::cerr << Error_NoFolder << std::endl;
    return 1;
  }
  if (argn < 3) {
    std::cerr << Error_NoFile << std::endl;
    return 1;
  }
  auto folder_path = std::filesystem::path(argc[1]);
  auto file = std::filesystem::path(argc[2]);

  if (!std::filesystem::is_directory(folder_path)) {
    std::cerr << Error_NoFolder << std::endl;
    return 1;
  }

  std::ofstream ofile(argc[2], std::ios::binary);
  auto e = get_entry(folder_path);
  ofile.write((char*)e.first, e.second);
  ofile.flush();

  std::cout << "Encoded " << file_count(e.first) << " files." << std::endl;

  return 0;
}