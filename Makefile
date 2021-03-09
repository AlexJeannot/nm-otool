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
NM_DIR	= nm/
OTOOL_DIR = otool/
BASE_DIR = base/

NM_DOBJS	= ./comp/nm/
BASE_DOBJS	= ./comp/base/


# SOURCES
SRCS = display.c parse.c nm.c otool.c
NM_SRCS = nm.c
BASE_SRCS = display.c \
			parse.c		\
			control.c
			
# EXEC_SRCS = macos_exec.c

NM_OBJS = $(NM_SRCS:%.c=$(NM_DOBJS)%.o)
BASE_OBJS = $(BASE_SRCS:%.c=$(BASE_DOBJS)%.o)     


HEADERS = ./incs/nm.h \
			./incs/otool.h \
			./incs/base.h


all: $(NAME) #exec
	echo "$(GREEN)DONE ✔$(RESET)"

$(NAME): $(NM_OBJS) $(BASE_OBJS)
	$(CC) $(FLAGS) $(NM_OBJS) $(BASE_OBJS) -o $(NAME)

# exec: $(EXEC_OBJS)
# 	$(CC) $(FLAGS) $(EXEC_OBJS) -o macos_exec

test: all
	./ft_nm

$(NM_OBJS): | $(NM_DOBJS)
$(BASE_OBJS): | $(BASE_DOBJS)

# $(EXEC_OBJS): | $(DOBJS)

$(NM_DOBJS):
	mkdir $(NM_DOBJS)

$(BASE_DOBJS):
	mkdir $(BASE_DOBJS)

$(NM_DOBJS)%.o: $(DSRCS)$(NM_DIR)%.c
	$(CC) -c $< -o $@

$(BASE_DOBJS)%.o: $(DSRCS)$(BASE_DIR)%.c
	$(CC) -c $< -o $@

# CLEAR
clean:
	$(RM) $(NM_DOBJS) $(BASE_DOBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
.SILENT: all