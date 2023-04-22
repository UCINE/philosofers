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

# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <pthread.h>
# include <limits.h>
#include <sys/time.h>


int	ft_atoi(const char *str);
void exit_program(const char* error_message);

typedef struct s_info
{
	int				num_of_philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				num_of_times_each_philo_must_eat;
	long			start_time;
	pthread_mutex_t	msg_mutex;
	pthread_mutex_t	mutex;
}	t_info;

typedef struct s_philo
{
	int				id;
	int				num_of_times_eaten;
	long			last_time_ate;
	pthread_t		philo_thread;
	pthread_mutex_t	*left_fork_mutex;
	pthread_mutex_t	*right_fork_mutex;
	struct s_philo *left;
	struct s_philo *right;
	pthread_mutex_t	fork;
	t_info	*global_info;
}	t_philo;

#endif
