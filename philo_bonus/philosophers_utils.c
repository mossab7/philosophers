/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 21:56:10 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/06 21:56:11 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

size_t	get_time(t_program *program)
{
	struct timeval	curtime;

	if (gettimeofday(&curtime, NULL) != 0)
	{
		perror("gettimeofday failed");
		return (0);
	}
	return ((curtime.tv_sec * 1000 + curtime.tv_usec / 1000)
		- program->start_time);
}

void	ft_sleep(t_philosophers *philosopher, size_t milliseconds)
{
	size_t	start;
	bool	is_stopped;
	size_t	current;

	start = get_time(philosopher->program);
	while (1)
	{
		sem_wait(&philosopher->program->stop_mutex);
		is_stopped = philosopher->program->simulation_stop;
		sem_post(&philosopher->program->stop_mutex);
		if (is_stopped)
			break ;
		current = get_time(philosopher->program);
		if (current - start >= milliseconds)
			break ;
		usleep(50);
	}
}

bool	is_simulation_stopped(t_program *program)
{
	bool	is_stopped;

	sem_wait(&program->stop_mutex);
	is_stopped = program->simulation_stop;
	sem_post(&program->stop_mutex);
	return (is_stopped);
}

void	set_simulation_stopped(t_program *program)
{
	sem_wait(&program->stop_mutex);
	program->simulation_stop = true;
	sem_post(&program->stop_mutex);
}

void	print_status(t_philosophers *philosopher, char *status)
{
	bool	is_stopped;
	size_t	time;

	sem_wait(&philosopher->program->print);
	sem_wait(&philosopher->program->stop_mutex);
	is_stopped = philosopher->program->simulation_stop;
	sem_post(&philosopher->program->stop_mutex);
	time = get_time(philosopher->program);
	if (!is_stopped)
		printf("%zu %d %s\n", time, philosopher->id + 1, status);
	sem_post(&philosopher->program->print);
}
