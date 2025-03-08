#include "philosophers_bonus.h"

void init_philosophers(int ac, char **av, t_program *program)
{
	int	i;

	i = 0;
	while (i < program->philosopher_count)
	{
		program->philosophers[i].id = i + 1;
		program->philosophers[i].time_to_die = ft_atoi(av[2]);
		program->philosophers[i].time_to_eat = ft_atoi(av[3]);
		program->philosophers[i].time_to_sleep = ft_atoi(av[4]);
		program->philosophers[i].number_of_philosopher = program->philosopher_count;
		program->philosophers[i].meal_count = 0;
		program->philosophers[i].number_times_to_eat = -1;
		if (ac == 6)
			program->philosophers[i].number_times_to_eat = ft_atoi(av[5]);
		else
			program->philosophers[i].number_times_to_eat = -1;
		program->philosophers[i].last_meal = get_time(program);
		sem_init(&program->philosophers[i].meal_lock, 0, 1);
		program->philosophers[i].left_fork = &program->forks[i];
		program->philosophers[i].right_fork = &program->forks[(i + 1) % program->philosopher_count];
		program->philosophers[i].program = program;
		i++;
	}
}

void program_init(int ac,char **av,t_program *program)
{
	int	i;
	char *id;

	program->philosopher_count = ft_atoi(av[1]);
	program->start_time = 0;
	program->start_time = get_time(program);
	program->all_ate_enough = false;
	program->simulation_stop = false;
	program->forks = malloc(sizeof(sem_t) * program->philosopher_count);
	if (!program->forks)
		error_exit("malloc failed");
	i = 0;
	while (i < program->philosopher_count)
	{
		id = ft_itoa(i);
		sem_open(ft_strjoin("/fork_",id), O_CREAT, 0644, 1);
		free(id);
		i++;
	}
	sem_open("/print", O_CREAT, 0644, 1);
	sem_open("/stop_mutex", O_CREAT, 0644, 1);
	program->philosophers = malloc(sizeof(t_philosophers) * program->philosopher_count);
	if (!program->philosophers)
		error_exit("malloc failed");
	init_philosophers(ac,av, program);
}

bool	check_arguments(int ac, char **av)
{
	int	i;
	int	j;

	if (ac < 5 || ac > 6)
	{
		printf("Usage:\
			%s number_of_philosophers time_to_die time_to_eat\
				time_to_sleep[number_of_times_each_philosopher_must_eat]\n ",
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

void	eat_sleep_think(t_philosophers *philo)
{
	long	time_till_death;

	sem_wait(&philo->meal_lock);
	philo->last_meal = get_time(philo->program);
	print_status(philo, "is eating");
	sem_post(&philo->meal_lock);
	ft_sleep(philo, philo->time_to_eat);
	sem_wait(&philo->meal_lock);
	philo->meal_count++;
	sem_post(&philo->meal_lock);
	release_fork(philo);
	print_status(philo, "is sleeping");
	ft_sleep(philo, philo->time_to_sleep);
	print_status(philo, "is thinking");
	if (philo->id % 2 == 0)
		return ;
	time_till_death = get_time(philo->program) - philo->last_meal;
	if (time_till_death < philo->time_to_die * 0.7)
		usleep((philo->time_to_die - time_till_death) * 600);
}

void	*philo_routine(void *arg)
{
	t_philosophers	*philo;

	philo = (t_philosophers *)arg;
	if (philo->number_of_philosopher == 1)
	{
		handle_single_philo(philo);
		return (NULL);
	}
	if (philo->id % 2 == 0)
		ft_sleep(philo, 50);
	while (!is_simulation_stopped(philo->program))
	{
		take_fork(philo);
		if (is_simulation_stopped(philo->program))
		{
			release_fork(philo);
			break ;
		}
		eat_sleep_think(philo);
	}
	return (NULL);
}

void	*monitor_routine(void *arg)
{
	t_philosophers	*philo;
	int			i;
	size_t		current_time;

	philo = (t_philosophers *)arg;
	current_time = get_time(philo);
	while(1)
	{
		sem_wait(&philo->program->stop_sem);
		sem_wait(&philo->meal_lock);
		if(philo->last_meal - current_time > philo->time_to_die)
		{
			i = 0;
			while(i < philo->program->philosopher_count)
			{
				sem_post(&philo->program->simulation_stop);
				i++;
			}
			print_status(philo, "died");
			sem_post(&philo->meal_lock);
			set_simulation_stopped(philo->program);
			break ;
		}
		sem_post(&philo->meal_lock);
		sem_post(&philo->program->stop_sem);
	}
	return (NULL);
}

void	*death_listener(void *arg)
{
	t_philosophers	*philo;

	philo = (t_philosophers *)arg;
	while(1)
	{
		sem_wait(&philo->program->stop_sem);
		cleanup(philo);
		exit(0);
	}
}

void start_simulation(t_program *program)
{
	pid_t *pid;
	int i;

	i = 0;
	pid = malloc(sizeof(pid_t) * program->philosopher_count);
	while (i < program->philosopher_count)
	{
		pid[i] = fork();
		if (pid[i] == 0)
		{
			pthread_create(&program->philosophers[i].thread, NULL, philo_routine, &program->philosophers[i]);
			pthread_creat(&program->philosophers[i].thread, NULL, monitor_routine, &program->philosophers[i]);
			pthread_creat(&program->philosophers[i].thread, NULL, death_listener, &program->philosophers[i]);
			pthread_join(program->philosophers[i].thread, NULL);
			pthread_join(program->philosophers[i].thread, NULL);
			pthread_join(program->philosophers[i].thread, NULL);
			exit(0);
		}
		i++;
	}
	i = 0;
	while (i < program->philosopher_count)
	{
		waitpid(pid[i], NULL, 0);
		i++;
	}
}

int	main(int ac, char **av)
{
	t_program	*program;

	if (!check_arguments(ac, av))
		return (EXIT_FAILURE);
	program = malloc(sizeof(t_program));
	if (!program)
		error_exit("malloc failed");
	program_init(ac, av, program);
	start_simulation(program);
	free_resources(program);
	return (EXIT_SUCCESS);
}
