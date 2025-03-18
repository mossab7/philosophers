#include "philosopher.h"
#include <signal.h>
#include <sys/wait.h>

char *ft_itoa(int n)
{
    int len = 1;
    int is_neg = 0;
    int tmp = n;
    
    if (n < 0)
    {
        is_neg = 1;
        len++;
        tmp = -tmp;
    }
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
    
    if (is_neg)
    {
        str[0] = '-';
        n = -n;
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

void cleanup_semaphores(t_program *program) 
{
    sem_unlink("forks_sem");
    sem_unlink("print_sem");
    sem_unlink("death_sem");
    sem_unlink("philo_full_sem");
    for (int i = 0; i < program->number_of_philosophers; i++) 
    {
        char *id = ft_itoa(i);
        char *stop_name = ft_strjoin("stop_sem_", id);
        char *meal_name = ft_strjoin("meal_sem_", id);
        sem_unlink(stop_name);
        sem_unlink(meal_name);
        free(id);
        free(stop_name);
        free(meal_name);
    }
}

t_philosophers *philosopher_init(t_program *program)
{
    t_philosophers *philosopher = malloc(sizeof(t_philosophers) * program->number_of_philosophers);
    if (!philosopher)
    {
        printf("Error: Failed to allocate memory for philosophers.\n");
        return NULL;
    }
    char *name;
    char *id;
    for (int i = 0; i < program->number_of_philosophers; i++)
    {
        philosopher[i].id = i;
        philosopher[i].program = program;
        id = ft_itoa(i);
        name = ft_strjoin("stop_sem_", id);
        philosopher[i].stop_sem = open_sem(name, O_CREAT, 0644, 1);
        free(name);
        free(id);
        id = ft_itoa(i);
        name = ft_strjoin("meal_sem_", id);
        philosopher[i].meal_sem = open_sem(name, O_CREAT, 0644, 1);
        free(name);
        free(id);
        philosopher[i].meal_count = 0;
        philosopher[i].last_meal = 0;
        philosopher[i].simulation_stopped = false;
    }
    return philosopher;
}

t_program *program_init(int ac, char **av)
{
    t_program *program = malloc(sizeof(t_program));
    if (!program)
    {
        printf("Error: Failed to allocate memory for program.\n");
        return NULL;
    }
    program->number_of_philosophers = atoi(av[1]);
    program->time_to_die = atoi(av[2]);
    program->time_to_eat = atoi(av[3]);
    program->time_to_sleep = atoi(av[4]);
    program->number_of_times_each_philosopher_must_eat = (ac == 6) ? atoi(av[5]) : -1;
    program->forks_sem = open_sem("forks_sem", O_CREAT, 0644, program->number_of_philosophers);
    program->print_sem = open_sem("print_sem", O_CREAT, 0644, 1);
    program->death_sem = open_sem("death_sem", O_CREAT, 0644, 0);
    if (program->number_of_times_each_philosopher_must_eat != -1)
    {
        program->philo_full_sem = open_sem("philo_full_sem", O_CREAT, 0644, 0);
        if (program->philo_full_sem == SEM_FAILED)
        {
            printf("Error: Failed to create philo_full semaphore.\n");
            sem_close(program->forks_sem);
            sem_close(program->print_sem);
            sem_close(program->death_sem);
            cleanup_semaphores(program);
            free(program);
            return NULL;
        }
    }
    else
    {
        program->philo_full_sem = NULL;
    }
    program->pids = malloc(program->number_of_philosophers * sizeof(pid_t));
    if (!program->pids)
    {
        printf("Error: Failed to allocate memory for process IDs.\n");
        sem_close(program->forks_sem);
        sem_close(program->print_sem);
        sem_close(program->death_sem);
        if (program->philo_full_sem)
            sem_close(program->philo_full_sem);
        free(program);
        return NULL;
    }
    program->philosophers = philosopher_init(program);
    if (!program->philosophers)
    {
        printf("Error: Failed to initialize philosophers.\n");
        sem_close(program->forks_sem);
        sem_close(program->print_sem);
        sem_close(program->death_sem);
        if (program->philo_full_sem)
            sem_close(program->philo_full_sem);
        free(program->pids);
        free(program);
        return NULL;
    }
    return program;
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

void release_fork(t_philosophers *philosopher)
{
    sem_post(philosopher->program->forks_sem);
    sem_post(philosopher->program->forks_sem);
}

void take_fork(t_philosophers *philosopher)
{
    sem_wait(philosopher->program->forks_sem);
    print_status(philosopher, "has taken a fork");
    sem_wait(philosopher->program->forks_sem);
    print_status(philosopher, "has taken a fork");
}

bool is_simulation_stopped(t_philosophers *philosopher)
{
    sem_wait(philosopher->stop_sem);
    bool stopped = philosopher->simulation_stopped;
    sem_post(philosopher->stop_sem);
    return stopped;
}

void set_simulation_stopped(t_philosophers *philosopher)
{
    sem_wait(philosopher->stop_sem);
    philosopher->simulation_stopped = true;
    sem_post(philosopher->stop_sem);
}

void handle_single_philosopher(t_philosophers *philosopher)
{
    print_status(philosopher, "has taken a fork");
    ft_sleep(philosopher, philosopher->program->time_to_die);
    print_status(philosopher, "died");
    set_simulation_stopped(philosopher);
}

void eat_sleep_think(t_philosophers *philosopher)
{
    sem_wait(philosopher->meal_sem);
    philosopher->last_meal = get_time(philosopher->program);
    sem_post(philosopher->meal_sem);
    print_status(philosopher, "is eating");
    ft_sleep(philosopher, philosopher->program->time_to_eat);
    sem_wait(philosopher->meal_sem);
    philosopher->meal_count++;
    sem_post(philosopher->meal_sem);
    release_fork(philosopher);
    print_status(philosopher, "is sleeping");
    ft_sleep(philosopher, philosopher->program->time_to_sleep);
    print_status(philosopher, "is thinking");
    long long time_till_death = philosopher->program->time_to_die - (get_time(philosopher->program) - philosopher->last_meal);
    ft_sleep(philosopher, (time_till_death > 0) ? (time_till_death * 0.7) : 0);
}

void *philosopher_routine(void *arg)
{
    t_philosophers *philosopher = (t_philosophers *)arg;
    if (philosopher->program->number_of_philosophers == 1)
    {
        handle_single_philosopher(philosopher);
        return NULL;
    }
    if (philosopher->id % 2 == 0)
        ft_sleep(philosopher, philosopher->program->time_to_eat / 2);
    while (!is_simulation_stopped(philosopher))
    {
        take_fork(philosopher);
        if (is_simulation_stopped(philosopher))
        {
            release_fork(philosopher);
            break;
        }
        eat_sleep_think(philosopher);
    }
    return NULL;
}

bool check_philo_death(t_philosophers *philosopher)
{
    long long current_time = get_time(philosopher->program);
    sem_wait(philosopher->meal_sem);
    bool is_dead = (current_time - philosopher->last_meal) > philosopher->program->time_to_die;
    sem_post(philosopher->meal_sem);
    return is_dead;
}

void report_philo_death(t_philosophers *philosopher)
{
    print_status(philosopher, "died");
    signal_death(philosopher->program);
    set_simulation_stopped(philosopher);
}

bool check_eat_enough(t_philosophers *philosopher)
{
    if (philosopher->program->number_of_times_each_philosopher_must_eat == -1)
        return false;
    sem_wait(philosopher->meal_sem);
    bool enough = philosopher->meal_count >= philosopher->program->number_of_times_each_philosopher_must_eat;
    sem_post(philosopher->meal_sem);
    return enough;
}

bool check_all_ate_enough(t_program *program)
{
    int philo_full_count = 0;
    while (philo_full_count < program->number_of_philosophers)
    {
        sem_wait(program->philo_full_sem);
        philo_full_count++;
    }
    return true;
}

void signal_death(t_program *program)
{
    for (int i = 0; i < program->number_of_philosophers; i++)
        sem_post(program->death_sem);
}

void meals_monitor(t_program *program)
{
    check_all_ate_enough(program);
    signal_death(program);
}

void *monitor_routine(void *arg)
{
    t_philosophers *philosopher = (t_philosophers *)arg;
    while (!is_simulation_stopped(philosopher))
    {
        if (check_philo_death(philosopher))
        {
            report_philo_death(philosopher);
            return NULL;
        }
        if (check_eat_enough(philosopher))
        {
            sem_post(philosopher->program->philo_full_sem);
            set_simulation_stopped(philosopher);
            return NULL;
        }
        usleep(1000);
    }
    return NULL;
}

void cleanup_process(t_philosophers *philosopher)
{
    char *name;
    char *id;

    sem_close(philosopher->program->forks_sem);
    sem_close(philosopher->program->print_sem);
    sem_close(philosopher->program->death_sem);
    if (philosopher->program->philo_full_sem)
        sem_close(philosopher->program->philo_full_sem);
    sem_close(philosopher->meal_sem);
    sem_close(philosopher->stop_sem);
    id = ft_itoa(philosopher->id);
    name = ft_strjoin("stop_sem_", id);
    sem_unlink(name);
    free(name);
    name = ft_strjoin("meal_sem_", id);
    sem_unlink(name);
    free(name);
    free(id); 
}

void *death_listener_routine(void *arg)
{
    t_philosophers *philosopher = (t_philosophers *)arg;
    sem_wait(philosopher->program->death_sem);
    set_simulation_stopped(philosopher);
    return (NULL);
}

void philosopher_start(t_philosophers *philosopher)
{
    pthread_t philosopher_thread, monitor_thread, death_listener_thread;
    philosopher->last_meal = get_time(philosopher->program);
    if (pthread_create(&monitor_thread, NULL, monitor_routine, philosopher) != 0 ||
    pthread_create(&philosopher_thread, NULL, philosopher_routine, philosopher) != 0 ||
    pthread_create(&death_listener_thread, NULL, death_listener_routine, philosopher) != 0)
    {
        printf("Error: Failed to create thread.\n");
        cleanup_process(philosopher);
        exit(1);
    }
    pthread_join(philosopher_thread, NULL);
    pthread_join(monitor_thread, NULL);
    pthread_join(death_listener_thread, NULL);
    cleanup_process(philosopher);
    exit(0);
}

void program_start(t_program *program)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    program->start_time = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    for (int i = 0; i < program->number_of_philosophers; i++)
    {
        program->pids[i] = fork();
        if (program->pids[i] == 0)
        {
            philosopher_start(&program->philosophers[i]);
        }
        else if (program->pids[i] < 0)
        {
            printf("Error: Failed to create child process.\n");
            for (int j = 0; j < i; j++)
                kill(program->pids[j], SIGTERM);
            program_destroy(program);
            exit(1);
        }
    }
    pid_t meals_pid = -1;
    if (program->number_of_times_each_philosopher_must_eat != -1)
    {
        meals_pid = fork();
        if (meals_pid == 0)
        {
            meals_monitor(program);
            exit(0);
        }
        else if (meals_pid < 0)
        {
            printf("Error: Failed to create meals monitor process.\n");
            exit(1);
        }
    }
    for (int i = 0; i < program->number_of_philosophers; i++)
        waitpid(program->pids[i], NULL, 0);
    if(program->number_of_times_each_philosopher_must_eat != -1)
        kill(meals_pid, SIGKILL);
}

void program_destroy(t_program *program) 
{
    sem_close(program->forks_sem);
    sem_close(program->print_sem);
    sem_close(program->death_sem);
    if (program->philo_full_sem)
        sem_close(program->philo_full_sem);
    cleanup_semaphores(program);
    free(program->pids);
    free(program->philosophers);
    free(program);
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