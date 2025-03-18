/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_meals_monitor.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 04:55:18 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/18 04:55:18 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

bool check_eat_enough(t_philosophers *philosopher)
{
	if (philosopher->program->number_of_times_each_philosopher_must_eat == -1)
		return false;
	sem_wait(philosopher->meal_sem);
	bool enough = philosopher->meal_count >= philosopher->program->number_of_times_each_philosopher_must_eat;
	sem_post(philosopher->meal_sem);
	return enough;
}

bool check_all_ate_enough(t_program *program)
{
	int philo_full_count = 0;
	while (philo_full_count < program->number_of_philosophers)
	{
		sem_wait(program->philo_full_sem);
		philo_full_count++;
	}
	return true;
}

void meals_monitor(t_program *program)
{
	check_all_ate_enough(program);
	signal_death(program);

	sem_close(program->forks_sem);
	sem_close(program->print_sem);
	sem_close(program->death_sem);
	if (program->philo_full_sem)
		sem_close(program->philo_full_sem);

	free(program->pids);
	free(program->philosophers);
	free(program);

	exit(0);
}
