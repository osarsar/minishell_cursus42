/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils1.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osarsar <osarsar@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 11:35:50 by isidki            #+#    #+#             */
/*   Updated: 2023/09/05 03:14:03 by osarsar          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	char_in_set(char *set, char c)
{
	while (*set)
	{
		if (*set == c)
			return (1);
		set++;
	}
	return (0);
}

char	*ft_substr(char *s, int start, int len)
{
	char	*str;
	int		i;

	i = 0;
	if (!s)
		return (NULL);
	str = (char *)ft_malloc(sizeof(char) * (len + 1));
	if (!str)
		return (NULL);
	if (len > ft_strlen(s + start))
		len = ft_strlen(s + start);
	while (i < len)
	{
		str[i] = s[start];
		i++;
		start++;
	}
	str[i] = '\0';
	return (str);
}

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	size_t	i;

	i = 0;
	if (dst == src)
		return (dst);
	while (i < n)
	{
		*(char *)(dst + i) = *(char *)(src + i);
		i++;
	}
	return ((void *) dst);
}

static int	count_digit(int n)
{
	int	nbr;

	nbr = 0;
	if (n < 0)
		nbr++;
	while (n)
	{
		nbr++;
		n /= 10;
	}
	return (nbr);
}

char	*ft_itoa(int n)
{
	char		*str;
	int			i;
	long int	nb;

	nb = n;
	i = count_digit(n);
	if (n == 0)
		return (ft_strdup("0"));
	str = ft_malloc(sizeof(char) * (i + 1));
	if (!str)
		return (NULL);
	if (nb < 0)
		nb *= -1;
	str[i] = '\0';
	while (--i >= 0)
	{
		str[i] = (nb % 10) + 48;
		nb /= 10;
	}
	if (n < 0)
		str[0] = '-';
	return (str);
}
