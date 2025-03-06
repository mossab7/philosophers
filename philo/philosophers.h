#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <pthread.h>
# include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct s_program t_program;

typedef struct s_philosophers {
    int id;
    int time_to_die;
    int time_to_eat;
    int time_to_sleep;
    int number_of_philosopher;
    int meal_count;
    int number_times_to_eat;
    size_t last_meal;
    pthread_mutex_t meal_lock;
    pthread_mutex_t *left_fork;
    pthread_mutex_t *right_fork;
    pthread_t thread;
    t_program *program;
} t_philosophers;

typedef struct s_program {
    int philosopher_count;
    bool all_ate_enough;
    bool simulation_stop;
    pthread_mutex_t *forks;
    t_philosophers *philosophers;
    pthread_mutex_t print;
    pthread_mutex_t stop_mutex;
} t_program;




#endif // PHILOSOPHERS_H
