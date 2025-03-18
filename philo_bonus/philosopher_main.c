/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_main.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 04:55:18 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/18 04:55:18 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

bool check_arguments(int ac, char **av)
{
	if (ac < 5 || ac > 6)
	{
		printf("Usage: %s number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n", av[0]);
		return false;
	}
	for (int i = 1; i < ac; i++)
	{
		int j = 0;
		while (av[i][j])
		{
			if (av[i][j] < '0' || av[i][j] > '9')
			{
				printf("Error: Arguments must be positive integers.\n");
				return false;
			}
			j++;
		}
		if (atoi(av[i]) <= 0)
		{
			printf("Error: Arguments must be positive integers greater than zero.\n");
			return false;
		}
	}
	return true;
}

int main(int ac, char **av)
{
	if (!check_arguments(ac, av))
		return 0;
	t_program *program = program_init(ac, av);
	if (!program)
		return 0;
	program_start(program);
	program_destroy(program);
	return 0;
}
