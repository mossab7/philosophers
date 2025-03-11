/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers_fork.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 22:01:48 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/06 22:01:53 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	take_forks_even(t_philosophers *philo)
{
	pthread_mutex_lock(philo->right_fork);
	print_status(philo, "has taken a fork");
	pthread_mutex_lock(philo->left_fork);
	print_status(philo, "has taken a fork");
	philo->forks_acquired = true;
}

void	take_forks_odd(t_philosophers *philo)
{
	pthread_mutex_lock(philo->left_fork);
	print_status(philo, "has taken a fork");
	pthread_mutex_lock(philo->right_fork);
	print_status(philo, "has taken a fork");
	philo->forks_acquired = true;
}

void	take_fork(t_philosophers *philosopher)
{
	philosopher->forks_acquired = false;
	if (is_simulation_stopped(philosopher->program))
		return ;
	if (philosopher->id % 2 == 0)
		take_forks_even(philosopher);
	else
		take_forks_odd(philosopher);
}

void	release_fork(t_philosophers *philosopher)
{
	if(philosopher->forks_acquired == false)
		return ;
	pthread_mutex_unlock(philosopher->left_fork);
	pthread_mutex_unlock(philosopher->right_fork);
}

bool	check_all_ate(t_program *program)
{
	int		i;
	bool	all_ate;

	i = 0;
	all_ate = true;
	while (i < program->philosopher_count)
	{
		pthread_mutex_lock(&program->philosophers[i].meal_lock);
		if (program->philosophers[i].number_times_to_eat != -1
			&& program->philosophers[i].meal_count
			< program->philosophers[i].number_times_to_eat)
			all_ate = false;
		pthread_mutex_unlock(&program->philosophers[i].meal_lock);
		i++;
	}
	return (all_ate);
}
