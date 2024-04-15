all: ChaineMain ReconstitueReseau

# Lien de l'exécutable ChaineMain
ChaineMain: ChaineMain.o Chaine.o SVGwriter.o
	gcc -g -o ChaineMain ChaineMain.o Chaine.o SVGwriter.o -lm

# Lien de l'exécutable ReconstitueReseau
ReconstitueReseau: ReconstitueReseau.o Chaine.o SVGwriter.o Reseau.o Hachage.o
	gcc -g -o ReconstitueReseau ReconstitueReseau.o Chaine.o SVGwriter.o Reseau.o Hachage.o -lm

# Compilation des fichiers source en fichiers objet
ChaineMain.o: ChaineMain.c Chaine.h SVGwriter.h
	gcc -g -c ChaineMain.c

ReconstitueReseau.o: ReconstitueReseau.c Chaine.h SVGwriter.h Reseau.h Hachage.h
	gcc -g -c ReconstitueReseau.c

Chaine.o: Chaine.c Chaine.h
	gcc -g -c Chaine.c

SVGwriter.o: SVGwriter.c SVGwriter.h
	gcc -g -c SVGwriter.c

Reseau.o: Reseau.c Reseau.h Chaine.h
	gcc -g -c Reseau.c

Hachage.o: Hachage.c Hachage.h Reseau.h
	gcc -g -c Hachage.c 

clean:
	rm -f *.o *.html ChaineMain ReconstitueReseau
