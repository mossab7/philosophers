CC = cc -O3
NAME = philo_bonus
FLAGS = -Wall -Wextra -Werror
SRC = philosopher_bonus.c philosopher_utils_bonus.c philosopher_launcher_bonus.c philosopher_semaphore_bonus.c philosopher_operations_bonus.c \
      philosopher_monitor_routine_bonus.c philosopher_meals_monitor_bonus.c philosopher_main_bonus.c philosopher_init_bonus.c philosopher_forks_bonus.c \
      philosopher_flow_control_bonus.c philosopher_death_handlers_bonus.c philosopher_cleanup_bonus.c philosopher_atoi_bonus.c philosopher_libft_bonus.c

OBJS=$(SRC:%.c=%.o)
HEADER = philosopher_bonus.h

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


