#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PERMS 0644
struct my_msgbuf {
   long mtype;
   char mtext[4];
};

int main(void) {
   struct my_msgbuf buf;
   int msqid;
   int toend;
   key_t key;
   system("touch msgq.txt");

   if ((key = ftok("msgq.txt", 'B')) == -1) {
      perror("ftok");
      exit(1);
   }

   if ((msqid = msgget(key, PERMS | IPC_CREAT)) == -1) {
      perror("msgget");
      exit(1);
   }

   // if ((msqid = msgget(key, PERMS)) == -1) { /* connect to the queue */
   //    perror("msgget");
   //    exit(1);
   // }
   printf("message queue: ready to receive messages.\n");

   for(;;) { 
      
      if (msgrcv(msqid, &buf, sizeof(buf.mtext), 0, 0) == -1) {
         printf("Failed here wtf bruv\n");
         perror("msgrcv");     
         system("rm msgq.txt");
         exit(1);
      }
      // printf("recvd: \"%s\"\n", buf.mtext);
      int v;
      for(int i = 0; i < 4; i++) {
         int bv = (int)buf.mtext[i] & 0xFF;
         //printf("%02x", bv);
         printf("%08x -> ", v);
         v = (v | bv);
         if(i != 3)
            v = v << 8;
      }
      printf("%08x -> ", v);
      printf("%d\n", v);
   }
   printf("message queue: done receiving messages.\n");

   if (msgctl(msqid, IPC_RMID, NULL) == -1) {
      perror("msgctl");
      exit(1);
   }
   system("rm msgq.txt");
   return 0;
}
