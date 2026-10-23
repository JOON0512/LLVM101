LIBRARY=DynamicCallCount.so
SOURCES=DynamicCallCount.cpp

PASSNAME=dyn-call-count
TEST=test
TEST_LIB_FLAG=../Runtime/callCount.o -lstdc++

include ../../Makefile.common

$(TEST).exe: ../Runtime/callCount.o

../Runtime/callCount.o: ../Runtime/callCount.cpp
	$(MAKE) -C ../Runtime
