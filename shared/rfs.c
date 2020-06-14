#include "rfs.h"

#include <string.h>

struct File read_file(struct Entry* entry, const char** path, uint16_t depth) {
  struct NameEntry* names = entry->names;
  uint8_t max = entry->dirs;
  uint8_t nlen = strlen(path[0]);
  char* n_ptr = (char*)(names + entry->dirs + entry->files);
  uint32_t offset = sizeof(struct Entry) +
                    sizeof(struct NameEntry) * (entry->dirs + entry->files) +
                    entry->dlen + entry->flen;
  uint32_t this = 0;

  if (depth == 0) {
    names += entry->dirs;
    n_ptr += entry->dlen;
    max = entry->files;
  }

  for (uint8_t name = 0; name < max; name++) {
    if (names[name].length == nlen && strncmp(n_ptr, path[0], nlen) == 0) {
      this = names[name].offset;
      goto found;
    }
    n_ptr += names[name].length;
    offset += names[name].offset;
  }

  return (struct File){};

found:

  if (depth == 0) {
    return (struct File){.size = this,
                         .data = (char*)((uint8_t*)entry + offset)};
  } else {
    struct Entry* subdir = (struct Entry*)((uint8_t*)entry + this * 4);
    return read_file(subdir, path + 1, depth - 1);
  }
}

uint32_t file_count(struct Entry* entry) {
  uint32_t retval = 0;
  struct NameEntry* names = entry->names;
  uint32_t offset = sizeof(struct Entry) +
                    sizeof(struct NameEntry) * (entry->dirs + entry->files) +
                    entry->dlen + entry->flen;

  for (uint8_t i = 0; i < entry->dirs; i++) {
    retval += file_count((struct Entry*)((uint8_t*)entry + names->offset * 4));
    offset += names->offset;
    names++;
  }

  retval += entry->files;

  return retval;
}