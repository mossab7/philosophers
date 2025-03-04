#include "philosophers.h"

size_t	get_time(void)
{
	struct timeval	curtime;

	if (gettimeofday(&curtime, NULL) != 0)
	{
		perror("gettimeofday failed");
		return (0);
	}
	return (curtime.tv_sec * 1000 + curtime.tv_usec / 1000);
}

void	ft_sleep(size_t milliseconds)
{
	size_t	start;
	size_t	current;

	start = get_time();
	while (1)
	{
		current = get_time();
		if (current - start >= milliseconds)
			break ;
		usleep(100);
	}
}

void	print_status(t_philosophers *philosopher, char *status)
{
	size_t	time;

	sem_wait(&philosopher->program->print);
	time = get_time();
	printf("%zu %d %s\n", time, philosopher->id + 1, status);
	sem_post(&philosopher->program->print);
}

bool	check_arguments(int ac, char **av)
{
	int	i;
	int	j;

	if (ac < 5 || ac > 6)
	{
		printf("Usage:\
			%s number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n",
			av[0]);
		return (false);
	}
	i = 1;
	while (i < ac)
	{
		j = 0;
		while (av[i][j])
		{
			if (av[i][j] < '0' || av[i][j] > '9')
			{
				printf("Error: Arguments must be positive integers.\n");
				return (false);
			}
			j++;
		}
		i++;
	}
	return (true);
}


void	take_fork(t_philosophers *philosopher)
{
	if (philosopher->id % 2 == 0)
	{
		sem_wait(philosopher->right_fork);
		print_status(philosopher, "has taken a fork");
		sem_wait(philosopher->left_fork);
		print_status(philosopher, "has taken a fork");
	}
	else
	{
		sem_wait(philosopher->left_fork);
		print_status(philosopher, "has taken a fork");
		sem_wait(philosopher->right_fork);
		print_status(philosopher, "has taken a fork");
	}
}

void	release_fork(t_philosophers *philosopher)
{
	sem_post(philosopher->left_fork);
	sem_post(philosopher->right_fork);
}

void	*philo_routine(void *arg)
{
	t_philosophers	*philosopher;

	philosopher = (t_philosophers *)arg;
	if (philosopher->number_of_philosopher == 1)
	{
		print_status(philosopher, "has taken a fork");
		ft_sleep(philosopher->time_to_die);
		return (NULL);
	}
	if (philosopher->id % 2 == 0)
		ft_sleep(50);
	while (!simulation_end(philosopher->program))
	{
		take_fork(philosopher);
		sem_wait(&philosopher->meal_lock);
		philosopher->last_meal = get_time();
		print_status(philosopher, "is eating");
		sem_post(&philosopher->meal_lock);
		ft_sleep(philosopher->time_to_eat);
		sem_wait(&philosopher->meal_lock);
		philosopher->meal_count++;
		sem_post(&philosopher->meal_lock);
		release_fork(philosopher);
		print_status(philosopher, "is sleeping");
		ft_sleep(philosopher->time_to_sleep);
		print_status(philosopher, "is thinking");
		if(philosopher->time_to_eat > philosopher->time_to_sleep)
			ft_sleep(philosopher->time_to_eat - philosopher->time_to_sleep);
		long time_till_death = get_time() - philosopher->last_meal;
		if (time_till_death < philosopher->time_to_die * 0.7)
			ft_sleep(1);
	}
	return (NULL);
}

void	*monitor_routine(void *arg)
{
	t_program	*program;
	int			i;
	bool		all_ate;
	size_t		current_time;

	program = (t_program *)arg;
	while (!simulation_end(program))
	{
		i = 0;
		all_ate = true;
		while (i < program->philosopher_count)
		{
			sem_wait(&program->philosophers->meal_lock);
			current_time = get_time();
			if (current_time
				- program->philosophers[i].last_meal > (size_t)program->philosophers[i].time_to_die)
			{
				sem_post(&program->philosophers->meal_lock);
				sem_post(&program->dead_flag);
				printf("%zu %d died\n", current_time,
					program->philosophers[i].id + 1);
				return (NULL);
			}
			if (program->philosophers[i].number_times_to_eat != -1
				&& program->philosophers[i].meal_count < program->philosophers[i].number_times_to_eat)
			{
				all_ate = false;
			}
			sem_post(&program->philosophers->meal_lock);
			i++;
		}
		if (all_ate && program->philosophers[0].number_times_to_eat != -1)
		{
			sem_wait(&program->dead_flag);
			program->all_ate_enough = true;
			sem_post(&program->dead_flag);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}

void destroy_sems(t_program *program)
{
	int i = 0;
	while(i < program->philosopher_count)
	{
		sem_destroy(&program->forks[i]);
		sem_destroy(&program->philosophers[i].meal_lock);
		i++;
	}
	sem_destroy(&program->dead_flag);
}

void	free_resources(t_program *program)
{
	destroy_sems(program);
	free(program->forks);
	free(program->philosophers);
	free(program);
}

void death_listener(void *args)
{
	t_program *program;

	program = (t_program *)args;
	sem_wait(&program->dead_flag);
	free_resources(program);
	exit(EXIT_SUCCESS);
}

void	program_init(int ac, char **args, t_program *program)
{
	int	i;
	int	count;

	count = atoi(args[1]);
	program->philosopher_count = count;
	program->all_ate_enough = false;
	program->forks = malloc(count * sizeof(sem_t));
	if (!program->forks)
		error_exit("malloc failed");
	i = 0;
	while(i < count)
	{
		sem_init(&program->forks[i], 1, 1);
		i++;
	}
	i = 0;
	while (i < count)
	{
		program->philosophers[i].id = i;
		program->philosophers[i].time_to_die = atoi(args[2]);
		program->philosophers[i].time_to_eat = atoi(args[3]);
		program->philosophers[i].time_to_sleep = atoi(args[4]);
		program->philosophers[i].number_of_philosopher = count;
		program->philosophers[i].meal_count = 0;
		program->philosophers[i].last_meal = get_time();
		program->philosophers[i].dead_flag = &program->dead_flag;
		program->philosophers[i].program = program;
		program->philosophers[i].left_fork = &program->forks[i];
		program->philosophers[i].right_fork = &program->forks[(i + 1) % count];
		sem_init(&program->philosophers[i].meal_lock,1,1);
		if (ac == 6)
			program->philosophers[i].number_times_to_eat = atoi(args[5]);
		else
			program->philosophers[i].number_times_to_eat = -1;
		i++;
	}
	sem_init(&program->print,0,1);
	sem_init(&program->dead_flag,0,0);
}

void start_simulation(t_program *program)
{
	int i = 0;
	while(i < program->philosopher_count)
	{
		pid_t pid = fork();
		if(pid == 0)
		{
			pthread_t monitor;
			pthread_t death_listener;
			philo_routine(program);
			if (pthread_create(&monitor, NULL, monitor_routine, program) != 0)
				error_exit("pthread_create failed");
			if (pthread_create(&death_listener, NULL, death_listener, program) != 0)
				error_exit("pthread_create failed");
			pthread_join(&monitor,NULL);
			pthread_join(&monitor,NULL);
		}
	}
}

int main(int ac,char **av)
{
	t_program *program;

	if (!check_arguments(ac, av))
	return (EXIT_FAILURE);

	program = malloc(sizeof(t_program));
	if (!program)
		error_exit("malloc failed");

	program->philosophers = malloc(atoi(av[1]) * sizeof(t_philosophers));
	if (!program->philosophers)
	{
		free(program);
		error_exit("malloc failed");
	}

	program_init(ac,av,program);
	start_simulation(program);
	return (0);
}
