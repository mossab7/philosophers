#include "philosopher.h"

int main(int ac,char **av)
{
	if(!check_arguments(ac,av))
	{
		printf("\n");
		return (0);
	}
}
