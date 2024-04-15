#include "Reseau.h"
#include "Chaine.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <fichier_cha> <methode>\n", argv[0]);
        printf("Methode: 1 pour liste chaînée, 2 pour table de hachage, 3 pour arbre.\n");
        return 1;
    }

    int methode = atoi(argv[2]);
    FILE *fichier = fopen(argv[1], "r");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return 1;
    }

    Chaines *chaines = lectureChaines(fichier);
    fclose(fichier);
    if (chaines == NULL) {
        printf("Erreur lors de la lecture des chaines.\n");
        return 1;
    }

    Reseau *reseau;
    switch (methode) {
        case 1:
            reseau = reconstitueReseauListe(chaines);
            break;
        case 2:
            // reseau = reconstitueReseauHachage(chaines, tailleTable); // À implémenter plus tard Partie hachage 
            break;
        case 3:
            // reseau = reconstitueReseauArbre(chaines); // A implémenter plus tard partie arbre
            break;
        default:
            printf("Methode inconnue.\n");
            freeChaines(chaines);
            return 1;
    }

    if (reseau == NULL) {
        printf("Erreur lors de la reconstitution du réseau.\n");
        freeChaines(chaines);
        return 1;
    }

    ecrireReseau(reseau, stdout); // Ici, on l'écrit sur la sortie standard
    afficheReseauSVG(reseau, "mon_reseau");


    // Libération de la mémoire
    freeReseau(reseau);
    freeChaines(chaines);
    
    return 0;
}
 
