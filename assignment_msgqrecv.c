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
   int mint;
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

   printf("message queue: ready to receive messages.\n");

   for(int i = 0; i < 50; i++) {
      if (msgrcv(msqid, &buf, sizeof(buf), 1, 0) == -1) {
         printf("Failed here wtf bruv\n");
         perror("msgrcv");
         system("rm msgq.txt");
         exit(1);
      }
      printf("recvd: \"%d\"\n", buf.mint);
   }
   printf("message queue: done receiving messages.\n");

   if (msgctl(msqid, IPC_RMID, NULL) == -1) {
      perror("msgctl");
      exit(1);
   }
   system("rm msgq.txt");
   return 0;
}
