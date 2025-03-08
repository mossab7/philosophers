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

#include "philosophers_bonus.h"

void	take_forks_even(t_philosophers *philo)
{
	sem_wait(philo->right_fork);
	print_status(philo, "has taken a fork");
	sem_wait(philo->left_fork);
	print_status(philo, "has taken a fork");
}

void	take_forks_odd(t_philosophers *philo)
{
	sem_wait(philo->left_fork);
	print_status(philo, "has taken a fork");
	sem_wait(philo->right_fork);
	print_status(philo, "has taken a fork");
}

void	take_fork(t_philosophers *philosopher)
{
	if (is_simulation_stopped(philosopher->program))
		return ;
	if (philosopher->id % 2 == 0)
		take_forks_even(philosopher);
	else
		take_forks_odd(philosopher);
}

void	release_fork(t_philosophers *philosopher)
{
	sem_post(philosopher->left_fork);
	sem_post(philosopher->right_fork);
}
