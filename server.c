#include <sys/types.h>         
#include <sys/socket.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#define BUFF_SIZE 200
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

void readline(int sock,char buf[])
{char c;
int i=0;
while(read(sock,&c,1)>0&&i<BUFF_SIZE)
{printf("%c",c);
buf[i++]=c;
if(c=='\n')
break;
}
buf[i]='\0';
}

int main(int argv,char *args[])
{char name[30],line[200];
int j;
printf("%s\n",args[1]);
fgets(name,30,stdin);
struct sockaddr_in loc,rem;
socklen_t rlen;
setaddr(&loc,NULL,INADDR_ANY,atoi(args[1]));
int sockfd, newsockfd;
printf("Messsages:\n");
if ((sockfd=socket(AF_INET,SOCK_STREAM,0)) < 0)
   {printf ("error1 ...\n"); exit(1);}
printf("1%s\n",strerror(errno));
if ((bind(sockfd,(struct sockaddr *)&loc,sizeof(loc)))<0)
   {printf ("error2 ...\n"); exit(1);}
printf("2%s\n",strerror(errno));
if (listen(sockfd,10) < 0)
   {printf ("error3 ...\n"); exit(1);}
printf("3%s\n",strerror(errno));
for (;;)
{newsockfd=accept(sockfd,(struct sockaddr *)&rem,&rlen);/* blocare pentru asteptare cereri*/
 printf("4%s\n",strerror(errno)); 
if (newsockfd < 0) 
   printf ("error4 ...\n"); 
  else
  {
  if(fork()==0)
  {readline(newsockfd,line); /*deservire cerere*/
  printf("line: %s",line);
if(strcmp(line,"~exit\n")!=0)
 printf("Finish\n");
  close (newsockfd);  
}}
}
return 0;}
