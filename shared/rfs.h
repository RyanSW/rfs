#ifndef RFS_LIB_H
#define RFS_LIB_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

struct NameEntry {
  uint32_t length : 8;
  uint32_t offset : 24;
};

struct Entry {
  uint32_t dirs : 8;
  uint32_t dlen : 24;
  uint32_t files : 8;
  uint32_t flen : 24;
  struct NameEntry names[];
};

struct File {
  uint32_t size;
  char* data;
};

struct File read_file(struct Entry* entry, const char** path, uint16_t depth);

uint32_t file_count(struct Entry* entry);

#ifdef __cplusplus
static_assert(sizeof(struct NameEntry) == 4);
static_assert(sizeof(struct Entry) == 8);
}
#endif
#endif /* RFS_LIB_H */
