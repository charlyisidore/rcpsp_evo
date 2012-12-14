CC = g++
CFLAGS = -O2 -Iinclude -ansi -Wall -pedantic
LDFLAGS = -O2

EXEC = rcpsp_evo

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

all : $(EXEC)

$(EXEC) : $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

src/%.o : src/%.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean :
	rm -f $(EXEC) $(OBJ)

mrproper:
	find . -name '*~' -print0 | xargs -0 -r rm

