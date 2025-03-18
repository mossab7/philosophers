/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_operations.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 04:55:18 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/18 04:55:18 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

void	eat_sleep_think(t_philosophers *philosopher)
{
	long long	time_till_death;

	sem_wait(philosopher->meal_sem);
	philosopher->last_meal = get_time(philosopher->program);
	sem_post(philosopher->meal_sem);
	print_status(philosopher, "is eating");
	ft_sleep(philosopher, philosopher->program->time_to_eat);
	sem_wait(philosopher->meal_sem);
	philosopher->meal_count++;
	sem_post(philosopher->meal_sem);
	release_fork(philosopher);
	print_status(philosopher, "is sleeping");
	ft_sleep(philosopher, philosopher->program->time_to_sleep);
	print_status(philosopher, "is thinking");
	time_till_death = philosopher->program->time_to_die
		- (get_time(philosopher->program) - philosopher->last_meal);
	ft_sleep(philosopher, (time_till_death > 0) ? (time_till_death * 0.7) : 0);
}
