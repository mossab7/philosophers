#include "philosopher.h"


bool check_arguments(int ac, char **av) {
    if (ac < 5 || ac > 6) {
        printf("Usage: %s number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n", av[0]);
        return false;
    }
    for (int i = 1; i < ac; i++) {
        int j = 0;
        while (av[i][j]) {
            if (av[i][j] < '0' || av[i][j] > '9') {
                printf("Error: Arguments must be positive integers.\n");
                return false;
            }
            j++;
        }
        if (atoi(av[i]) <= 0) {
            printf("Error: Arguments must be positive integers greater than zero.\n");
            return false;
        }
    }
    return true;
}

void cleanup_semaphores(void) {
    sem_unlink("forks_sem");
    sem_unlink("print_sem");
    sem_unlink("meal_sem");
    sem_unlink("stop_sem");
}

t_philosophers *philosopher_init(t_program *program) {
    cleanup_semaphores();
    t_philosophers *philosopher = malloc(sizeof(t_philosophers) * program->number_of_philosophers);
    if (!philosopher) {
        printf("Error: Failed to allocate memory for philosopher.\n");
        return NULL;
    }
    sem_t *forks_sem = sem_open("forks_sem", O_CREAT, 0644, program->number_of_philosophers);
    sem_t *print_sem = sem_open("print_sem", O_CREAT, 0644, 1);
    sem_t *meal_sem = sem_open("meal_sem", O_CREAT, 0644, 1);
    sem_t *stop_sem = sem_open("stop_sem", O_CREAT, 0644, 1);
    if (forks_sem == SEM_FAILED || print_sem == SEM_FAILED || meal_sem == SEM_FAILED || stop_sem == SEM_FAILED) {
        printf("Error: Failed to create semaphores.\n");
        sem_close(forks_sem);
        sem_close(print_sem);
        sem_close(meal_sem);
        sem_close(stop_sem);
        cleanup_semaphores();
        free(philosopher);
        return NULL;
    }
    for (int i = 0; i < program->number_of_philosophers; i++) {
        philosopher[i].id = i;
        philosopher[i].program = program;
        philosopher[i].forks_sem = forks_sem;
        philosopher[i].print_sem = print_sem;
        philosopher[i].meal_sem = meal_sem;
        philosopher[i].death_sem = stop_sem;
        philosopher[i].meal_count = 0;
        philosopher[i].last_meal = 0;
    }
    return philosopher;
}

t_program *program_init(int ac, char **av) {
    t_program *program = malloc(sizeof(t_program));
    if (!program) {
        printf("Error: Failed to allocate memory for program.\n");
        return NULL;
    }
    program->number_of_philosophers = atoi(av[1]);
    program->time_to_die = atoi(av[2]);
    program->time_to_eat = atoi(av[3]);
    program->time_to_sleep = atoi(av[4]);
    program->number_of_times_each_philosopher_must_eat = (ac == 6) ? atoi(av[5]) : -1;
    program->stop_sem = sem_open("stop_sem", O_CREAT, 0644, 1);
    if (program->stop_sem == SEM_FAILED) {
        printf("Error: Failed to create stop semaphore.\n");
        free(program);
        return NULL;
    }
    program->pids = malloc(program->number_of_philosophers * sizeof(pid_t));
    if (!program->pids) {
        printf("Error: Failed to allocate memory for process IDs.\n");
        sem_close(program->stop_sem);
        free(program);
        return NULL;
    }
    program->philosophers = philosopher_init(program);
    if (!program->philosophers) {
        printf("Error: Failed to initialize philosophers.\n");
        sem_close(program->stop_sem);
        free(program->pids);
        free(program);
        return NULL;
    }
    program->simulation_stopped = false;
    return program;
}

long long get_time(t_program *program) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000)) - program->start_time;
}

void ft_sleep(t_philosophers *philosopher, int time_ms) {
    long long start = get_time(philosopher->program);
    while (!is_simulation_stopped(philosopher->program)) {
        long long current = get_time(philosopher->program);
        if (current - start >= time_ms) break;
        usleep(500);
    }
}

int is_simulation_stopped(t_program *program) {
    int result;
    sem_wait(program->stop_sem);
    result = program->simulation_stopped;
    sem_post(program->stop_sem);
    return result;
}

void print_status(t_philosophers *philosopher, char *status) {
    if (!is_simulation_stopped(philosopher->program)) {
        sem_wait(philosopher->print_sem);
        printf("%lld %d %s\n", get_time(philosopher->program), philosopher->id + 1, status);
        sem_post(philosopher->print_sem);
    }
}

void release_fork(t_philosophers *philosopher) {
    sem_post(philosopher->forks_sem);
    sem_post(philosopher->forks_sem);
}

void take_fork(t_philosophers *philosopher) {
    sem_wait(philosopher->forks_sem);
    print_status(philosopher, "has taken a fork");
    sem_wait(philosopher->forks_sem);
    print_status(philosopher, "has taken a fork");
}

void handle_single_philosopher(t_philosophers *philosopher) {
    print_status(philosopher, "has taken a fork");
    ft_sleep(philosopher, philosopher->program->time_to_die);
    sem_wait(philosopher->program->stop_sem);
    philosopher->program->simulation_stopped = true;
    sem_post(philosopher->program->stop_sem);
    print_status(philosopher, "died");
    exit(1);
}

void eat_sleep_think(t_philosophers *philosopher) {
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
}

void *philosopher_routine(void *arg) {
    t_philosophers *philosopher = (t_philosophers *)arg;
    if (philosopher->program->number_of_philosophers == 1) {
        handle_single_philosopher(philosopher);
        return NULL;
    }
    if (philosopher->id % 2 == 0)
        ft_sleep(philosopher, philosopher->program->time_to_eat / 2);
    while (!is_simulation_stopped(philosopher->program)) {
        take_fork(philosopher);
        if (is_simulation_stopped(philosopher->program)) {
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

bool check_philo_death(t_philosophers *philosopher) {
    long long current_time = get_time(philosopher->program);
    bool is_dead = false;
    
    sem_wait(philosopher->meal_sem);
    if (current_time - philosopher->last_meal > philosopher->program->time_to_die) {
        sem_post(philosopher->meal_sem);
        
        sem_wait(philosopher->program->stop_sem);
        if (!philosopher->program->simulation_stopped) {
            philosopher->program->simulation_stopped = true;
            is_dead = true;
        }
        sem_post(philosopher->program->stop_sem);
        
        if (is_dead) {
            sem_wait(philosopher->print_sem);
            printf("%lld %d died\n", get_time(philosopher->program), philosopher->id + 1);
            sem_post(philosopher->print_sem);
            exit(1);
        }
        return is_dead;
    }
    sem_post(philosopher->meal_sem);
    return false;
}

bool check_all_ate_enough(t_program *program) {
    if (program->number_of_times_each_philosopher_must_eat == -1)
        return false;
    bool all_ate_enough = true;
    for (int i = 0; i < program->number_of_philosophers; i++) {
        sem_wait(program->philosophers[i].meal_sem);
        if (program->philosophers[i].meal_count < program->number_of_times_each_philosopher_must_eat)
            all_ate_enough = false;
        sem_post(program->philosophers[i].meal_sem);
        if (!all_ate_enough) break;
    }
    if (all_ate_enough) {
        sem_wait(program->stop_sem);
        program->simulation_stopped = true;
        sem_post(program->stop_sem);
    }
    return all_ate_enough;
}

void *monitor_routine(void *arg) {
    t_philosophers *philosopher = (t_philosophers *)arg;
    while (!is_simulation_stopped(philosopher->program)) {
        if (check_philo_death(philosopher)) {
            exit(1);
        }
        usleep(1000);
    }
    return NULL;
}

void cleanup_process(t_philosophers *philosopher) {
    sem_close(philosopher->forks_sem);
    sem_close(philosopher->print_sem);
    sem_close(philosopher->meal_sem);
    sem_close(philosopher->death_sem);
    sem_close(philosopher->program->stop_sem);
}

void philosopher_start(t_philosophers *philosopher) {
    pthread_t philosopher_thread, monitor_thread;
    philosopher->last_meal = get_time(philosopher->program);
    philosopher->meal_count = 0;
    if (pthread_create(&monitor_thread, NULL, monitor_routine, philosopher) != 0 ||
        pthread_create(&philosopher_thread, NULL, philosopher_routine, philosopher) != 0) {
        printf("Error: Failed to create threads.\n");
        exit(1);
    }
    pthread_join(philosopher_thread, NULL);
    pthread_join(monitor_thread, NULL);
    cleanup_process(philosopher);
    exit(0);
}

void program_start(t_program *program) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    program->start_time = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    for (int i = 0; i < program->number_of_philosophers; i++) {
        program->pids[i] = fork();
        if (program->pids[i] == 0) {
            philosopher_start(&program->philosophers[i]);
            exit(1);
        }
        usleep(100);
    }
    pid_t meal_monitor_pid = -1;
    if (program->number_of_times_each_philosopher_must_eat != -1) {
        meal_monitor_pid = fork();
        if (meal_monitor_pid == 0) {
            while (!is_simulation_stopped(program)) {
                if (check_all_ate_enough(program)) {
                    for (int i = 0; i < program->number_of_philosophers; i++)
                        kill(program->pids[i], SIGKILL);
                    exit(0);
                }
                usleep(1000);
            }
            exit(0);
        }
    }
    int status;
    pid_t exited_pid;
    while ((exited_pid = waitpid(-1, &status, 0))) {
        if (exited_pid == -1) break;
        if (exited_pid == meal_monitor_pid) {
            for (int i = 0; i < program->number_of_philosophers; i++)
                kill(program->pids[i], SIGKILL);
        } else if (WIFEXITED(status) && WEXITSTATUS(status) == 1) {
            for (int i = 0; i < program->number_of_philosophers; i++)
                if (program->pids[i] != exited_pid)
                    kill(program->pids[i], SIGKILL);
            if (meal_monitor_pid != -1) kill(meal_monitor_pid, SIGKILL);
            break;
        }
    }
    while (waitpid(-1, NULL, WNOHANG) > 0);
    if (meal_monitor_pid != -1) {
        kill(meal_monitor_pid, SIGKILL);
        waitpid(meal_monitor_pid, NULL, 0);
    }
}

void program_destroy(t_program *program) {
    sem_close(program->stop_sem);
    free(program->pids);
    free(program->philosophers);
    free(program);
}

int main(int ac, char **av) {
    cleanup_semaphores();
    if (!check_arguments(ac, av)) return 0;
    t_program *program = program_init(ac, av);
    if (!program) return 0;
    program_start(program);
    program_destroy(program);
    return 0;
}