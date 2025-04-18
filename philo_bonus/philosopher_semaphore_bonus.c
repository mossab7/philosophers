/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_semaphore_bonus.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 04:55:18 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/20 02:59:29 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher_bonus.h"

sem_t	*open_sem(char *name, int oflag, mode_t mode, unsigned int value)
{
	sem_t	*sem;

	sem_unlink(name);
	sem = sem_open(name, oflag, mode, value);
	if (sem == SEM_FAILED)
	{
		printf("Error: Failed to create %s semaphore.\n", name);
		return (NULL);
	}
	return (sem);
}
