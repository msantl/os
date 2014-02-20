#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>

int g_br, d2_br, d3_br, kraj, broj_rijeci, zbroj;
int TXT = 1, HTML = 0;
char MS[ 1024 ];
FILE *f;

int izbroji_rijeci( char *niz ) {
  char *c;
  int ret = 0;
  
  for( c = niz; *c; ++c ){
    for( ; isalpha( *c ); ++c );
    ++ret;
  }
  
  return ret;
}

int izbroji_tagove( char *niz ) {
  char *c;
  int ret = 0;
  
  for( c = niz; *c; ++c ){
    if( *c == '<' && *(c + 1) != '/' ){
      for( ; *c && *c != '>' ; ++c );
      ++ret;
    }
  }
  
  return ret;
}

void* dohvati( void *arg ){
  g_br = 0;
  kraj = 0;
  char *c;
  
  while( 1 ){
    strcpy( MS, "" );
    c = fgets( MS, 1024, f  );
    g_br++;
    
    if( c == NULL )break;
        
    while( d2_br <= g_br || d3_br <= g_br );  
  }
  
  kraj = 1;
  return NULL;
}

void* broji_rijeci( void *arg ){
  d2_br = 1;
  
  int tip = *(int *)arg;
  
  while( kraj == 0 ){
    while( d2_br > g_br );
    
    if( tip == TXT )broj_rijeci += izbroji_rijeci( MS );
    else broj_rijeci += izbroji_tagove( MS );
    
    d2_br++;
  }
  
  return NULL;
}

void* suma( void *arg ){
  int len, i;
  d3_br = 1;
  
  while( kraj == 0 ){
    while( d3_br > g_br );
    
    len = strlen( MS );
    for( i = 0; i < len; ++i ){
      zbroj ^= MS[ i ];
    }
    
    d3_br++;
  }
  
  return NULL;
}

void* ispis( void *arg ){
  while( kraj == 0 ){
    sleep( 1 );
    printf( "%d %d %d %d\n", g_br, d2_br, d3_br, kraj );
  }
  return NULL;
}

void obradi_txt( char *filename ){
  int i;
  f = fopen( filename, "r" );
  pthread_t thr_id[ 3 ];  
  
  broj_rijeci = zbroj = 0;

  
  if (pthread_create(&thr_id[0], NULL, dohvati, NULL ) != 0) {
      printf("Greska pri stvaranju dretve!\n");
      exit(1);
    }
  if (pthread_create(&thr_id[1], NULL, broji_rijeci, &TXT ) != 0) {
      printf("Greska pri stvaranju dretve!\n");
      exit(1);
    }
  if (pthread_create(&thr_id[2], NULL, suma, NULL ) != 0) {
      printf("Greska pri stvaranju dretve!\n");
      exit(1);
    }
  
  for( i = 0; i < 3; ++i )
    pthread_join( thr_id[i], NULL );
  
  /* ispis statistike */
  printf( "Broj rijeci u \"%s\": %d, suma: %d\n", filename, broj_rijeci, zbroj );
  
  fclose( f );
  return;
}

void obradi_html( char *filename ){
  int i;
  f = fopen( filename, "r" );
  pthread_t thr_id[ 3 ];  
  
  broj_rijeci = zbroj = 0;
  
  if (pthread_create(&thr_id[0], NULL, dohvati, NULL ) != 0) {
      printf("Greska pri stvaranju dretve!\n");
      exit(1);
    }
  if (pthread_create(&thr_id[1], NULL, broji_rijeci, &HTML ) != 0) {
      printf("Greska pri stvaranju dretve!\n");
      exit(1);
    }
  if (pthread_create(&thr_id[2], NULL, suma, NULL ) != 0) {
      printf("Greska pri stvaranju dretve!\n");
      exit(1);
    }
    
  for( i = 0; i < 3; ++i )
    pthread_join( thr_id[i], NULL );
  
  /* ispis statistike */
  printf( "Broj tagova u \"%s\": %d, suma: %d\n", filename, broj_rijeci, zbroj );
  
  fclose( f );
  return;
}

int main( int argc, char **argv ){
  int i;
  int valid = 0;
  
  for( i = 1; i < argc; ++i ){
    if( strcmp( argv[i] + strlen(argv[i]) - 4 , ".txt" ) == 0 ){
      ++valid;
      if( fork() == 0 ){
        obradi_txt( argv[i] );
        exit( 0 );
      }
    }
    else if( strcmp( argv[i] + strlen(argv[i]) - 5 , ".html" ) == 0 ){
      ++valid;
      if( fork() == 0 ){
        obradi_html( argv[i] );
        exit( 0 );
      }
    } else{
      break;
    }
  }
  
  
  while( valid-- ){
    wait( NULL );  
  }
  
  if( argv[i] )printf( "Analiza obustavljena za datoteku \"%s\"\n", argv[i] );
  
  return 0;
}
