/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_death_handlers.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 04:55:18 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/18 04:55:18 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

bool	check_philo_death(t_philosophers *philosopher)
{
	long long	current_time;
	bool		is_dead;

	current_time = get_time(philosopher->program);
	sem_wait(philosopher->meal_sem);
	is_dead = (current_time
			- philosopher->last_meal) > philosopher->program->time_to_die;
	sem_post(philosopher->meal_sem);
	return (is_dead);
}

void	report_philo_death(t_philosophers *philosopher)
{
	print_status(philosopher, "died");
	signal_death(philosopher->program);
	set_simulation_stopped(philosopher);
}

void	signal_death(t_program *program)
{
	for (int i = 0; i < program->number_of_philosophers; i++)
		sem_post(program->death_sem);
}

void	*death_listener_routine(void *arg)
{
	t_philosophers	*philosopher;

	philosopher = (t_philosophers *)arg;
	sem_wait(philosopher->program->death_sem);
	set_simulation_stopped(philosopher);
	return (NULL);
}
