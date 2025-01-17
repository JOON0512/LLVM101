# LLVM101

LLVM tutorial @ CORELAB: 8 practices and 1 toy project

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

