/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tadupin <tadupin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/05 14:18:06 by tadupin           #+#    #+#             */
/*   Updated: 2026/11/13 17:50:51 by tadupin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <stddef.h>
# include <unistd.h>
# include <stdlib.h>
# include <fcntl.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1
# endif

typedef struct s_gnl
{
	int				fd;
	int				eof;
	char			*stored;
	struct s_gnl	*next;
}					t_gnl;

char	*get_next_line(int fd);
char	*ft_strchr(const char *s, int c, int *j);
size_t	ft_strlcpy(char *dest, const char *src, size_t size);
char	*alloc_upsize(char *s1, char *s2, size_t *capacity, int i);
char	*capacity_update(size_t new_capacity, char *s1, char *s2, size_t lens2);
size_t	ft_strlen(const char *s);

#endif