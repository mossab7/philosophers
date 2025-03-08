#ifndef PHILOSOPHERS_BONUS_H
#define PHILOSOPHERS_BONUS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>

// Define constants
#define MAX_PHILOSOPHERS 200
#define SEM_FORKS "/forks_sem"
#define SEM_MUTEX "/mutex_sem"
#define SEM_PRINT "/print_sem"
#define SEM_STOP "/stop_sem"

// Philosopher structure
typedef struct s_philosophers {
    int id;
    int time_to_die;
    int time_to_eat;
    int time_to_sleep;
    int meal_count;
    int number_times_to_eat;
    size_t last_meal;
    sem_t meal_lock;
    struct s_program *program;
} t_philosophers;

// Program structure
typedef struct s_program {
    int philosopher_count;
    size_t start_time;
    bool simulation_stop;
    sem_t *forks_sem;
    sem_t *mutex_sem;
    sem_t *print_sem;
    sem_t *stop_sem;
    t_philosophers *philosophers;
} t_program;

// Function prototypes
size_t get_time(t_program *program);
void ft_sleep(t_philosophers *philo, size_t milliseconds);
bool is_simulation_stopped(t_program *program);
void set_simulation_stopped(t_program *program);
void print_status(t_philosophers *philo, char *status);
void take_forks(t_philosophers *philo);
void release_forks(t_philosophers *philo);
void eat_sleep_think(t_philosophers *philo);
void *monitor_routine(void *arg);
void philo_routine(t_philosophers *philo);
void init_philosophers(int ac, char **av, t_program *program);
void program_init(int ac, char **av, t_program *program);
bool check_arguments(int ac, char **av);
void start_simulation(t_program *program);
void free_resources(t_program *program);
void error_exit(char *message);

#endif
