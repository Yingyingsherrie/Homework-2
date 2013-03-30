/* -*- compile-command: "gcc chat.c -g -o chat -lpthread" -*- */

#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

#define BUFLEN  1024

void myperror(const char *msg)
{
    perror(msg);
    exit(1);
}

int openfifo(const char *path, int flags)
{
    /* if the fifo doesn't yet exist, create a fifo */
    if (access(path, F_OK)) {
	mkfifo(path, 0600);  
    }
    /* open the fifo, quit on error */
    int fd = open(path, flags);
    if (fd < 0)
	myperror("cannot open pipe");
    return fd;
}

/* stdin to pipe */
void *thread_a2b(void *_path)
{
    char buf[BUFLEN];
    int n, m, fd;

    fd = openfifo((const char *)_path, O_WRONLY);
    /* read strings from stdin(0) into the buffer pointed to by buf */
    while ((n = read(0, buf, BUFLEN)) > 0) {
	/* write strings in the buffer to fifo */
	m = write(fd, buf, n);
	if (m < n) {
	    fprintf(stderr, "error writing to fifo");
	    return NULL;
	}
    }
    close(fd);
    return NULL;
}

/* pipe to stdout */
void *thread_b2a(void *_path)
{
    char buf[BUFLEN];
    int n, m, fd;

    fd = openfifo((const char*)_path, O_RDONLY);
    /* read strings from the fifo into the buffer pointed to by buf */
    while ((n = read(fd, buf, BUFLEN)) > 0) {
	 /* write strings in the buffer to stdout(1) */
	m = write(1, buf, n);
	if (m < n) {
	    fprintf(stderr, "error writing to stdout");
	    return NULL;
	}
    }
    close(fd);
    return NULL;
}

int main(int argc, const char *argv[])
{
    char a2b[PATH_MAX], b2a[PATH_MAX];

    /* check usage */
    if (argc != 3) {
	fprintf(stderr, "usage: %s <me> <friend>", argv[0]);
	return 1;
    }
    
    snprintf(a2b, PATH_MAX, "%s2%s", argv[1], argv[2]);
    snprintf(b2a, PATH_MAX, "%s2%s", argv[2], argv[1]);
    
    pthread_t h1, h2;

    /* create a thread h1: a sends message to b */
    pthread_create(&h1, NULL, thread_a2b, a2b);
 
    /* create a thread h2: a reads message from b */
    pthread_create(&h2, NULL, thread_b2a, b2a);

    pthread_join(h1, NULL);
    pthread_join(h2, NULL);
    return 0;
}
