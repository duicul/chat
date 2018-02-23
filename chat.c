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
while(read(sock,&c,1)>0&&i<strlen(buf))
{buf[i++]=c;
}
buf[i]='\0';
}

int main(int argv,char *args[])
{char name[30],line[200];
int j,t=getpid();
printf("pid2 %d\n",t);
printf("%s\n",args[1]);
//gets(name);
struct sockaddr_in loc,rem;
socklen_t rlen;
setaddr(&loc,NULL,INADDR_ANY,atoi(args[1]));
int sockfd, newsockfd;
if ((sockfd=socket(AF_INET,SOCK_STREAM,0)) < 0)
   {printf ("error1 ...\n"); /*exit(1);*/}
if (bind(sockfd,(struct sockaddr *)&loc,sizeof(loc))<0)
   {printf ("error2 ...\n"); /*exit(1);*/}
if (listen(sockfd,10) < 0)
   {printf ("error3 ...\n"); /*exit(1);*/}
printf("Message board\n");
for (;;)
{newsockfd=accept(sockfd,(struct sockaddr *)&rem,&rlen);/* blocare pentru asteptare cereri*/
 if (newsockfd < 0) 
   printf ("error4 ...\n"); 
  else
  {readline(newsockfd,line); /*deservire cerere*/
  printf("%s",line);}
close (newsockfd);
}
return 0;}
