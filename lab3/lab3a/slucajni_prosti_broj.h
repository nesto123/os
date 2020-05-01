#pragma once // zaštita od višestrukog uključivanja

#include <gmp.h>
#include <inttypes.h>

struct gmp_pomocno {
	gmp_randstate_t stanje;
	mpz_t slucajan_broj, prosti_broj;
};

void inicijaliziraj_generator(struct gmp_pomocno *, unsigned);
void obrisi_generator(struct gmp_pomocno *);
uint64_t daj_novi_slucajan_prosti_broj (struct gmp_pomocno *);
