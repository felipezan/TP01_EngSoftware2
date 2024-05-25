CXX = g++
CXXFLAGS = -std=c++11

SRCS = sudoku_solver.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = sudoku_solver

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(EXEC)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
