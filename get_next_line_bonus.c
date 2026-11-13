/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tadupin <tadupin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 14:41:20 by tadupin           #+#    #+#             */
/*   Updated: 2026/11/13 17:12:03 by tadupin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include "get_next_line_bonus.h"

static t_gnl	**manage_list(t_gnl **list, int fd)
{
	t_gnl	*new_elem;

	if (fd < 0 || BUFFER_SIZE < 1)
		return (NULL);
	while (*list && (*list)->fd != fd)
		list = &(*list)->next;
	if (*list)
		return (list);
	new_elem = malloc(sizeof(t_gnl));
	if (!new_elem)
		return (NULL);
	new_elem->stored = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!new_elem->stored)
	{
		free(new_elem);
		return (NULL);
	}
	new_elem->stored[0] = '\0';
	new_elem->fd = fd;
	new_elem->eof = 0;
	new_elem->next = NULL;
	*list = new_elem;
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

static char	*clean_node(char *line, int newline, t_gnl **current)
{
	t_gnl	*node;
	char	*tmp;

	if (line && newline != -1)
		tmp = malloc(sizeof(char) * (ft_strlen(line) + 1));
	if ((line && newline != -1) && !tmp)
		newline = -1;
	else if ((line && newline != -1) && tmp)
	{
		ft_strlcpy(tmp, line, ft_strlen(line) + 1);
		free(line);
		line = tmp;
	}
	if (newline == -1 || ((*current)->eof && !(*current)->stored[0]))
	{
		if (newline == -1)
			free(line);
		node = *current;
		*current = node->next;
		free(node->stored);
		free(node);
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
	if (!manage_list(&list, fd))
		return (NULL);
	line = NULL;
	buffer = NULL;
	newline = read_stored(*manage_list(&list, fd), &line, &capacity);
	if (newline == 0 && !(*manage_list(&list, fd))->eof)
	{
		buffer = malloc(sizeof(char) * (BUFFER_SIZE + 1));
		if (!buffer)
			newline = -1;
	}
	while (newline == 0 && !(*manage_list(&list, fd))->eof)
		newline = read_line(*manage_list(&list, fd), &line, &capacity,
				buffer);
	free(buffer);
	return (clean_node(line, newline, manage_list(&list, fd)));
}

/*#include <stdio.h>

int	main(void)
{
	int		fd1;
	int		fd2;
	int		fd3;
	char	*line;

	fd1 = open("test1.txt", O_RDONLY);
	fd2 = open("test2.txt", O_RDONLY);
	fd3 = open("test3.txt", O_RDONLY);

	if (fd1 < 0 || fd2 < 0 || fd3 < 0)
		return (1);

	printf("=== Lecture alternee ===\n");

	line = get_next_line(fd1);
	printf("FD1 : %s", line);
	free(line);

	line = get_next_line(fd2);
	printf("FD2 : %s", line);
	free(line);

	line = get_next_line(fd3);
	printf("FD3 : %s", line);
	free(line);

	line = get_next_line(fd1);
	printf("FD1 : %s", line);
	free(line);

	line = get_next_line(fd2);
	printf("FD2 : %s", line);
	free(line);

	line = get_next_line(fd3);
	printf("FD3 : %s", line);
	free(line);

	line = get_next_line(fd1);
	printf("FD1 : %s\n", line);
	free(line);

	line = get_next_line(fd2);
	printf("FD2 : %s\n", line);
	free(line);

	line = get_next_line(fd3);
	printf("FD3 : %s\n", line);
	free(line);

	line = get_next_line(fd1);
	printf("FD1 : %s\n", line);
	free(line);

	line = get_next_line(fd2);
	printf("FD2 : %s\n", line);
	free(line);

	line = get_next_line(fd3);
	printf("FD3 : %s\n", line);
	free(line);

	close(fd1);
	close(fd2);
	close(fd3);

	return (0);
}*/