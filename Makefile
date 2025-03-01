NAME = ircserver

CC = c++

CFLAGS = -Wall -Werror -Wextra -std=c++98

SRC = main.cpp \
	Server.cpp \
	Client.cpp \
	Commands.cpp \ 
	Utils.cpp \ 

	

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -fr $(OBJ)

fclean: clean
	rm -fr $(NAME)

re: fclean all

.PHONY: all clean fclean re

