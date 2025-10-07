NAME = webserv

FLAGS = c++ -Wall -Wextra -Werror -std=c++17

SRC = ./src/main.cpp \
      ./src/Cgi.cpp \
      ./src/Chunked.cpp \
      ./src/Client.cpp \
      ./src/HttpRequest.cpp \
      ./src/HttpRequestParsing.cpp \
      ./src/HttpResponse.cpp \
      ./src/LocationBlock.cpp \
      ./src/Parsing.cpp \
      ./src/Server.cpp \
      ./src/ServerBlock.cpp \
      ./src/Utils.cpp \
      ./src/UtilsParsing.cpp

OBJ = $(SRC:./src/%.cpp=./obj/%.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(FLAGS) $(OBJ) -o $(NAME)

./obj/%.o: ./src/%.cpp
	$(FLAGS) -c $< -o $@

run: $(NAME)
	./$(NAME) original.config

clean:
	rm -f ./obj/*.o

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re run