.PHONY: all debug release clean

Compiler = g++
RELEASE_FLAGS = -O2 -Wall -DNDEBUG -Iheaders
DEBUG_FLAGS   = -g -O0 -Wall -Iheaders
RELEASE_EXEC  = averageOfNumbers
DEBUG_EXEC    = averageOfNumbers-dbg
SOURCES       = main.cpp Auth.cpp ClientDataBase.cpp Communicator.cpp Calculator.cpp Logger.cpp server.cpp
LIBARY        = -lboost_program_options -lcryptopp
all: debug release

debug: $(DEBUG_EXEC)

$(DEBUG_EXEC): $(SOURCES)
	$(Compiler) $(DEBUG_FLAGS) $(SOURCES) -o $(DEBUG_EXEC) $(LIBARY)

release: $(RELEASE_EXEC)

$(RELEASE_EXEC): $(SOURCES)
	$(Compiler) $(RELEASE_FLAGS) $(SOURCES) -o $(RELEASE_EXEC) $(LIBARY)

clean:
	rm -f $(RELEASE_EXEC) $(DEBUG_EXEC)
