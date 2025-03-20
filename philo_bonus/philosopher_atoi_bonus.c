/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_atoi_bonus.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 13:33:26 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/20 02:59:28 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher_bonus.h"

static int	ft_isdigit_(char c)
{
	return (c >= '0' && c <= '9');
}

size_t	ft_strlen(const char *s)
{
	size_t	len;

	len = 0;
	while (s[len])
		len++;
	return (len);
}

static int	ft_derive_sign(char c, int *index)
{
	if (c == '-')
	{
		*index = *index + 1;
		return (-1);
	}
	if (c == '+')
		*index = *index + 1;
	return (1);
}

static int	fetch_digit(unsigned long *num, char digit, int *index, int sign)
{
	if (ft_isdigit_(digit))
	{
		*num = (*num * 10) + (digit - '0');
		if ((*num * 10 + digit - '0') >= 9223372036854775807)
		{
			if (sign == 1)
				*num = -1;
			else
				*num = 0;
			return (0);
		}
		*index = (*index + 1);
		return (1);
	}
	*num = (sign * (*num));
	return (0);
}

int	ft_atoi(const char *str)
{
	unsigned long	num;
	int				index;
	int				sign;

	sign = 1;
	index = 0;
	while (str[index] == ' ' || (str[index] >= 9 && str[index] <= 13))
		index++;
	sign = ft_derive_sign(str[index], &index);
	num = 0;
	while (fetch_digit(&num, str[index], &index, sign))
		continue ;
	return (num);
}
