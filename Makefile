# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: osarsar <osarsar@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/09/06 16:33:21 by osarsar           #+#    #+#              #
#    Updated: 2023/09/06 16:33:21 by osarsar          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRCS =	main.c		\
		main_utils.c	\
		execution/minishell.c	\
		execution/exec_path.c	\
		execution/split.c		\
		execution/minishell_utils.c	\
		execution/create_node.c	\
		execution/minishell_utils2.c	\
		parsing/grb_collector_linkd_list.c \
		parsing/utils1.c			\
		parsing/utils2.c			\
		parsing/linked_list_cmd.c	\
		parsing/linked_list_env.c		\
		parsing/linked_list_lexer.c	\
		parsing/utils3.c	\
		parsing/utils4.c	\
		parsing/utils5.c	\
		parsing/utils6.c	\
		parsing/utils7.c	\
		parsing/utils8.c	\
		parsing/utils9.c	\
		parsing/utils10.c	\
		parsing/her_doc.c	\
		parsing/her_doc_utils.c	\
		parsing/expand.c	\
		parsing/split_pipe.c	\
		parsing/syntax_error.c	\
		parsing/redirection.c	\
		execution/builtins.c			\
		execution/builtins_ex.c			\
		execution/builtins_utils.c			\
		execution/builtins_utils2.c	\
		execution/key_and_value.c			\
		execution/builtins_utils_ex.c		\
		execution/export_list.c		\
		execution/pipe.c	\
		parsing/split_ls.c	\
		parsing/split_white_spc.c \
		parsing/free_array.c

READLINE = $(shell brew --prefix readline)

NAME = mini.a

FLAGS = -Wall -Werror -Wextra -I$(READLINE)/include

%.o : %.c minishell.h
	@echo "Compiling: $<"
	@cc $(FLAGS)  -c $< -o $@

$(NAME) : $(SRCS:.c=.o)
	@ar -r $(NAME) $(SRCS:.c=.o)
	@cc main.c  $(FLAGS)  $(NAME) -lreadline  -L$(READLINE)/lib -o minishell

all : $(NAME)

clean :
	@rm -f $(SRCS:.c=.o)

fclean : clean
	@rm -f mini.a
	@rm -f minishell

re : fclean all

.PHONY : all clean fclean re