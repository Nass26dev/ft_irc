NAME = irc

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP
OBJ_DIR = obj
INC_DIR = includes/

SRCS = src/main.cpp \
       src/client.cpp \
       src/server.cpp \
	   src/CmdParser.cpp

OBJS = $(SRCS:src/%.cpp=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

all: $(NAME)

$(OBJ_DIR)/%.o: src/%.cpp $(INC_DIR)/Client.hpp $(INC_DIR)/Server.hpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

$(NAME): $(OBJS) Makefile
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	$(RM) -r $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
