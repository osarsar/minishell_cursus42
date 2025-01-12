/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osarsar <osarsar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 16:11:58 by osarsar           #+#    #+#             */
/*   Updated: 2023/09/05 12:59:16 by osarsar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_builting(t_cmd *data)
{
	if (!data->cmd || !data->cmd[0] || !data)
		return (1);
	if (!ft_strcmp(*data->cmd, "echo") || !ft_strcmp(*data->cmd, "cd") 
		|| !ft_strcmp(*data->cmd, "env") || !ft_strcmp(*data->cmd, "export")
		||!ft_strcmp(*data->cmd, "unset") || !ft_strcmp(*data->cmd, "exit") 
		||!ft_strcmp(*data->cmd, "pwd") || !ft_strcmp(*data->cmd, "exit"))
		return (0);
	return (1);
}

void	ft_exit_status(int *pid, int flag)
{
	int	i;
	int	num;

	if (flag == 0)
	{
		waitpid(*pid, &i, 0);
		while (wait(NULL) > 0)
			;
		if (WIFEXITED(i))
			g_glb.exit_status = WEXITSTATUS(i);
		else if (WIFSIGNALED(i))
		{
			num = 0;
			g_glb.exit_status = num;
		}
	}
}

int	exec_with_or_without_pipe(t_cmd **data, int *pid)
{
	int	id;

	if ((*data)->next)
	{
		exec_with_pipe_middle(data);
		exec_with_pipe_last(*data, pid);
		return (1);
	}
	else
	{
		id = exec_with_no_pipe(*data, pid);
		if (id == -2)
		{
			ft_putstr_fd(2, "minishell : command not found\n");
			g_glb.exit_status = 127;
			return (0);
		}
		else if (id == 3)
			return (0);
	}
	return (1);
}

int	execution_and_redirection(t_cmd *data)
{
	int	in;
	int	out;
	int	pid;
	int	flag;

	flag = 0;
	pid = -1;
	in = dup(0);
	out = dup(1);
	signal(SIGINT, handle);
	signal(SIGQUIT, handle);
	if (!exec_with_or_without_pipe(&data, &pid))
		flag = 1;
	dup2(in, 0);
	close(in);
	dup2(out, 1);
	close(out);
	ft_exit_status(&pid, flag);
	return (0);
}
