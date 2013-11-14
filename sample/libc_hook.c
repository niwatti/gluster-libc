#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dlfcn.h>

/* http://www.tcm.phy.cam.ac.uk/sw/wrap.html */

int  connect(int  sockfd,  const  struct sockaddr *serv_addr, socklen_t
             addrlen){
  static int (*connect_real)(int, const  struct sockaddr*, socklen_t)=NULL;
  unsigned char *c;
  int port,ok=1;

  if (!connect_real) connect_real=dlsym(RTLD_NEXT,"connect");

  if (serv_addr->sa_family==AF_INET6) return EACCES;

  if (serv_addr->sa_family==AF_INET){
    c=serv_addr->sa_data;
    port=256*c[0]+c[1];
    c+=2;
    ok=0;

    // Allow all contacts with localhost
    if ((*c==127)&&(*(c+1)==0)&&(*(c+2)==0)&&(*(c+3)==1)) ok=1;

    // Allow contact to any WWW cache on 8080
    if (port==8080) ok=1;
  }

  if (ok) return connect_real(sockfd,serv_addr,addrlen);

  if (getenv("WRAP_TCP_DEBUG"))
      fprintf(stderr,"connect() denied to address %d.%d.%d.%d port %d\n",
              (int)(*c),(int)(*(c+1)),(int)(*(c+2)),(int)(*(c+3)),
              port);

  return EACCES;
}
