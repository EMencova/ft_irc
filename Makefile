# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ashobajo <ashobajo@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/05 12:02:25 by mac               #+#    #+#              #
#    Updated: 2025/03/09 09:47:10 by ashobajo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

CC = c++
CFLAGS = -Wall -Werror -Wextra -std=c++98

SRC = main.cpp \
		src/Server.cpp \
		src/Client.cpp \
		src/Channel.cpp \
		src/commands/Mode.cpp \
		src/commands/Auth.cpp \
		src/commands/Invite.cpp \
		src/commands/Join.cpp \
		src/commands/Kick.cpp \
		src/commands/Nick.cpp \
		src/commands/Operator.cpp \
		src/commands/Privmsg.cpp \
		src/commands/Topic.cpp \
		src/commands/User.cpp

OBJDIR = obj
OBJ = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRC))

# ANSI color codes
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
MGNT = \033[0;35m
CYAN = \033[0;36m
RESET = \033[0m

all: $(NAME) banner

$(NAME): $(OBJ)
	@echo "\n$(GREEN)Linking object files into executable $(NAME)...$(RESET)"
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJ)
	@echo "$(GREEN)Done!$(RESET)"

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@printf "$(YELLOW)Compiling $(CYAN)%-40s$(RESET)\r" "$<"
	@$(CC) $(CFLAGS) -c $< -o $@
	@printf "$(GREEN)Compiled $(CYAN)%-40s$(RESET)\n" "$<"

banner:
	@echo "$(RED)           ;               ,$(RESET)"
	@echo "$(RED)         ,;                 '.$(RESET)"
	@echo "$(RED)        ;:                   :;$(RESET)"
	@echo "$(RED)       ::                     ::$(RESET)"
	@echo "$(RED)       ::                     ::$(RESET)"
	@echo "$(RED)       ':                     :$(RESET)"
	@echo "$(RED)        :.                    :$(RESET)"
	@echo "$(RED)     ;' ::                   ::  '$(RESET)"
	@echo "$(RED)    .'  ';                   ;'  '.$(RESET)"
	@echo "$(RED)   ::    :;                 ;:    ::$(RESET)"
	@echo "$(RED)   ;      :;.             ,;:     ::$(RESET)"
	@echo "$(RED)   :;      :;:           ,;\"      ::$(RESET)"
	@echo "$(RED)   ::.      ':;  ..,.;  ;:'     ,.;:$(RESET)"
	@echo "$(RED)    \"'\"\"...   '::,::::: ;:   .;.;\"\"'$(RESET)"
	@echo "$(RED)        '\"\"\"....;:::::;,;.;\"\"\"$(RESET)"
	@echo "$(RED)    .:::.....'\"':::::::'\",...;::::;.$(RESET)"
	@echo "$(MGNT)Welcome to IRC by AbdulAzeez and Eliska$(RESET)"
	@echo "$(RED)   ;:' '\"'\"\";.,;:::::;.'\"\"\"\"\"  ':;$(RESET)"
	@echo "$(RED)  ::'         ;::;:::;::..         :;$(RESET)"
	@echo "$(RED) ::         ,;:::::::::::;:..       ::$(RESET)"
	@echo "$(RED) ;'     ,;;:;::::::::::::::;\";..    ':.$(RESET)"
	@echo "$(RED)::     ;:\"  ::::::\"\"\"'::::::  \":     ::$(RESET)"
	@echo "$(RED) :.    ::   ::::::;  :::::::   :     ;$(RESET)"
	@echo "$(RED)  ;    ::   :::::::  :::::::   :    ;$(RESET)"
	@echo "$(RED)   '   ::   ::::::....:::::'  ,:   '$(RESET)"
	@echo "$(RED)    '  ::    :::::::::::::\"   ::$(RESET)"
	@echo "$(RED)       ::     ':::::::::\"'    ::$(RESET)"
	@echo "$(RED)       ':       \"\"\"\"\"\"\"'      ::$(RESET)"
	@echo "$(RED)        ::                   ;:$(RESET)"
	@echo "$(RED)        ':;                 ;:\"$(RESET)"
	@echo "$(RED)          ';              ,;'$(RESET)"
	@echo "$(RED)            \"'           '\"$(RESET)"

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -rf $(OBJDIR)
	@echo "$(GREEN)Done!$(RESET)"

fclean: clean
	@echo "$(RED)Removing executable $(NAME)...$(RESET)"
	@rm -f $(NAME)
	@echo "$(GREEN)Done!$(RESET)"

re: fclean all

.PHONY: all clean fclean re banner
