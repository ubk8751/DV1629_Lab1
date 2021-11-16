#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PERMS 0644

const long int N = 69420;
const long int M = -69420;

struct my_msgbuf {
   long mtype;
   int mint;
};

// it works do not touch
int rand_num() {
  long int r = rand();
  long int m = N - M + 1;
  float t = m;
  float n = (RAND_MAX / t + 1);
  int R = (int)(M + r / n);
  return R;
}

int main(void) {
   int nums[50];
   for(int i = 0; i < 50; i++) {
      nums[i] = rand_num();
   }

   struct my_msgbuf buf;
   buf.mtype = 1;
   int msqid;
   int len;
   key_t key;

   if ((key = ftok("msgq.txt", 'B')) == -1) {
      perror("ftok");
      exit(1);
    }

   if ((msqid = msgget(key, PERMS)) == -1) { /* connect to the queue */
      perror("msgget");
      exit(1);
   }

   int i;
   char r;
   for(i = 0; i < 50; i++){
      buf.mint = nums[i];
      if (msgsnd(msqid, &buf, sizeof(buf), 1) == -1) /* +1 for '\0' */
         perror("msgsnd");
      printf("Sent \"%d\"\n", buf.mint);
   }

   printf("message queue: done sending messages.\n");
   return 0;
}
