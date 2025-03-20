/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_libft_bonus.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouhia <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 01:28:13 by mbouhia           #+#    #+#             */
/*   Updated: 2025/03/20 02:59:28 by mbouhia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher_bonus.h"

static size_t	ft_numlen(int n)
{
	long	nb;
	int		sign;
	size_t	len;

	nb = n;
	sign = (n < 0);
	len = 0;
	if (nb == 0)
		return (1);
	if (sign)
		nb = -nb;
	while (nb)
	{
		len++;
		nb /= 10;
	}
	return (len + sign);
}

char	*ft_itoa(int n)
{
	size_t	len;
	long	nb;
	int		sign;
	char	*number;

	len = ft_numlen(n);
	sign = (n < 0);
	number = (char *)malloc((len + 1) * sizeof(char));
	if (!number)
		return (NULL);
	nb = n;
	if (sign)
	{
		nb = -nb;
		number[0] = '-';
	}
	number[len] = '\0';
	while (len > (size_t)sign)
	{
		number[--len] = (nb % 10) + '0';
		nb /= 10;
	}
	return (number);
}

void	ft_strcpy(char *dst, char *src)
{
	while (*src)
	{
		*dst = *src;
		dst++;
		src++;
	}
	*dst = '\0';
}

void	ft_strcat(char *dst, char *src)
{
	while (*dst)
		dst++;
	while (*src)
	{
		*dst = *src;
		dst++;
		src++;
	}
	*dst = '\0';
}

char	*ft_strjoin(char *s1, char *s2)
{
	int		len1;
	int		len2;
	char	*result;

	if (!s1 || !s2)
		return (NULL);
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	result = malloc(len1 + len2 + 1);
	if (!result)
		return (NULL);
	ft_strcpy(result, s1);
	ft_strcat(result, s2);
	return (result);
}
