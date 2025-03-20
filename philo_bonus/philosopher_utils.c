/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 04:55:18 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/18 04:55:18 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

long long	get_time(t_program *program)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (((tv.tv_sec * 1000) + (tv.tv_usec / 1000)) - program->start_time);
}

void	ft_sleep(t_philosophers *philosopher, int time_ms)
{
	long long	start;
	long long	current;

	start = get_time(philosopher->program);
	while (!is_simulation_stopped(philosopher))
	{
		current = get_time(philosopher->program);
		if (current - start >= time_ms)
			break ;
		usleep(500);
	}
}

void	print_status(t_philosophers *philosopher, char *status)
{
	if (!is_simulation_stopped(philosopher))
	{
		sem_wait(philosopher->program->print_sem);
		printf("%lld %d %s\n", get_time(philosopher->program), philosopher->id
			+ 1, status);
		sem_post(philosopher->program->print_sem);
	}
}
