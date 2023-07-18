NAME	=	webserv
INCLUDE	=	./includes
SRC_DIR	=	./srcs
OBJ_DIR	=	./obj
CXX		=	c++
CFLAGS	=	-Wall -Wextra -Werror -g3 -fsanitize=address -std=c++98 -O3
RM		=	rm -f
SRCS	=	$(shell find $(SRC_DIR) -name '*.cpp' -execdir basename {} \;)
OBJS	:=	$(SRCS:%.cpp=$(OBJ_DIR)/%.o)

ifeq ($(DB), 1)
	CFLAGS += -D DEBUG=1
	# CFLAGS += -pg
endif

all	:	
		mkdir -p $(OBJ_DIR)
		make $(NAME)

$(NAME)	:	$(OBJS)
		$(CXX) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CFLAGS) -I $(INCLUDE) -c $< -o $@

debug	:	fclean
			@echo " DEBUG MODE "
			@$(MAKE) DB=1

clean	:
			$(RM) -r $(OBJ_DIR)

fclean	:	clean
			$(RM) $(NAME)

re		:	fclean all

.PHONY	: clean fclean all re