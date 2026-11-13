/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tadupin <tadupin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 14:41:20 by tadupin           #+#    #+#             */
/*   Updated: 2026/11/13 16:59:51 by tadupin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include "get_next_line.h"

static t_gnl	*manage_list(t_gnl *list, int fd)
{
	if (!list)
	{
		list = malloc(sizeof(t_gnl));
		if (!list)
			return (NULL);
		list->stored = malloc(sizeof(char) * (BUFFER_SIZE + 1));
		if (!list->stored)
		{
			free(list);
			return (NULL);
		}
		list->fd = fd;
		list->eof = 0;
		list->stored[0] = '\0';
		list->next = NULL;
	}
	return (list);
}

static int	read_line(t_gnl *list, char **line, size_t *capacity,
		char *buffer)
{
	int		bytesread;
	char	*sep;
	char	*tmp;
	int		i;

	bytesread = read(list->fd, buffer, BUFFER_SIZE);
	if (bytesread <= 0)
	{
		if (bytesread == 0)
			list->eof = 1;
		return (bytesread);
	}
	buffer[bytesread] = '\0';
	i = -1;
	sep = ft_strchr(buffer, '\n', &i);
	if (i != -1)
		ft_strlcpy(list->stored, sep + 1, BUFFER_SIZE + 1);
	tmp = alloc_upsize(*line, buffer, capacity, i);
	if (!tmp)
		return (-1);
	*line = tmp;
	return (i != -1);
}

static int	read_stored(t_gnl *list, char **line, size_t *capacity)
{
	char	*sep;
	char	*tmp;
	int		i;

	*capacity = 0;
	if (!list->stored[0])
		return (0);
	i = -1;
	sep = ft_strchr(list->stored, '\n', &i);
	tmp = alloc_upsize(*line, list->stored, capacity, i);
	if (!tmp)
		return (-1);
	*line = tmp;
	if (i != -1)
		ft_strlcpy(list->stored, sep + 1, BUFFER_SIZE + 1);
	else
		list->stored[0] = '\0';
	return (i != -1);
}

static char	*crop_extra_space(char *line, int newline, t_gnl **list)
{
	char	*tmp;

	if (line && newline != -1)
	{
		tmp = malloc(sizeof(char) * (ft_strlen(line) + 1));
		if (!tmp)
			newline = -1;
		else
		{
			ft_strlcpy(tmp, line, ft_strlen(line) + 1);
			free(line);
			line = tmp;
		}
	}
	if (newline == -1 || ((*list)->eof && !(*list)->stored[0]))
	{
		if (newline == -1)
			free(line);
		free((*list)->stored);
		free(*list);
		*list = NULL;
		if (newline == -1)
			return (NULL);
	}
	return (line);
}

char	*get_next_line(int fd)
{
	char			*line;
	char			*buffer;
	int				newline;
	size_t			capacity;
	static t_gnl	*list = NULL;

	if (fd < 0 || BUFFER_SIZE < 1)
		return (NULL);
	list = manage_list(list, fd);
	if (!list)
		return (NULL);
	line = NULL;
	buffer = NULL;
	newline = read_stored(list, &line, &capacity);
	if (newline == 0 && !list->eof)
	{
		buffer = malloc(sizeof(char) * (BUFFER_SIZE + 1));
		if (!buffer)
			newline = -1;
	}
	while (newline == 0 && !list->eof)
		newline = read_line(list, &line, &capacity, buffer);
	free(buffer);
	line = crop_extra_space(line, newline, &list);
	return (line);
}
/*#include <stdio.h>
int	main(void)
{
	int		fd;
	char	*line;
	int		i;

	fd = open("test.txt", O_RDONLY);
	if (fd == -1)
		return (1);
	i = 1;
	line = get_next_line(fd);
	while (line)
	{
		printf("LIGNE %d : [%s]\n", i++, line);
		free(line);
		line = get_next_line(fd);
	}
	close(fd);
	
	return (0);
}*/
