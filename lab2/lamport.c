#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>

#define N 10
int n;

struct Varijable{
  int TRAZIM[ N ];
  int BROJ[ N ];
};

typedef struct Varijable varijable;

varijable zajednicke;

#define MAX(a, b) (((a)<(b))?(b):(a))

void ulaz_k_o( int i ){
  int j;
  zajednicke.TRAZIM[i] = 1;
  
  zajednicke.BROJ[i] = zajednicke.BROJ[0];
  for( j = 1; j < n; ++j )
    zajednicke.BROJ[i] = MAX( zajednicke.BROJ[i], zajednicke.BROJ[j] );
  zajednicke.BROJ[i] ++;
  
  zajednicke.TRAZIM[i] = 0;

  for( j = 0; j < n; ++j ){
    while( zajednicke.TRAZIM[j] != 0 );
    while( zajednicke.BROJ[j] != 0 && ( zajednicke.BROJ[j] < zajednicke.BROJ[i] || ( zajednicke.BROJ[j] == zajednicke.BROJ[i] && j < i ) ) );
  }

  return;
}
 
void izlaz_k_o( int i ){
  zajednicke.BROJ[i] = 0;
  return;
}


void* dretva( void *x ){
  int i = *( (int *)x );
  int k, m;
  for( k = 0; k < 5; ++k ){
    ulaz_k_o( i );
    for( m = 0; m < 5; ++m ){
      printf( "Dretva: %d\t K.O broj: %d\t Izvrseno: (%d / 5)\n", i + 1, k + 1, m + 1 );
      usleep( 100000 );
    }
    izlaz_k_o( i );
  }
  
  return;
}


void izlaz( int sig ){
  exit( 0 );
}

int main( int argc, char **argv ){
  int i;
  
  if( argc < 2 ){ printf( "Nedovoljno argumenata!\n" ); return 0; }
  sscanf( argv[1] , "%d", &n );
  
  sigset( SIGINT, izlaz );
  
  pthread_t thr_id[ N ];
  int id[ N ];

  
  for( i = 0; i < n; ++i ){
    id[i] = i;
    if (pthread_create(&thr_id[i], NULL, dretva, &id[i]) != 0) {
      printf("Greska pri stvaranju dretve!\n");
      exit(1);
    }
  }
  
  for( i = 0; i < n; ++i ){
    pthread_join(thr_id[i], NULL);
  }

  return 0;
}
