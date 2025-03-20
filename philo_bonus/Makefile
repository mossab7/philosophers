CC = cc -g -O3 
NAME = philo_bonus
FLAGS = -Wall -Wextra -Werror
SRC = philosopher.c philosopher_utils.c philosopher_launcher.c philosopher_semaphore.c philosopher_operations.c \
	philosopher_monitor_routine.c philosopher_meals_monitor.c philosopher_main.c philosopher_init.c philosopher_forks.c \
		philosopher_flow_control.c philosopher_death_handlers.c philosopher_cleanup.c philosopher_atoi.c philosopher_libft.c
OBJS=$(SRC:%.c=%.o)
HEADER = philosopher.h

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


