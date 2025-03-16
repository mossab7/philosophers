#ifndef PHILOSOPHER_H
#define PHILOSOPHER_H

#include <stdbool.h>
#include <semaphore.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/wait.h>

typedef struct s_program {
    int number_of_philosophers;
    int time_to_die;
    int time_to_eat;
    int time_to_sleep;
    int number_of_times_each_philosopher_must_eat;
    sem_t *stop_sem;
    bool simulation_stopped;
    pid_t *pids;
    struct s_philosophers *philosophers;
    long long start_time;
} t_program;

typedef struct s_philosophers {
    int id;
    struct s_program *program;
    sem_t *forks_sem;
    sem_t *print_sem;
    sem_t *meal_sem;
    sem_t *death_sem;
    int meal_count;
    long long last_meal;
} t_philosophers;

bool check_arguments(int ac, char **av);
void cleanup_semaphores(void);
t_philosophers *philosopher_init(t_program *program);
t_program *program_init(int ac, char **av);
long long get_time(t_program *program);
void ft_sleep(t_philosophers *philosopher, int time_ms);
int is_simulation_stopped(t_program *program);
void print_status(t_philosophers *philosopher, char *status);
void release_fork(t_philosophers *philosopher);
void take_fork(t_philosophers *philosopher);
void handle_single_philosopher(t_philosophers *philosopher);
void eat_sleep_think(t_philosophers *philosopher);
void *philosopher_routine(void *arg);
bool check_philo_death(t_philosophers *philosopher);
bool check_all_ate_enough(t_program *program);
void *monitor_routine(void *arg);
void cleanup_process(t_philosophers *philosopher);
void philosopher_start(t_philosophers *philosopher);
void program_start(t_program *program);
void program_destroy(t_program *program);

#endif