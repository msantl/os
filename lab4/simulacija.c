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

void dodajLISTA( struct LISTA_t **glavap, struct ZAGLAVLJE_t *z )
{
	struct LISTA_t *novi = ( struct LISTA_t * )malloc( sizeof( struct LISTA_t ) );
	novi->z = z;
	
	if ( *glavap == NULL || (*glavap)->z->size < z->size ) {
		novi->next = *glavap;
		*glavap = novi;
	} else {
		while ( (*glavap)->next && (*glavap)->next->z->size > z->size ) {
			glavap = &( (*glavap)->next );
		}
		novi->next = (*glavap)->next;
		(*glavap)->next = novi;
	}

	return;
}

void makniLISTA( struct LISTA_t **glavap, struct ZAGLAVLJE_t *z)
{
	struct LISTA_t *temp = NULL;
	
	if ( *glavap == NULL) {
		return;
	}

	while (*glavap) {
		if ( (*glavap)->z == z) {
			temp = *glavap;
			break;
		}
		glavap = &( (*glavap)->next );
	}
	
	if ( temp == NULL ) {
		return;
	}

	free( temp );
	*glavap = (*glavap)->next;

	return;
}

void* dodijeli(unsigned velicina)
{
	struct LISTA_t *temp = opt, *prev = NULL;
	struct ZAGLAVLJE_t *blok;
	int _size = velicina + ZAGLAVLJE_size;
	
	while (temp && temp->z->size >= _size) {
		prev = temp;
		temp = temp->next;
	}

	if( !prev )return NULL;
	blok = prev->z;
	
	makniLISTA( &opt, blok );
	
	slobodni--;
	blok->flag = 'z';

	if ( blok->size > _size ) {
		blokova++;
		slobodni++;
		struct ZAGLAVLJE_t *novi1 = (struct ZAGLAVLJE_t *)malloc( ZAGLAVLJE_size );
		
		novi1->flag = 's';
		novi1->size = blok->size - _size;
		novi1->next = blok->next;

		blok->next = novi1;
		blok->size = _size;
		dodajLISTA( &opt, novi1 );
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
		//obrisi next
		//obrisi prev
		makniLISTA( &opt, next );
		makniLISTA( &opt, prev );
		prev->next = next->next;
		prev->size = prev->size + curr->size + next->size;
		free( curr );
		free( next );
		blokova -= 2;
		slobodni -= 2;
		//dodaj prev
		dodajLISTA( &opt, prev );
	} else if( prev && prev->flag == 's' ) {
		//obrisi prev
		makniLISTA( &opt, prev );
		prev->next = curr->next;
		prev->size = prev->size + curr->size;
		free( curr );
		blokova--;
		slobodni--;
		//dodaj prev
		dodajLISTA( &opt, prev );
	}	else if( next && next->flag == 's' ) {
		//obrisi next
		makniLISTA( &opt, next );
		curr->next = next->next;
		curr->size = curr->size + next->size;
		free( next );
		blokova--;
		slobodni--;
		//dodaj curr
		dodajLISTA( &opt, curr );
	} else {
		//dodaj curr
		dodajLISTA( &opt, curr );
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
	printf("Brisem zaglavlja\n");
	struct ZAGLAVLJE_t *it1 = memorija, *prev1;
	while (it1) {
		prev1 = it1;
		it1 = it1->next;
		free( prev1 );
	}
	
	printf("Brisem listu\n");
	struct LISTA_t *it2 = opt, *prev2;
	while (it2) {
		prev2 = it2;
		it2 = it2->next;
		free( prev2 );
	}
	
	printf("Izlazim\n");
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
		puts( "" );
		printf("RS: broj blokova %d, slobodni %d, zauzeti %d\n", blokova, slobodni, blokova - slobodni);
	
		
		for (cnt = suma = 0, it = memorija; it; it=it->next, ++cnt) {
				printf("%d. pocetak %d, velicina %d, oznaka %c\n", cnt + 1, suma, it->size, it->flag);
				suma += it->size;
		}
		
		puts( "" );

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
				if ( velicina == adresa && it->flag == 'z' ) {
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
