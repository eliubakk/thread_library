CXX = g++
CXXFLAGS = -g -std=c++11 -Wall -Werror
DEBUGFLAGS = -DDEBUG

LIBS = libcpu.o -ldl -pthread

EXECUTABLE = app

PROJECTFILE = app.cc

TESTSOURCES = $(wildcard test*.cc)
TESTS = $(TESTSOURCES:%.cc=%)

SOURCES = $(wildcard *.cc)
SOURCES := $(filter-out $(TESTSOURCES), $(SOURCES))
OBJECTS = $(SOURCES:%.cc=%.o)

all: $(EXECUTABLE)

release: CXXFLAGS += -DNDEBUG
release: all

debug: CXXFLAGS += $(DEBUGFLAGS)
debug: clean all

$(EXECUTABLE): $(OBJECTS) #$(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LIBS) -o $(EXECUTABLE)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $*.cc

define make_tests
    SRCS = $$(filter-out $$(PROJECTFILE), $$(SOURCES))
    OBJS = $$(SRCS:%.cc=%.o)
    HDRS = $$(wildcard *.h)
    $(1): CXXFLAGS += $$(DEBUGFLAGS)
    $(1): $$(HDRS) $$(OBJS) $(1).cc
	$$(CXX) $$(CXXFLAGS) $$(LIBS) $$(OBJS) $(1).cc -o $(1)
endef
$(foreach test, $(TESTS), $(eval $(call make_tests, $(test))))

alltests: clean $(TESTS)

thread_globals.o: thread_globals.h thread_globals.cc
cpu_impl.o: cpu_impl.h cpu_impl.cc
thread_impl.o: thread_impl.h thread_impl.cc
cpu.o: thread_globals.o thread_impl.o cpu_impl.o cpu.h cpu.cc

clean: 
	rm -f $(OBJECTS) $(EXECUTABLE) $(TESTS)

.PHONY: all clean alltests debug release
.SUFFIXES: