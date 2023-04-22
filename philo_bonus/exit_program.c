/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_program.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lahamoun <lahamoun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/18 22:52:49 by lahamoun          #+#    #+#             */
/*   Updated: 2023/03/18 22:52:49 by lahamoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void exit_program(const char* error_message)
{
    fprintf(stderr, "%s\n", error_message);
    exit(EXIT_FAILURE);
}
//"Forbiden
