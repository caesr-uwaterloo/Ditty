#pragma once
#ifndef VITIS_SYSTEM_ADDRESS_MAP_H
#define VITIS_SYSTEM_ADDRESS_MAP_H
// #include "common.h"
#include <iostream>

// 0 - Core
// 1 - N - Cache
// N+1 - +infty - Directory

// convert id to core id
#define getCoreID(x)  0
#define isCore(x) ((x) == 0)
// 1 - N
#define getCacheID(x) (x + 1)
#define isCache(x) (1 <= (x) && (x) <= N_)
// N + 1 - 2N
#define getDirID(x) (N_ + (x + 1))
#define isDir(x)    (N_ < (x))

machine_destination_t new_machine_destination() {
  return 0;
}

bool contains_destination(machine_destination_t dest, target_t target) {
  return (dest & (machine_destination_t (1) << target)) != 0;
}

target_t address_to_directory_id(addr_t a) {
  // currently all cache lines are mapped to the same directory
  return getDirID(0);
}

machine_destination_t add_dest(machine_destination_t dest, target_t target) {
  return dest | (1 << target);
}


#endif //VITIS_SYSTEM_ADDRESS_MAP_H
