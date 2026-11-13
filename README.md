*This project has been created as part of the 42 curriculum by tadupin.*

# get_next_line

## Description

**get_next_line** is a project from the 42 curriculum. Its goal is to write a C function that reads from a file descriptor and returns one line at a time.

The function must return a line ending with `\n` when a newline is present. When the end of the file is reached, it returns the remaining characters if there are any. It returns `NULL` when there is nothing left to read or when an error occurs.

The main challenge is that a single call to `read()` does not necessarily read exactly one line. A read operation can contain part of a line, several lines, or the end of one line followed by the beginning of another. The program therefore has to preserve the characters that belong to the next call.

The project includes:

- **Mandatory part:** read one line at a time from a file descriptor.
- **Bonus part:** manage several file descriptors at the same time, while using only one static variable in `get_next_line()`.

## Instructions

### Compilation

The project does not provide its own executable or `main()` function. The following example programs can be used to test the mandatory and bonus implementations. Keep them as local test files; they are not part of the files required by the 42 subject.

### Mandatory example: `main.c`

This program opens one file and prints it one line at a time using `get_next_line()`.

```c
#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int     fd;
    char    *line;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return (1);
    }
    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        return (1);
    }
    while ((line = get_next_line(fd)) != NULL)
    {
        fputs(line, stdout);
        free(line);
    }
    close(fd);
    return (0);
}
```

Compile and run it with:

```sh
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 \
    main.c get_next_line.c get_next_line_utils.c -o gnl
./gnl test.txt
```

### Bonus example: `main_bonus.c`

This program opens three files and alternates calls between their file descriptors. It demonstrates that each descriptor keeps its own reading state.

```c
#include "get_next_line_bonus.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int     fds[3];
    int     i;
    int     active;
    char    *line;

    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <file1> <file2> <file3>\n", argv[0]);
        return (1);
    }
    i = 0;
    while (i < 3)
    {
        fds[i] = open(argv[i + 1], O_RDONLY);
        if (fds[i] < 0)
        {
            perror(argv[i + 1]);
            while (i > 0)
                close(fds[--i]);
            return (1);
        }
        i++;
    }
    active = 3;
    while (active > 0)
    {
        i = 0;
        while (i < 3)
        {
            if (fds[i] != -1)
            {
                line = get_next_line(fds[i]);
                if (line == NULL)
                {
                    close(fds[i]);
                    fds[i] = -1;
                    active--;
                }
                else
                {
                    printf("FD%d: ", i + 1);
                    fputs(line, stdout);
                    free(line);
                }
            }
            i++;
        }
    }
    return (0);
}
```

Compile and run it with:

```sh
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 \
    main_bonus.c get_next_line_bonus.c get_next_line_utils_bonus.c -o gnl_bonus
./gnl_bonus test1.txt test2.txt test3.txt
```

These commands assume that `BUFFER_SIZE` is guarded in the relevant header so it can be overridden by the compiler. Otherwise, set `BUFFER_SIZE` in the header before compiling. You can replace `42` with another positive value to test different read sizes.

Do not compile the mandatory and bonus implementations into the same executable: both provide a function named `get_next_line()`.

### Expected files

The source files submitted for the mandatory part are:

```text
get_next_line.c
get_next_line.h
get_next_line_utils.c
```

The source files submitted for the bonus part are:

```text
get_next_line_bonus.c
get_next_line_bonus.h
get_next_line_utils_bonus.c
```

The example files `main.c` and `main_bonus.c` are local test programs only. Do not include them in the submitted project files.

## Algorithm and technical choices

### Reading one line at a time

The implementation uses a temporary read buffer and a persistent storage area to separate the line being returned from the characters that must be kept for the next call.

The general algorithm is:

1. **Check the saved data first.** Before calling `read()`, inspect the characters left over from the previous call. They may already contain a complete line.
2. **Read more data when needed.** If the saved data does not contain a newline, call `read()` and append the new characters to the current line.
3. **Look for the newline.** If `\n` is found, append the characters up to and including it to the line being built.
4. **Preserve the remaining characters.** Characters located after the newline are copied into persistent storage. They are not lost; they will be processed by the next call.
5. **Grow the line when necessary.** The line can be longer than `BUFFER_SIZE`, so its allocation is enlarged when more capacity is needed. The implementation uses a dynamically allocated line rather than assuming that one read is enough.
6. **Handle end-of-file and errors.** At EOF, return the final non-empty line if one remains. Once a file descriptor has no more data to return, release the memory associated with it. On an error, release the temporary and saved allocations associated with the affected state.

This approach works with different values of `BUFFER_SIZE`, including values smaller than a line or values large enough to read multiple lines at once.

### Bonus: handling multiple file descriptors

The bonus implementation uses **one static variable** in `get_next_line()`: a pointer to the head of a linked list.

Each node in that list stores the reading state of one file descriptor:

- `fd`: the file descriptor identifying the input stream;
- `stored`: characters already read but not yet returned;
- `eof`: whether the end of that file has been reached;
- `next`: a pointer to the next node in the list.

When `get_next_line(fd)` is called, the function searches the list for the node whose `fd` matches the argument. If no matching node exists, it creates and initializes one. The reading algorithm then uses only that node's saved data and file descriptor.

When a newline is found, the characters after it remain in that node's `stored` buffer. A later call for the same descriptor can resume from that saved state. Calls for other descriptors use their own nodes, so interleaving calls such as `get_next_line(fd1)`, `get_next_line(fd2)`, and then `get_next_line(fd1)` does not mix their buffered data.

When a descriptor reaches EOF and has no saved characters left, its node is removed from the linked list and its allocated memory is freed. Removing only the matching node preserves the state of every other descriptor.

A linked list is suitable here because the number of descriptors being read can vary. It also allows the bonus implementation to keep all persistent state behind a single static pointer instead of using one static variable per descriptor.

## Resources

- **42 Get Next Line subject:** the project specification provided by the 42 curriculum. It defines the expected behavior, permitted functions, mandatory files, and bonus requirements.
- **Linux man-pages — `read(2)`:** https://man7.org/linux/man-pages/man2/read.2.html
- **Linux man-pages — `malloc(3)` and `free(3)`:** https://man7.org/linux/man-pages/man3/malloc.3.html
- **Linux man-pages — `open(2)`:** https://man7.org/linux/man-pages/man2/open.2.html
- **Linux man-pages — `close(2)`:** https://man7.org/linux/man-pages/man2/close.2.html

### AI usage

AI assistance was used as a learning and debugging aid during this project. It helped explain the behavior of `read()`, dynamic memory allocation, pointers, and linked lists; discuss how to preserve independent reading states for multiple file descriptors; and identify possible issues in buffer management, node removal, and memory handling. AI was also used to help structure and write this README.
