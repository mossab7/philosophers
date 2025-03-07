/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 21:46:55 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/06 21:56:01 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"


void	handle_single_philo(t_philosophers *philo)
{
	print_status(philo, "has taken a fork");
	ft_sleep(philo, philo->time_to_die);
}

void	eat_sleep_think(t_philosophers *philo)
{
	long time_till_death;

	pthread_mutex_lock(&philo->meal_lock);
	philo->last_meal = get_time();
	print_status(philo, "is eating");
	pthread_mutex_unlock(&philo->meal_lock);
	ft_sleep(philo, philo->time_to_eat);
	pthread_mutex_lock(&philo->meal_lock);
	philo->meal_count++;
	pthread_mutex_unlock(&philo->meal_lock);
	release_fork(philo);
	print_status(philo, "is sleeping");
	ft_sleep(philo, philo->time_to_sleep);
	print_status(philo, "is thinking");
	// if (philo->time_to_eat > philo->time_to_sleep)
	// 	ft_sleep(philo, philo->time_to_eat - philo->time_to_sleep);
	time_till_death = get_time() - philo->last_meal;
	if (time_till_death < philo->time_to_die * 0.7)
			ft_sleep(philo,1);
}

void	*philo_routine(void *arg)
{
	t_philosophers	*philo;

	philo = (t_philosophers *)arg;
	if (philo->number_of_philosopher == 1)
	{
		handle_single_philo(philo);
		return (NULL);
	}
	if (philo->id % 2 == 0)
		ft_sleep(philo, 50);
	while (!is_simulation_stopped(philo->program))
	{
		take_fork(philo);
		if (is_simulation_stopped(philo->program))
		{
			release_fork(philo);
			break ;
		}
		eat_sleep_think(philo);
	}
	return (NULL);
}

bool	check_philo_death(t_program *program, int i, size_t current_time)
{
	pthread_mutex_lock(&program->philosophers[i].meal_lock);
	if (current_time - program->philosophers[i].last_meal
		> (size_t)program->philosophers[i].time_to_die)
	{
		pthread_mutex_unlock(&program->philosophers[i].meal_lock);
		pthread_mutex_lock(&program->print);
		pthread_mutex_lock(&program->stop_mutex);
		if (!program->simulation_stop)
		{
			program->simulation_stop = true;
			printf("%zu %d died\n"\
				, current_time, program->philosophers[i].id + 1);
		}
		pthread_mutex_unlock(&program->stop_mutex);
		pthread_mutex_unlock(&program->print);
		return (true);
	}
	pthread_mutex_unlock(&program->philosophers[i].meal_lock);
	return (false);
}

void	*monitor_routine(void *arg)
{
	t_program	*program;
	int			i;
	size_t		current_time;

	program = (t_program *)arg;
	while (!is_simulation_stopped(program))
	{
		i = 0;
		while (i < program->philosopher_count)
		{
			current_time = get_time();
			if (check_philo_death(program, i, current_time))
				return (NULL);
			i++;
		}
		if (check_all_ate(program)
			&& program->philosophers[0].number_times_to_eat != -1)
		{
			set_simulation_stopped(program);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}

