/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 04:55:18 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/18 04:55:18 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

char *ft_itoa(int n)
{
	int len = 1;
	int tmp = n;
	while (tmp /= 10)
		len++;
	char *str = malloc(len + 1);
	if (!str)
		return NULL;
	str[len] = '\0';
	if (n == 0)
	{
		str[0] = '0';
		return str;
	}
	while (n > 0)
	{
		str[--len] = (n % 10) + '0';
		n /= 10;
	}
	return str;
}

char *ft_strjoin(char *s1, char *s2)
{
	if (!s1 || !s2)
		return NULL;
	int len1 = strlen(s1);
	int len2 = strlen(s2);
	char *result = malloc(len1 + len2 + 1);
	if (!result)
		return NULL;
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}


long long get_time(t_program *program)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000)) - program->start_time;
}

void ft_sleep(t_philosophers *philosopher, int time_ms)
{
	long long start = get_time(philosopher->program);
	while (!is_simulation_stopped(philosopher))
	{
		long long current = get_time(philosopher->program);
		if (current - start >= time_ms)
			break;
		usleep(500);
	}
}

void print_status(t_philosophers *philosopher, char *status)
{
	if (!is_simulation_stopped(philosopher))
	{
		sem_wait(philosopher->program->print_sem);
		printf("%lld %d %s\n", get_time(philosopher->program), philosopher->id + 1, status);
		sem_post(philosopher->program->print_sem);
	}
}
