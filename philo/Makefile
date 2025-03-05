CC = cc -g -O3
NAME = philosophers
FLAGS =  -Wall -Wextra -Werror
SRC = philosophers.c
OBJS=$(SRC:%.c=%.o)
HEADER = philosophers.h

all: $(NAME)

%.o: %.c
	$(CC) $(FLAGS) -c $^ -o $@

$(NAME) : $(OBJS) $(HEADER)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(NAME)

re : fclean all


