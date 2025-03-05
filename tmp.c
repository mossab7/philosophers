#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_PHILOSOPHERS 200

typedef struct s_philosophers {
    int id;
    int number_of_philosophers;
    int time_to_die;
    int time_to_eat;
    int time_to_sleep;
    int number_times_to_eat;
    
    size_t last_meal;
    int meal_count;
    pid_t pid;

    sem_t *left_fork;
    sem_t *right_fork;
    sem_t *meal_lock;

    sem_t *print_sem;
    sem_t *dead_flag_sem;
    sem_t *terminate_sem;
} t_philosophers;

typedef struct s_program {
    int philosopher_count;
    bool all_ate_enough;
    t_philosophers philosophers[MAX_PHILOSOPHERS];
    sem_t *print_sem;
    sem_t *dead_flag_sem;
    sem_t *terminate_sem;
} t_program;

size_t get_time(void) {
    struct timeval curtime;
    gettimeofday(&curtime, NULL);
    return curtime.tv_sec * 1000 + curtime.tv_usec / 1000;
}

void ft_sleep(size_t milliseconds) {
    size_t start = get_time();
    while (get_time() - start < milliseconds)
        usleep(500);
}

void print_status(t_philosophers *philo, const char *status) {
    sem_wait(philo->print_sem);
    printf("%zu %d %s\n", get_time(), philo->id + 1, status);
    sem_post(philo->print_sem);
}

void error_exit(const char *msg) {
    write(STDERR_FILENO, msg, strlen(msg));
    write(STDERR_FILENO, "\n", 1);
    exit(EXIT_FAILURE);
}

void cleanup_resources(t_philosophers *philo) {
    char sem_name[256];
    snprintf(sem_name, sizeof(sem_name), "/fork_%d", philo->id);
    sem_close(philo->left_fork);
    sem_unlink(sem_name);
    snprintf(sem_name, sizeof(sem_name), "/fork_%d", (philo->id + 1) % philo->number_of_philosophers);
    sem_close(philo->right_fork);
    sem_unlink(sem_name);
    snprintf(sem_name, sizeof(sem_name), "/meal_lock_%d", philo->id);
    sem_close(philo->meal_lock);
    sem_unlink(sem_name);
    sem_close(philo->print_sem);
    sem_close(philo->dead_flag_sem);
    sem_close(philo->terminate_sem);
}

void take_forks(t_philosophers *philo) {
    if (philo->id % 2 == 0) {
        sem_wait(philo->right_fork);
        print_status(philo, "has taken a fork");
        sem_wait(philo->left_fork);
        print_status(philo, "has taken a fork");
    } else {
        sem_wait(philo->left_fork);
        print_status(philo, "has taken a fork");
        sem_wait(philo->right_fork);
        print_status(philo, "has taken a fork");
    }
}

void release_forks(t_philosophers *philo) {
    sem_post(philo->left_fork);
    sem_post(philo->right_fork);
}

void *death_listener(void *arg) 
{
    t_philosophers *philo = (t_philosophers *)arg;
    while (1) {
        sem_wait(philo->meal_lock);
        if (get_time() - philo->last_meal > (size_t)philo->time_to_die) {
            sem_post(philo->dead_flag_sem);
            sem_post(philo->meal_lock);
            return NULL;
        }
        sem_post(philo->meal_lock);
        usleep(1000);
    }
    return NULL;
}

void *philo_routine(void *arg) {
    t_philosophers *philo = (t_philosophers *)arg;
    philo->last_meal = get_time();
    pthread_t death_thread;
    pthread_create(&death_thread, NULL, death_listener, philo);
    pthread_detach(death_thread);

    while (1) 
    {
        take_forks(philo);
        sem_wait(philo->meal_lock);
        philo->last_meal = get_time();
        print_status(philo, "is eating");
        sem_post(philo->meal_lock);
        ft_sleep(philo->time_to_eat);
        sem_wait(philo->meal_lock);
        philo->meal_count++;
        sem_post(philo->meal_lock);
        release_forks(philo);
        print_status(philo, "is sleeping");
        ft_sleep(philo->time_to_sleep);
        print_status(philo, "is thinking");
    }
    return NULL;
}

void *monitor_routine(void *arg) {
    t_program *program = (t_program *)arg;
    sem_wait(program->dead_flag_sem);
    for (int i = 0; i < program->philosopher_count; i++) {
        kill(program->philosophers[i].pid, SIGKILL);
    }
    return NULL;
}

void program_init(int ac, char **av, t_program *program) {
    program->philosopher_count = atoi(av[1]);
    program->all_ate_enough = false;

    for (int i = 0; i < program->philosopher_count; i++) {
        char sem_name[256];
        snprintf(sem_name, sizeof(sem_name), "/fork_%d", i);
        sem_unlink(sem_name);
        sem_t *fork = sem_open(sem_name, O_CREAT, 0644, 1);
        sem_close(fork);
        snprintf(sem_name, sizeof(sem_name), "/meal_lock_%d", i);
        sem_unlink(sem_name);
        sem_t *meal_lock = sem_open(sem_name, O_CREAT, 0644, 1);
        sem_close(meal_lock);
    }

    sem_unlink("/print_sem");
    sem_unlink("/dead_flag_sem");
    sem_unlink("/terminate_sem");
    program->print_sem = sem_open("/print_sem", O_CREAT, 0644, 1);
    program->dead_flag_sem = sem_open("/dead_flag_sem", O_CREAT, 0644, 0);
    program->terminate_sem = sem_open("/terminate_sem", O_CREAT, 0644, 0);
}

void start_simulation(int ac,char **av,t_program *program) {
    pthread_t monitor;
    pthread_create(&monitor, NULL, monitor_routine, program);
    pthread_detach(monitor);

    for (int i = 0; i < program->philosopher_count; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            t_philosophers philo;
            philo.id = i;
            philo.number_of_philosophers = program->philosopher_count;
            philo.time_to_die = atoi(av[2]);
            philo.time_to_eat = atoi(av[3]);
            philo.time_to_sleep = atoi(av[4]);
            philo.number_times_to_eat = (ac == 6) ? atoi(av[5]) : -1;
            philo.meal_count = 0;

            char sem_name[256];
            snprintf(sem_name, sizeof(sem_name), "/fork_%d", i);
            philo.left_fork = sem_open(sem_name, O_RDWR);
            snprintf(sem_name, sizeof(sem_name), "/fork_%d", (i + 1) % program->philosopher_count);
            philo.right_fork = sem_open(sem_name, O_RDWR);
            snprintf(sem_name, sizeof(sem_name), "/meal_lock_%d", i);
            philo.meal_lock = sem_open(sem_name, O_RDWR);
            philo.print_sem = sem_open("/print_sem", O_RDWR);
            philo.dead_flag_sem = sem_open("/dead_flag_sem", O_RDWR);
            philo.terminate_sem = sem_open("/terminate_sem", O_RDWR);

            philo_routine(&philo);
            exit(0);
        }
        program->philosophers[i].pid = pid;
    }

    for (int i = 0; i < program->philosopher_count; i++) {
        waitpid(program->philosophers[i].pid, NULL, 0);
    }
    sem_post(program->terminate_sem);
}

int main(int ac, char **av) {
    if (ac < 5 || ac > 6) {
        printf("Usage: %s number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n", av[0]);
        return 1;
    }

    t_program program;
    program_init(ac, av, &program);
    start_simulation(ac,av,&program);

    sem_close(program.print_sem);
    sem_close(program.dead_flag_sem);
    sem_close(program.terminate_sem);
    sem_unlink("/print_sem");
    sem_unlink("/dead_flag_sem");
    sem_unlink("/terminate_sem");
    for (int i = 0; i < program.philosopher_count; i++) {
        char sem_name[256];
        snprintf(sem_name, sizeof(sem_name), "/fork_%d", i);
        sem_unlink(sem_name);
        snprintf(sem_name, sizeof(sem_name), "/meal_lock_%d", i);
        sem_unlink(sem_name);
    }
    return 0;
}