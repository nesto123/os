# Primjer datoteke za prevođenje

CFLAGS = -Wall -MMD
LDFLAGS = -O
LDLIBS = -lgmp -lm

OBJEKTI = program.o slucajni_prosti_broj.o
PROGRAM = prog

$(PROGRAM): $(OBJEKTI)
	$(CC) $(LDFLAGS) $(OBJEKTI) $(LDLIBS) -o $(PROGRAM)

.PHONY: pokreni obrisi
pokreni: $(PROGRAM)
	@./$(PROGRAM)

H_VEZE := $(OBJEKTI:.o=.d)

obrisi:
	-rm -rf $(OBJEKTI) $(H_VEZE) $(PROGRAM)

-include $(H_VEZE)
