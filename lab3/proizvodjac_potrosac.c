#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/shm.h>
int shmid;

#define semPISI		0
#define semPUN		1
#define semPRAZAN 2
#define semGO1		3
#define semGO2		4

typedef struct s_varijable{
	
	int ULAZ, IZLAZ;
	char M[ 5 ];
	int semid;

} varijable;

varijable *zajednicke;

void get_sem( int n, int *semid ){
	*semid = semget( IPC_PRIVATE, n, 0600 );
	if( *semid == -1 ){
		printf( "Greska kod stvaranja semafora!\n" );
		exit( 1 );
	}
	return;
}

void set_sem( int val, int n,  int semid ){
	if( semctl( semid, n, SETVAL, val ) ){
		printf( "Greska kod postavlajnja semafora!\n" );	
	}
	return;
}

void op_sem( int semid, int n,  int semoper ){
	struct sembuf temp;
	// semoper > 0 postavi
	// semoper < 0 cekaj
	temp.sem_num = n;
	temp.sem_op = semoper;
	temp.sem_flg = 0;

	if( semop( semid, &temp, 1 ) ){
		printf( "Greska kod operacije sa semaforom!\n" );
	}
	return;	
}

void clean_shmmem( void ){
	shmdt( (char *)zajednicke );
	shmctl( shmid, IPC_RMID, NULL );
	return;
}

void clean_shmsem( void ){
	semctl( zajednicke->semid, 0, IPC_RMID, 0 );
	return;
}

void clean_exit( int sig ){
	clean_shmsem();
	clean_shmmem();
	exit( 0 );
}

void unos_poruke( char *poruka, int id ){
	op_sem( zajednicke->semid, semPISI, -1 );
	
	printf( "Poruka od proizvodjaca %d : ", id );
	scanf( "%s", poruka );

	op_sem( zajednicke->semid, semPISI, 1 );

	op_sem( zajednicke->semid, id ? semGO1 : semGO2 , 1 );
	return;	
}

void proizvodac( int id ){
	char poruka[ 31 ];
	int i = -1;
	
	unos_poruke( poruka, id );

	op_sem( zajednicke->semid, id ? semGO2 : semGO1 , -1);

	do{

		op_sem( zajednicke->semid, semPUN, -1 );	
		op_sem( zajednicke->semid, semPISI, -1 );
		
		++i;

		printf( "Proizvodjac %d -> %c\n", id, poruka[i] );
		
		zajednicke->M[ zajednicke->ULAZ  ] = poruka[i];
		zajednicke->ULAZ = ( zajednicke->ULAZ + 1 ) % 5;

		op_sem( zajednicke->semid, semPISI, 1 );
		op_sem( zajednicke->semid, semPRAZAN, 1 );

    sleep( 1 );

	} while( poruka[ i ] );

	return;
}

void potrosac( void ){
	char poruka[ 63 ], c;
	int i = 0;
	int kraj = 0;

	while( kraj < 2 ){
		op_sem( zajednicke->semid, semPRAZAN, -1 );
		
		c = zajednicke->M[ zajednicke->IZLAZ ];
    printf( "Potrosac <- %c\n", c );

		poruka[ i++ ] = c;
		
    if( !c )++kraj;

		zajednicke->IZLAZ = ( zajednicke->IZLAZ + 1 ) % 5;
	
		op_sem( zajednicke->semid, semPUN, 1 );
	}

	printf( "Primljeno %s\n", poruka );

	return;	
}

int main(int argc, char **argv) {
	shmid = shmget( IPC_PRIVATE, sizeof( varijable ) * 100, 0600 );
	if( shmid == -1 ){
		printf( "Greska prilikom stvaranja zajednicke memorije\n" );
		exit(1);	
	}

	zajednicke = ( varijable * ) shmat( shmid, NULL, 0 );
	
	get_sem( 5, & (zajednicke->semid) );
	// postavljanje semafora
	set_sem( 5, semPUN, zajednicke->semid );
	set_sem( 0, semPRAZAN, zajednicke->semid );
	set_sem( 1, semPISI, zajednicke->semid );
	set_sem( 0, semGO1, zajednicke->semid );
	set_sem( 0, semGO2, zajednicke->semid );

	sigset( SIGINT, clean_exit );
	
	switch( fork() ){
		case -1:
			printf( "Greska prilikom stvaranja procesa!\n" );
			break;
		case 0:
			proizvodac( 0 );
			exit( 0 );
			break;
	}

	switch( fork() ){
		case -1:
			printf( "Greska prilikom stvaranja procesa!\n" );
			break;
		case 0:
			proizvodac( 1 );
			exit( 0 );
			break;
	}

	switch( fork() ){
		case -1:
			printf( "Greska prilikom stvaranja procesa!\n" );
			break;
		case 0:
			potrosac();
			exit( 0 );
			break;
	}
	

	wait( NULL );	wait( NULL );	wait( NULL );

	clean_exit( 0 );
	return 0;
}
