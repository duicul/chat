#include <sys/types.h>         
#include <sys/socket.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
int sockfd;

int setaddr(struct sockaddr_in *addr,char inaddr[],u_int32_t a,short sinport) {
struct hostent *h;
memset((void*)addr,0,sizeof(addr));
if(inaddr!=NULL)
{h = gethostbyname(inaddr);
if(h!=NULL)
addr->sin_addr.s_addr=*(u_int32_t*)h->h_addr_list[0];
}
else
addr->sin_addr.s_addr=a;
addr->sin_family = AF_INET ;
addr->sin_port = htons(sinport);
return 0 ;
}

void end_proc()
{char aux[]="/exit";
write(sockfd,aux,strlen(aux));
exit(0);
}

void start_chat(char aux[],char exec[])
{char val[100],pip[10];
strcpy(val,"");
strcat(val,exec);
strcat(val,"\ ");
strcat(val,aux);
printf("%s",val);
execlp("gnome-terminal","gnome-terminal","-e",val,NULL);}

int main(int argv,char *args[])
{char text[100];
int j,pid;
signal(SIGQUIT,end_proc);
signal(SIGINT,end_proc);
signal(SIGHUP,end_proc);
signal(SIGTERM,end_proc);
struct sockaddr_in loc,rem;
if((pid=fork())==0)
{printf("args2 %s\n",args[2]);
start_chat(args[2],args[3]);
exit(1);
}
setaddr(&loc,NULL,INADDR_ANY,0);
if (sockfd=socket(PF_INET,SOCK_STREAM,0) < 0)
   {printf ("error1 ...\n"); /*exit(1);*/}
/*if (bind(sockfd,(struct sockaddr *)&loc,sizeof(loc))<0)
   {printf ("error2 ...\n"); exit(1);}*/
setaddr(&rem,args[1],0,atoi(args[2]));
printf("con %d\n",connect(sockfd,(struct sockaddr *)&rem,sizeof(rem)));
do{
fgets(text,100,stdin);
printf("sock %d\n",sockfd);
write(sockfd,text,strlen(text));
}while(strcmp(text,"/exit\n")!=0);
return 0;}
