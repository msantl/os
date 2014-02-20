#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

pthread_mutex_t kljuc;
pthread_cond_t u_sob, u_pat, djed, konzultacije, sobovi;
pthread_t dretve[ 1024 ];
int cnt_d;
int p, q; // p - sob, q - pat
int gladni;
int cnt_s, cnt_p;

void *djed_mraz(void *arg)
{
	int i;
	while (1) {
		pthread_mutex_lock( &kljuc );
		
		pthread_cond_wait( &djed, &kljuc );

		while ( p == 0 || q == 0 ) {
			pthread_cond_wait( &u_sob, &kljuc );
			pthread_cond_wait( &u_pat, &kljuc );
		}
		p = q = 0;

		/*ulazak u monitor*/	

		printf( "Djedica se budi, s = %d p = %d\n", cnt_s, cnt_p );
		if (cnt_s == 10) {
			if (cnt_p) {
				printf("Raznosim pokolne!\n");
				sleep(1);
				
				cnt_s = 0;

				for (i = 0; i < 10; i++) {
					pthread_cond_broadcast( &sobovi );
				}

			} else if( gladni ){
				gladni = 0;
				printf("Hranim sobove!\n");
				sleep(2);
			}
		}

		if( cnt_p == 3 ) {
			printf("Konzultacije s patuljcima\n");
			cnt_p = 0;
			for (i = 0; i < 3; i++) {
				pthread_cond_broadcast( &konzultacije );
			}
			sleep( 1 );
		}
		
		printf("Djedica odlazi spavati\n");
		/*izlazak iz monitora*/
		p = q = 1;
		pthread_cond_broadcast( &u_sob );
		pthread_cond_broadcast( &u_pat );
		pthread_mutex_unlock( &kljuc );
	}
	return NULL;
}

void *sob(void *arg)
{
	int id;
	pthread_mutex_lock( &kljuc );
	while (p == 0) {
		pthread_cond_wait( &u_sob, &kljuc );
	}
	p = 0;
	
	/*ulazak u monitor*/

	id = ++cnt_s;
	gladni = 1;

	printf("Sob %d se vratio sa godisnjeg odmora\n", id);

	if (cnt_s == 10) {
		pthread_cond_broadcast( &djed );
	}

	/*izlazak iz monitora*/

	p = 1;
	pthread_cond_broadcast( &u_sob );
	pthread_cond_wait( &sobovi, &kljuc );
	printf("Sob %d odlazi na godisnji odmor\n", id);
	pthread_mutex_unlock( &kljuc );
	return NULL;
}

void *pat(void *arg)	
{
	int id;
	pthread_mutex_lock( &kljuc );
	while (q == 0) {
		pthread_cond_wait( &u_pat, &kljuc );
	}
	q = 0;
	
	/*ulazak u monitor*/
	id = ++cnt_p;

	printf("Patuljak %d je zatrazio konzultacije\n", id);

	if (cnt_p == 3) {
		pthread_cond_broadcast( &djed );
	}


	/*izlazak iz monitora*/
	
	q = 1;
	pthread_cond_broadcast( &u_pat );
	pthread_cond_wait( &konzultacije, &kljuc );
	printf("Patuljak %d se vraca izradi igracaka\n", id);
	pthread_mutex_unlock( &kljuc );
	return NULL;
}

void *sjeverni_pol(void *arg)
{
	srand( (unsigned)time(NULL) );
	while (1) {
		//fprintf(stderr, "... %d %d ...\n", p, q);
		sleep( (rand() % 3) + 1 );
		if ( cnt_s < 10 && (rand() & 1) ) {
			if (pthread_create( &dretve[ cnt_d], NULL, sob, NULL  )) {
				printf("Greska prilikom stvaranja dretve!\n");
			}else {
				cnt_d++;
			}
		}
		if (rand() & 1) {
			if (pthread_create( &dretve[ cnt_d], NULL, pat, NULL )) {
				printf("Greska prilikom stvaranja dretve!\n");
			}else {
				cnt_d++;
			}
		}
	}
	return NULL;
}

int main(int argc, const char *argv[])
{
	int i;
	cnt_d = 2;
	p = q = 1;

	pthread_mutex_init( &kljuc, NULL );

	if (pthread_create( &dretve[0], NULL, sjeverni_pol, NULL )) {
		printf("Greska prilikom stvaranja dretve!\n");
	}
	
	if (pthread_create( &dretve[1], NULL, djed_mraz, NULL )) {
		printf("Greska prilikom stvaranja dretve!\n");
	}

	for (i = 0; i < cnt_d; i++) {
		pthread_join( dretve[i], NULL );
	}

	return 0;
}
