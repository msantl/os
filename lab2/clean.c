#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <values.h>
int main ( void ) {
   int i;
   for (i = 1; i < MAXLONG; i++) {
      if (shmctl(i, IPC_RMID, NULL) != -1)
         printf("Obrisao zajednicku memoriju %d\n", i); 

      if (semctl(i, 0, IPC_RMID, 0) != -1)
         printf("Obrisao skup semafora %d\n", i); 

      if (msgctl(i, IPC_RMID, NULL) != -1)
         printf("Obrisao red poruka %d\n", i); 
   }
   return 0;
}
