# VARIABLES
GREEN 	= \033[38;5;40m
RESET 	= \033[0m
NM 		= ft_nm
OTOOL 	= ft_otool

# COMPILATION
CC 		= gcc
FLAGS 	= -Wall -Wextra -Werror
RM 		= rm -rf


# DIRECTORIES
DSRCS		= ./srcs/
NM_DIR		= nm/
OTOOL_DIR 	= otool/
BASE_DIR 	= base/

NM_DOBJS	= ./comp/nm/
OTOOL_DOBJS	= ./comp/otool/
BASE_DOBJS	= ./comp/base/


# SOURCES
NM_SRCS 	= 	nm.c
OTOOL_SRCS 	= 	otool.c
BASE_SRCS 	= 	clear.c		\
				control.c	\
				display.c	\
				exit.c 		\
				fat.c		\
				lib.c		\
				parse.c		\
				resources.c	\
				segment.c	\
				swap.c		\
				symbol.c	\
				text.c

# OBJS
NM_OBJS 	= $(NM_SRCS:%.c=$(NM_DOBJS)%.o)
OTOOL_OBJS 	= $(OTOOL_SRCS:%.c=$(OTOOL_DOBJS)%.o)
BASE_OBJS 	= $(BASE_SRCS:%.c=$(BASE_DOBJS)%.o)     


#H EADER FILE
HEADER = ./incs/nm_otool.h


# MAKE
all: $(NM) $(OTOOL)


# COMPILATION
$(NM): $(NM_OBJS) $(BASE_OBJS)
	$(CC) $(FLAGS) $(NM_OBJS) $(BASE_OBJS) -o $(NM)
	echo "$(GREEN)NM DONE ✔$(RESET)"

$(OTOOL): $(OTOOL_OBJS) $(BASE_OBJS)
	$(CC) $(FLAGS) $(OTOOL_OBJS) $(BASE_OBJS) -o $(OTOOL)
	echo "$(GREEN)OTOOL DONE ✔$(RESET)"

$(NM_OBJS): | $(NM_DOBJS)
$(OTOOL_OBJS): | $(OTOOL_DOBJS)
$(BASE_OBJS): | $(BASE_DOBJS)

$(NM_DOBJS)%.o: $(DSRCS)$(NM_DIR)%.c $(HEADER)
	$(CC) $(FLAGS) -c $< -o $@

$(OTOOL_DOBJS)%.o: $(DSRCS)$(OTOOL_DIR)%.c $(HEADER)
	$(CC) $(FLAGS) -c $< -o $@

$(BASE_DOBJS)%.o: $(DSRCS)$(BASE_DIR)%.c $(HEADER)
	$(CC) $(FLAGS) -c $< -o $@


# OBJECT FILE DIRECTORY CREATION
$(NM_DOBJS):
	mkdir -p $(NM_DOBJS)

$(OTOOL_DOBJS):
	mkdir -p $(OTOOL_DOBJS)

$(BASE_DOBJS):
	mkdir -p $(BASE_DOBJS)


# CLEAR
clean:
	$(RM) ./comp

fclean: clean
	$(RM) $(NM) $(OTOOL)

re: fclean all

.PHONY: all clean fclean re
.SILENT: all $(NM) $(OTOOL)