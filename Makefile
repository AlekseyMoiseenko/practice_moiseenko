CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
SRCS     = common.cpp list.cpp file_io.cpp ui.cpp special.cpp tests.cpp main.cpp
OBJS     = $(SRCS:.cpp=.o)
TARGET   = app

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

test: $(TARGET)
	./$(TARGET) --test

.PHONY: all clean test