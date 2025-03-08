#include "philosophers_bonus.h"

size_t	ft_strlen(char const *s)
{
	int	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	s1l;
	size_t	s2l;
	char	*new;
	int		iter;

	if (!s1 || !s2)
		return (NULL);
	s1l = ft_strlen(s1);
	s2l = ft_strlen(s2);
	iter = 0;
	new = malloc(s1l + s2l + 1);
	if (!new)
		return (NULL);
	while (s1[iter])
	{
		new[iter] = s1[iter];
		iter++;
	}
	while (s2[iter - s1l])
	{
		new[iter] = s2[iter - s1l];
		iter++;
	}
	new[iter] = 0;
	return (new);
}
