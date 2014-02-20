#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define ZAGLAVLJE_size (sizeof(struct ZAGLAVLJE_t))

int blokova, slobodni;

struct ZAGLAVLJE_t {
	int size, flag;
	struct ZAGLAVLJE_t *next;
} *memorija;

struct LISTA_t {
	struct ZAGLAVLJE_t *z;
	struct LISTA_t *next;
} *opt;

void* dodijeli(unsigned velicina);
void oslobodi(void *kazaljka);


void* dodijeli(unsigned velicina)
{
	struct ZAGLAVLJE_t *temp = memorija, *prev = NULL;
	struct ZAGLAVLJE_t *blok;
	int _size = velicina + ZAGLAVLJE_size;
	int best = 0x7fffffff;
	
	while (temp) {
		if( temp->size >= _size && temp->flag == 's' && ( temp->size - _size < best ) ) {
			prev = temp;
			best = temp->size - _size;
		}
		temp = temp->next;
	}
	
	if( !prev )return NULL;
	blok = prev;
	
	slobodni--;
	blok->flag = 'z';

	if ( blok->size > _size ) {
		blokova++;
		slobodni++;
		struct ZAGLAVLJE_t *novi1 = (struct ZAGLAVLJE_t *)malloc( ZAGLAVLJE_size );
		//struct LISTA_t *novi2 = ( struct LISTA_t * )malloc( sizeof( struct LISTA_t ) );

		novi1->flag = 's';
		novi1->size = blok->size - _size;
		novi1->next = blok->next;

		blok->next = novi1;
		blok->size = _size;
	}

	return blok;
}


void oslobodi( void *kazaljka )
{
	struct ZAGLAVLJE_t *curr = (struct ZAGLAVLJE_t *)kazaljka;
	struct ZAGLAVLJE_t *it = memorija, *next = curr->next, *prev = NULL;
	
	while (it) {
		if ( it->next == curr ) {
			prev = it;
			break;
		}
		it = it->next;
	}

	curr->flag = 's';
	slobodni++;

	if ( prev && next && prev->flag == 's' && next->flag == 's') {
		prev->next = next->next;
		prev->size = prev->size + curr->size + next->size;
		free( curr );
		free( next );
		blokova -= 2;
		slobodni -= 2;
	} else if( prev && prev->flag == 's' ) {
		prev->next = curr->next;
		prev->size = prev->size + curr->size;
		free( curr );
		blokova--;
		slobodni--;
	}	else if( next && next->flag == 's' ) {
		curr->next = next->next;
		curr->size = curr->size + next->size;
		free( next );
		blokova--;
		slobodni--;
	}

	return;
}

void init(void)
{
	blokova = slobodni = 1;
	memorija = (struct ZAGLAVLJE_t *)malloc( ZAGLAVLJE_size );
	memorija->size = 10000;
	memorija->flag = 's';
	memorija->next = NULL;

	opt = (struct LISTA_t *)malloc( sizeof( struct LISTA_t ) );
	opt->z = memorija;
	opt->next = NULL;

	return;
}

void clean_memory( int sig )
{
	struct ZAGLAVLJE_t *it1 = memorija, *prev1;
	while (it1) {
		prev1 = it1;
		it1 = it1->next;
		free( prev1 );
	}
	
	struct LISTA_t *it2 = opt, *prev2;
	while (it2) {
		prev2 = it2;
		it2 = it2->next;
		free( prev2 );
	}

	exit( 0 );
	return;
}

int main(int argc, const char *argv[])
{
	struct ZAGLAVLJE_t *it;
	int cnt, velicina, adresa, suma;
	char cmd[5];
	
	init();
	
	sigset( SIGINT, clean_memory );

	while (1) {

		printf("RS: broj blokova %d, slobodni %d, zauzeti %d\n", blokova, slobodni, blokova - slobodni);
	
		
		for (cnt = suma = 0, it = memorija; it; it=it->next, ++cnt) {
				printf("%d. pocetak %d, velicina %d, oznaka %c\n", cnt + 1, suma, it->size, it->flag);
				suma += it->size;
		}
		
		printf("Unesi zahtjev (d - dodijeli, o - oslobodi, x - izadji) ");
		scanf( "%s", cmd );

		if ( *cmd == 'd') {
			printf("Unesi velicinu programa (u oktetima) ");
			scanf( "%d", &velicina );
			it = dodijeli( ( unsigned )velicina );
			
			if ( it == NULL ) {
				printf("Greska kod dodijeljivanja memorije\n");
				continue;
			}

			adresa = 0;
			while (it) {
				adresa += it->size;
				it = it->next;
			}

			printf("Dodijeljen blok na adresi %d\n", suma - adresa);

		} else if( *cmd == 'o' ) {
			printf("Unesi pocetnu adresu programa ");
			scanf( "%d", &adresa );
			
			velicina = 0;
			it = memorija;
			while (it) {
				if ( velicina == adresa ) {
					oslobodi( it );
					break;
				}
				velicina += it->size;
				it = it->next;
			}

		}	else {
			break;
		}

	}
	
	clean_memory( 0 );
	return 0;
}
