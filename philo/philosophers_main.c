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
	fprintf(stderr, "Error: %s\n", message);
	exit(EXIT_FAILURE);
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
