#_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_ COLORS _/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_
RESET	= \033[0m
BLACK 	= \033[1;30m
RED 	= \033[1;31m
GREEN 	= \033[1;32m
YELLOW 	= \033[1;33m
BLUE	= \033[1;34m
PURPLE 	= \033[1;35m
CYAN 	= \033[1;36m
WHITE 	= \033[1;37m

#_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_ COMMANDS _/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_
CPP = c++
RM = rm -rf
AR = ar -rcs

#_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_ FLAGS _/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_
CPPFLAGS	= -Wall -Wextra -Werror -std=c++98 -g -DDEBUG
MK		= --no-print-directory

#_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_ FOLDERS _/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_
INC				= includes
SRCS			= srcs
VPATH			= $(SRCS)
OBJ_DIR			= bin

#_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_ FILES _/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_
NAME			= webserv
_FILES			= main utils Parser TcpServer Server
OBJS			= $(_FILES:%=%.o)
TARGET			= $(addprefix $(OBJ_DIR)/, $(OBJS))


#_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_ RULES _/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_/=\_
all: $(NAME)

$(NAME): $(OBJ_DIR) $(TARGET)
	$(CPP) $(CPPFLAGS) $(TARGET) -o $(NAME) -I $(INC)

	echo "$(GREEN)Done.$(RESET)"

$(OBJ_DIR)/%.o : %.cpp
	echo "[$(CYAN)Compiling$(RESET)] $(CPPFLAGS) $(GREEN)$<$(RESET)"
	$(CPP) $(CPPFLAGS) -c $< -o $@ -I $(INC)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	echo "[$(RED) Deleted $(RESET)] $(GREEN)$(OBJ_DIR)$(RESET)"
	$(RM) $(OBJ_DIR)

fclean: clean
	echo "[$(RED) Deleted $(RESET)] $(GREEN)$(NAME)$(RESET)"
	$(RM) $(NAME)

re: fclean all

run:
	./$(NAME)

r: re run

.SILENT: