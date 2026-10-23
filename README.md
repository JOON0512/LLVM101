# LLVM101

LLVM tutorial @ CORELAB: 8 practices and 1 toy project

## Implementation and verification

CORELAB's tutorial is the starting point of this repository; the original scaffold and
exercise descriptions are credited below. The existing analysis exercises were retained.
The 2026 completion adds `inc(i32)`, an even-number guard for `printInt`, a dynamic
call counter, and load/store/malloc/free instrumentation with a runtime summary.
It also fixes the loop-pass registration typo, first-call counting, and the memory
example's undersized allocation. The broken nested `llvm-tutorial` gitlink was removed;
the actual exercises live directly in `practices/` and `projects/`.

Use **LLVM 14** (legacy pass manager). On Linux with clang/opt/llvm-config 14 on PATH:

```sh
python3 tests/run.py
```

Or build the supplied Dockerfile and run the suite against this checkout:

```sh
docker build -t llvm101-test .
docker run --rm --network none -v "$PWD:/work" llvm101-test
```

Verified on 2026-10-24 with LLVM 14.0.6 on Ubuntu 24.04 x86-64: all 8 pass libraries
compile, transformed IR passes LLVM verification, executable results match expected
increment/even-number/call/access counts, applying transformations twice does not
double-instrument, and the memory example passes ASan/UBSan. The Dockerfile uses
Ubuntu 22.04's LLVM 14 packages; that image was not used for the recorded local run.

These are teaching passes for individual Linux C translation units. The dynamic counter
counts executed call sites, including indirect calls, excluding LLVM intrinsics and its
own runtime. The supplied sample reports **61** calls. Normal return and `exit()` produce
a summary; `_Exit()`, crashes, and forced termination do not.

MemoryProfiler counts fixed-size address-space-0 loads/stores and direct `malloc`/`free`
calls. It observes access attempts without reading application memory. It does not
implement a leak detector, bounds checking, calloc/realloc tracking, memory intrinsics,
atomic RMW counting, scalable-vector access sizes, or exception-based allocation calls.
Other address spaces and unsupported allocation forms are skipped. Link runtime code
separately after instrumenting the application.

The `inc` exercise supplies `int inc(int)` without rewriting existing arithmetic. The
`printInt` guard handles positive/negative/zero values and preserves the existing body.
Compiler/library upgrades beyond LLVM 14 require a separate migration.

References: [CORELAB tutorial](https://github.com/corelab-src/llvm-tutorial),
[LLVM legacy passes](https://llvm.org/docs/WritingAnLLVMPass.html).

# LLVM Tutorial

## General Information

This LLVM tutorial is to help get familiar with LLVM provided by CORELAB. :)

* List of Practices
  * Basic
      - (1) First Compilation
      - (2) Control Flow Graph
  * Optimization 
      * Analysis
        - (3) Function Name Printer
        - (4) Instruction Counter
        - (5) Call Instruction Counter
        - (6) Loop Info 
      * Transformation
        - (7) Insert inc Function
        - (8) Print Even Number 
        - (9) Dynamic Call Counter

* List of Toy Projects
  * Optimization
    * MemoryProfiler

## Prerequisite
### LLVM Installation

For your convenience, I wrote a Makefile that automatically downloads and builds LLVM 14.0.0.  
Make directory llvm and move Makefile into it.
And just hit make!

## Instructions

### Testing

#### Compile your pass

In each practice folder, type
~~~bash
$ make
~~~
You will get the library file that contains your pass.

#### Apply your pass to a test program

In each practice folder, type
~~~bash
$ make test
~~~
You will get the optimized IR code (test.opt.ll) for the test program.

#### Run the optimized program

In each practice folder, type
~~~bash
$ make run
~~~
You will get the output of the optimized program.

---
This repository is distributed under the University of Illinois Open Source License.
