/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 04:55:17 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/18 04:55:17 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

void	handle_single_philosopher(t_philosophers *philosopher)
{
	print_status(philosopher, "has taken a fork");
	ft_sleep(philosopher, philosopher->program->time_to_die);
	if (philosopher->program->number_of_times_each_philosopher_must_eat != -1)
		sem_post(philosopher->program->philo_full_sem);
	report_philo_death(philosopher);
}

void	*philosopher_routine(void *arg)
{
	t_philosophers	*philosopher;

	philosopher = (t_philosophers *)arg;
	if (philosopher->program->number_of_philosophers == 1)
	{
		handle_single_philosopher(philosopher);
		return (NULL);
	}
	if (philosopher->id % 2 == 0)
		ft_sleep(philosopher, philosopher->program->time_to_eat / 2);
	while (!is_simulation_stopped(philosopher))
	{
		take_fork(philosopher);
		if (is_simulation_stopped(philosopher))
		{
			release_fork(philosopher);
			break ;
		}
		eat_sleep_think(philosopher);
	}
	return (NULL);
}
