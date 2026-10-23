#include <stdio.h>
#include <atomic>
#include <cstdint>

static std::atomic<uint64_t> allocations{0}, frees{0}, loads{0}, stores{0};
static std::atomic<uint64_t> allocatedBytes{0}, loadedBytes{0}, storedBytes{0};

extern "C"
void traceMalloc(void *addr, size_t size) {
    if (!addr) return; // Failed allocations do not count as allocated memory.
    allocations.fetch_add(1, std::memory_order_relaxed);
    allocatedBytes.fetch_add(size, std::memory_order_relaxed);
}

extern "C"
void traceLoad(void *addr, size_t size) {
    (void)addr; // Observes access counts; never dereferences program pointers.
    loads.fetch_add(1, std::memory_order_relaxed);
    loadedBytes.fetch_add(size, std::memory_order_relaxed);
}

extern "C"
void traceStore(void *addr, size_t size) {
    (void)addr;
    stores.fetch_add(1, std::memory_order_relaxed);
    storedBytes.fetch_add(size, std::memory_order_relaxed);
}

extern "C" void traceFree(void *addr) {
    if (addr) frees.fetch_add(1, std::memory_order_relaxed);
}

extern "C" void printMemorySummary() {
    fprintf(stderr, "Memory profile: malloc=%llu bytes=%llu free=%llu load=%llu bytes=%llu store=%llu bytes=%llu\n",
        (unsigned long long)allocations.load(), (unsigned long long)allocatedBytes.load(),
        (unsigned long long)frees.load(), (unsigned long long)loads.load(),
        (unsigned long long)loadedBytes.load(), (unsigned long long)stores.load(),
        (unsigned long long)storedBytes.load());
}
