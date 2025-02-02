//this is some code from stackoverflow to use for chroot thing
//will leave it here to later examing and utilize somewhere alse
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int pipefd[2];
    pid_t cpid;
    char buf;

    pipe(pipefd);  // create the pipe
    cpid = fork(); // duplicate the current process
    if (cpid == 0) // if I am the child then
    {
        close(pipefd[1]);                    // close the write-end of the pipe, I'm not going to use it
        while (read(pipefd[0], &buf, 1) > 0) // read while EOF
            write(1, &buf, 1);
        write(1, "\n", 1);
        close(pipefd[0]); // close the read-end of the pipe
        exit(EXIT_SUCCESS);
    }
    else // if I am the parent then
    {
        close(pipefd[0]);                           // close the read-end of the pipe, I'm not going to use it
        write(pipefd[1], argv[1], strlen(argv[1])); // send the content of argv[1] to the reader
        close(pipefd[1]);                           // close the write-end of the pipe, thus sending EOF to the reader
        wait(NULL);                                 // wait for the child process to exit before I do the same
        exit(EXIT_SUCCESS);
    }
    return 0;
}