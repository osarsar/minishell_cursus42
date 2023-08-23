/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osarsar <osarsar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 18:26:24 by isidki            #+#    #+#             */
/*   Updated: 2023/08/20 07:00:39 by osarsar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_token	which_token(char c1, char c2)
{
	if (c1 == '|')
		return (PIPE);
	if (c1 == '>' && c2 == '>')
		return (APPEND);
	if (c1 == '<' && c2 == '<')
		return (HEREDOC);
	if (c1 == '<')
		return (IN);
	if (c1 == '>')
		return (OUT);
	if (c1 == ' ')
		return (SPC);
	if (c1 == '$')
		return (DLR);
	if (c1 == '"')
		return (DQU);
	if (c1 == '\'')
		return (SQU);
	return (WORD);
}

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	if (!str)
		return (0);
	while (str[i])
		i++;
	return (i);
}

void	concat_words(t_lexer *tmp, t_lexer *current)
{
	char		*concatenated;
	t_lexer*	next_tmp;

	concatenated = NULL;
	while (tmp && tmp->token == WORD)
	{
		concatenated = ft_strjoin(current->cmd, tmp->cmd);
		if (!concatenated)
			break ;
		ft_free(current->cmd);
		current->cmd = concatenated;
		next_tmp = tmp->next;
		if (tmp->next)
			next_tmp->prev = current;
		ft_free(tmp->cmd);
		ft_free(tmp);
		tmp = next_tmp;
	}
	current->next = tmp;
}

void	concat_word_lexer(t_lexer **l)
{
	t_lexer	*current;
	t_lexer	*tmp;

	current = *l;
	while (current && current->next)
	{
		if (current->token == WORD)
		{
			tmp = current->next;
			concat_words(tmp, current);
		}
		current = current->next;
	}
}

void	concat_between_qu(t_lexer *start, t_lexer *end, t_token tok)
{
	t_lexer	*current;
	t_lexer	*tmp;
	char	*concatenated;

	current = start;
	current->token = tok;
	tmp = start->next;
	while (tmp && tmp != end) 
	{
		concatenated = ft_strjoin(current->cmd, tmp->cmd);
		if (!concatenated)
			break ;
		ft_free (current->cmd);
		current->cmd = concatenated;
		current->token = tok;
		current->next = tmp->next;
		if (tmp->next)
			tmp->next->prev = current;
		ft_free(tmp->cmd);
		ft_free(tmp);
		if (current->next)
			tmp = current->next;
		else
			return ;
	}
}

void	delete_spc(t_lexer **head, t_lexer *temp)
{
	t_lexer	*hold;

	temp = temp->next;
	while (temp->token == SPC)
	{
		hold = temp->next;
		ft_delete_node(head, temp);
		temp = hold;
	}
}

void	delete_consctv_spaces(t_lexer **head)
{
	t_lexer	*temp;

	temp = *head;
	while (temp)
	{
		if (temp->token == SPC && temp->next
			&& temp->next->token == SPC)
			delete_spc(head, temp);
		temp = temp->next;
	}
}

t_lexer	*find_matching_quote(t_lexer *start, t_token tok)
{
	t_lexer	*temp;

	temp = NULL;
	if (start->next)
		temp = start->next;
	while (temp && temp->token != tok)
		temp = temp->next;
	return (temp);
}

int	handle_dqu(t_lexer *temp, t_lexer *hold, t_token tok)
{
	if (!temp)
	{
		ft_putstr_fd(2, "minishell: syntax error near unclosed quotes\n");
		g_glb.exit_status = 258;
		return (1);
	}
	else if (hold != temp && hold)
		concat_between_qu(hold, temp, tok); //hold is first elm after '\"' and temp is the closing '\"'
	return (0);
}

void	empty_string(t_lexer **l, t_lexer *hold, t_token tok)
{
	ft_free(hold->cmd);
	hold->cmd = ft_strdup("");
	hold->token = tok;
	ft_delete_node(l, hold->prev);
}

void	assign_tok(t_lexer *tmp, t_token *tok)
{
	if (tmp->token == DQU)
		*tok = DQU;
	else if (tmp->token == SQU)
		*tok = SQU;
}

int	ft_check_quotes(t_lexer **l)
{
	t_lexer	*temp;
	t_lexer	*hold;
	t_token	tok;

	temp = *l;
	while (temp)
	{
		tok = temp->token;
		assign_tok(temp, &tok);
		if (tok == SQU || tok == DQU)
		{
			hold = temp->next;
			temp = find_matching_quote(temp, tok);
			if (handle_dqu(temp, hold, tok))
				return (1);
			else if (hold == temp && hold)
				empty_string(l, hold, tok);
		}
		else if (temp->token == SPC && temp->next && temp->next->token == SPC)
			delete_spc(l, temp);
		if (temp)
			temp = temp->next;
	}
	return (0);
}

void	tokenize_redirections(t_lexer **head)
{
	t_lexer	*temp;

	temp = *head;
	while (temp)
	{
		if (temp->token == APPEND)
		{
			ft_free(temp->cmd);
			temp->cmd = ft_strdup(">>");
			ft_delete_node(head, temp->next);
		}
		else if (temp->token == HEREDOC)
		{
			ft_free(temp->cmd);
			temp->cmd = ft_strdup("<<");
			ft_delete_node(head, temp->next);
		}
		if (temp)
			temp = temp->next;
	}
}

void	ft_lexer(char *input, t_lexer **head)
{
	int		i;

	i = 0;
	while (input[i])
	{
		ft_lstadd_back_lex(head, ft_lstnew_lex(ft_substr(input, i, 1),
				which_token(input[i], input[i + 1])));
		i++;
	}
	concat_word_lexer(head);
	tokenize_redirections(head);
}

char	*ft_strjoin(char *s1, char *s2)
{
	char	*str;
	int		i;
	int		j;
	int		l;

	if (!s1)
		return (s2);
	if (!s2)
		return (s1);
	i = ft_strlen(s1);
	j = ft_strlen(s2);
	l = 0;
	str = (char *)ft_malloc(i + j + 1);
	if (!str)
		return (NULL);//exit if ft_malloc fails
	while (*s1)
		str[l++] = *s1++;
	while (*s2)
		str[l++] = *s2++;
	str[l] = '\0';
	return (str);
}

char	*ft_strtrim(char *s1, char *set)
{
	int	i;
	int	len;

	i = 0;
	if (!s1 || !set)
		return (NULL);
	len = ft_strlen(s1) - 1;
	while (s1[i] && char_in_set(set, s1[i]))
		i++;
	while (len >= i && char_in_set(set, s1[len]))
		len--;
	if (len == -1)
		return (ft_strdup(""));
	return (ft_substr(s1, i, (len - i + 1)));
}

void	print_cmd_linked_list(t_cmd** head) //delete
{
	t_cmd*	temp;

	temp = *head;

	while (temp)
	{
		printf("-----------\n");
		int ii = 0;
		while (temp->cmd && temp->cmd[ii])
			printf("|%s|\n",temp->cmd[ii++]);
		printf("cmd->fd[0]= %d\n", temp->fd[0]);
		printf("cmd->fd[1]= %d\n", temp->fd[1]);
		printf("-----------\n");
		temp = temp->next;
	}
	printf("END\n");
}

void	print_linked_list(t_lexer** head) //delete
{
	t_lexer*	temp;

	temp = *head;
	while (temp)
	{
		printf("(%s, %d) -> ", temp->cmd, (int)temp->token);
		temp = temp->next;
	}
	printf("END\n");
}

void	printLinkedListReverse(t_lexer** head) //delete
{
	t_lexer* temp = *head;

	while (temp && temp->next)
		temp = temp->next;
	while (temp)
	{
		printf("(%s, %d) -> ", temp->cmd, (int)temp->token);
		temp = temp->prev;
	}
	printf("start\n");
}

void	delete_quotes(t_lexer** head)
{
	t_lexer*	current;
	t_lexer*	temp;

	current = *head;
	while (current)
	{
		if ((!ft_strcmp(current->cmd, "\'") && current->token == SQU)
			|| (!ft_strcmp(current->cmd, "\"") && current->token == DQU))
		{
			temp = current->next;
			ft_delete_node(head, current);
			current = temp;
		}
		else
			current = current->next;
	}
	delete_consctv_spaces(head);
}

void	ft_check_env(t_lexer *dlr_ptr)
{
	char	*value;

	value = ft_split_concat_dqu_dlr(-1, dlr_ptr->cmd);
	if (value)
	{
		ft_free(dlr_ptr->cmd);
		dlr_ptr->cmd = value;
	}
}

char	*ft_env_var(char *var)
{
	t_env	*temp;

	temp = g_glb.env;
	while (temp)
	{
		if (!ft_strcmp(var, temp->key))
			return (ft_strdup(temp->value));
		temp = temp->next;
	}
	return (NULL);
}

int	ft_isalnum(char c)
{
	if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z')
		|| (c >= 'a' && c <= 'z'))
		return (1);
	return (0);
}

int	is_identifier(char c)
{
	if (ft_isalnum(c) || c == '_')
		return (1);
	return (0);
}

char	*concat_var(char *wrd_expd, int i, int j, char *var)
{
	char	*concatenated;
	char	*first_sub;
	char	*last_sub;
	
	while (wrd_expd[i] && is_identifier(wrd_expd[i]))
		i++;
	first_sub = ft_substr(wrd_expd, 0, j - 1);
	var = ft_substr(wrd_expd, j, i - j);
	last_sub = ft_substr(wrd_expd, i, ft_strlen(wrd_expd) - i);
	if (!first_sub || !var || !last_sub)
		return (NULL);
	if (ft_env_var(var))
	{
		concatenated = ft_strjoin(first_sub, ft_env_var(var));
		concatenated = ft_strjoin(concatenated, last_sub);
	}
	else if (!*first_sub && !*last_sub)
		concatenated = ft_strdup("");
	else
		concatenated = ft_strjoin(first_sub, last_sub);
	return (concatenated);
}

char	*ft_split_concat_dqu_dlr(int i, char *wrd_expd)
{
	int		j;
	char	*concatenated;
	char	*var;

	j = ++i;
	var = NULL;
	if (wrd_expd[i] == '?')
	{
		concatenated = ft_strjoin(ft_substr(wrd_expd, 0, i - 1),
			ft_itoa(g_glb.exit_status));
		concatenated = ft_strjoin(concatenated,
				ft_substr(wrd_expd, i + 1, ft_strlen(wrd_expd) - i));
		return (concatenated);
	}
	concatenated = concat_var(wrd_expd, i, j, var);
	ft_free(var);
	return (concatenated);
}

void	look_for_dlr(t_lexer *tmp)
{
	int		i;
	char	*concatenated;

	i = 0;
	if (tmp)
	{
		while (tmp->cmd[i]) 
		{
			if (tmp->cmd[i] == '$' && tmp->cmd[i + 1] != '$')
			{
				concatenated = ft_split_concat_dqu_dlr(i, tmp->cmd);
				ft_free(tmp->cmd);
				tmp->cmd = concatenated;
			}
			i++;
			if (!*tmp->cmd)
				break ;
		}
	}
}

void	ft_ambigs_redirect(t_lexer *dlr, t_lexer *word)
{
	ft_check_env(word);
	if (!*word->cmd)
	{
		if (dlr->prev && dlr->prev->token == SPC && dlr->prev->prev)
			dlr = dlr->prev->prev;
		else if (dlr->prev)
			dlr = dlr->prev;
		if (dlr && (dlr->token == IN || dlr->token == OUT
			|| dlr->token == APPEND))
			dlr->fd = -3;
	}
}

void	empty_two_nodes(t_lexer **tmp)
{
	ft_free((*tmp)->cmd);
	(*tmp)->cmd = ft_strdup("");
	ft_free((*tmp)->next->cmd);
	(*tmp)->next->cmd = ft_strdup("");		
	(*tmp) = (*tmp)->next;
}

int	no_expand_in_herdc_delim(t_lexer **tmp)
{
	if ((*tmp)->prev && ((*tmp)->prev->token == HEREDOC
		|| ((*tmp)->prev->token == SPC && (*tmp)->prev->prev
			&& (*tmp)->prev->prev->token == HEREDOC)))
	{
		(*tmp) = (*tmp)->next;
		return (1);
	}
	return (0);
}

void	ft_expand(t_lexer **head)
{
	t_lexer	*current;
	t_lexer	*tmp;

	tmp = *head;
	while (tmp)
	{
		if (no_expand_in_herdc_delim(&tmp))
			continue ;
		if (tmp->token == DLR && tmp->next)
		{
			if (tmp->next->token == WORD)
			{
				current = tmp->next;
				ft_ambigs_redirect(tmp, tmp->next);
				ft_delete_node(head, tmp);
				tmp = current;
			}
		}
		if (tmp->token == DLR && tmp->next && tmp->next->token == DLR)
			empty_two_nodes(&tmp);
		else if (tmp->token == DLR && !tmp->next)
			tmp->token = WORD;
		else if (tmp->token == DQU)
			look_for_dlr(tmp);
		tmp = tmp->next;
	}
}

t_lexer	*find_delimiter(t_lexer **head)
{
	t_lexer	*tmp;

	tmp = *head;
	while (tmp)
	{
		if (tmp->token == HEREDOC && tmp->next)
		{
			if (tmp->next->token == SPC && tmp->next->next)
				return (tmp->next->next);
			else
				return (tmp->next);
		}
		tmp = tmp->next;
	}
	return (NULL);
}

void	check_heredoc_delimiter(t_lexer *tmp, t_lexer *hold)
{
	if (tmp->token == HEREDOC && tmp->next)
	{
		hold = tmp->next;
		if (hold->token == SPC && hold->next)
			hold = hold->next;
		while (hold && (hold->token == WORD || hold->token == DQU
				|| hold->token == SQU || hold->token == DLR))
		{
			if (hold->token == DQU || hold->token == SQU)
				g_glb.dqu = 1;
			hold->token = WORD;
			if (hold)
				hold = hold->next;
		}
	}
}

t_lexer	*ft_heredoc_delimiter(t_lexer **head)
{
	t_lexer	*tmp;
	t_lexer	*hold;

	tmp = *head;
	hold = NULL;
	g_glb.sg = 0;
	while (tmp)
	{
		check_heredoc_delimiter(tmp, hold);
		if (tmp)
			tmp = tmp->next;
	}
	concat_word_lexer(head);
	tmp = find_delimiter(head);
	return (tmp);
}

void	handle_delete_node(t_lexer **head, t_lexer *node)
{
	if (node->prev && node->next)
	{
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	else if (!node->prev && node->next)
	{
		*head = node->next;
		node->next->prev = NULL;
	}
	if (!node->next && node->prev)
		node->prev->next = NULL;
	if (!node->next && !node->prev) 
		*head = NULL;
	ft_free(node->cmd);
	ft_free(node);
}

void	ft_delete_node(t_lexer **head, t_lexer *node)
{
	t_lexer	*temp;

	temp = *head;
	while (temp)
	{
		if (node && temp == node)
		{
			handle_delete_node(head, node);
			break ;
		}
		if (temp)
			temp = temp->next;
	}
}

void	ft_check_expand_in_line(char **line)
{
	int		i;
	char	*lin;
	char	*concat;

	i = 0;
	lin = *line;
	while (lin && i < ft_strlen(lin))
	{
		if (lin[i] == '$' && lin[i + 1] != '$')
		{
			concat = ft_split_concat_dqu_dlr(i, lin);
			free(lin);
			lin = concat;
			i = -1;
		}
		i++;
		if (!*lin)
			break;
	}
	*line = lin;
}

void	ft_sig_handler(int sig)
{
	if (sig == SIGINT)
	{
		g_glb.sg = 1;
		ioctl(0, TIOCSTI, "\4");
	}
}

int	ft_readline_herdc(int *tt, t_lexer **loop, t_lexer *tmp, int *fd_return)
{
	int		fds[2];
	char	*line;

	if (pipe(fds) == -1)
		return (g_glb.exit_status = 1, -1);
	line = readline("> ");
	if (!line)
	{
		(*loop) = (*loop)->next;
		*tt = 1;
	}
	while (line && ft_strcmp(line, tmp->cmd) && !*tt)
	{
		if (!g_glb.dqu)
			ft_check_expand_in_line(&line);
		write(fds[1], line, ft_strlen(line));
		write(fds[1], "\n", 1);
		free(line);
		line = readline("> ");
	}
	free(line);
	if (g_glb.sg == 1)
		return (-1);
	return (g_glb.dqu = 0, close(fds[1]), *fd_return = fds[0], 0);
}

int	ft_heredoc(t_lexer **head)
{
	t_lexer	*tmp;
	t_lexer	*loop;
	int		tt;
	int		fd_return;

	loop = *head;
	tmp = NULL;
	while (loop)
	{
		tt = 0;
		tmp = ft_heredoc_delimiter(&loop);
		signal(SIGINT, ft_sig_handler);
		if (tmp)
		{
			if (ft_readline_herdc(&tt, &loop, tmp, &fd_return) == -1)
				return (-1);
			if (tmp->next)
				loop = tmp->next;
			else
				break;
		}
		else if (loop)
			loop = loop->next;		
	}
	return (fd_return);
}

int	unexpected_tok(t_lexer *tmp)
{
	if ((tmp->token == IN || tmp->token == OUT
			|| tmp->token == APPEND || tmp->token == HEREDOC)
		&& tmp->next && ((tmp->next->token != WORD
		&& tmp->next->token != DQU && tmp->next->token != SQU
		&& tmp->next->token != DLR && tmp->next->token != SPC)
		|| (tmp->next->token == SPC && tmp->next->next
		&& tmp->next->next->token != WORD && tmp->next->next->token != DQU
		&& tmp->next->next->token != SQU && tmp->next->next->token != DLR)))
	{
		ft_putstr_fd(2, "minishell: syntax error near unexpected token\n");
		return (1);
	}
	return (0);
}

int	ft_syntax_error(t_lexer *tmp)
{
	if ((tmp->token == IN || tmp->token == OUT
			|| tmp->token == APPEND || tmp->token == HEREDOC
			|| tmp->token == PIPE) && !(tmp->next))
	{
		ft_putstr_fd(2, "minishell: syntax error near unexpected token `newline'\n");
		return (1);
	}
	if (unexpected_tok(tmp))
		return (1);
	if (tmp->token == PIPE && tmp->next && (tmp->next->token == PIPE
			|| (tmp->next->token == SPC && tmp->next->next
				&& tmp->next->next->token == PIPE)))
	{
		ft_putstr_fd(2, "minishell: syntax error near unexpected token `|'\n");
		return (1);
	}
	return (0);
}

int	ft_parser(t_lexer **head)
{
	t_lexer	*tmp;

	tmp = *head;
	if (tmp && !tmp->prev && tmp->token == PIPE)
	{
		ft_putstr_fd(2, "minishell: syntax error near unexpected token `|'\n");
		return (1);
	}
	while (tmp)
	{
		if (ft_syntax_error(tmp))
			return (1);
		tmp = tmp->next;
	}
	return (0);
}

void sig_handl(int sig)
{
	(void)sig;
	if (waitpid(0, NULL, WNOHANG))
	{
		printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
		g_glb.exit_status = 1;
		
	}
}

void	assign_fd(t_lexer **tmp, int *fd_in, int *fd_out, int fd_herdc)
{
	if ((*tmp)->token == OUT || (*tmp)->token == APPEND)
	{
		if (*fd_out != -2 && *fd_out != -1)
			close(*fd_out);
		*fd_out = (*tmp)->fd;
	}
	else if ((*tmp)->token == IN)
	{
		if (*fd_in != -2 && *fd_in != -1)
			close(*fd_in);
		*fd_in = (*tmp)->fd;
	}
	else if ((*tmp)->token == HEREDOC)
	{
		if (fd_in != 0)
			close(*fd_in);
		*fd_in = fd_herdc;
	}					
	(*tmp) = (*tmp)->next;
}

void	check_redirect_open(t_lexer **tmp, t_lexer *hold, int *fd)
{
	if ((*tmp)->token == SPC)
		(*tmp) = (*tmp)->prev;
	if (((*tmp)->token == IN || (*tmp)->token == OUT
		|| (*tmp)->token == APPEND) && (*tmp)->fd == -3)
	{
		*fd = -3;
		return ; 
	}
	if ((*tmp)->token == IN)
		*fd = open(hold->cmd, O_RDONLY);
	else if ((*tmp)->token == OUT)
		*fd = open(hold->cmd, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if ((*tmp)->token == APPEND)
		*fd = open(hold->cmd, O_WRONLY | O_CREAT | O_APPEND, 0644);
}

void	ft_open_files(t_lexer **temp)
{
	t_lexer	*hold;
	t_lexer	*tmp;
	int		fd;

	tmp = *temp;
	while (tmp && tmp->token != PIPE)
	{
		hold = tmp->next;
		if (hold && hold->token == FILENAME)
		{
			check_redirect_open(&tmp, hold, &fd);
			if (fd == -1)
			{
				ft_putstring_fd(2, hold->cmd, strerror(errno), 1);
				return ;
			}
			if (fd == -3)
			{
				ft_putstring_fd(2, "", " ambiguous redirect", 1);
				return ;
			}
			tmp->fd = fd;
		}
		tmp = hold;
	}
}

void	fd_cmd(t_lexer **head, t_cmd **cmd, int fd_herdc)
{
	int		fd_in;
	int		fd_out;
	t_lexer	*tmp;
	t_cmd	*cmnd;

	cmnd = *cmd;
	tmp = *head;
	while (tmp)
	{
		fd_in = -2;
		fd_out = -2;
		while (tmp && tmp->token != PIPE)
			assign_fd(&tmp, &fd_in, &fd_out, fd_herdc);
		if (cmnd)
		{
			cmnd->fd[0] = fd_in;
			cmnd->fd[1] = fd_out;
			cmnd = cmnd->next;			
		}
		if (tmp && tmp->token == PIPE)
			tmp = tmp->next;
	}
}

void	redirections(t_lexer **head, t_cmd **cmd, int fd_herdc)
{
	t_lexer	*tmp;
	t_lexer	*hold;
	
	tmp = *head;
	while (tmp->next)
		tmp = tmp->next;
	while (tmp)
	{
		while (tmp->prev && tmp->prev->token != PIPE)
			tmp = tmp->prev;
		hold = tmp;
		ft_open_files(&tmp);
		tmp = hold;
		tmp = tmp->prev;
		if (tmp && tmp->token == PIPE)
			tmp = tmp->prev;
	}
	fd_cmd(head, cmd, fd_herdc);
}

int	ft_nbr_cmd(t_lexer *tmp)
{
	int	i;

	i = 0;
	while (tmp && tmp->token != PIPE)
	{
		if (tmp->token == WORD || tmp->token == DQU
			|| tmp->token == SQU)
			i++;
		tmp = tmp->next;
	}
	return (i);
}

void	split_cmd(t_lexer **tmp, int i, t_lexer *hold, char ***cmnd)
{
	int	j;

	j = 0;
	*cmnd = (char **)ft_malloc((sizeof(char *) * (i + 1)));
	if (!(*cmnd))
		return ; //exit
	(*tmp) = hold;
	while ((*tmp) && (*tmp)->token != PIPE)
	{
		if ((*tmp)->token == WORD || (*tmp)->token == DQU
			|| (*tmp)->token == SQU)
			(*cmnd)[j++] = ft_strdup((*tmp)->cmd);
		(*tmp) = (*tmp)->next;
	}
	(*cmnd)[j] = NULL;
}

void	concat_dqu_squ_word(t_lexer *tmp, t_lexer *current)
{
	char		*concatenated;
	t_lexer*	next_tmp;

	concatenated = NULL;
	while (tmp && (tmp->token == WORD || tmp->token == DQU
		|| tmp->token == SQU))
	{
		concatenated = ft_strjoin(current->cmd, tmp->cmd);
		if (!concatenated)
			break ;
		ft_free(current->cmd);
		current->cmd = concatenated;
		next_tmp = tmp->next;
		if (tmp->next)
			next_tmp->prev = current;
		ft_free(tmp->cmd);
		ft_free(tmp);
		tmp = next_tmp;
	}
	current->next = tmp;
}

void	 ft_concat_word_dqu_squ(t_lexer **head)
{
	t_lexer	*tmp;
	t_lexer	*current;

	current = *head;
	while (current && current->next)
	{
		if (current->token == WORD || current->token == DQU
			|| current->token == SQU)
		{
			tmp = current->next;
			concat_dqu_squ_word(tmp, current);
		}
		current = current->next;
	}
}

t_garbage	*get_garbage_collector(void)
{
	static t_garbage	grb;

	return (&grb);
}

void	*ft_malloc(size_t size)
{
	t_garbage	*last;
	void		*allocated;

	allocated = malloc(size);
	if (!allocated)
	{
		write(2, "malloc error!\n", 14);
		exit(1); //call ft_exit
	}
	last = get_garbage_collector();
	while (last->next)
		last = last->next;
	last->next = malloc(sizeof(t_garbage));
	if (!last->next)
	{
		write(2, "malloc error!\n", 14);
		exit(1);
	}
	last->next->ptr = allocated;
	last->next->is_freed = 0;
	last->next->next = NULL;
	return (allocated);
}

void	ft_free(void *ptr)
{
	t_garbage	*grb;
	
	if (!ptr)
		return ;
	grb = get_garbage_collector();
	while (grb)
	{
		if (grb->ptr == ptr)
		{
			if (grb->is_freed == 0)
			{
				free(ptr);
				grb->is_freed = 1;
			}
		}
		grb = grb->next;
	}
}

void	ft_split_pipe(t_lexer **head, t_cmd **cmd)
{
	t_lexer	*tmp;
	t_lexer	*hold;
	char	**cmnd;
	int		i;

	ft_file_redirect(head);
	tmp = *head;
	cmnd = NULL;
	while (tmp)
	{
		hold = tmp;
		i = ft_nbr_cmd(tmp);
		if (i > 0)
			split_cmd(&tmp, i, hold, &cmnd);
		else if (i == 0)
		{
			cmnd = NULL;
			tmp = hold;
			while (tmp && tmp->token != PIPE)
				tmp = tmp->next;
		}
		ft_lstadd_back_cmd(cmd, ft_lstnew_cmd(cmnd));
		if (tmp && tmp->token == PIPE)
			tmp = tmp->next;
	}
}

void	ft_change_dlr_word(t_lexer **head)
{
	t_lexer	*tmp;
	t_lexer	*hold;

	tmp = *head;
	while (tmp)
	{
		hold = tmp->next;
		if (tmp->token == DLR && tmp->next && (tmp->next->token == DQU
			|| tmp->next->token == SQU))
			ft_delete_node(head, tmp);
		else if (tmp->token == DLR && tmp->prev && (tmp->prev->token == WORD
			|| tmp->prev->token == DQU || tmp->prev->token == SQU
			|| tmp->prev->token == SPC))
			tmp->token = WORD;
		if (tmp)
			tmp = hold;
	}
}

void	ft_file_redirect(t_lexer **head)
{
	t_lexer	*tmp;

	ft_change_dlr_word(head);
	ft_concat_word_dqu_squ(head);
	tmp = *head;
	while (tmp)
	{
		if ((tmp->token == IN || tmp->token == OUT
			|| tmp->token == APPEND || tmp->token == HEREDOC)
			&& tmp->next)
		{
			if (tmp->next->token == SPC)
				tmp = tmp->next->next;
			else
				tmp = tmp->next;
			if (tmp && (tmp->token == WORD || tmp->token == DQU
				|| tmp->token == SQU))
				tmp->token = FILENAME;
		}
		if (tmp)
			tmp = tmp->next;
	}
}

void	ft_signal(void)
{
	rl_catch_signals = 0;
	if (signal(SIGINT, sig_handl) == SIG_ERR
		|| signal(SIGQUIT, SIG_IGN) == SIG_ERR
		|| signal(SIGTSTP, SIG_IGN) == SIG_ERR)
	{
		ft_putstr_fd(2, "signal error\n");
		g_glb.exit_status = 1;
	}
}

t_cmd	*parsing(char *input)
{
	char	*s;
	t_lexer	*l;
	t_cmd	*cmd;
	int		fd_in_herdoc;
	
	l = NULL;
	cmd = NULL;
	s = ft_strtrim(input, " \t\v\n\r\f");
	ft_free(input);
	ft_lexer(s, &l);
	ft_free(s);
	if (ft_check_quotes(&l))
		return (ft_lstclear_lex(&l), NULL);
	delete_quotes(&l);
	fd_in_herdoc = ft_heredoc(&l);
	if (fd_in_herdoc == -1)
		return (ft_lstclear_lex(&l), NULL);
	if (ft_parser(&l))
		return (g_glb.exit_status = 258, ft_lstclear_lex(&l), NULL);
	ft_expand(&l);
	ft_split_pipe(&l, &cmd);
	redirections(&l, &cmd, fd_in_herdoc);
	ft_lstclear_lex(&l);
	return (cmd);
}
