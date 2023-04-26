/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lahamoun <lahamoun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/08 04:14:29 by lahamoun          #+#    #+#             */
/*   Updated: 2023/03/08 04:14:29 by lahamoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


# ifndef PHILO_H
# define PHILO_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/time.h>

int	ft_atoi(const char *str);
void exit_program(const char* error_message);

typedef struct s_info
{
	int				num_of_philos;
	int				num_of_times_each_philo_must_eat;
	unsigned long				time_to_die;
	unsigned long				time_to_eat;
	unsigned long				time_to_sleep;
	unsigned long			start_time;
	sem_t * msg_sem;
	pthread_mutex_t	msg_mutex;
	sem_t * msg_lock;
	pthread_mutex_t	mutex;
	int number_eat;
	sem_t *forks;
}	t_info;

typedef struct s_philo
{
	int				id;
	int				num_of_times_eaten;
	unsigned long			last_time_ate;
	pthread_t		philo_thread;
	struct s_philo *left;
	struct s_philo *right;
	t_info	*global_info;
	pthread_t *thread; 
	int fd;
}	t_philo;
void check_dead(t_philo *philos);

#endif
