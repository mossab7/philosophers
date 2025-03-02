#include "philosophers.h"

bool	check_arguments(int ac, char **av)
{
	int	i;
	int	j;

	if (ac < 5 || ac > 6)
	{
		printf("Usage:\
			%s number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n",
			av[0]);
		return (false);
	}
	i = 1;
	while (i < ac)
	{
		j = 0;
		while (av[i][j])
		{
			if (av[i][j] < '0' || av[i][j] > '9')
			{
				printf("Error: Arguments must be positive integers.\n");
				return (false);
			}
			j++;
		}
		i++;
	}
	return (true);
}

int main(int ac,char **av)
{
	if (!check_arguments(ac, av))
	return (EXIT_FAILURE);
}
