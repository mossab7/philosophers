#include "philosophers.h"


size_t get_time(void)
{
	struct timeval	curtime;

	gecurtimeofday(&curtime, NULL);
	return (curtime.tv_sec * 1000 + curtime.tv_usec / 1000);
}

void *philo_routing(t_philosophers *philosopher)
{
	while(!simulation_end())
	{
		take_fork(philosopher);
		pthread_mutex_lock(philosopher->meal_lock);
		philosopher->last_meal = get_time();
		pthread_mutex_unlock(philosopher->meal_lock);
		print_status(philosopher,"eating");
		ft_sleep();
		pthread_mutex_lock(philosopher->meal_lock);
		philosopher->meal_count++;
		pthread_mutex_unlock(philosopher->meal_lock);
		release_fork(philosopher);
		print_status(philosopher,"sleeping");
		ft_sleep();
		print_status(philosopher,"thinking");
		ft_sleep();
	}
}

void *monitor(t_program *program)
{
	int i = 0;
	while(!simulation_end())
	{
		while(i < program->philosophers->number_of_philosopher)
		{
			pthread_mutex_lock(program->philosophers[i].meal_lock);
			size_t time = get_time();
			if(time - program->philosophers[i].last_meal > program->philosophers[i].time_to_die)
			{
				pthread_mutex_lock(program->philosophers[i].dead_flag_lock);
				program->dead_flag = 1;
				pthread_mutex_unlock(program->philosophers[i].dead_flag_lock);
			}
			pthread_mutex_unlock(program->philosophers[i].meal_lock);
		}
		i++;
	}
}

void take_fork(t_philosophers *philosopher)
{
	if (philosopher->id % 2)
	{
		pthread_mutex_lock(philosopher->left_fork);
		pthread_mutex_lock(philosopher->right_fork);
	}
	else
	{
		pthread_mutex_lock(philosopher->right_fork);
		pthread_mutex_lock(philosopher->left_fork);
	}
}

void release_fork(t_philosophers *philosopher)
{
	pthread_mutex_unlock(philosopher->left_fork);
	pthread_mutex_unlock(philosopher->right_fork);
}

void program_init(int ac,char **args,t_program *program)
{
	int i = 0;
	int count = atoi(args[1]);
	pthread_mutex_t *forkes = malloc(count * sizeof(pthread_mutex_t));
	while(i < count)
	{
		pthread_mutex_init(&forkes[i],NULL);
		i++;
	}
	i = 0;
	while(i < count)
	{
		pthread_create(program->philosophers[i].philosopher,NULL,philo_routing,&program->philosophers[i]);
		pthread_join(program->philosophers[i].philosopher,NULL);
		program->philosophers->left_fork = &forkes[i];
		program->philosophers->right_fork =  &forkes[(i + 1) % count];
		program->philosophers[i].time_to_die = atoi(args[2]);
		program->philosophers[i].time_to_eat = atoi(args[3]);
		program->philosophers[i].time_to_sleep = atoi(args[4]);
		program->philosophers[i].id = i;
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
