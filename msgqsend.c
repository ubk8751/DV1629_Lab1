#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PERMS 0644

struct my_msgbuf {
   long mtype;
   char mtext[4];
};

int main(void) {
   int nums[50];
   for(int i = 0; i < 50; i++) {
      nums[i] = rand();
   }

   struct my_msgbuf buf;
   int msqid;
   int len;
   key_t key;

   if ((key = ftok("msgq.txt", 'B')) == -1) {
      perror("ftok");
      exit(1);
   }

   // if ((msqid = msgget(key, PERMS | IPC_CREAT)) == -1) {
   //    perror("msgget");
   //    exit(1);
   // }

   if ((msqid = msgget(key, PERMS)) == -1) { /* connect to the queue */
      perror("msgget");
      exit(1);
   }

   buf.mtype = 1;

   for(int i = 0; i < 50; i++)
   {
      int value = nums[i];
      for(int j = 0; j < 4; j++){
         char r = value & 0xFF;
         value = value >> 8;
         buf.mtext[j] = r;
         printf("%02x", r & 0xFF);
      }
      printf(" -> %d\n", nums[i]);

      len = strlen(buf.mtext);
      if (msgsnd(msqid, &buf, len + 1, 0) == -1) /* +1 for '\0' */
         perror("msgsnd");
   }

   // printf("message queue: ready to send messages.\n");
   // printf("Enter lines of text, ^D to quit:\n");
   // buf.mtype = 1; /* we don't really care in this case */

   // while(fgets(buf.mtext, sizeof buf.mtext, stdin) != NULL) {
   //    len = strlen(buf.mtext);
   //    /* remove newline at end, if it exists */
   //    if (buf.mtext[len-1] == '\n') buf.mtext[len-1] = '\0';
   //    if (msgsnd(msqid, &buf, len+1, 0) == -1) /* +1 for '\0' */
   //       perror("msgsnd");
   // }
   //strcpy(buf.mtext, "end");
   // len = strlen(buf.mtext);
   // if (msgsnd(msqid, &buf, len+1, 0) == -1) /* +1 for '\0' */
   //    perror("msgsnd");
   printf("message queue: done sending messages.\n");
   return 0;
}
