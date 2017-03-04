CXXFLAGS = -std=c++11 -Wall -Wextra -Wno-unused-parameter

BIN      = color
SRCS     = main.cpp
SRCS    += csscolorparser.cpp
OBJS     = $(patsubst %.cpp,%.o,$(SRCS))

build: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^

clean:
	rm -rf  *.o $(BIN)

.PHONY: clean
