/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lahamoun <lahamoun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/08 04:14:07 by lahamoun          #+#    #+#             */
/*   Updated: 2023/03/30 06:22:46 by lahamoun         ###   ########.fr       */
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

void initiaphilo(t_philo *philos, t_info *global_info)
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

void *philo_routine(void *arg)
{
    t_philo *philo;

    philo = (t_philo *)arg;
    while (1)
    {
        sem_wait(philo->global_info->forks);
        sem_wait(philo->global_info->forks);
        sem_wait(philo->global_info->msg_sem);
        printf("%ld philo %d has taken a fork\n", get_time() - philo->global_info->start_time, philo->id);
        printf("%ld philo %d has taken a fork\n", get_time() - philo->global_info->start_time, philo->id);
        sem_post(philo->global_info->msg_sem);
        philo->last_time_ate = get_time();
        sem_wait(philo->global_info->msg_sem);
        printf("%ld philo %d is eating\n", get_time() - philo->global_info->start_time, philo->id);
        sem_post(philo->global_info->msg_sem);
        ft_usleep(philo->global_info->time_to_eat );
        sem_post(philo->forks);
        sem_post(philo->forks);
        if (++philo->num_of_times_eaten == philo->global_info->num_of_times_each_philo_must_eat)
            return (NULL);
        sem_wait(philo->global_info->msg_sem);
        printf("%ld philo %d is sleeping\n", get_time() - philo->global_info->start_time, philo->id);
        sem_post(philo->global_info->msg_sem);
        ft_usleep(philo->global_info->time_to_sleep);
        sem_wait(philo->global_info->msg_sem);
        printf("%ld philo %d is thinking\n", get_time() - philo->global_info->start_time, philo->id);
        sem_post(philo->global_info->msg_sem);
    }
}
void wait_for_threads_to_finish(pthread_t *threads, int num_of_philos)
{
    int i;

    i = 0;
    while (i < num_of_philos)
    {
        pthread_join(threads[i], NULL);
        i++;
    }
}


int main(int ac, char **av)
{
    int num_of_philos;
    t_philo *philos;
    t_info *global_info;
    pthread_t *threads;
    int i;

    i = 0;
    if (check_args(ac, av))
        return (1);
    num_of_philos = ft_atoi(av[1]);
    philos = malloc(sizeof(t_philo) * num_of_philos);
    global_info = malloc(sizeof(t_info));
    threads = malloc(sizeof(pthread_t) * num_of_philos);
    if (!philos || !global_info || !threads)
        return (1);
    global_info->num_of_philos = num_of_philos;
    global_info->time_to_die = ft_atoi(av[2]);
    global_info->time_to_eat = ft_atoi(av[3]);
    global_info->time_to_sleep = ft_atoi(av[4]);
    unlink("/msg_lock");
    unlink("/msg_sem");
    if (sem_open("/msg_lock",O_CREAT,0644,1) ==  SEM_FAILED)
        exit_program("semphore initialization failed1");  
    if (sem_open("/msg_sem",O_CREAT,0644,1)== SEM_FAILED)
        exit_program("semphore initialization failed");
    if (sem_open("/forks",O_CREAT,0644,num_of_philos) ==  SEM_FAILED)
        exit_program("semphore initialization failed");
    global_info->num_of_times_each_philo_must_eat = -42;
    if (ac == 6)
        global_info->num_of_times_each_philo_must_eat = ft_atoi(av[5]);
    initialize_philos(philos, global_info);
    while (i < num_of_philos)
    {
        if (pthread_create(&threads[i], NULL, philo_routine, &philos[i]))
            exit_program("Error: pthread_create() failed");
        i++;
    }
    wait_for_threads_to_finish(threads, num_of_philos);
    return (0);
}


 /*
 while(i <= global_info->num_of_philos)
    {
        
         if (pthread_create(&thread[i], NULL, &soooon, philos) != 0)
            exit_program("Thread creation failed");
        pthread_detach(thread[i]);
        philos= philos->left;
        i++;
    }
    
 while(1)
    {
        i = 0;
        eat = 1;
        while(i < global_info->num_of_philos)
        {
            pthread_mutex_lock(&philos->global_info->mutex);
            if (global_info->num_of_times_each_philo_must_eat != -42 && philos->num_of_times_eaten >=  global_info->num_of_times_each_philo_must_eat)
            eat++;
            if (get_time() - philos->last_time_ate > (unsigned long)global_info->time_to_die)
            {
                pthread_mutex_lock(&philos->global_info->msg_mutex);
                printf("%ld philo %d is dead\n", get_time() - philos->global_info->start_time, philos->id);
                return(0) ;
            }  
            pthread_mutex_unlock(&philos->global_info->mutex);
            philos = philos->left;
            i++;
        }
        if (eat >= global_info->num_of_philos)
        {
            pthread_mutex_lock(&philos->global_info->msg_mutex);
            return(0) ;
        }
    }*/
