#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

int main()
{
	uint64_t a; // 64-bitovni nenegativni cijeli broj
	            // najčešće isto što i unsigned long int

	a = 0xfedcba9876543210ULL; // prefix 0x => heksadekadski zapis
	                           // sufix ULL => unsigned long long
	printf ("Prikaz 64-bitovnog broja u formatima:"
		"\nheksadekadski:      %" PRIx64       // "lx" ili slično
		"\nkao cijeli broj:    %" PRId64       // "ld" ili slično
		"\nkao pozitivan broj: %" PRIu64 "\n", // "lu" ili slično
		a, a, a
	);

	return 0;
}
/* prevođenje: gcc int64_t.c -lgmp -lm -o int64
   pokretanje: ./int64
   ispis:
   Prikaz 64-bitovnog broja u formatima:
   heksadekadski:      fedcba9876543210
   kao cijeli broj:    -81985529216486896
   kao pozitivan broj: 18364758544493064720
*/
