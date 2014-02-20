#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

int okvira, zahtjeva;
int zahtjev[ 127 ];

struct lista_t {
	int p, MOD;
	int flag[ 21 ];
	int id[ 21 ];
};

void init( struct lista_t *lista)
{
	lista->MOD = okvira;
	lista->p = 0;
	memset( lista->flag, 0, sizeof lista->flag );
	memset( lista->id, -1, sizeof lista->id );
	return;
}

int main(int argc, const char *argv[])
{
	if ( argc != 3 ) {
		printf("Potrebno je navesti tocno 2 argumenta\n");
		exit(0);
	}
	int i, j , pogodak;
	char ispis[ 15 ];

	srand( (unsigned)time(NULL) );

	sscanf( argv[1], "%d", &okvira );
	sscanf( argv[2], "%d", &zahtjeva );
	
	printf("Zahtjevi: ");

	for (i = 0; i < zahtjeva; i++) {
			zahtjev[i] = (rand() % 7) + 1;
			printf("%d ", zahtjev[i]);
	}
	printf("\n");
	
	//pocetak
	struct lista_t lista;
	init( &lista );
	
	printf("%5s", "#N");
	for (i = 0; i < okvira; i++) {
		printf("%7d", i + 1);
	}
	printf("\n-----------------------------------------\n");


	for (i = 0; i < zahtjeva; i++) {
		pogodak = -1;

		printf("%5d", zahtjev[i]);
		
		for (j = 0; j < okvira; j++) {
			if ( lista.id[ j ] == zahtjev[i] ) {
				pogodak = j;
				lista.flag[j] = 1;
				break;
			}
		}
	
		if ( pogodak == -1 ) {
			
			while (1) {
				if ( lista.flag[lista.p] == 0 ) {
					lista.id[ lista.p ] = zahtjev[i];
					pogodak = lista.p;
					lista.flag[ lista.p ] = 1;
					break;
				}
				lista.flag[ lista.p ] = 0;
				lista.p = (lista.p + 1) % lista.MOD;
			}

			for (j = 0; j < okvira; j++) {
				if ( ~lista.id[j] ) {
					sprintf( ispis, "%d", lista.id[j] );
				} else {
					sprintf( ispis, "-" );
				}

				if ( pogodak == j ) {
					sprintf( ispis, "[%d]", lista.id[j] );
				}

				printf("%7s", ispis);
			}
			printf("\n");

		} else {

			for (j = 0; j < okvira; j++) {
				if ( ~lista.id[j]) {
					sprintf( ispis, "%d", lista.id[j] );
				} else {
					sprintf( ispis, "-" );
				}
				
				if ( pogodak == j ) {
					sprintf( ispis, "(%d)", lista.id[j] );
				}

				printf("%7s", ispis);
			}
			printf("\t#pogodak\n");
		
		}


	}

	//kraj
	return 0;
}
