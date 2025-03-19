/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_launcher.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 04:55:17 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/18 04:55:17 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

void	open_local_semaphores(t_philosophers *philosopher)
{
	char	*id;

	id = ft_itoa(philosopher->id);
	philosopher->stop_sem_name = ft_strjoin("stop_sem_", id);
	philosopher->meal_sem_name = ft_strjoin("meal_sem_", id);
	free(id);
	philosopher->stop_sem = open_sem(philosopher->stop_sem_name, O_CREAT, 0644,
			1);
	philosopher->meal_sem = open_sem(philosopher->meal_sem_name, O_CREAT, 0644,
			1);
	if (!philosopher->stop_sem || !philosopher->meal_sem)
	{
		printf("Error: Failed to create philosopher semaphores.\n");
		cleanup_process(philosopher, philosopher->program, false);
		exit(1);
	}
}

void	philosopher_start(t_philosophers *philosopher)
{
	pthread_t	philosopher_thread;
	pthread_t	monitor_thread;
	pthread_t	death_listener_thread;

	open_local_semaphores(philosopher);
	philosopher->last_meal = get_time(philosopher->program);
	if (pthread_create(&monitor_thread, NULL, monitor_routine, philosopher) != 0
		|| pthread_create(&philosopher_thread, NULL, philosopher_routine,
			philosopher) != 0 || pthread_create(&death_listener_thread, NULL,
			death_listener_routine, philosopher) != 0)
	{
		printf("Error: Failed to create thread.\n");
		cleanup_process(philosopher, philosopher->program, false);
		exit(1);
	}
	pthread_join(philosopher_thread, NULL);
	pthread_join(monitor_thread, NULL);
	pthread_join(death_listener_thread, NULL);
	cleanup_process(philosopher, philosopher->program, true);
	exit(0);
}

void	creat_philosophers(t_program *program)
{
	int				i;
	int				j;
	struct timeval	tv;

	i = 0;
	gettimeofday(&tv, NULL);
	program->start_time = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	while (i < program->number_of_philosophers)
	{
		program->pids[i] = fork();
		if (program->pids[i] == 0)
		{
			philosopher_start(&program->philosophers[i]);
		}
		else if (program->pids[i] < 0)
		{
			printf("Error: Failed to create child process.\n");
			j = 0;
			while (j < i)
			{
				kill(program->pids[j], SIGTERM);
				j++;
			}
			program_destroy(program);
			exit(1);
		}
		i++;
	}
}

pid_t	create_meals_monitor(t_program *program)
{
	pid_t	meals_pid;
	int		i;

	meals_pid = fork();
	if (meals_pid == 0)
	{
		meals_monitor(program);
		exit(0);
	}
	if (meals_pid < 0)
	{
		printf("Error: Failed to create meals monitor process.\n");
		i = 0;
		while (i < program->number_of_philosophers)
		{
			kill(program->pids[i], SIGTERM);
			i++;
		}
		program_destroy(program);
		exit(1);
	}
	return (meals_pid);
}

void	program_start(t_program *program)
{
	pid_t meals_pid;
	int i;

	creat_philosophers(program);
	meals_pid = -1;
	if (program->number_of_times_each_philosopher_must_eat != -1)
	{
		meals_pid = create_meals_monitor(program);
	}
	i = 0;
	while (i < program->number_of_philosophers)
	{
		waitpid(program->pids[i], NULL, 0);
		i++;
	}
	if (program->number_of_times_each_philosopher_must_eat != -1
		&& meals_pid > 0)
	{
		kill(meals_pid, SIGTERM);
		waitpid(meals_pid, NULL, 0);
	}
}