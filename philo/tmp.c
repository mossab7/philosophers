#include "tmp.h"
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

size_t get_time(void)
{
    struct timeval curtime;
    if (gettimeofday(&curtime, NULL) != 0)
    {
        perror("gettimeofday failed");
        return 0;
    }
    return (curtime.tv_sec * 1000 + curtime.tv_usec / 1000);
}

void ft_sleep(t_philosophers *philosopher, size_t milliseconds)
{
    size_t start = get_time();
    while (1)
    {
        pthread_mutex_lock(&philosopher->program->stop_mutex);
        bool is_stopped = philosopher->program->simulation_stop;
        pthread_mutex_unlock(&philosopher->program->stop_mutex);
        
        if (is_stopped)
            break;
        
        size_t current = get_time();
        if (current - start >= milliseconds)
            break;
        
        usleep(50);
    }
}

bool is_simulation_stopped(t_program *program)
{
    pthread_mutex_lock(&program->stop_mutex);
    bool is_stopped = program->simulation_stop;
    pthread_mutex_unlock(&program->stop_mutex);
    return is_stopped;
}

void set_simulation_stopped(t_program *program)
{
    pthread_mutex_lock(&program->stop_mutex);
    program->simulation_stop = true;
    pthread_mutex_unlock(&program->stop_mutex);
}

void print_status(t_philosophers *philosopher, char *status)
{
    pthread_mutex_lock(&philosopher->program->print);
    
    pthread_mutex_lock(&philosopher->program->stop_mutex);
    bool is_stopped = philosopher->program->simulation_stop;
    pthread_mutex_unlock(&philosopher->program->stop_mutex);
    
    size_t time = get_time();
    if (!is_stopped)
        printf("%zu %d %s\n", time, philosopher->id + 1, status);
    
    pthread_mutex_unlock(&philosopher->program->print);
}

void error_exit(char *message)
{
    fprintf(stderr, "Error: %s\n", message);
    exit(EXIT_FAILURE);
}

void take_fork(t_philosophers *philosopher)
{
    if (is_simulation_stopped(philosopher->program))
        return;

    if (philosopher->id % 2 == 0)
    {
        pthread_mutex_lock(philosopher->right_fork);
        print_status(philosopher, "has taken a fork");
        pthread_mutex_lock(philosopher->left_fork);
        print_status(philosopher, "has taken a fork");
    }
    else
    {
        pthread_mutex_lock(philosopher->left_fork);
        print_status(philosopher, "has taken a fork");
        pthread_mutex_lock(philosopher->right_fork);
        print_status(philosopher, "has taken a fork");
    }
}

void release_fork(t_philosophers *philosopher)
{
    pthread_mutex_unlock(philosopher->left_fork);
    pthread_mutex_unlock(philosopher->right_fork);
}

void *philo_routine(void *arg)
{
    t_philosophers *philosopher = (t_philosophers *)arg;

    if (philosopher->number_of_philosopher == 1)
    {
        print_status(philosopher, "has taken a fork");
        ft_sleep(philosopher, philosopher->time_to_die);
        return NULL;
    }

    if (philosopher->id % 2 == 0)
        ft_sleep(philosopher, 50);

    while (1)
    {
        if (is_simulation_stopped(philosopher->program))
            break;

        take_fork(philosopher);
        
        if (is_simulation_stopped(philosopher->program))
        {
            release_fork(philosopher);
            break;
        }
        
        pthread_mutex_lock(&philosopher->meal_lock);
        philosopher->last_meal = get_time();
        print_status(philosopher, "is eating");
        pthread_mutex_unlock(&philosopher->meal_lock);
        
        ft_sleep(philosopher, philosopher->time_to_eat);
        
        pthread_mutex_lock(&philosopher->meal_lock);
        philosopher->meal_count++;
        pthread_mutex_unlock(&philosopher->meal_lock);
        
        release_fork(philosopher);
        
        if (is_simulation_stopped(philosopher->program))
            break;
        
        print_status(philosopher, "is sleeping");
        ft_sleep(philosopher, philosopher->time_to_sleep);
        
        if (is_simulation_stopped(philosopher->program))
            break;
        
        print_status(philosopher, "is thinking");
        
        if (philosopher->time_to_eat > philosopher->time_to_sleep)
            ft_sleep(philosopher, philosopher->time_to_eat - philosopher->time_to_sleep);
    }
    return NULL;
}

void *monitor_routine(void *arg)
{
    t_program *program = (t_program *)arg;
    int i;
    bool all_ate;
    size_t current_time;

    while (1)
    {
        i = 0;
        all_ate = true;
        
        while (i < program->philosopher_count)
        {
            pthread_mutex_lock(&program->philosophers[i].meal_lock);
            current_time = get_time();
            
            if (current_time - program->philosophers[i].last_meal > (size_t)program->philosophers[i].time_to_die)
            {
                pthread_mutex_unlock(&program->philosophers[i].meal_lock);
                
                pthread_mutex_lock(&program->print);
                pthread_mutex_lock(&program->stop_mutex);
                
                if (!program->simulation_stop)
                {
                    program->simulation_stop = true;
                    printf("%zu %d died\n", current_time, program->philosophers[i].id + 1);
                }
                
                pthread_mutex_unlock(&program->stop_mutex);
                pthread_mutex_unlock(&program->print);
                
                return NULL;
            }
            
            if (program->philosophers[i].number_times_to_eat != -1 &&
                program->philosophers[i].meal_count < program->philosophers[i].number_times_to_eat)
            {
                all_ate = false;
            }
            
            pthread_mutex_unlock(&program->philosophers[i].meal_lock);
            i++;
        }
        
        if (all_ate && program->philosophers[0].number_times_to_eat != -1)
        {
            pthread_mutex_lock(&program->print);
            pthread_mutex_lock(&program->stop_mutex);
            program->simulation_stop = true;
            pthread_mutex_unlock(&program->stop_mutex);
            pthread_mutex_unlock(&program->print);
            return NULL;
        }
        
        usleep(1000);
    }
    return NULL;
}

void program_init(int ac, char **args, t_program *program)
{
    int count = atoi(args[1]);
    program->philosopher_count = count;
    program->all_ate_enough = false;
    program->simulation_stop = false;
    program->forks = malloc(count * sizeof(pthread_mutex_t));
    
    if (!program->forks)
        error_exit("malloc failed");
    
    pthread_mutex_init(&program->stop_mutex, NULL);
    
    for (int i = 0; i < count; i++)
    {
        pthread_mutex_init(&program->forks[i], NULL);
    }
    
    for (int i = 0; i < count; i++)
    {
        program->philosophers[i].id = i;
        program->philosophers[i].time_to_die = atoi(args[2]);
        program->philosophers[i].time_to_eat = atoi(args[3]);
        program->philosophers[i].time_to_sleep = atoi(args[4]);
        program->philosophers[i].number_of_philosopher = count;
        program->philosophers[i].meal_count = 0;
        program->philosophers[i].last_meal = get_time();
        program->philosophers[i].program = program;
        program->philosophers[i].left_fork = &program->forks[i];
        program->philosophers[i].right_fork = &program->forks[(i + 1) % count];
        
        pthread_mutex_init(&program->philosophers[i].meal_lock, NULL);
        
        if (ac == 6)
            program->philosophers[i].number_times_to_eat = atoi(args[5]);
        else
            program->philosophers[i].number_times_to_eat = -1;
    }
    
    pthread_mutex_init(&program->print, NULL);
}

void start_simulation(t_program *program)
{
    pthread_t monitor;
    
    if (pthread_create(&monitor, NULL, monitor_routine, program) != 0)
        error_exit("pthread_create failed");
    
    for (int i = 0; i < program->philosopher_count; i++)
    {
        if (pthread_create(&program->philosophers[i].thread, NULL, philo_routine, &program->philosophers[i]) != 0)
            error_exit("pthread_create failed");
    }
    
    pthread_join(monitor, NULL);
    
    for (int i = 0; i < program->philosopher_count; i++)
    {
        pthread_join(program->philosophers[i].thread, NULL);
    }
}

int main(int ac, char **av)
{
    t_program *program;
    
    if (ac < 5 || ac > 6)
    {
        printf("Usage: %s number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n", av[0]);
        return EXIT_FAILURE;
    }
    
    program = malloc(sizeof(t_program));
    if (!program)
        error_exit("malloc failed");
    
    program->philosophers = malloc(atoi(av[1]) * sizeof(t_philosophers));
    if (!program->philosophers)
    {
        free(program);
        error_exit("malloc failed");
    }
    
    program_init(ac, av, program);
    start_simulation(program);
    
    return EXIT_SUCCESS;
}