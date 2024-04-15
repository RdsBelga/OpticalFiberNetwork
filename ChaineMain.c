#include <stdio.h>
#include <stdlib.h>
#include "Chaine.h"
#include "SVGwriter.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <fichier_instance_à_lire> <fichier_html_à_écrire>\n", argv[0]);
        return 1;
    }

    FILE *fichierEntree = fopen(argv[1], "r");
    if (fichierEntree == NULL) {
        perror("Erreur lors de l'ouverture du fichier d'entrée");
        return 1;
    }

    Chaines *chaines = lectureChaines(fichierEntree);
    fclose(fichierEntree);

    if (chaines == NULL) {
        printf("Erreur lors de la lecture des chaines.\n");
        return 1;
    }

    ecrireChaines(chaines, stdout);
    afficheChainesSVG(chaines, argv[2]);

    //double longueurTot = longueurTotale(chaines);
    // printf("Longueur totale des chaines: %f\n", longueurTot);

    //int totalPoints = comptePointsTotal(chaines);
    //printf("Nombre total de points: %d\n", totalPoints);

    freeChaines(chaines);

    return 0;
}
