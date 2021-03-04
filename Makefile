# VARIABLES
GREEN = \033[38;5;40m
RESET = \033[0m
NAME = ft_nm

# COMPILATION
CC = gcc
FLAGS = -Wall -Wextra -Werror
RM = rm -rf


# DIRECTORIES
DSRCS	= ./srcs/
DOBJS	= ./comp/


# SOURCES
SRCS = test.c
EXEC_SRCS = macos_exec.c

OBJS = $(SRCS:%.c=$(DOBJS)%.o)
EXEC_OBJS = $(EXEC_SRCS:%.c=$(DOBJS)%.o)


HEADERS = ./inc/traceroute.h


all: $(NAME) exec
	echo "$(GREEN)DONE ✔$(RESET)"

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

exec: $(EXEC_OBJS)
	$(CC) $(FLAGS) $(EXEC_OBJS) -o macos_exec

test: all
	./ft_nm

$(OBJS): | $(DOBJS)
$(EXEC_OBJS): | $(DOBJS)

$(DOBJS):
	mkdir $(DOBJS)

#$(DOBJS)%.o: $(DSRCS)%.c
$(DOBJS)%.o: %.c
	$(CC) -c $< -o $@


# CLEAR
clean:
	$(RM) $(DOBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
.SILENT: all