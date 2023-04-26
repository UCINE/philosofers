/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lahamoun <lahamoun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/08 04:14:07 by lahamoun          #+#    #+#             */
/*   Updated: 2023/04/26 16:27:34 by lahamoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int check_args(int ac, char **av)
{
    int i, j;

    if (ac < 5 || ac > 6)
    {
        write(2, "Error: wrong number of arguments\n", 33);
        return (1);
    }
	
    else if (ft_atoi(av[2]) <= ft_atoi(av[3]) || ft_atoi(av[2]) <= ft_atoi(av[4]))
    {
        write(2, "Error: time_to_die should be greater than time_to_eat and time_to_sleep\n", 72);
        return (1);
    }
        i = 1;
        while (++i < ac)
        {
            j = 0;
            while (av[i][j] && av[i][j] >= '0' && av[i][j] <= '9')
                j++;
            if (ft_atoi(av[i]) <= 0 || av[i][j])
            {
                write(2, "Error: number_of_times_each_philosopher_must(e,s,t)should be greater than 0\n", 79);
                return (1);
            }
        }

    return (0);
}

void initialize_philos(t_philo *philos, t_info *global_info)
{
    int i = 0;
    t_philo *tmp;
    
    struct timeval current_time;
    tmp = philos;
    while (i < global_info->num_of_philos)
    {
        gettimeofday(&current_time, NULL);
        global_info->start_time = current_time.tv_sec * 1000 + current_time.tv_usec / 1000;
        philos->id = i + 1;
        philos->global_info = global_info;
        philos->last_time_ate = global_info->start_time;
        philos->num_of_times_eaten = 0;
        philos->fd = 0;
        if (i < global_info->num_of_philos - 1)
        {
            philos->left = malloc(sizeof(t_philo));
            philos->left->right = philos;
            philos = philos->left;
        }
        else if (i == global_info-> num_of_philos - 1)
        {
            tmp->right = philos;
            philos->left = tmp;
        }
        i++;
    }
}

unsigned long get_time(void)
{
    struct timeval c;

    gettimeofday(&c, NULL);
    return (c.tv_sec * 1000 + c.tv_usec / 1000);
}
void ft_usleep(unsigned long time)
{ 
    unsigned long c;
    
    c = get_time();
    while(get_time() - c < time)
    {
        usleep(300);
    }
}

void *philosopher_routine(void *arg)
{
    t_philo *philo;

    philo = (t_philo *)arg;
    
    while (1)
    {
        sem_wait(philo->global_info->forks);
        sem_wait(philo->global_info->forks);
        sem_wait(philo->global_info->msg_sem);
        printf("%ld philo %d has taken a fork\n", get_time() - philo->global_info->start_time, philo->id);
        sem_post(philo->global_info->msg_sem);
        philo->last_time_ate = get_time();
        sem_wait(philo->global_info->msg_sem);
        printf("%ld philo %d is eating\n", get_time() - philo->global_info->start_time, philo->id);
        sem_post(philo->global_info->msg_sem);
        ft_usleep(philo->global_info->time_to_eat );
        sem_post(philo->global_info->forks);
        sem_post(philo->global_info->forks);
        if (++philo->num_of_times_eaten == philo->global_info->num_of_times_each_philo_must_eat)
            exit(0);
        sem_wait(philo->global_info->msg_sem);
        printf("%ld philo %d is sleeping\n", get_time() - philo->global_info->start_time, philo->id);
        sem_post(philo->global_info->msg_sem);
        ft_usleep(philo->global_info->time_to_sleep);
        sem_wait(philo->global_info->msg_sem);
        printf("%ld philo %d is thinking\n", get_time() - philo->global_info->start_time, philo->id);
        sem_post(philo->global_info->msg_sem);
    }
    exit(0);
}

void create_p(t_philo *philos)
{
    int  i = 0;
        while (i < philos->global_info->num_of_philos)
    {
        // Each philosopher is assigned a process
        philos->fd = fork();
        if (philos->fd == -1)
            exit_program("fork failed");
        if (philos->fd == 0)
        {
            pthread_t p;
            // Child process
            pthread_create(&p, NULL, philosopher_routine, philos);
            if(philos->id % 2 ==0)
                    usleep(3000 * 1000);
            // philosopher_routine(philos);
            check_dead(philos);
            exit(0);
        }
        else
        {
            philos = philos->left;
        }
        i++;
    }
}

void check_dead(t_philo *philos)
{
    while(1)
    {
        if (get_time() - philos->last_time_ate > philos->global_info->time_to_die)
        {
            sem_wait(philos->global_info->msg_sem);
            printf("%ldphilo %d is dead\n", get_time() - philos->global_info->start_time, philos->id);
            exit(1);
        }
        usleep(100);
    }
}

void wait_philo(t_philo *philos)
{
    int s, i = 0;
    while (wait(&s) != -1)
    {
        if (WEXITSTATUS(s))
        {
            while (i < philos->global_info->num_of_philos)
            {
                kill(philos->fd, SIGKILL);
                philos = philos->left;
                i++;
            }
            exit(0);
        }
            
    }
}
int main(int ac, char **av)
{
    int num_of_philos;
    t_philo *philos;
    t_info *global_info;
    int i;

    i = 0;
    if (check_args(ac, av))
        return (1);
    num_of_philos = ft_atoi(av[1]);
    philos = malloc(sizeof(t_philo) * num_of_philos);
    global_info = malloc(sizeof(t_info));
    if (!philos || !global_info)
        return (1);
    global_info->num_of_philos = num_of_philos;
    global_info->time_to_die = ft_atoi(av[2]);
    global_info->time_to_eat = ft_atoi(av[3]);
    global_info->time_to_sleep = ft_atoi(av[4]);
    sem_unlink("/msg_lock");
    sem_unlink("/msg_sem");
    sem_unlink("/my_forks");
    philos->global_info = global_info;
    philos->global_info->msg_lock = sem_open("/msg_lock", O_CREAT, 0644, 1) ;
    if (philos->global_info->msg_lock ==  SEM_FAILED)
        exit_program("semaphore initialization failed1"); 
    philos->global_info->msg_sem = sem_open("/msg_sem",O_CREAT,0644,1);
    if ( philos->global_info->msg_sem == SEM_FAILED)
        exit_program("semaphore initialization failed");
     philos->global_info->forks = sem_open("/my_forks",O_CREAT,0644,num_of_philos);
    if (philos->global_info->forks ==  SEM_FAILED)
        exit_program("semaphore initialization failed");
    global_info->num_of_times_each_philo_must_eat = -42;
    if (ac == 6)
        global_info->num_of_times_each_philo_must_eat = ft_atoi(av[5]);
    initialize_philos(philos, global_info);
    create_p(philos);
    wait_philo(philos);
    return (0);
}
