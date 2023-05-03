#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>

#define SH_TOK_BUFSIZE 64
#define SH_TOK_DELIM " \t\r\n\a"

pipeFunction(char **args)
{ // pipe file descriptors 0 read 1 write
    int fd[2], file_desc_d = 0, idx = 0, pid;
    do
    {

        int tempIdx = 0;
        char **localCmd;
        localCmd = malloc(SH_TOK_BUFSIZE * sizeof(char *));
        for (int i = 0; i < SH_TOK_BUFSIZE; i++)
        {
            localCmd[i] = NULL;
        }
        do
        {
            if (args[idx + 1] != NULL && strcmp(args[idx + 1], "|") == 0)
            {
                localCmd[tempIdx++] = args[idx++];
                break;
            }
            else if (strcmp(args[idx], "<") == 0)
            {
                idx++;
            }
            else
            {
                localCmd[tempIdx++] = args[idx++];
            }
        } while (args[idx] != NULL);
        localCmd[tempIdx] = NULL;
        pipe(fd);

        if (pid = fork() == 0)
        {
            dup2(file_desc_d, 0);
            if (args[idx + 1] != NULL)
            {
                dup2(fd[1], 1);
            }
            close(fd[0]);
            execvp(localCmd[0], localCmd);
            exit(1);
        }
        else
        {
            wait(0);
            close(fd[1]);
            file_desc_d = fd[0];
            idx++;
        }
    } while (args[idx] != NULL);
}

void executeCommand(char **cmd, char *fileName)
{
    int pid = fork();
    if (pid == 0)
    { // child process
        if (cmd != NULL)
        {
            close(STDOUT_FILENO);
            open(fileName, O_CREAT | O_WRONLY, S_IRWXU);
            execvp(cmd[0], cmd);
            printf("error occured\n");
        }
    }
    else
    { // parent process
        pid = wait(0);
    }
}

int sh_launch(char **args)
{
    int pid = fork();

    if (pid == 0)
    {
        execvp(args[0], args);
        printf("error occured");
    }
    else
    {
        pid = wait(0);
    }

    return 1;
}
int sh_execute(char **args)
{
    int idx = 0, cmdIdx = 0;
    if (args[0] == NULL)
    {
        return 1; // An empty command was entered.
    }
    int isPipe = 0;
    do
    {
        if (strcmp(args[idx], "|") == 0)
        {
            isPipe = 1;
        }
        idx++;
    } while (args[idx] != NULL);

    if (isPipe == 1)
    {
        pipeFunction(args);
    }
    else
    {
        idx = 0, cmdIdx = 0;
        char **cmd = malloc(SH_TOK_BUFSIZE * sizeof(char *));
        while (args[idx] != NULL)
        {
            if (strcmp(args[idx], ">") == 0)
            {
                // printf("%d -1\n ",idx);
                cmd[cmdIdx] = NULL;
                executeCommand(cmd, args[++idx]);
                idx++;
            }
            else if (strcmp(args[idx], ";") == 0)
            {
                cmd[cmdIdx] = NULL;
                sh_launch(cmd);
                free(cmd);
                cmdIdx = 0;
                idx++;
                cmd = malloc(SH_TOK_BUFSIZE * sizeof(char *));
            }
            else
            {
                cmd[cmdIdx] = args[idx];
                cmdIdx++;
                idx++;
                if (args[idx] == NULL)
                {
                    cmd[cmdIdx] = NULL;
                    sh_launch(cmd);
                    free(cmd);
                }
            }
        }
    }
    return 1; 
}

char **sh_split_line(char *line)
{
    int bufsize = SH_TOK_BUFSIZE;
    int idx = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token, **tokens_backup;

    if (!tokens)
    {
        fprintf(stderr, "sh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SH_TOK_DELIM);
    while (token != NULL)
    {
        tokens[idx] = token;
        idx++;

        if (idx >= bufsize)
        {
            bufsize += SH_TOK_BUFSIZE;
            tokens_backup = tokens;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens)
            {
                free(tokens_backup);
                fprintf(stderr, "sh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, SH_TOK_DELIM);
    }
    tokens[idx] = NULL;
    return tokens;
}

char *sh_read_line(void)
{
    char *line = NULL;
    size_t bufsize = 0; // have getline allocate a buffer for us

    if (getline(&line, &bufsize, stdin) == -1)
    {
        if (feof(stdin)) // EOF
        {
            fprintf(stderr, "EOF\n");
            exit(EXIT_SUCCESS);
        }
        else
        {
            fprintf(stderr, "Value of errno: %d\n", errno);
            exit(EXIT_FAILURE);
        }
    }
    return line;
}

void sh_loop(void)
{
    char *line;
    char **args;
    int status;

    do
    {
        printf("utsh$ ");
        line = sh_read_line();
        args = sh_split_line(line);
        status = sh_execute(args);

        free(line);
        free(args);
    } while (status);
}

int main(int argc, char **argv)
{
    sh_loop();
    return EXIT_SUCCESS;
}