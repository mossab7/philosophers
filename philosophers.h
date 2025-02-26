#ifndef PHILOSOPHERS_H
#define PHILOSOPHERS_H

# include <pthread.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>


typedef struct s_philosophers
{
	pthread_t philosopher;
	int id;
	int time_to_die;
	int time_to_eat;
	int time_to_sleep;
	int number_times_to_eat;
	int number_of_philosopher;
	int last_meal;
	int meal_count;
	pthread_mutex_t *left_fork;
	pthread_mutex_t *right_fork;
	pthread_mutex_t *meal_lock;
	pthread_mutex_t *dead_flag_lock;
}t_philosophers;

typedef struct s_program
{
	int dead_flag;
	t_philosophers *philosophers;
	pthread_t monitor;
}t_program;

void error_log(char *msg);

#endif //PHILOSOPHERS_H
