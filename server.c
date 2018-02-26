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
struct client{
struct sockaddr_in *h;
struct client *urm;
char user[100],pass[100];
}*rad=NULL;

struct data{
char user[100],pass[100];
}c[100];

int total=0;
void add_client(struct sockaddr_in *p)
{struct client *o=malloc(sizeof(struct client));
struct sockaddr_in *q=malloc(sizeof(struct sockaddr_in));
*q=*p;
o->h=q;
o->urm=NULL;
if(rad==NULL)
rad=o;
else{
o->urm=rad->urm;
rad=o;}
total++;
}

void remove_client(struct sockaddr_in *p)
{struct client *i,*aux,*q;
for(i=rad,q=NULL;i!=NULL&&(i->h->sin_addr.s_addr)!=(p->sin_addr.s_addr);q=i,i=i->urm);
if(q==NULL)
{aux=rad;
rad=rad->urm;
free(aux->h);
free(aux);}
else {aux=i;q->urm=i->urm;free(aux->h);
free(aux);}
total--;
}

int check_data(char luser[],char lpass[])
{int i;
for(i=0;i<total;i++)
if(strcmp(c[i].user,luser)==0&&strcmp(c[i].pass,lpass)==0)
return 0;
return -1;}

int check_client(struct sockaddr_in *p)
{struct client *i,*aux,*q;
for(i=rad,q=NULL;i!=NULL&&(i->h->sin_addr.s_addr)!=(p->sin_addr.s_addr);q=i,i=i->urm);
if(i==NULL)
return -1;
return 0;}

void list_client()
{struct client *i;
printf("Client list \n");
for(i=rad;i!=NULL;i=i->urm)
printf("%d\n",i->h->sin_addr.s_addr);
}

int setaddr(struct sockaddr_in *addr,char inaddr[],u_int32_t a,short sinport) {
struct hostent *h;
int i=0,n;
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

void read_data(int sock,char buf[])
{char c;
int i=0,n;
read(sock,&n,sizeof(int));
while(read(sock,&c,1)>0&&i<BUFF_SIZE&&i<n)
{printf("%c",c);
buf[i++]=c;}
buf[i]='\0';}

void send_data(int sock,char buf[],int len)
{char c;
int i=0,n=len;
write(sock,&n,sizeof(int));
while(write(sock,&buf[i++],1)>0&&i<BUFF_SIZE&&i<n);
buf[i]='\0';}

int main(int argv,char *args[])
{char name[30],line[200];
int j,init=1;
printf("%s\n",args[1]);
fgets(name,30,stdin);
struct sockaddr_in loc,rem;
socklen_t rlen=sizeof(rem);
setaddr(&loc,NULL,INADDR_ANY,atoi(args[1]));
int sockfd, newsockfd;
printf("Messsages:\n");
if ((sockfd=socket(AF_INET,SOCK_STREAM,0)) < 0)
   {printf ("error1 ...\n"); exit(1);}
printf("1%s\n",strerror(errno));
if ((bind(sockfd,(struct sockaddr *)&loc,sizeof(loc)))<0)
   {printf ("error2 ...\n"); exit(1);}
printf("2%s\n",strerror(errno));
if (listen(sockfd,1) < 0)
   {printf ("error3 ...\n"); exit(1);}
printf("3%s\n",strerror(errno));
for (;;)
{if((init==0)&&(total==0))
  {printf("loop1\n");break;}
newsockfd=accept(sockfd,(struct sockaddr *)&rem,&rlen);/* blocare pentru asteptare cereri*/
printf("4%s\n",strerror(errno));
if (newsockfd < 0)
   printf ("error4 ...\n");
  else{
  if(check_client(&rem)==-1)
   {add_client(&rem); //tratare user dupa verif conectat
    init=0;}
  read_data(newsockfd,line);
  if(strcmp(line,"~exit\n")==0)
  {remove_client(&rem);printf("Finish %d %d\n",total,init);send_data(newsockfd,line,strlen(line));list_client();continue;}
  if(fork()==0) /*deservire cerere*/
  {printf("line: %s",line);
  send_data(newsockfd,line,strlen(line));
  list_client();
  close(newsockfd);
  exit(0);}
  close(newsockfd);
printf("loop %d %d\n",init,total);
}}
printf("Exiting...\n");
exit(0);
return 0;}
