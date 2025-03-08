#ifndef PHILOSOPHERS_BONUS_H
# define PHILOSOPHERS_BONUS_H

# include <pthread.h>
# include <semaphore.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <sys/wait.h>
# include <unistd.h>
# include <fcntl.h>

struct s_program;

typedef struct s_philosophers
{
	int						id;
	int						time_to_die;
	int						time_to_eat;
	int						time_to_sleep;
	int						number_of_philosopher;
	int						meal_count;
	int						number_times_to_eat;
	size_t					last_meal;
	sem_t					meal_lock;
	sem_t					*left_fork;
	sem_t					*right_fork;
	pthread_t				thread;
	t_program				*program;
}							t_philosophers;

typedef struct s_program
{
	int						philosopher_count;
	size_t					start_time;
	bool					all_ate_enough;
	bool					simulation_stop;
	sem_t					*forks;
	t_philosophers			*philosophers;
	sem_t					print;
	sem_t					stop_sem;
}							t_program;

void						error_exit(char *message);
void						start_simulation(t_program *program);
void						*monitor_routine(void *arg);
bool						check_philo_death(t_program *program, int i,
								size_t current_time);
void						*philo_routine(void *arg);
void						eat_sleep_think(t_philosophers *philo);
void						handle_single_philo(t_philosophers *philo);
void						eat_sleep_think(t_philosophers *philo);
void						*philo_routine(void *arg);
bool						check_philo_death(t_program *program, int i,
								size_t current_time);
void						*monitor_routine(void *arg);
size_t						get_time(t_program *program);
void						ft_sleep(t_philosophers *philosopher,
								size_t milliseconds);
bool						is_simulation_stopped(t_program *program);
void						set_simulation_stopped(t_program *program);
void						print_status(t_philosophers *philosopher,
								char *status);
void						init_philosophers(int ac, char **av,
								 t_program *program);
void						program_init(int ac, char **args,
								t_program *program);
void						take_forks_even(t_philosophers *philo);
void						take_forks_odd(t_philosophers *philo);
void						take_fork(t_philosophers *philosopher);
void						release_fork(t_philosophers *philosopher);
bool						check_all_ate(t_program *program);
int							ft_atoi(const char *str);
size_t						ft_strlen(char const *s);
void						*death_listener(void *arg);
#endif 
