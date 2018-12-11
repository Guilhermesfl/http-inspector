IDIR =../include
CXX=g++
CXXFLAGS=-g -std=c++11 -I$(IDIR)

ODIR=obj
LDIR =
LIBS=

_DEPS = proxy.hpp spider.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = aracne.o proxy.o spider.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

aracne: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 