#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <pthread.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_philosophers
{
	pthread_t			thread;
	int					id;
	size_t				last_meal;
	int					meal_count;
	int					time_to_die;
	int					time_to_eat;
	int					time_to_sleep;
	int					number_times_to_eat;
	int					*dead_flag;
	int					number_of_philosopher;
	struct s_program	*program;
}						t_philosophers;

typedef struct s_program
{
	t_philosophers		*philosophers;
	int					dead_flag;
	int					philosopher_count;
	bool				all_ate_enough;
}						t_program;

size_t					get_time(void);
bool					simulation_end(t_program *program);
void					take_fork(t_philosophers *philosopher);
void					release_fork(t_philosophers *philosopher);
void					print_status(t_philosophers *philosopher, char *status);
void					*philo_routine(void *arg);
void					*monitor_routine(void *arg);
void					ft_sleep(size_t milliseconds);
void					program_init(int ac, char **args, t_program *program);
void					destroy_mutexes(t_program *program);
void					free_resources(t_program *program);
bool					check_arguments(int ac, char **av);
void					error_exit(char *message);

#endif
