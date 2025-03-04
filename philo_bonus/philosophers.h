#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <pthread.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>
# include <semaphore.h>
# include <signal.h>

struct s_program;

typedef struct s_philosophers
{
    pthread_t       thread;          
    int             id;              
    size_t          last_meal;       
    int             meal_count;      
    int             time_to_die;     
    int             time_to_eat;     
    int             time_to_sleep;   
    int             number_times_to_eat;
    int             *dead_flag;      
    int             number_of_philosopher;
    sem_t           *left_fork;      
    sem_t           *right_fork;     
    sem_t           meal_lock;       
    struct s_program *program;       
} t_philosophers;


typedef struct s_program
{
    t_philosophers  *philosophers;   
    sem_t           dead_flag;       
    sem_t           *forks;          
    sem_t           print;           
    int             philosopher_counters;
    bool            all_ate_enough;
	int             philosopher_count;
} t_program;


size_t get_time(void);
void ft_sleep(size_t milliseconds);
bool check_arguments(int ac, char **av);
bool simulation_end(t_program *program);
void print_status(t_philosophers *philosopher, char *status);
void take_fork(t_philosophers *philosopher);
void release_fork(t_philosophers *philosopher);
void *philo_routine(void *arg);
void *monitor_routine(void *arg);
void program_init(int ac, char **args, t_program *program);
void destroy_sems(t_program *program);
void free_resources(t_program *program);
void error_exit(char *message);

#endif // PHILOSOPHERS_H