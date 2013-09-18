#CXX=clang++ -std=c++11 -stdlib=libc++ -Wall -Wextra -U__STRICT_ANSI__ -D__STDC_FORMAT_MACROS
CXX=g++ -std=c++11 -Wall -Wextra -pedantic
LDFLAGS=-pthread
RM=rm -f

all: partone parttwo

partone: partone.o
	$(CXX) -o partone partone.o

parttwo: parttwo.o
	$(CXX) $(LDFLAGS) -o parttwo parttwo.o

partone.o: partone.cpp

parttwo.o: parttwo.cpp

clean:
	$(RM) partone.o parttwo.o

dist-clean:
	$(RM) partone parttwo
