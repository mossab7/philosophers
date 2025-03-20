/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers_main.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 22:00:56 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/06 22:00:59 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	error_exit(char *message)
{
	int	r;

	r = write(2, "Error: ", 8);
	r = write(2, message, strlen(message));
	r = write(2, "\n", 1);
	(void)r;
	exit(EXIT_FAILURE);
}

void	free_resources(t_program *program)
{
	int	i;

	if (program)
	{
		pthread_mutex_destroy(&program->print);
		pthread_mutex_destroy(&program->stop_mutex);
		i = 0;
		while (i < program->philosopher_count)
		{
			pthread_mutex_destroy(&program->forks[i]);
			pthread_mutex_destroy(&program->philosophers[i].meal_lock);
			i++;
		}
		free(program->philosophers);
		free(program->forks);
		free(program);
	}
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
