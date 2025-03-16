#include "philosopher.h"

bool check_arguments(int ac, char **av)
{
    int i;
    int j;

    if (ac < 5 || ac > 6)
    {
        printf("Usage: %s number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n", av[0]);
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

t_philosophers *philosopher_init(t_program *program)
{
    t_philosophers *philosopher;
    int i;
    sem_t *forks_sem;
    sem_t *print_sem;
    sem_t *eat_sem;
    sem_t *death_sem;

    sem_unlink("forks_sem");
    sem_unlink("print_sem");
    sem_unlink("eat_sem");
    sem_unlink("stop_sem");
    
    philosopher = malloc(sizeof(t_philosophers) * program->number_of_philosophers);
    if (!philosopher)
    {
        printf("Error: Failed to allocate memory for philosopher.\n");
        return (0);
    }
    
    forks_sem = sem_open("forks_sem", O_CREAT, 0644, program->number_of_philosophers);
    if (forks_sem == SEM_FAILED)
    {
        printf("Error: Failed to create forks semaphore.\n");
        free(philosopher);
        return (0);
    }
    
    print_sem = sem_open("print_sem", O_CREAT, 0644, 1);
    if (print_sem == SEM_FAILED)
    {
        printf("Error: Failed to create print semaphore.\n");
        free(philosopher);
        sem_close(forks_sem);
        sem_unlink("forks_sem");
        return (0);
    }
    
    eat_sem = sem_open("eat_sem", O_CREAT, 0644, 1);
    if (eat_sem == SEM_FAILED)
    {
        printf("Error: Failed to create eat semaphore.\n");
        free(philosopher);
        sem_close(forks_sem);
        sem_close(print_sem);
        sem_unlink("forks_sem");
        sem_unlink("print_sem");
        return (0);
    }
    
    death_sem = sem_open("stop_sem", O_CREAT, 0644, 1);
    if (death_sem == SEM_FAILED)
    {
        printf("Error: Failed to create stop semaphore.\n");
        free(philosopher);
        sem_close(forks_sem);
        sem_close(print_sem);
        sem_close(eat_sem);
        sem_unlink("forks_sem");
        sem_unlink("print_sem");
        sem_unlink("eat_sem");
        return (0);
    }
    
    i = 0;
    while (i < program->number_of_philosophers)
    {
        philosopher[i].id = i;
        philosopher[i].program = program;
        philosopher[i].forks_sem = forks_sem;
        philosopher[i].print_sem = print_sem;
        philosopher[i].eat_sem = eat_sem;
        philosopher[i].death_sem = death_sem;
        philosopher[i].meal_count = 0;
        philosopher[i].last_meal = get_time(program); 
        i++;
    }
    return (philosopher);
}

t_program *program_init(int ac, char **av)
{
    t_program *program;

    program = malloc(sizeof(t_program));
    if (!program)
    {
        printf("Error: Failed to allocate memory for program.\n");
        return (0);
    }
    
    program->number_of_philosophers = atoi(av[1]);
    program->time_to_die = atoi(av[2]);
    program->time_to_eat = atoi(av[3]);
    program->time_to_sleep = atoi(av[4]);
    
    if (ac == 6)
        program->number_of_times_each_philosopher_must_eat = atoi(av[5]);
    else
        program->number_of_times_each_philosopher_must_eat = -1;
    
    sem_unlink("stop_sem");
    program->stop_sem = sem_open("stop_sem", O_CREAT, 0644, 0);
    if (program->stop_sem == SEM_FAILED)
    {
        printf("Error: Failed to create death semaphore.\n");
        free(program);
        return (0);
    }
    
    program->philosophers = philosopher_init(program);
    if (!program->philosophers)
    {
        printf("Error: Failed to initialize philosophers.\n");
        sem_close(program->stop_sem);
        sem_unlink("stop_sem");
        free(program);
        return (NULL);
    }
    
    program->simulation_stopped = 0;
    return (program);
}

void release_fork(t_philosophers *philosopher)
{
    sem_post(philosopher->forks_sem);
    sem_post(philosopher->forks_sem);
}

void take_fork(t_philosophers *philosopher)
{
    sem_wait(philosopher->forks_sem);
    print_status(philosopher, "has taken a fork");
    sem_wait(philosopher->forks_sem);
    print_status(philosopher, "has taken a fork");
}

void handle_single_philosopher(t_philosophers *philosopher)
{
    print_status(philosopher, "has taken a fork");
    ft_sleep(philosopher, philosopher->program->time_to_die);
    print_status(philosopher, "died");
}

int is_simulation_stopped(t_program *program)
{
    int result;
    
    sem_wait(program->philosophers[0].death_sem);
    result = program->simulation_stopped;
    sem_post(program->philosophers[0].death_sem);
    return result;
}

void print_status(t_philosophers *philosopher, char *status)
{
    if (!is_simulation_stopped(philosopher->program))
    {
        sem_wait(philosopher->print_sem);
        printf("%lld %d %s\n", get_time(philosopher->program), philosopher->id + 1, status);
        sem_post(philosopher->print_sem);
    }
}

long long get_time(t_program *program)
{
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000)) - program->start_time;
}

void ft_sleep(t_philosophers *philosopher, int time_ms)
{
    long long start;
    long long current;
    
    start = get_time(philosopher->program);
    while (!is_simulation_stopped(philosopher->program))
    {
        current = get_time(philosopher->program);
        if (current - start >= time_ms)
            break;
        usleep(500);
    }
}

void eat_sleep_think(t_philosophers *philosopher)
{
    sem_wait(philosopher->eat_sem);
    philosopher->last_meal = get_time(philosopher->program);
    print_status(philosopher, "is eating");
    sem_post(philosopher->eat_sem);
    
    ft_sleep(philosopher, philosopher->program->time_to_eat);
    
    sem_wait(philosopher->eat_sem);
    philosopher->meal_count++;
    sem_post(philosopher->eat_sem);
    
    release_fork(philosopher);
    
    print_status(philosopher, "is sleeping");
    ft_sleep(philosopher, philosopher->program->time_to_sleep);
    
    print_status(philosopher, "is thinking");  
}

void cleanup_process(t_philosophers *philosopher)
{
    sem_close(philosopher->forks_sem);
    sem_close(philosopher->print_sem);
    sem_close(philosopher->eat_sem);
    sem_close(philosopher->death_sem);
    sem_close(philosopher->program->stop_sem);
    
}

void *philosopher_routine(void *arg)
{
    t_philosophers *philosopher;

    philosopher = (t_philosophers *)arg;
    if (philosopher->program->number_of_philosophers == 1)
    {
        handle_single_philosopher(philosopher);
        return NULL;
    }
    
    if (philosopher->id % 2 == 0)
        ft_sleep(philosopher, 50);
        
    while (!is_simulation_stopped(philosopher->program))
    {
        take_fork(philosopher);
        if (is_simulation_stopped(philosopher->program))
        {
            release_fork(philosopher);
            break;
        }
        eat_sleep_think(philosopher);
        
        if (philosopher->program->number_of_times_each_philosopher_must_eat != -1 && 
            philosopher->meal_count >= philosopher->program->number_of_times_each_philosopher_must_eat)
            break;
    }
    return NULL;
}

void *monitor_routine(void *arg)
{
    t_philosophers *philosopher;

    philosopher = (t_philosophers *)arg;
    while (!is_simulation_stopped(philosopher->program))
    {
        sem_wait(philosopher->eat_sem);
        if (get_time(philosopher->program) - philosopher->last_meal > philosopher->program->time_to_die &&
            (philosopher->program->number_of_times_each_philosopher_must_eat == -1 ||
             philosopher->meal_count < philosopher->program->number_of_times_each_philosopher_must_eat))
        {
            sem_post(philosopher->eat_sem);
            sem_wait(philosopher->death_sem);
            print_status(philosopher, "died");
            philosopher->program->simulation_stopped = 1;
            sem_post(philosopher->death_sem);
            
            for (int i = 0; i < philosopher->program->number_of_philosophers; i++)
                sem_post(philosopher->program->stop_sem);
                
            return NULL;
        }
        sem_post(philosopher->eat_sem);
        usleep(1000);
    }
    return NULL;
}

void *death_listener_routine(void *arg)
{
    t_philosophers *philosopher = (t_philosophers *)arg;
    
    sem_wait(philosopher->program->stop_sem);  
    
    cleanup_process(philosopher);
    
    pthread_exit(NULL);
    return NULL;
}

void philosopher_start(t_philosophers *philosopher)
{
    pthread_t philosopher_thread;
    pthread_t monitor_thread;
    pthread_t death_listener_thread;

    philosopher->last_meal = get_time(philosopher->program);
    philosopher->meal_count = 0;
    
    if (pthread_create(&death_listener_thread, NULL, death_listener_routine, philosopher) != 0)
    {
        printf("Error: Failed to create death listener thread.\n");
        return;
    }
    
    if (pthread_create(&monitor_thread, NULL, monitor_routine, philosopher) != 0)
    {
        printf("Error: Failed to create monitor thread.\n");
        pthread_cancel(death_listener_thread);
        pthread_join(death_listener_thread, NULL);
        return;
    }
    
    if (pthread_create(&philosopher_thread, NULL, philosopher_routine, philosopher) != 0)
    {
        printf("Error: Failed to create philosopher thread.\n");
        pthread_cancel(monitor_thread);
        pthread_cancel(death_listener_thread);
        pthread_join(monitor_thread, NULL);
        pthread_join(death_listener_thread, NULL);
        return;
    }
    
    pthread_join(philosopher_thread, NULL);
    
    pthread_cancel(monitor_thread);
    pthread_join(monitor_thread, NULL);
    
    if (!is_simulation_stopped(philosopher->program))
    {
        cleanup_process(philosopher);
        sem_post(philosopher->program->stop_sem);
    }
    
    pthread_join(death_listener_thread, NULL);
    
    exit(0);
}

void program_start(t_program *program)
{
    int i;
    pid_t pid;
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    program->start_time = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    
    i = 0;
    while (i < program->number_of_philosophers)
    {
        pid = fork();
        if (pid == -1)
        {
            printf("Error: Failed to fork.\n");
            return;
        }
        if (pid == 0)
        {
            philosopher_start(&program->philosophers[i]);
        }
        i++;
    }
    
    i = 0;
    while (i < program->number_of_philosophers)
    {
        waitpid(-1, NULL, 0);
        i++;
    }
}

void program_destroy(t_program *program)
{
    sem_close(program->philosophers[0].forks_sem);
    sem_close(program->philosophers[0].print_sem);
    sem_close(program->philosophers[0].eat_sem);
    sem_close(program->philosophers[0].death_sem);
    sem_close(program->stop_sem);
    
    sem_unlink("forks_sem");
    sem_unlink("print_sem");
    sem_unlink("eat_sem");
    sem_unlink("stop_sem");
    
    free(program->philosophers);
    free(program);
}

int main(int ac, char **av)
{
    t_program *program;

    if (!check_arguments(ac, av))
        return (0);
        
    program = program_init(ac, av);
    if (!program)
        return (0);
        
    program_start(program);
    program_destroy(program);
    
    return (0);
}