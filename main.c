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
    int i;
    int j;

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
void initialphilos(t_philo *philos, t_info *global_info)
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
        if (pthread_mutex_init(&philos->fork, NULL) != 0)
        {
            exit_program("Mutex initialization failed");
        }
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

long get_time(void)
{
    struct timeval c;

    gettimeofday(&c, NULL);
    return (c.tv_sec * 1000 + c.tv_usec / 1000);
}
void ft_usleep(int time)
{
    long c;
    
    c = get_time();
    while(get_time() - c < time)
    {
        usleep(100);
    }
}
void *soooon(void *t)
{
    t_philo			*philos;

    philos = (t_philo *)t;
    if (philos->id % 2 == 0)
        usleep(1000);
    while(1)
    {
        pthread_mutex_lock(&philos->fork);
        pthread_mutex_lock(&philos->left->fork);
        pthread_mutex_lock(&philos->global_info->msg_mutex);
        printf("%ld philo %d has taken forks\n", get_time() - philos->global_info->start_time, philos->id);
        printf("%ld philo %d is eating\n", get_time() - philos->global_info->start_time, philos->id);
        pthread_mutex_unlock(&philos->global_info->msg_mutex);
        ft_usleep(philos->global_info->time_to_eat);
                pthread_mutex_lock(&philos->global_info->mutex);

        philos->num_of_times_eaten++;

        philos->last_time_ate = get_time();
                pthread_mutex_unlock(&philos->global_info->mutex);
        pthread_mutex_unlock(&philos->left->fork);
        pthread_mutex_unlock(&philos->fork);
        pthread_mutex_lock(&philos->global_info->msg_mutex);
        printf("%ld philo %d is sleeping\n", get_time() - philos->global_info->start_time, philos->id);
        pthread_mutex_unlock(&philos->global_info->msg_mutex);
        ft_usleep(philos->global_info->time_to_sleep);
        pthread_mutex_lock(&philos->global_info->msg_mutex);
        printf("%ld philo %d is thinkig\n", get_time() - philos->global_info->start_time, philos->id);
        pthread_mutex_unlock(&philos->global_info->msg_mutex);
    }
}

int main(int ac, char **av)
{
    int				num_of_philos;
    t_philo			*philos;
    t_info			*global_info;
    pthread_t       *thread;
    int i;
   int eat = 0;

    i = 0;
    if (check_args(ac, av))
        return (1);
    num_of_philos = ft_atoi(av[1]);
    philos = malloc(sizeof(t_philo));
    global_info = malloc(sizeof(t_info));
    if (!philos || !global_info)
        return (1);
    global_info->num_of_philos = num_of_philos;
    global_info->time_to_die = ft_atoi(av[2]);
    global_info->time_to_eat = ft_atoi(av[3]);
    global_info->time_to_sleep = ft_atoi(av[4]);
    if (pthread_mutex_init(&global_info->mutex, NULL) != 0)
    {
        exit_program("Mutex initialization failed");
    }
    if (pthread_mutex_init(&global_info->msg_mutex, NULL) != 0)
    {
        exit_program("Mutex initialization failed");
    }
    global_info->num_of_times_each_philo_must_eat = -42;
    thread = malloc(sizeof(pthread_t) * global_info->num_of_philos);
    if (ac == 6)
        global_info->num_of_times_each_philo_must_eat = ft_atoi(av[5]);
    initilaphilos(philos, global_info);
    while(i < global_info->num_of_philos)
    {
         if (pthread_create(&thread[i], NULL, &soooon, philos) != 0)
        {
            exit_program("Thread creation failed");
        }
        pthread_detach(thread[i]);
        philos= philos->left;
        i++;
    }
   while(1)
    {
        i = 0;
        eat = 0;
        while(i < global_info->num_of_philos)
        {
            pthread_mutex_lock(&philos->global_info->mutex);
            if (global_info->num_of_times_each_philo_must_eat != -42 && philos->num_of_times_eaten >=  global_info->num_of_times_each_philo_must_eat)
            eat++;
            if (get_time() - philos->last_time_ate > global_info->time_to_die)
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
    } 
    return (0);
}
