#ifndef MIGRATION_INFO_HH
#define MIGRATION_INFO_HH

#include <cstdint>

class MigrationInfo {
 public:
  uint16_t index;
  int16_t source;
  int16_t target;

  char* mem_ptr;
  ulong sl_index;
  char target_fn[32];
  char store_fn[32];

  MigrationInfo() : source(0), target(0), mem_ptr(nullptr){};
  ~MigrationInfo() = default;
  // source node, target node, mem ptr for slices, offset of slices
  MigrationInfo(int16_t s, int16_t t, ulong sl_i)
      : source(s), target(t), mem_ptr{nullptr}, sl_index(sl_i) {}
};

#endif