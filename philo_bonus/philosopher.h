#ifndef PHILOSOPHER_H
# define PHILOSOPHER_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdbool.h>
# include <pthread.h>
# include <sys/time.h>
# include <sys/wait.h>
# include <semaphore.h>
# include <fcntl.h>
# include <signal.h>

typedef struct s_program t_program;
typedef struct s_philosophers t_philosophers;

typedef struct s_philosophers
{
    int             id;
    int             meal_count;
    long long       last_meal;
    pthread_t       philosopher_thread;
    pthread_t       monitor_thread;
    pthread_t       death_listener_thread;
    sem_t           *forks_sem;
    sem_t           *print_sem;
    sem_t           *meal_sem;
    sem_t           *death_sem;
    t_program       *program;
} t_philosophers;

typedef struct s_program
{
    int             number_of_philosophers;
    int             time_to_die;
    int             time_to_eat;
    int             time_to_sleep;
    int             number_of_times_each_philosopher_must_eat;
    bool            simulation_stopped;
    long long       start_time;
    sem_t           *stop_sem;
	pid_t				*pids;
    t_philosophers  *philosophers;
} t_program;

bool            check_arguments(int ac, char **av);
t_program       *program_init(int ac, char **av);
t_philosophers  *philosopher_init(t_program *program);
void            program_start(t_program *program);
void            program_destroy(t_program *program);
void            philosopher_start(t_philosophers *philosopher);
void            *philosopher_routine(void *arg);
void            *monitor_routine(void *arg);
void            *death_listener_routine(void *arg);
void            take_fork(t_philosophers *philosopher);
void            release_fork(t_philosophers *philosopher);
void            eat_sleep_think(t_philosophers *philosopher);
void            handle_single_philosopher(t_philosophers *philosopher);
void            cleanup_process(t_philosophers *philosopher);
void            print_status(t_philosophers *philosopher, char *status);
long long       get_time(t_program *program);
void            ft_sleep(t_philosophers *philosopher, int time_ms);
int             is_simulation_stopped(t_program *program);

#endif /* PHILOSOPHER_H */
