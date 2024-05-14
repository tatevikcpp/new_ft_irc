NAME = ircserv

TMP = objs

CXX = c++

CXXFLAGS = -I./includes -std=c++98  -Wall -Wextra -Werror -g -fsanitize=address

LDFLAGS = -lasan -L/path/to/lib

SRCS = $(wildcard src/*.cpp)

OBJS = $(patsubst src/%.cpp, ./$(TMP)/%.o, $(SRCS))

RM = rm -fr

HEADER = $(wildcard includes/*.hpp)

./$(TMP)/%.o: src/%.cpp $(HEADER) Makefile
	$(CXX) $(CXXFLAGS) -o $@ -c $<

all: $(NAME) 

$(NAME): $(TMP) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(TMP):
	@mkdir $(TMP)

clean:
	$(RM) $(OBJS_DIR)
	$(RM) $(TMP)

fclean: clean
	$(RM) $(NAME)

re:	fclean all

.PHONY: all clean fclean re bonus
