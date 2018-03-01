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
#include <pthread.h>

pthread_t t[100];

#define BUFF_SIZE 200

struct client{
struct sockaddr_in *h;
struct client *urm;
int thread_no;
char user[100],pass[100];
}*rad=NULL;

struct data{
char user[100],pass[100];
}c[100];

int total=0,no_log=0;

int check_data(char luser[],char lpass[])
{int i;
for(i=0;i<no_log;i++)
{printf("User %s pass %s \n",c[i].user,c[i].pass);
if(strcmp(c[i].user,luser)==0&&strcmp(c[i].pass,lpass)==0)
return 0;}
return -1;}

void add_client(struct sockaddr_in *p,char user[],char pass[])
{int v=check_data(user,pass);
  printf("Add %s %s %d \n",user,pass,v);
if(v==0)
{struct client *o=malloc(sizeof(struct client));
struct sockaddr_in *q=malloc(sizeof(struct sockaddr_in));
*q=*p;
o->h=q;
o->urm=NULL;
strcpy(o->user,user);
strcpy(o->pass,pass);
if(rad==NULL)
rad=o;
else{
o->urm=rad->urm;
rad=o;}
total++;}
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
total--;}

struct client * check_client(struct sockaddr_in *p)
{struct client *i,*aux,*q;
for(i=rad,q=NULL;i!=NULL&&(i->h->sin_addr.s_addr)!=(p->sin_addr.s_addr);q=i,i=i->urm);
if(i==NULL)
return NULL;
return i;}

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
return 0 ;}

void read_data(int sock,char buf[])
{char c;
int i=0,n;
read(sock,&n,sizeof(int));
printf("n=%d\n",n);
read(sock,buf,n);
buf[n]='\0';
printf("%s\n",buf);}

void send_data(int sock,char buf[])
{char c;
int i=0,n=strlen(buf);
write(sock,&n,sizeof(int));
write(sock,buf,n);}

void send_mess(char mess[]){
    struct client *i;
    struct sockaddr_in rem;
    int sockfd;
    for(i=rad;i!=NULL;i=i->urm)
    {printf("Post messsage :\n");
        if((sockfd=socket(PF_INET,SOCK_STREAM,0)) < 0)
       {printf ("error1 ...\n"); continue;}
       printf("1%s\n",strerror(errno));

       setaddr(&rem,NULL,i->h->sin_addr.s_addr,i->h->sin_port);
       connect(sockfd,(struct sockaddr *)&rem,sizeof(rem));
       printf("2%s\n",strerror(errno));

       send_data(sockfd,mess);
       printf("sock %d \n",sockfd);
       close(sockfd);}}

void read_file(char file_name[])
{FILE *f=fopen(file_name,"rt");
  while(!feof(f))
  {fscanf(f,"%s %s\n",c[no_log].user,c[no_log].pass);
  printf("%s %s\n",c[no_log].user,c[no_log].pass);
  no_log++;}
  fclose(f);}

void * thread_main(void *arg)
{int sock=*(int *)arg;
char line[100];
printf("Sock %d\n",sock);
read_data(sock,line);
printf("thread %s \n ",line);
return NULL;
}

int main(int argv,char *args[])
{char name[30],line[200],user[100],pass[100];
int j,init=1;
read_file(args[2]);
struct client * p;
printf("%s\n",args[1]);
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
else{if((p=check_client(&rem))==NULL)
   {printf("Logging user \n");
    read_data(newsockfd,user);
    read_data(newsockfd,pass);
    add_client(&rem,user,pass); //tratare user dupa verif conectat
    if(check_client(&rem)!=NULL)
    {init=0;send_data(newsockfd,"okay");}
    else send_data(newsockfd,"wrong");
    close(newsockfd);continue;}

  p=check_client(&rem);
  pthread_create(&t[0],NULL,thread_main,(void *)&newsockfd);


  if(strcmp(line,"~exit\n")==0)
  {remove_client(&rem);printf("Finish %d %d\n",total,init);
  send_data(newsockfd,line);list_client();close(newsockfd);continue;}

  if(fork()==0) /*deservire cerere*/
  {printf("%s : %s",p->user,line);
   //send_mess(line);
  //send_data(newsockfd,line);
  close(newsockfd);
  exit(0);}

  close(newsockfd);}}

printf("Exiting...\n");
exit(0);
return 0;}
