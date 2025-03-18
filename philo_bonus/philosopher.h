#ifndef PHILOSOPHER_H
#define PHILOSOPHER_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>

typedef struct s_philosophers t_philosophers;
typedef struct s_program t_program;

struct s_program 
{
    int number_of_philosophers;
    int time_to_die;
    int time_to_eat;
    int time_to_sleep;
    int number_of_times_each_philosopher_must_eat;
    sem_t *forks_sem;
    sem_t *print_sem;
    sem_t *death_sem;
    sem_t *philo_full_sem;
    pid_t *pids;
    t_philosophers *philosophers;
    long long start_time;
};

struct s_philosophers 
{
    int id;
    t_program *program;
    sem_t *stop_sem;
    sem_t *meal_sem;
    int meal_count;
    long long last_meal;
    bool simulation_stopped;
};

// Function prototypes
bool check_arguments(int ac, char **av);
t_philosophers *philosopher_init(t_program *program);
t_program *program_init(int ac, char **av);
long long get_time(t_program *program);
void ft_sleep(t_philosophers *philosopher, int time_ms);
void print_status(t_philosophers *philosopher, char *status);
void release_fork(t_philosophers *philosopher);
void take_fork(t_philosophers *philosopher);
void set_simulation_stopped(t_philosophers *philosopher);
void handle_single_philosopher(t_philosophers *philosopher);
void eat_sleep_think(t_philosophers *philosopher);
void *philosopher_routine(void *arg);
bool check_philo_death(t_philosophers *philosopher);
void report_philo_death(t_philosophers *philosopher);
void kill_all_philosophers(t_program *program);
void meals_monitor(t_program *program);
bool check_all_ate_enough(t_program *program);
bool check_eat_enough(t_philosophers *philosopher);
void *monitor_routine(void *arg);
void cleanup_process(t_philosophers *philosopher);
void philosopher_start(t_philosophers *philosopher);
void program_start(t_program *program);
void program_destroy(t_program *program);
void cleanup_semaphores(t_program *program);
sem_t *open_sem(char *name, int oflag, mode_t mode, unsigned int value);
bool is_simulation_stopped(t_philosophers *philosopher);
char *ft_itoa(int n);
char *ft_strjoin(char *s1, char *s2);
void signal_death(t_program *program);


#endif