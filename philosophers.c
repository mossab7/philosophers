#include "philosophers.h"

int glob = 0;

void *philo_routing(void *args)
{
	usleep(1);
	(void)args;
	// pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
	// pthread_mutex_lock(&mtx);
	glob++;
    printf("Thread %lu incrementing glob to %d\n", pthread_self(), glob);
	return args;
}

void program_init(int ac,char **args,t_program *program)
{
	int i = 0;
	while(i < atoi(args[1]))
	{
		pthread_create(program->philosophers[i].philosopher,NULL,philo_routing,program);
		pthread_join(program->philosophers[i].philosopher,NULL);
		program->philosophers[i].number_of_philosopher = atoi(args[1]);
		program->philosophers[i].time_to_die = atoi(args[2]);
		program->philosophers[i].time_to_eat = atoi(args[3]);
		program->philosophers[i].time_to_sleep = atoi(args[4]);
		if(ac == 6)
			program->philosophers[i].number_times_to_eat = atoi(args[5]);
		else
			program->philosophers[i].number_times_to_eat = -1;
		i++;
	}
}

int main(int ac,char **av)
{
	(void)ac;
	t_program *program = malloc(sizeof(program));
	program->philosophers = malloc(atoi(av) * sizeof(t_philosophers));
	program_init(ac,av,program);
	return (0);
}
