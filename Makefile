#CXX=clang++ -std=c++11 -stdlib=libc++ -Wall -Wextra -U__STRICT_ANSI__ -D__STDC_FORMAT_MACROS
CXX=g++ -std=c++11 -Wall -Wextra -pedantic
RM=rm -f

SRCS=partone.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: partone

partone: $(OBJS)
	$(CXX) -o partone $(OBJS)

partone.o: partone.cpp

clean:
	$(RM) $(OBJS)

dist-clean:
	$(RM) partone
