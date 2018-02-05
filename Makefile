read_asm: test/read_asm.s

CXXFLAGS = --std=c++14 -O2 -I.

%.s: %.cpp
	$(CXX) -o $@ -S $< $(CXXFLAGS)

doc: test++/test++.h
	doxygen