#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <termios.h>

int port;
int sd;

void detect_CTRLC(int signum)
{
    printf("\n\n=================================================================================\n\nYou left the game.\n\nConnection closed.\n\n");
    close(sd);
    exit(signum);
}

void clean_stdin()
{
        int stdin_copy = dup(STDIN_FILENO);
        tcdrain(stdin_copy);
        tcflush(stdin_copy, TCIFLUSH);
        close(stdin_copy);
}

int main (int argc, char *argv[])
{
    signal(SIGINT, detect_CTRLC);

    struct sockaddr_in server;
    char msg[800];
    char msgrasp[100];
    int ready=0;

    if (argc != 3)
    {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

    port = atoi (argv[2]);

    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
      {
        perror ("Eroare la socket().\n");
        return errno;
      }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons (port);
    
    if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
        perror ("Eroare la connect().\n");
        return errno;
    }
    else
    {
        printf("Connected to the server.\n\n");
    }

    struct termios term;
    tcgetattr(STDIN_FILENO, &term);

    while(1)
    {
        term.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &term);
        
        strcpy(msg,"\0");
        bzero (msg, 800);
        
        if (read (sd, msg, sizeof(msg)) < 0)
        {
            perror ("Eroare la read() de la server.\n");
            return errno;
        }
        else
        {
            if(strstr(msg,"CHECK_CONNECTION")==0)
            {
                write(0,msg,strlen(msg));
            }
        }

        if(strstr(msg,"notations")!=0)
        {
            continue;
        }
        else if(strstr(msg,"YOU WON")!=0)
        {
            sleep(2);
            printf("\nClosing the connection...\n\n");
            sleep(2);
            break;
        }
        else if(strstr(msg,"disconnected")!=0)
        {
            sleep(2);
            printf("\nClosing the connection...\n\n");
            sleep(2);
            break;
        }
        else if(strstr(msg,"YOU LOST")!=0)
        {
            sleep(2);
            printf("\nClosing the connection...\n\n");
            sleep(2);
            break;
        }
        else if(strstr(msg,"wait until")!=0)
        {
            continue;
        }
        else if(strstr(msg,"CHECK_CONNECTED")!=0)
        {
            continue;
        }
        else if(strstr(msg,"__")!=0)
        {
            continue;
        }
        else if(strstr(msg,"turn to reply")!=0)
        {
            continue;
        }
        else if(strstr(msg,"give an answer")!=0)
        {
            continue;
        }
        else if(strstr(msg,"The game continues")!=0)
        {
            continue;
        }
        else if(strstr(msg,"declined")!=0)
        {
            continue;
        }
        else if(strstr(msg,"already requested")!=0)
        {
            continue;
        }
        else if(strstr(msg,"NEVER GIVE UP")!=0)
        {
            continue;
        }
        else if(strstr(msg,"CHECK_CONNECTION")!=0)
        {
            continue;
        }
        else if(strstr(msg,"YOU SURRENDERED")!=0)
        {
            sleep(2);
            printf("\nClosing the connection...\n\n");
            sleep(2);
            break;
        }
        else if(strstr(msg,"DRAW BY MUTUAL AGREEMENT")!=0)
        {
            sleep(2);
            printf("\nClosing the connection...\n\n");
            sleep(2);
            break;
        }
        else
        {
            bzero (msgrasp, 100);

            term.c_lflag |= ECHO;
            tcsetattr(STDIN_FILENO, TCSANOW, &term);

            clean_stdin();

            read (0, msgrasp, 100);

            if(strstr(msg,"when you're ready")!=0)
                printf("READY!\n\n");
            
            if (write (sd, msgrasp, sizeof(msgrasp)) <= 0)
            {
                perror ("[client]Eroare la write() spre server.\n");
                return errno;
            }
        }
    }
    
    close(sd);    
}
