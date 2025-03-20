/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_flow_control_bonus.c                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 04:55:18 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/20 02:59:28 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher_bonus.h"

bool	is_simulation_stopped(t_philosophers *philosopher)
{
	bool	stopped;

	sem_wait(philosopher->stop_sem);
	stopped = philosopher->simulation_stopped;
	sem_post(philosopher->stop_sem);
	return (stopped);
}

void	set_simulation_stopped(t_philosophers *philosopher)
{
	sem_wait(philosopher->stop_sem);
	philosopher->simulation_stopped = true;
	sem_post(philosopher->stop_sem);
}
