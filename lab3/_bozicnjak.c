#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
int broj_sobova = 0, broj_patuljaka = 0;
int sob_id[ 11 ], patuljak_id[ 1024 ];

pthread_cond_t u_patuljak[ 1024 ], u_sob[ 11 ], djed;
pthread_mutex_t kljuc;
pthread_t dretve[ 1024 ];
int dretve_size;

void clean_exit( int sig ){
	exit( 0 );	
}

void ulaz_M( pthread_cond_t *uvjet, int *temp )
{
	pthread_mutex_lock( &kljuc );

	while( temp == 0 )
		pthread_cond_wait( uvjet, &kljuc );
	
	*temp = 0;

	pthread_mutex_unlock( &kljuc );
	return;
}

void izlaz_M( pthread_cond_t *uvjet, int *temp )
{
	pthread_mutex_lock( &kljuc );
	
	*temp = 1;
	pthread_cond_broadcast( uvjet );

	pthread_mutex_unlock( &kljuc );
	return;
}

void *sobovi( void *arg ){
	int id = *( int * )arg;
	pthread_mutex_lock( &kljuc );

	printf( "Vratio se sob %d\n", id + 1 );

	if ( broj_sobova == 10 ) {
		pthread_cond_broadcast( &djed );
	}
	
	pthread_cond_wait( &u_sob[ id ], &kljuc );

	pthread_mutex_unlock( &kljuc );
	return NULL;
}

void *patuljci( void *arg ){
	int id = *( int * )arg;
	pthread_mutex_lock( &kljuc );
	
	printf( "Vratio se patuljak %d\n", id + 1 );
	
	if ( broj_patuljaka >= 3 ) {
		pthread_cond_broadcast( &djed );
	}

	pthread_cond_wait( &u_patuljak[ id ], &kljuc );

	pthread_mutex_unlock( &kljuc );
	return NULL;
}

void *djedica( void *arg ){
	int i;
	pthread_mutex_lock( &kljuc );
	while ( 1 ) {

		pthread_cond_wait( &djed, &kljuc );

		if ( broj_sobova == 10 && broj_patuljaka > 0 ){
			printf( "Raznosim poklone\n" );
			usleep( 2000000 );
			for ( i = 0; i < broj_sobova; ++i ) {
				pthread_cond_broadcast( &u_sob[ i ] );
			}
			broj_sobova = 0;

		} else if ( broj_sobova == 10 ){
			printf( "Hranim sobove\n" );
			usleep( 2000000 );

		} else if( broj_patuljaka >= 3 ){
			printf( "Konzultacije s patuljcima\n" );

			while ( broj_patuljaka >= 3  ) {
				usleep( 2000000 );
				for ( i = 1; i <= 3; ++i ) {
					pthread_cond_broadcast( &u_patuljak[ broj_patuljaka - i ] );
				}
				broj_patuljaka -= 3;
			}
		}
	}
	pthread_mutex_unlock( &kljuc );
	return NULL;	
}

void *sjeverni_pol( void *arg ){
	while ( 1 ) {
		//printf( "Sjeverni pol!\n" );

		if( broj_sobova < 10 && rand() & 1 ){
			pthread_mutex_lock( &kljuc );

			sob_id[ broj_sobova ] = broj_sobova;
			dretve_size++;
			
			if ( pthread_create( &dretve[dretve_size], NULL, sobovi, &( sob_id[broj_sobova] )  ) ){
				printf( "Greska prlikom stvaraja dretve!\n" );
				clean_exit( 0 );
			}
			broj_sobova++;

			pthread_mutex_unlock( &kljuc );
		}
		
		usleep( 1000000 );

		if( rand() & 1 ){
			pthread_mutex_lock( &kljuc );

			patuljak_id[ broj_patuljaka ] = broj_patuljaka;
			dretve_size++;
			
			if( pthread_create( &dretve[dretve_size], NULL, patuljci, &( patuljak_id[ broj_patuljaka ] ) ) ){
				printf( "Greska prlikom stvaranja dretve" );
				clean_exit( 0 );	
			}
			broj_patuljaka++;

			pthread_mutex_unlock( &kljuc );
		}

		usleep( 1000000 );
	}
  return NULL;
}


int main(int argc, char **argv) {
	srand( (unsigned)time( NULL ) );
	int i = 0;
	
	sigset( SIGINT, clean_exit );

	pthread_mutex_init( &kljuc, NULL );

	if( pthread_create( &dretve[ dretve_size ], NULL, sjeverni_pol, NULL ) ) {
		printf( "Greska prilikom stvaranaj dretve1\n" );
		clean_exit( 0 );
	}
	
	++dretve_size;

	if ( pthread_create( &dretve[ dretve_size ], NULL, djedica, NULL ) ) {
		printf( "Greska prilikom stvaranja dretve!\n" );
		clean_exit( 0 );
	}

	
	for ( i = 0; i <= dretve_size; ++i ) {
		pthread_join( dretve[ i ], NULL );
	}

	return 0;
}
