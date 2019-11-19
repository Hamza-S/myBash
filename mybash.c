#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

const char* namedPipe = "namedPipe";
char buffer[10000];
char command[100];
char prgrm[100];
char yes[] = "Y";
char no[] = "N";
char *split;
char *execargs[100];
int fifofd, count;

int main()
{
  mkfifo(namedPipe,0644);
  printf("Enter reguar linux command: ");
  scanf("%[^\n\r]", command);
  split = strtok(command, " ");
  while (split != NULL)
  {
    execargs[count++] = split;
    split = strtok(NULL, " ");
  }
  strcpy(prgrm, "/bin/");
  strcat(prgrm, execargs[0]);

  pid_t myShell = fork(); //System call to create child process

  if (myShell > 0)
  {
    //the parent process
    fifofd = open(namedPipe, O_RDONLY); //Pipe must be open in parent to recieve output from child
    if (fifofd < 0)
    {
       perror("open error");
       exit(1);
    }
    else
    {
      printf("..working on request..\n");
      pid_t child_pid = wait(NULL); //wait for child process to finish executing
      if (myShell == child_pid) //If the child process terminated succesfully then continue
      {
        printf("..output is ready. Display it now? [Y/N]:");
        char userDisplay[2];
        scanf("%s", userDisplay);
        if (strcmp(userDisplay , yes) == 0) //If the user selects 'Y' then display the output
        {
          read(fifofd, buffer, sizeof(buffer));
          printf("%s ",buffer );
          close(fifofd); //error check
        }
        else
        {
          printf("Exiting\n");
        }
      }
      else
      {
        perror("error");
        exit(1);
      }
    }
  }

  else if (myShell == 0)
  {
    fifofd = open(namedPipe, O_WRONLY);
    dup2(fifofd,1); //redirect contents of STDOUT into namedPipe to be read by parent process by
    close(fifofd);
    execv(prgrm, execargs);
    exit(1);
  }

  else if (myShell < 0)
  {
    perror("Error creating process");
    exit(-1);
  }

  return 0;
}
