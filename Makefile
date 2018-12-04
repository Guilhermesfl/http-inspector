CXX = g++
CXXFLAGS = -g
DEPS = aracne.hpp

%.o: %.c $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

aracne: main.o aracne.o
	$(CXX) -o aracne main.o aracne.o