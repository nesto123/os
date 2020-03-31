/*
 * Jednostavni primjer za generiranje prostih brojeva korištenjem
   "The GNU Multiple Precision Arithmetic Library" (GMP)
 */

#include <stdio.h>
#include <time.h>

#include "slucajni_prosti_broj.h"

#define MASKA(bitova)			(-1 + (1<<(bitova)) )
#define UZMIBITOVE(broj,prvi,bitova) 	( ( (broj) >> (64-(prvi)) ) & MASKA(bitova) )

struct gmp_pomocno p;

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

uint64_t generiraj_dobar_broj(uint64_t	velicina_grupe)
{
	uint64_t	najbolji_broj = 0, broj, z;
	uint64_t	najbolja_zbrckanost = 0;
	uint64_t i/*,	velicina_grupe	=	1000*/;

	for( i = 0 ; i	<=	velicina_grupe-1;	i++){
		broj = daj_novi_slucajan_prosti_broj	(&p);
		z = zbrckanost (broj);
		if (	z > najbolja_zbrckanost){
			najbolja_zbrckanost = z;
			najbolji_broj = broj;
		}
	}
	return najbolji_broj;
}

#define	N	10
uint64_t	MS[N];
uint64_t	ULAZ=0, IZLAZ=0; //zajedničke varijable

void stavi_u_MS(uint64_t	broj)
{
	MS[ULAZ] = broj;
	ULAZ = (ULAZ + 1) % N;
}
uint64_t	uzmi_iz_MS()
{
	uint64_t	broj = MS[IZLAZ];
	IZLAZ = (IZLAZ + 1) % N;
	return broj;
}

uint64_t	procjeni_velicinu_grupe()
{
	uint64_t	M = 1000; //(ne preveliki da jedna petlja bude preduga,
	          //ni premali da dohvat vremena utječe na rezultat)
	uint64_t	SEKUNDI = 10 ;//(može i malo manje, ali barem 5);
	time_t t1	=	time(NULL);
	uint64_t	k = 0, i;

	uint64_t	velicina_grupe = 1; //da generiraj_dobar_broj() obavi 1 iteraciju
	while (	time(NULL) < t1 + SEKUNDI	){
		k++;
		for (i = 0 ;	i <	M-1 ;	i++){
			uint64_t	broj = generiraj_dobar_broj(velicina_grupe);
			stavi_u_MS(	broj	);

			//mora se osigurati da ova petlja preživi optimizaciju
			//lako se kasnije ULAZ i IZLAZ ponovno postave na nulu
		}

	}
	uint64_t	brojeva_u_sekundi = k * M / SEKUNDI;
	velicina_grupe = brojeva_u_sekundi *2/5; //(ili bolje *2/5 ili slično)
	return	velicina_grupe;
}
uint64_t polje[100];

int main(int argc, char *argv[])
{
	uint64_t broj_ispisa	=	0,	broj;
	//clock_t c;
	time_t t1;
	inicijaliziraj_generator (&p, 0);
	ULAZ = IZLAZ = 0;
	uint64_t	velicina_grupe	=	procjeni_velicinu_grupe();
	t1	=	time(NULL); // in seconds

	while ( broj_ispisa < 10 ){

		broj = generiraj_dobar_broj(velicina_grupe); //kod ispod

		stavi_u_MS( broj	);

		if( time(NULL) != t1 ) {
//printf("%lx _ time:%ld\n", broj_ispisa+1, time(NULL));
			printf("%lx\n" ,	uzmi_iz_MS()	);//uzmi broj iz MS i ispiši ga --uzmi_iz_MS
			broj_ispisa++;
			t1 = time(NULL);
		}
	}
//t	=	((double)clock())/CLOCKS_PER_SEC; // in seconds
/*
	while ( broj_ispisa < 10 ){

		broj = generiraj_dobar_broj(velicina_grupe); //kod ispod

		stavi_u_MS( broj	);

		if( (((double)clock())/CLOCKS_PER_SEC) != t ) {
printf("%lx _ time:%f\n", broj_ispisa+1, (((double)clock())/CLOCKS_PER_SEC));
			printf("%lx\n" ,	uzmi_iz_MS()	);//uzmi broj iz MS i ispiši ga --uzmi_iz_MS
			broj_ispisa++;
			t = ((double)clock())/CLOCKS_PER_SEC;
		}
	}
*/
	obrisi_generator (&p);

	return 0;
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
