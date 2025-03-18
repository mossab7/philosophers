/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_monitor_routine.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 04:55:18 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/18 04:55:18 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

void	*monitor_routine(void *arg)
{
	t_philosophers	*philosopher;

	philosopher = (t_philosophers *)arg;
	while (!is_simulation_stopped(philosopher))
	{
		if (check_philo_death(philosopher))
		{
			report_philo_death(philosopher);
			return (NULL);
		}
		if (check_eat_enough(philosopher))
		{
			sem_post(philosopher->program->philo_full_sem);
			set_simulation_stopped(philosopher);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}
