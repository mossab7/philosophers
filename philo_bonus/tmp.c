#include "philosophers_bonus.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>

void	error_exit(char *message)
{
	int	r;

	r = write(2, "Error: ", 8);
	r = write(2, message, strlen(message));
	r = write(2, "\n", 1);
	(void)r;
	exit(EXIT_FAILURE);
}

size_t	get_time(t_program *program)
{
	struct timeval	curtime;

	gettimeofday(&curtime, NULL);
	return ((curtime.tv_sec * 1000 + curtime.tv_usec / 1000) - program->start_time);
}

void	ft_sleep(t_philosophers *philo, size_t milliseconds)
{
	size_t	start = get_time(philo->program);
	while (get_time(philo->program) - start < milliseconds)
		usleep(500);
}

bool	is_simulation_stopped(t_program *program)
{
	bool	stopped;

	sem_wait(program->stop_sem);
	stopped = program->simulation_stop;
	sem_post(program->stop_sem);
	return (stopped);
}

void	set_simulation_stopped(t_program *program)
{
	sem_wait(program->stop_sem);
	program->simulation_stop = true;
	sem_post(program->stop_sem);
}

void	print_status(t_philosophers *philo, char *status)
{
	sem_wait(philo->program->print_sem);
	if (!is_simulation_stopped(philo->program))
		printf("%zu %d %s\n", get_time(philo->program), philo->id, status);
	sem_post(philo->program->print_sem);
}

void	take_forks(t_philosophers *philo)
{
	sem_wait(philo->program->mutex_sem);
	sem_wait(philo->program->forks_sem);
	sem_wait(philo->program->forks_sem);
	sem_post(philo->program->mutex_sem);
	print_status(philo, "has taken a fork");
	print_status(philo, "has taken a fork");
}

void	release_forks(t_philosophers *philo)
{
	sem_post(philo->program->forks_sem);
	sem_post(philo->program->forks_sem);
}

void	eat_sleep_think(t_philosophers *philo)
{
	take_forks(philo);
	sem_wait(&philo->meal_lock);
	philo->last_meal = get_time(philo->program);
	print_status(philo, "is eating");
	sem_post(&philo->meal_lock);
	ft_sleep(philo, philo->time_to_eat);
	sem_wait(&philo->meal_lock);
	philo->meal_count++;
	sem_post(&philo->meal_lock);
	release_forks(philo);
	print_status(philo, "is sleeping");
	ft_sleep(philo, philo->time_to_sleep);
	print_status(philo, "is thinking");
}

void	*monitor_routine(void *arg)
{
	t_philosophers	*philo = (t_philosophers *)arg;

	while (1)
	{
		usleep(1000);
		sem_wait(&philo->meal_lock);
		if (get_time(philo->program) - philo->last_meal > philo->time_to_die)
		{
			print_status(philo, "died");
			sem_post(philo->program->stop_sem);
			exit(1);
		}
		if(philo->)
		sem_post(&philo->meal_lock);
		if (is_simulation_stopped(philo->program))
			break ;
	}
	return (NULL);
}

void	philo_routine(t_philosophers *philo)
{
	pthread_t	monitor;

	philo->last_meal = get_time(philo->program);
	if (philo->id % 2 == 0)
		usleep(philo->time_to_eat * 900);
	while (!is_simulation_stopped(philo->program))
		eat_sleep_think(philo);
	exit(0);
}

void	init_philosophers(int ac, char **av, t_program *program)
{
	int	i;

	i = 0;
	while (i < program->philosopher_count)
	{
		program->philosophers[i].id = i + 1;
		program->philosophers[i].time_to_die = ft_atoi(av[2]);
		program->philosophers[i].time_to_eat = ft_atoi(av[3]);
		program->philosophers[i].time_to_sleep = ft_atoi(av[4]);
		program->philosophers[i].meal_count = 0;
		program->philosophers[i].number_times_to_eat = (ac == 6) ? ft_atoi(av[5]) : -1;
		sem_init(&program->philosophers[i].meal_lock, 1, 1);
		program->philosophers[i].program = program;
		i++;
	}
}

void	program_init(int ac, char **av, t_program *program)
{
	program->philosopher_count = ft_atoi(av[1]);
	program->start_time = get_time(program);
	program->simulation_stop = false;

	program->forks_sem = sem_open("/forks_sem", O_CREAT, 0644, program->philosopher_count);
	program->mutex_sem = sem_open("/mutex_sem", O_CREAT, 0644, 1);
	program->print_sem = sem_open("/print_sem", O_CREAT, 0644, 1);
	program->stop_sem = sem_open("/stop_sem", O_CREAT, 0644, 1);

	if (program->forks_sem == SEM_FAILED || program->mutex_sem == SEM_FAILED ||
		program->print_sem == SEM_FAILED || program->stop_sem == SEM_FAILED)
		error_exit("sem_open failed");

	program->philosophers = malloc(sizeof(t_philosophers) * program->philosopher_count);
	if (!program->philosophers)
		error_exit("malloc failed");
	init_philosophers(ac, av, program);
}

bool	check_arguments(int ac, char **av)
{
	int	i, j;

	if (ac < 5 || ac > 6)
	{
		printf("Usage: %s philo_count die_time eat_time sleep_time [meal_count]\n", av[0]);
		return (false);
	}
	for (i = 1; i < ac; i++)
	{
		for (j = 0; av[i][j]; j++)
			if (av[i][j] < '0' || av[i][j] > '9')
				return (printf("Error: Non-numeric argument\n"), false);
		if (ft_atoi(av[i]) <= 0)
			return (printf("Error: Invalid argument\n"), false);
	}
	return (true);
}

void	start_simulation(t_program *program)
{
	pid_t	*pids = malloc(sizeof(pid_t) * program->philosopher_count);
	int		i;

	for (i = 0; i < program->philosopher_count; i++)
	{
		pids[i] = fork();
		if (pids[i] == 0)
		{
			pthread_t monitor;
			philo_routine(&program->philosophers[i]);
			pthread_create(&monitor, NULL, monitor_routine, &program->philosophers[i]);
			pthread_join(&program->philosophers[i],NULL);
			pthread_join(&monitor,NULL);
		}
	}
	for (i = 0; i < program->philosopher_count; i++)
		waitpid(pids[i],NULL,0);
	for (i = 0; i < program->philosopher_count; i++)
		kill(pids[i], SIGKILL);
	free(pids);
}

void	free_resources(t_program *program)
{
	sem_close(program->forks_sem);
	sem_unlink("/forks_sem");
	sem_close(program->mutex_sem);
	sem_unlink("/mutex_sem");
	sem_close(program->print_sem);
	sem_unlink("/print_sem");
	sem_close(program->stop_sem);
	sem_unlink("/stop_sem");
	free(program->philosophers);
	free(program);
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
