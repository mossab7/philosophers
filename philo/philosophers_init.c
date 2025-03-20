/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers_init.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 21:58:17 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/06 21:58:20 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	init_philosophers(t_program *program, int count, char **args, int ac)
{
	int	i;

	i = 0;
	while (i < count)
	{
		program->philosophers[i].id = i;
		program->philosophers[i].time_to_die = ft_atoi(args[2]);
		program->philosophers[i].time_to_eat = ft_atoi(args[3]);
		program->philosophers[i].time_to_sleep = ft_atoi(args[4]);
		program->philosophers[i].number_of_philosopher = count;
		program->philosophers[i].meal_count = 0;
		program->philosophers[i].last_meal = get_time(program);
		program->philosophers[i].program = program;
		program->philosophers[i].left_fork = &program->forks[i];
		program->philosophers[i].right_fork = &program->forks[(i + 1) % count];
		pthread_mutex_init(&program->philosophers[i].meal_lock, NULL);
		if (ac == 6)
			program->philosophers[i].number_times_to_eat = ft_atoi(args[5]);
		else
			program->philosophers[i].number_times_to_eat = -1;
		i++;
	}
}

void	program_init(int ac, char **args, t_program *program)
{
	int	count;
	int	i;

	count = ft_atoi(args[1]);
	program->philosopher_count = count;
	program->all_ate_enough = false;
	program->simulation_stop = false;
	program->start_time = 0;
	program->start_time = get_time(program);
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
