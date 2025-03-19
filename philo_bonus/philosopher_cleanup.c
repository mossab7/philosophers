/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_cleanup.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 04:55:18 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/18 04:55:18 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

void	program_destroy(t_program *program)
{
	int	i;

	sem_close(program->forks_sem);
	sem_close(program->print_sem);
	sem_close(program->death_sem);
	if (program->philo_full_sem)
		sem_close(program->philo_full_sem);
	i = 0;
	while (i < program->number_of_philosophers)
	{
		sem_close(program->philosophers[i].stop_sem);
		sem_close(program->philosophers[i].meal_sem);
		free(program->philosophers[i].stop_sem_name);
		free(program->philosophers[i].meal_sem_name);
		i++;
	}
	cleanup_semaphores(program);
	free(program->pids);
	free(program->philosophers);
	free(program);
}

void	cleanup_process(t_philosophers *philosopher, t_program *program,
		bool executed)
{
	if (philosopher->stop_sem)
	{
		sem_close(philosopher->stop_sem);
		sem_unlink(philosopher->stop_sem_name);
	}
	if (philosopher->meal_sem)
	{
		sem_close(philosopher->meal_sem);
		sem_unlink(philosopher->meal_sem_name);
	}
	free(philosopher->stop_sem_name);
	free(philosopher->meal_sem_name);
	sem_close(philosopher->program->forks_sem);
	sem_close(philosopher->program->print_sem);
	sem_close(philosopher->program->death_sem);
	if (philosopher->program->philo_full_sem)
		sem_close(philosopher->program->philo_full_sem);
	if (executed)
	{
		free(program->pids);
		free(program->philosophers);
		free(program);
	}
}

void	cleanup_semaphores(t_program *program)
{
	sem_close(program->forks_sem);
	sem_close(program->print_sem);
	sem_close(program->death_sem);
	if (program->philo_full_sem)
		sem_close(program->philo_full_sem);
	sem_unlink("forks_sem");
	sem_unlink("print_sem");
	sem_unlink("death_sem");
	sem_unlink("philo_full_sem");
}
