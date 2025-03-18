/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_forks.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 04:55:18 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/18 04:55:18 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

void release_fork(t_philosophers *philosopher)
{
	sem_post(philosopher->program->forks_sem);
	sem_post(philosopher->program->forks_sem);
}

void take_fork(t_philosophers *philosopher)
{
	sem_wait(philosopher->program->forks_sem);
	print_status(philosopher, "has taken a fork");
	sem_wait(philosopher->program->forks_sem);
	print_status(philosopher, "has taken a fork");
}
