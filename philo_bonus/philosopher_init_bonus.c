/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_init_bonus.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 04:55:18 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/20 02:59:28 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher_bonus.h"

void	close_sem(t_program *program)
{
	sem_close(program->forks_sem);
	sem_close(program->print_sem);
	sem_close(program->death_sem);
	if (program->philo_full_sem)
		sem_close(program->philo_full_sem);
	cleanup_semaphores(program);
}

t_philosophers	*philosopher_init(t_program *program)
{
	t_philosophers	*philosopher;
	int				i;

	philosopher = malloc(sizeof(t_philosophers)
			* program->number_of_philosophers);
	if (!philosopher)
	{
		printf("Error: Failed to allocate memory for philosophers.\n");
		return (NULL);
	}
	i = 0;
	while (i < program->number_of_philosophers)
	{
		philosopher[i].id = i;
		philosopher[i].program = program;
		philosopher[i].stop_sem_name = NULL;
		philosopher[i].meal_sem_name = NULL;
		philosopher[i].stop_sem = NULL;
		philosopher[i].meal_sem = NULL;
		philosopher[i].meal_count = 0;
		philosopher[i].last_meal = 0;
		philosopher[i].simulation_stopped = false;
		i++;
	}
	return (philosopher);
}

bool	construct_program(t_program *program, int ac, char **av)
{
	program->number_of_philosophers = ft_atoi(av[1]);
	program->time_to_die = ft_atoi(av[2]);
	program->time_to_eat = ft_atoi(av[3]);
	program->time_to_sleep = ft_atoi(av[4]);
	if (ac == 6)
		program->number_of_times_each_philosopher_must_eat = ft_atoi(av[5]);
	else
		program->number_of_times_each_philosopher_must_eat = -1;
	program->forks_sem = open_sem("forks_sem", O_CREAT, 0644,
			program->number_of_philosophers);
	program->print_sem = open_sem("print_sem", O_CREAT, 0644, 1);
	program->death_sem = open_sem("death_sem", O_CREAT, 0644, 0);
	if (program->number_of_times_each_philosopher_must_eat != -1)
	{
		program->philo_full_sem = open_sem("philo_full_sem", O_CREAT, 0644, 0);
		if (program->philo_full_sem == SEM_FAILED)
		{
			printf("Error: Failed to create philo_full semaphore.\n");
			close_sem(program);
			return (free(program), false);
		}
	}
	else
		program->philo_full_sem = NULL;
	return (true);
}

bool	construct_pids(t_program *program)
{
	program->pids = malloc(program->number_of_philosophers * sizeof(pid_t));
	if (!program->pids)
	{
		printf("Error: Failed to allocate memory for process IDs.\n");
		close_sem(program);
		free(program);
		return (false);
	}
	return (true);
}

t_program	*program_init(int ac, char **av)
{
	t_program	*program;

	program = malloc(sizeof(t_program));
	if (!program)
	{
		printf("Error: Failed to allocate memory for program.\n");
		return (NULL);
	}
	if (!construct_program(program, ac, av))
		return (NULL);
	if (!construct_pids(program))
		return (NULL);
	program->philosophers = philosopher_init(program);
	if (!program->philosophers)
	{
		printf("Error: Failed to initialize philosophers.\n");
		free(program->pids);
		free(program);
		return (NULL);
	}
	return (program);
}
