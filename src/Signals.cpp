/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signals.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <mac@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 22:16:24 by mac               #+#    #+#             */
/*   Updated: 2025/03/10 22:56:20 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE
# include <unistd.h>
# include <signal.h>


void	handle_sigint(int sig)
{
	(void)sig;
	write(1, "\n", 1);
}

void	handle_sigquit(int sig)
{
	(void)sig;
	write(1, "\n", 1);
}

void	handle_signals(void)
{
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, handle_sigquit);
}
