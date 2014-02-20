#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

int pid = 0;
int signali[]={SIGABRT, SIGBUS, SIGHUP, SIGSYS};


void prekidna_rutina(int sig){
  /* pošalji SIGKILL procesu 'pid'*/
  kill( pid, SIGKILL );
  exit(0);
}

int main(int argc, char *argv[]){
  pid = atoi(argv[1]);
  sigset(SIGINT, prekidna_rutina);
  
  int spavaj_r, signal_r;
  
  srand( (unsigned)time( NULL ) );

  while(1){
    /* odspavaj 3-5 sekundi */
    /* slučajno odaberi jedan signal (od 4) */
    /* pošalji odabrani signal procesu 'pid' funkcijom kill*/
    spavaj_r = ( rand() % 3 ) + 3;
    signal_r = ( rand() % 4 );
    
    sleep( spavaj_r );
    #ifdef DEBUG
      printf( "Prioritet: %d\n", signal_r + 1 );
    #endif
    kill( pid, signali[ signal_r ] );
  }
  
  return 0;
}
