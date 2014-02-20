#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

char stanje[ 5 ];
int vilica[ 5 ];
pthread_cond_t uvjet[ 5 ];
pthread_mutex_t kljuc;

void ispis_stanja( int id ){
	int i;
	for ( i = 0; i < 5; ++i) {
		printf( "%2c", stanje[i] );
	}
	printf( "(%d)\n", id );
	return;	
}

void ulaz_k_o( int id ){
	pthread_mutex_lock( &kljuc );
	return;
}

void izlaz_k_o( int id ){
	pthread_mutex_unlock( &kljuc );
	return;
}

void misliti( int id ){ usleep( 1000000 ); return; } 

void jesti( int id ){
	ulaz_k_o( id );
	stanje[ id ] = 'o';
	while( vilica[ id ] == 0 || vilica[ ( id + 1 ) % 5 ] == 0  )
		pthread_cond_wait( &uvjet[ id ], &kljuc );
	
	stanje[id] = 'X';
	vilica[ id ] = vilica[ (id + 1) % 5 ] = 0;

	ispis_stanja( id );
	izlaz_k_o( id );

	usleep( 1000000 );
	
	ulaz_k_o( id );
	stanje[ id ] = 'O';

	vilica[ id ] = vilica[ (id + 1) % 5 ] = 1;

	pthread_cond_broadcast( &uvjet[( id + 1 ) % 5] );
	pthread_cond_broadcast( &uvjet[( id + 4 ) % 5] );
	
	ispis_stanja( id );
	izlaz_k_o( id );
	return;	
}

void *filozof( void *arg ){
	int id = *( int * )arg;
	
	while( 1 ){
		misliti( id );
		jesti( id );
	}

	return NULL;
}

void clean_exit( int sig ){
	exit( 0 );	
}

int main(int argc, char **argv) {
	sigset( SIGINT, clean_exit );

	pthread_t dretve[ 5 ];
	int i;
	int id[ 5 ];
	strcpy( stanje, "OOOOO" );
	
	pthread_mutex_init( &kljuc, NULL );

	for ( i = 0; i < 5; ++i ) {
		id[ i ] = i;
		vilica[ i ] = 1;

		if( pthread_create( &dretve[i], NULL, filozof, &id[i] ) ){
			printf( "Greska prilkom stvarnja dretve!\n" );
			exit( 0 );
		}
	}
	

	for ( i = 0; i < 5; ++i ) {
		pthread_join( dretve[i], NULL );
	}
	
	return 0;
}
