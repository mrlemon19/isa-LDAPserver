CC = g++

CFLAGS = -Wall -std=c++17 -pthread

SRCS := $(wildcard *.cpp)
HDRS := $(wildcard *.h)

OBJS := $(SRCS:.cpp=.o)

TARGET = isa-ldapserver

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp $(HDRS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean