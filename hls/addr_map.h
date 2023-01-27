#pragma once
#ifndef VITIS_SYSTEM_ADDR_MAP_H
#define VITIS_SYSTEM_ADDR_MAP_H
#include "constants.h"
#include <iostream>

// most likely N = 16 max
// 0 - Core
// 0 - N-1 - Cache
// N - +infty - Directory

// convert id to core id
// #define getCoreID(x)  0
// #define isCore(x) ((x) == 0)
// 1 - N
#define getCacheID(x) (x)
#define isCache(x) (0 <= (x) && (x) < N_)
// N + 1 - 2N
#define getDirID(x) (N_ + x)
#define isDir(x)    (N_ <= (x))

// could support multi-cast or forwarding into a directory
// We don't need to handle the core ids, that will be derived from the addr field
#warning "machine_destination_t is hard coded, scaling up requires a different approach"
typedef ap_uint<32> machine_destination_t;
typedef ap_uint<ADDR_W>  addr_t;
#warning "target_t is hard coded, scaling up requires a different approach"
// target_t is 15 because it is compressed within the word field
typedef ap_uint<15> target_t;


inline machine_destination_t new_machine_destination() {
  return 0;
}

inline bool contains_destination(machine_destination_t dest, target_t target) {
  return (dest & (machine_destination_t (1) << target)) != 0;
}

inline target_t address_to_directory_id(addr_t a) {
  // currently all cache lines are mapped to the same directory
  return getDirID(0);
}

inline machine_destination_t add_dest(machine_destination_t dest, target_t target) {
  return dest | (1 << target);
}

inline machine_destination_t remove_dest(machine_destination_t dest, target_t target) {
  return dest & ~(1 << target);
}

inline machine_destination_t single_destination(target_t target) {
  return add_dest(new_machine_destination(), target);
}

inline addr_t construct_llc_address(addr_t tag_without_set, llc_set_t set, llc_offset_t offset) {
  return (tag_without_set, set, offset);
}


inline int get_x(target_t t) {
  return 0;
}
inline int get_y(target_t t) {
  return 0;
}

#endif //VITIS_SYSTEM_ADDR_MAP_H
