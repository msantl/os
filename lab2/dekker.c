#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int Id; /* identifikacijski broj segmenta */

struct Varijable{
  int PRAVO;
  int ZASTAVICA[ 2 ];
};

typedef struct Varijable varijable;

varijable *zajednicke;

void brisi(int sig) {
   /* oslobađanje zajedničke memorije */
  (void) shmdt((char *) zajednicke);
  (void) shmctl(Id, IPC_RMID, NULL);
  exit(0);
}

void ulaz_k_o(int i, int j){
  zajednicke->ZASTAVICA[i] = 1;
  while( zajednicke->ZASTAVICA[j] != 0 ) {
    if( zajednicke->PRAVO == j ) {
      zajednicke->ZASTAVICA[i] = 0;
      
      while( zajednicke->PRAVO == j );
      
      zajednicke->ZASTAVICA[i] = 1;
    }
  }
  return;
}
 
void izlaz_k_o( int i, int j ){
  zajednicke->PRAVO = j;  
  zajednicke->ZASTAVICA[i] = 0;
  return;
}

void proces( int i ){
  int k, m;
  for( k = 0; k < 5; ++k ){
    ulaz_k_o( i, 1 - i );
    for( m = 0; m < 5; ++m ){
      printf( "Proces: %d\t K.O broj: %d\t Izvrseno: (%d / 5)\n", i + 1, k + 1, m + 1 );
      usleep( 100000 );
    }
    izlaz_k_o( i, 1 - i );
  }
  
  return;
}

int main( int argc, char **argv ) {  
  /* zauzimanje zajedničke memorije */
  Id = shmget(IPC_PRIVATE, sizeof( varijable )*100, 0600);
  if (Id == -1)
    exit(1);  
  
  zajednicke = (varijable *) shmat(Id, NULL, 0);
  
  zajednicke->PRAVO = 0;
  zajednicke->ZASTAVICA[0] = zajednicke->ZASTAVICA[1] = 0;
  
  sigset(SIGINT, brisi);

  /* pokretanje paralelnih procesa */
  
  switch( fork() ){
    case -1:
      printf( "Greska prilikom stvaranja novog procesa!\n" );
      break;
    case 0:
      proces( 0 );
      exit( 0 );
      break;
    default:
      break;
  }
  
  switch( fork() ){
    case -1:
      printf( "Greska prilikom stvaranja novog procesa!\n" );
      ( void )wait( NULL );
      break;
    case 0:
      proces( 1 );
      exit( 0 );
      break;
    default:
      break;
  }
  
  (void) wait(NULL);
  (void) wait(NULL);
  
  brisi(0);
  return 0;
}
