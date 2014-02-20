#include <stdio.h>
#include <signal.h>

#define N 6    /* broj razina prekida */ 

int OZNAKA_CEKANJA[N];
int PRIORITET[N];
int TEKUCI_PRIORITET;
int sig[]={SIGABRT, SIGBUS, SIGHUP, SIGSYS, SIGINT};

void zabrani_prekidanje(){
  int i;
  for(i = 0; i < 5; i++)
    sighold(sig[i]);
    
  return;
}

void dozvoli_prekidanje(){
  int i;
  for(i = 0; i < 5; i++)
    sigrelse(sig[i]);
    
  return;
}

 
/* obrada se simulira trošenjem vremena,
   obrada traje 5 sekundi, ispis treba biti svake sekunde */
void obrada_prekida( int i ){
  int j, k, x;
  for( k = 0; k < N; ++k ){
    if( k == i )printf( "P  " );
    else printf( "-  " );
  }
  
  #ifdef DEBUG
    printf( " O_CEK[" );
    for( x = 0; x < N; ++x )
      printf( "%d ", OZNAKA_CEKANJA[x] );
    printf( "]" );
    printf( " TEK_PRIOR=%d ", TEKUCI_PRIORITET );
    printf( " PRIOR[" );
    for( x = 0; x < N; ++x )
      printf( "%d ", PRIORITET[x] );
    printf( "]" );
  #endif
  
  printf( "\n" );
  
    
  for( j = 0; j < 5; ++j ){
    sleep( 1 );
    
    for( k = 0; k < N; ++k ){
      if( k == i )printf( "%d  ", j + 1 );
      else printf( "-  " );
    }
    
    #ifdef DEBUG
      printf( " O_CEK[" );
      for( x = 0; x < N; ++x )
        printf( "%d ", OZNAKA_CEKANJA[x] );
      printf( "]" );
      printf( " TEK_PRIOR=%d ", TEKUCI_PRIORITET );
      printf( " PRIOR[" );
      for( x = 0; x < N; ++x )
        printf( "%d ", PRIORITET[x] );
      printf( "] " );
    #endif

    printf( "\n" );
  }
  
  
  for( k = 0; k < N; ++k ){
    if( k == i )printf( "K  " );
    else printf( "-  " );
  }
  
  #ifdef DEBUG
    printf( " O_CEK[" );
    for( x = 0; x < N; ++x )
      printf( "%d ", OZNAKA_CEKANJA[x] );
    printf( "]" );
    printf( " TEK_PRIOR=%d ", TEKUCI_PRIORITET );
    printf( " PRIOR[" );
    for( x = 0; x < N; ++x )
      printf( "%d ", PRIORITET[x] );
    printf( "]" );
  #endif

  
  printf( "\n" );
  
  return;
}

void prekidna_rutina( int sig ){
  int n = 1, x, j;

  zabrani_prekidanje();
  
  switch(sig){
    case SIGABRT: 
       n = 1; 
       printf("-  X  -  -  -  -  ");
       break;
    case SIGBUS: 
       n = 2; 
       printf("-  -  X  -  -  -  ");
       break;
    case SIGHUP: 
       n = 3; 
       printf("-  -  -  X  -  -  ");
       break;
    case SIGSYS: 
       n = 4; 
       printf("-  -  -  -  X  -  ");
       break;
    case SIGINT: 
       n = 5; 
       printf("-  -  -  -  -  X  ");
       break;
    default:
      break;
  }
  
  #ifdef DEBUG
    printf( " O_CEK[" );
    for( x = 0; x < N; ++x )
      printf( "%d ", OZNAKA_CEKANJA[x] );
    printf( "]" );
    printf( " TEK_PRIOR=%d ", TEKUCI_PRIORITET );
    printf( " PRIOR[" );
    for( x = 0; x < N; ++x )
      printf( "%d ", PRIORITET[x] );
    printf( "]" );
  #endif
  printf( "\n" );

  
  OZNAKA_CEKANJA[n]++;
  
  do{
    x = 0;    
    
    for( j = TEKUCI_PRIORITET + 1; j < N; ++j ){
      if( OZNAKA_CEKANJA[ j ] > 0 ){
        x = j;
      }
    }
    
    if( x > 0 ){
      OZNAKA_CEKANJA[ x ]--;
      PRIORITET[ x ] = TEKUCI_PRIORITET;
      TEKUCI_PRIORITET = x;
      dozvoli_prekidanje();
      obrada_prekida( x );
      zabrani_prekidanje();
      TEKUCI_PRIORITET = PRIORITET[ x ];
    }
    
  } while( x > 0 );
  
  
  dozvoli_prekidanje();
  
  return;
}

 

int main ( int argc, char **argv ){
  int i, x;
  for( i = 0; i < 5; ++i )
    sigset ( sig[i], prekidna_rutina);

  TEKUCI_PRIORITET = 0;

  printf("Proces obrade prekida, PID=%d\n", getpid());
  
  printf( "GP S1 S2 S3 S4 S5\n" );   
  printf( "-----------------\n" );
  
  for( i = 0; i < 10; ++i ){
    printf( "%2d -  -  -  -  -  ", i + 1 );
    #ifdef DEBUG
      printf( " O_CEK[" );
      for( x = 0; x < N; ++x )
        printf( "%d ", OZNAKA_CEKANJA[x] );
      printf( "]" );
      printf( " TEK_PRIOR=%d ", TEKUCI_PRIORITET );
      printf( " PRIOR[" );
      for( x = 0; x < N; ++x )
        printf( "%d ", PRIORITET[x] );
      printf( "]" );
    #endif
    printf( "\n" );
    sleep( 1 );
  }
  

  printf ("Zavrsio osnovni program\n");
  return 0;
}
