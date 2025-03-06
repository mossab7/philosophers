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
		usleep(50);
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

void	error_exit(char *message)
{
	fprintf(stderr, "Error: %s\n", message);
	exit(EXIT_FAILURE);
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

void	*death_listener(void *arg)
{
	t_program	*program;

	program = (t_program *)arg;
	sem_wait(simulation_end);
	cleanup(program);
	exit(EXIT_SUCCESS);
}

bool simulation_end(t_philosophers *philosopher)
{
	bool result;

	sem_wait(&philosopher->program->simulation_end);
	result = philosopher->ate_enough || philosopher->program->simulation_end_flag;
	sem_post(&philosopher->program->simulation_end);
	return (result);
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
	while (1)
	{
		if (simulation_end(philosopher->program))
			break ;
		take_fork(philosopher);
		if (simulation_end(philosopher->program))
		{
			release_fork(philosopher);
			break ;
		}
		sem_wait(&philosopher->meal_lock);
		philosopher->last_meal = get_time();
		print_status(philosopher, "is eating");
		sem_post(&philosopher->meal_lock);
		ft_sleep(philosopher->time_to_eat);
		sem_wait(&philosopher->meal_lock);
		philosopher->meal_count++;
		sem_post(&philosopher->meal_lock);
		release_fork(philosopher);
		if (simulation_end(philosopher->program))
			break ;
		print_status(philosopher, "is sleeping");
		ft_sleep(philosopher->time_to_sleep);
		if (simulation_stopped(philosopher->program))
			break ;
		print_status(philosopher, "is thinking");
		if (philosopher->time_to_eat > philosopher->time_to_sleep)
			ft_sleep(philosopher->time_to_eat - philosopher->time_to_sleep);
	}
	return (NULL);
}

void	*monitor_routine(void *arg)
{
	t_philosophers	*philosopher;
	int			i;
	bool		all_ate;
	size_t		current_time;

	philosopher = (t_philosophers *)arg;
	while (1)
	{
		i = 0;
		sem_wait(&philosopher->meal_lock);
		current_time = get_time();
		if (current_time
			- philosopher->last_meal > (size_t)philosopher->time_to_die)
		{
			sem_post(&philosopher->meal_lock);
			for(int i = 0; i < philosopher->program->philosopher_count;i++)
				sem_post(&philosopher->program->simulation_end);
			printf("%zu %d died\n", current_time,
				philosopher->id + 1);
			return (NULL);
		}
		if (philosopher->number_times_to_eat != -1
			&& philosopher->meal_count == philosopher->number_times_to_eat)
		{
			sem_wait(&philosopher->meal_lock);
			philosopher->program->simulation_end_flag = true;
			sem_post(&philosopher->meal_lock);
			return(NULL);
		}
		sem_post(&philosopher->meal_lock);
		i++;
		usleep(1000);
	}
	return (NULL);
}

void	program_init(int ac, char **args, t_program *program)
{
	int	count;

	count = atoi(args[1]);
	program->philosopher_count = count;
	program->forks = malloc(count * sizeof(sem_t));
	if (!program->forks)
		error_exit("malloc failed");
	for (int i = 0; i < count; i++)
	{
		sem_init(&program->forks[i], 0, 1);
	}
	for (int i = 0; i < count; i++)
	{
		program->philosophers[i].id = i;
		program->philosophers[i].time_to_die = atoi(args[2]);
		program->philosophers[i].time_to_eat = atoi(args[3]);
		program->philosophers[i].time_to_sleep = atoi(args[4]);
		program->philosophers[i].number_of_philosopher = count;
		program->philosophers[i].meal_count = 0;
		program->philosophers[i].last_meal = get_time();
		program->philosophers[i].program = program;
		program->philosophers[i].left_fork = &program->forks[i];
		program->philosophers[i].right_fork = &program->forks[(i + 1) % count];
		sem_init(&program->philosophers[i].meal_lock, 0, 1);
		if (ac == 6)
			program->philosophers[i].number_times_to_eat = atoi(args[5]);
		else
			program->philosophers[i].number_times_to_eat = -1;
		program->philosophers[i].ate_enough = false;
	}
	sem_init(&program->print, 0, 1);
	sem_init(&program->dead_flag, 0, 0);
}

void	start_simulation(t_program *program)
{
	pthread_t	monitor;
	pthread_t	death_listener;
	pid_t		*pid;
	int			i;

	i = 0;
	pid = malloc(program->philosopher_count * sizeof(pid_t));
	while (i < program->philosopher_count)
	{
		pid[i] = fork();
		if(pid[i] == 0)
		{
			if (pthread_create(&monitor, NULL, monitor_routine, &program->philosophers[i]) != 0)
				error_exit("pthread_create failed");
			if (pthread_create(&program->philosophers[i].thread, NULL,
					philo_routine, &program->philosophers[i]) != 0)
					error_exit("pthread_create failed");
			if(pthread_create(death_listener,NULL,death_listener,program) != 0)
				error_exit("pthread_creat failed");
			pthread_join(monitor, NULL);
			pthread_join(death_listener,NULL);
			pthread_join(program->philosophers[i].thread, NULL);
		}
		else
			error_exit("fork failed");
		i++;
	}
	i = 0;
	while(i < program->philosopher_count)
	{
		waitpid(pid[i],NULL,NULL);
		i++;
	}
}

int	main(int ac, char **av)
{
	t_program	*program;

	if (ac < 5 || ac > 6)
	{
		printf("Usage:\
			%s number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n",
			av[0]);
		return (EXIT_FAILURE);
	}
	program = malloc(sizeof(t_program));
	if (!program)
		error_exit("malloc failed");
	program->philosophers = malloc(atoi(av[1]) * sizeof(t_philosophers));
	if (!program->philosophers)
	{
		free(program);
		error_exit("malloc failed");
	}
	program_init(ac, av, program);
	start_simulation(program);
	return (EXIT_SUCCESS);
}
