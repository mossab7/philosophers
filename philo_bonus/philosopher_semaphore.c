/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_semaphore.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 04:55:18 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/18 04:55:18 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

sem_t *open_sem(char *name, int oflag, mode_t mode, unsigned int value)
{
	sem_unlink(name);
	sem_t *sem = sem_open(name, oflag, mode, value);
	if (sem == SEM_FAILED)
	{
		printf("Error: Failed to create %s semaphore.\n", name);
		return NULL;
	}
	return sem;
}
