#include <stdio.h>
#include <map>
#include <atomic>
#include <mutex>
#include <cstdint>

static std::atomic<uint64_t> count{0};
static std::map<int, uint64_t> CountMap;
static std::mutex CountMutex;

extern "C"
void countCall() {
  count.fetch_add(1, std::memory_order_relaxed);
}

extern "C"
void printResult() {
  printf("Dynamic call count: %llu\n", (unsigned long long)count.load());
}

extern "C"
void countCallPerInst(int id) {
  std::lock_guard<std::mutex> Guard(CountMutex);
  ++CountMap[id];
}

extern "C"
void printResultPerInst() {
  std::lock_guard<std::mutex> Guard(CountMutex);
  printf("Dynamic call count: \n");
  for(auto &entry : CountMap) {
    printf("Inst ID: %d / Call count: %llu\n", entry.first, (unsigned long long)entry.second);
  }
}
