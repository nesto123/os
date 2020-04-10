/*
 * Jednostavni primjer za generiranje prostih brojeva korištenjem
   "The GNU Multiple Precision Arithmetic Library" (GMP)
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include "slucajni_prosti_broj.h"

#define MASKA(bitova)			(-1 + (1<<(bitova)) )
#define UZMIBITOVE(broj,prvi,bitova) 	( ( (broj) >> (64-(prvi)) ) & MASKA(bitova) )


#define	N	5
#define RADNE	5
#define NERADNE	5

uint64_t	MS[N];
uint64_t	ULAZ=0, IZLAZ=0, BROJAC; //zajedničke varijable
char	*kraj	=	"ne";
uint64_t	ULAZ_D[RADNE+NERADNE],	BROJ[RADNE+NERADNE];
uint64_t	velicina_grupe;
struct gmp_pomocno p[RADNE+NERADNE+1];

void tmp_print();

uint64_t zbrckanost (uint64_t x)
{
	uint64_t	z	=	0,	i,	j,	b1	=	0,	pn;

	for ( i = 0; i < 64-6; i++) {
		//podniz bitova od x: x[i] do x[i+5]
		pn	=	UZMIBITOVE	(x,	i+6,	6);
		b1	=	0;
		for ( j = 0; j < 6; j++)
			if ( (1<<j) & pn )
					b1++;
		if ( b1	>	4	)
			z	+=	b1	-	4;
		else if (	b1	<	2	)
			z	+=	2	-	b1;
	}
	return z;
}

uint64_t generiraj_dobar_broj(uint64_t	velicina_grupe,	uint16_t	k)
{
	uint64_t	najbolji_broj = 0, broj, z;
	uint64_t	najbolja_zbrckanost = 0;
	uint64_t i/*,	velicina_grupe	=	1000*/;

	for( i = 0 ; i	<=	velicina_grupe-1;	i++){
		broj = daj_novi_slucajan_prosti_broj	(&p[k]);
		z = zbrckanost (broj);
		if (	z > najbolja_zbrckanost){
			najbolja_zbrckanost = z;
			najbolji_broj = broj;
		}
	}
	return najbolji_broj;
}


void	stavi_u_MS(uint64_t	broj)
{
	MS[ULAZ] = broj;
	ULAZ = (ULAZ + 1) % N;
	BROJAC++;
	if(BROJAC > N){ //ako prepisuje nepročitanu poruku
		BROJAC--;
		IZLAZ = (IZLAZ + 1) % N;
	}
}

uint64_t	uzmi_iz_MS()
{
//tmp_print();

	uint64_t	broj = MS[IZLAZ];
	if(BROJAC > 0){
		IZLAZ = (IZLAZ + 1) % N;
		BROJAC--;
	} //ako je međuspremnik prazan ne pomičem IZLAZ
	return broj;
}

uint64_t	procjeni_velicinu_grupe()
{
	uint64_t	M = 1000; //(ne preveliki da jedna petlja bude preduga,
	          //ni premali da dohvat vremena utječe na rezultat)
	uint64_t	SEKUNDI = 10 ;//(može i malo manje, ali barem 5);
	time_t t1	=	time(NULL);
	uint64_t	k = 0, i;

	BROJAC	=	0;

	uint64_t	velicina_grupe = 1; //da generiraj_dobar_broj() obavi 1 iteraciju
	while (	time(NULL) < t1 + SEKUNDI	){
		k++;
		for (i = 0 ;	i <	M-1 ;	i++){
			uint64_t	broj = generiraj_dobar_broj(velicina_grupe, RADNE+NERADNE);
			if (broj != 0){
			
			stavi_u_MS(	broj	);}

			//mora se osigurati da ova petlja preživi optimizaciju
			//lako se kasnije ULAZ i IZLAZ ponovno postave na nulu
		}

	}
	uint64_t	brojeva_u_sekundi = k * M / SEKUNDI;
	velicina_grupe = brojeva_u_sekundi *2/5; //(ili bolje *2/5 ili slično)
	
	
	return	velicina_grupe;
}

//////////////////////////////////////////////////////////////////////
uint64_t	max()
{
	uint64_t i, max	=	0;
	for ( i = 0; i < RADNE+NERADNE;	i++){
		if(	BROJ[i]	>	max	)
			max	=	BROJ[i];}
		return max;
}

void	udi_u_KO(uint64_t I){
	
	ULAZ_D[I]	=	1;
	BROJ[I]	=	max()	+	1;
	ULAZ_D[I]	=	0;

	for ( uint64_t J = 0; J < RADNE+NERADNE; J++)
	{
		while (ULAZ_D[J]	==	1);
		while (BROJ[J]	!=	0	&&	(BROJ[J]	<	BROJ[I]	||	(BROJ[J]	==	BROJ[I]	&&	J<I)	)	);				
	}
}

void	izadi_iz_KO(uint64_t I){
	BROJ[I]	=	0;
}

void	*radna_dretva (void	*id)
{
	uint64_t	*ID = id;
	uint64_t	x;

	inicijaliziraj_generator (&p[*ID], *ID);

	do {
		x = generiraj_dobar_broj(velicina_grupe, *ID); //sa svojim generatorom

		udi_u_KO(*ID);

		stavi_u_MS(x);
		printf("stavio: %lx\n", x);

		izadi_iz_KO(*ID);
	}
	while(	strcmp( kraj , "KRAJ_RADA"));

	//obriši svoj generator slučajnih prostih brojeva(id)
	obrisi_generator (&p[*ID]);

	return	NULL;
}

void	*neradna_dretva(void	*id)
{
	uint64_t	*ID = id;
	uint64_t	y;

	do {
		sleep(3);

		udi_u_KO(*ID);

		y = uzmi_iz_MS();
		printf("uzeo: %lx\n", y);

		izadi_iz_KO(*ID);
	}
	while(	strcmp( kraj , "KRAJ_RADA"));

	return	NULL;
}


int main(int argc, char *argv[])
{
	uint64_t	br[RADNE+NERADNE];
	pthread_t	dretve_id[RADNE+NERADNE];

	//	INICIJALIZACIJA
	inicijaliziraj_generator (&p[RADNE+NERADNE], RADNE+NERADNE);
	ULAZ = IZLAZ = 0;
	for ( uint64_t i = 0; i < RADNE+NERADNE; i++)
	{
		ULAZ_D[i]	=	BROJ[i]	=	0;
		if(i<N)
			MS[i]	=	1;
	}
	velicina_grupe	=	procjeni_velicinu_grupe();

	//	DRETVE

	for(	uint64_t	i	=	0;	i	<	RADNE;	++i) {
		br[i]	=	i;
		if (	pthread_create(&dretve_id[i],	NULL,	radna_dretva,	&br[i])	!=	0) {
			printf("Greska pri stvaranju dretve\n");
			exit(1);
		}
	}
	//sleep(3);

	for(	uint64_t	i	=	RADNE;	i	<	RADNE+NERADNE;	++i){
		br[i]	=	i;
		if (	pthread_create(&dretve_id[i],	NULL,	neradna_dretva,	&br[i])	!=	0) {
			printf("Greska pri stvaranju dretve\n");
			exit(1);
		}
	}
	sleep(20);
	kraj = "KRAJ_RADA";

	for(	uint64_t	i	=	0;	i	<	RADNE+NERADNE;	++i){
		pthread_join(dretve_id[i], NULL);
	}


	obrisi_generator (&p[RADNE+NERADNE]);

	return 0;
}
void tmp_print(){printf("\n-Izlaz-%ld", IZLAZ);
	for( int i=0;i<N;++i)
		printf("%lx, ", MS[i]);
	printf("\n");
}
/*
  prevođenje:
  - ručno: gcc program.c slucajni_prosti_broj.c -lgmp -lm -o program
  - preko Makefile-a: make
  pokretanje:
  - ./program
  - ili: make pokreni
  nepotrebne datoteke (.o, .d, program) NE stavljati u repozitorij
  - obrisati ih ručno ili s make obrisi
*/
