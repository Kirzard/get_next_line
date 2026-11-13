/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tadupin <tadupin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 20:24:47 by tadupin           #+#    #+#             */
/*   Updated: 2026/11/12 20:27:43 by tadupin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

size_t	ft_strlen(const char *s)
{
	size_t	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
		i++;
	return (i);
}

char	*ft_strchr(const char *s, int c, int *j)
{
	int	i;

	i = 0;
	while (s[i] != (unsigned char)c && s[i])
		i++;
	if (s[i] == (unsigned char)c)
	{
		*j = i;
		return ((char *)&s[i]);
	}
	return (NULL);
}

size_t	ft_strlcpy(char *dest, const char *src, size_t size)
{
	size_t	len;

	len = 0;
	if (size != 0)
	{
		while (src[len] && len < size - 1)
		{
			dest[len] = src[len];
			len++;
		}
		dest[len] = 0;
	}
	while (src[len])
		len++;
	return (len);
}

char	*capacity_update(size_t new_capacity, char *s1, char *s2, size_t lens2)
{
	char	*tmp;
	size_t	lens1;

	lens1 = ft_strlen(s1);
	tmp = malloc(sizeof(char) * new_capacity);
	if (!tmp)
		return (NULL);
	if (s1)
	{
		ft_strlcpy(tmp, s1, lens1 + 1);
		free(s1);
	}
	if (s2)
		ft_strlcpy(tmp + lens1, s2, lens2 + 1);
	return (tmp);
}

char	*alloc_upsize(char *s1, char *s2, size_t *capacity, int i)
{
	char	*tmp;
	size_t	new_capacity;
	size_t	lens1;
	size_t	lens2;

	lens1 = ft_strlen(s1);
	lens2 = ft_strlen(s2);
	if (s2 && i >= 0)
		lens2 = i + 1;
	if (!s1 && !s2)
		return (NULL);
	new_capacity = *capacity;
	if (new_capacity == 0)
		new_capacity = 128;
	while (lens1 + lens2 + 1 > new_capacity)
		new_capacity *= 2;
	if (new_capacity > *capacity)
	{
		*capacity = new_capacity;
		tmp = capacity_update(new_capacity, s1, s2, lens2);
		return (tmp);
	}
	if (s2)
		ft_strlcpy(s1 + lens1, s2, lens2 + 1);
	return (s1);
}
