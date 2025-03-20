CC = cc -O3
NAME = philo
FLAGS =  -Wall -Wextra -Werror
SRC = philosophers.c philosophers_fork.c philosophers_init.c philosophers_main.c philosophers_utils.c ft_atoi.c
OBJS=$(SRC:%.c=%.o)
HEADER = philosophers.h

all: $(NAME)
$(NAME) : $(OBJS) $(HEADER)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(FLAGS) -c $^ -o $@


clean :
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(NAME)

re : fclean all


