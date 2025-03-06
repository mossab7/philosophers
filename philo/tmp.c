/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tmp.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 21:46:55 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/06 21:47:05 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

static size_t	get_time(void)
{
	struct timeval	curtime;

	if (gettimeofday(&curtime, NULL) != 0)
	{
		perror("gettimeofday failed");
		return (0);
	}
	return (curtime.tv_sec * 1000 + curtime.tv_usec / 1000);
}

void	ft_sleep(t_philosophers *philosopher, size_t milliseconds)
{
	size_t	start;
	bool	is_stopped;
	size_t	current;

	start = get_time();
	while (1)
	{
		pthread_mutex_lock(&philosopher->program->stop_mutex);
		is_stopped = philosopher->program->simulation_stop;
		pthread_mutex_unlock(&philosopher->program->stop_mutex);
		if (is_stopped)
			break ;
		current = get_time();
		if (current - start >= milliseconds)
			break ;
		usleep(50);
	}
}

bool	is_simulation_stopped(t_program *program)
{
	bool	is_stopped;

	pthread_mutex_lock(&program->stop_mutex);
	is_stopped = program->simulation_stop;
	pthread_mutex_unlock(&program->stop_mutex);
	return (is_stopped);
}

static void	set_simulation_stopped(t_program *program)
{
	pthread_mutex_lock(&program->stop_mutex);
	program->simulation_stop = true;
	pthread_mutex_unlock(&program->stop_mutex);
}

void	print_status(t_philosophers *philosopher, char *status)
{
	bool	is_stopped;
	size_t	time;

	pthread_mutex_lock(&philosopher->program->print);
	pthread_mutex_lock(&philosopher->program->stop_mutex);
	is_stopped = philosopher->program->simulation_stop;
	pthread_mutex_unlock(&philosopher->program->stop_mutex);
	time = get_time();
	if (!is_stopped)
		printf("%zu %d %s\n", time, philosopher->id + 1, status);
	pthread_mutex_unlock(&philosopher->program->print);
}

void	error_exit(char *message)
{
	fprintf(stderr, "Error: %s\n", message);
	exit(EXIT_FAILURE);
}

static void	take_forks_even(t_philosophers *philo)
{
	pthread_mutex_lock(philo->right_fork);
	print_status(philo, "has taken a fork");
	pthread_mutex_lock(philo->left_fork);
	print_status(philo, "has taken a fork");
}

static void	take_forks_odd(t_philosophers *philo)
{
	pthread_mutex_lock(philo->left_fork);
	print_status(philo, "has taken a fork");
	pthread_mutex_lock(philo->right_fork);
	print_status(philo, "has taken a fork");
}

void	take_fork(t_philosophers *philosopher)
{
	if (is_simulation_stopped(philosopher->program))
		return ;
	if (philosopher->id % 2 == 0)
		take_forks_even(philosopher);
	else
		take_forks_odd(philosopher);
}

void	release_fork(t_philosophers *philosopher)
{
	pthread_mutex_unlock(philosopher->left_fork);
	pthread_mutex_unlock(philosopher->right_fork);
}

static void	handle_single_philo(t_philosophers *philo)
{
	print_status(philo, "has taken a fork");
	ft_sleep(philo, philo->time_to_die);
}

static void	eat_sleep_think(t_philosophers *philo)
{
	pthread_mutex_lock(&philo->meal_lock);
	philo->last_meal = get_time();
	print_status(philo, "is eating");
	pthread_mutex_unlock(&philo->meal_lock);
	ft_sleep(philo, philo->time_to_eat);
	pthread_mutex_lock(&philo->meal_lock);
	philo->meal_count++;
	pthread_mutex_unlock(&philo->meal_lock);
	release_fork(philo);
	print_status(philo, "is sleeping");
	ft_sleep(philo, philo->time_to_sleep);
	print_status(philo, "is thinking");
	if (philo->time_to_eat > philo->time_to_sleep)
		ft_sleep(philo, philo->time_to_eat - philo->time_to_sleep);
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

static bool	check_philo_death(t_program *program, int i, size_t current_time)
{
	pthread_mutex_lock(&program->philosophers[i].meal_lock);
	if (current_time - program->philosophers[i].last_meal
		> (size_t)program->philosophers[i].time_to_die)
	{
		pthread_mutex_unlock(&program->philosophers[i].meal_lock);
		pthread_mutex_lock(&program->print);
		pthread_mutex_lock(&program->stop_mutex);
		if (!program->simulation_stop)
		{
			program->simulation_stop = true;
			printf("%zu %d died\n"\
				, current_time, program->philosophers[i].id + 1);
		}
		pthread_mutex_unlock(&program->stop_mutex);
		pthread_mutex_unlock(&program->print);
		return (true);
	}
	pthread_mutex_unlock(&program->philosophers[i].meal_lock);
	return (false);
}

static bool	check_all_ate(t_program *program)
{
	int		i;
	bool	all_ate;

	i = 0;
	all_ate = true;
	while (i < program->philosopher_count)
	{
		if (program->philosophers[i].number_times_to_eat != -1
			&& program->philosophers[i].meal_count
			< program->philosophers[i].number_times_to_eat)
			all_ate = false;
		i++;
	}
	return (all_ate);
}

void	*monitor_routine(void *arg)
{
	t_program	*program;
	int			i;
	size_t		current_time;

	program = (t_program *)arg;
	while (!is_simulation_stopped(program))
	{
		i = 0;
		while (i < program->philosopher_count)
		{
			current_time = get_time();
			if (check_philo_death(program, i, current_time))
				return (NULL);
			i++;
		}
		if (check_all_ate(program)
			&& program->philosophers[0].number_times_to_eat != -1)
		{
			set_simulation_stopped(program);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}

static void
	init_philosophers(t_program *program, int count, char **args, int ac)
{
	int	i;

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
		program->philosophers[i].program = program;
		program->philosophers[i].left_fork = &program->forks[i];
		program->philosophers[i].right_fork = &program->forks[(i + 1) % count];
		pthread_mutex_init(&program->philosophers[i].meal_lock, NULL);
		if (ac == 6)
			program->philosophers[i].number_times_to_eat = atoi(args[5]);
		else
			program->philosophers[i].number_times_to_eat = -1;
		i++;
	}
}

void	program_init(int ac, char **args, t_program *program)
{
	int		count;
	int		i;

	count = atoi(args[1]);
	program->philosopher_count = count;
	program->all_ate_enough = false;
	program->simulation_stop = false;
	program->forks = malloc(count * sizeof(pthread_mutex_t));
	if (!program->forks)
		error_exit("malloc failed");
	pthread_mutex_init(&program->stop_mutex, NULL);
	i = 0;
	while (i < count)
		pthread_mutex_init(&program->forks[i++], NULL);
	program->philosophers = malloc(count * sizeof(t_philosophers));
	if (!program->philosophers)
		error_exit("malloc failed");
	init_philosophers(program, count, args, ac);
	pthread_mutex_init(&program->print, NULL);
}

void	start_simulation(t_program *program)
{
	pthread_t	monitor;
	int			i;

	if (pthread_create(&monitor, NULL, monitor_routine, program) != 0)
		error_exit("pthread_create failed");
	i = 0;
	while (i < program->philosopher_count)
	{
		if (pthread_create(&program->philosophers[i].thread, NULL,
				philo_routine, &program->philosophers[i]) != 0)
			error_exit("pthread_create failed");
		i++;
	}
	pthread_join(monitor, NULL);
	i = 0;
	while (i < program->philosopher_count)
		pthread_join(program->philosophers[i++].thread, NULL);
}

int	main(int ac, char **av)
{
	t_program	*program;

	if (ac < 5 || ac > 6)
	{
		printf("Usage: %s number_of_philosophers time_to_die time_to_eat "
			"time_to_sleep [number_of_times_each_philosopher_must_eat]\n",
			av[0]);
		return (EXIT_FAILURE);
	}
	program = malloc(sizeof(t_program));
	if (!program)
		error_exit("malloc failed");
	program_init(ac, av, program);
	start_simulation(program);
	free(program->philosophers);
	free(program->forks);
	free(program);
	return (EXIT_SUCCESS);
}
