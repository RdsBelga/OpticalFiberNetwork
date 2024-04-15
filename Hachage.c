#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Chaine.h"
#include "Reseau.h"
#include "Hachage.h"

int fonction_h(double x, double y){
    return (int)(y+(x+y)*(x+y+1)*1.0/2);
} // Nous apercevons que la 

void test_fonction_h(){
    int x, y;
    for (x = 0; x <= 10; x++) {
        for (y = 0; y <= 10; y++) {
            printf("fonction_h(%d, %d) = %d\n", x, y, fonction_h(x, y));
        }
    }
}

int key_hash(int m, double x, double y){

    int k=fonction_h(x,y);
    double A=(sqrt(5)-1)/2;
    return m*(k*A-(int)(k*A));
}

Noeud* rechercheCreeNoeudHachage(Reseau* R, TableHachage* H, double x, double y) {

    int index = key_hash(H->tailleMax, x, y);

    // On recherche si le noeud est dans la table de hashage 
    CellNoeud* courant = H->T[index];
    while (courant != NULL) {
        if (courant->nd->x == x && courant->nd->y == y) {
            return courant->nd; // On trouve le noeud
        }
        courant = courant->suiv;
    }

    // Si on le trouve pas, on le cree
    Noeud* nouv_noeud = (Noeud*)malloc(sizeof(Noeud));
    if (!nouv_noeud) {

        return NULL;
    }
    nouv_noeud->x = x;
    nouv_noeud->y = y;
    nouv_noeud->num = ++R->nbNoeuds; // rien capter c'est chatgpt qui ma dit de corriger comme ca 

    // on ajoute le nv noeud ds la table 
    //  on creer une nouvelle structure CellNoeud qui va contenir un pointeur vers nouv_noeud
    CellNoeud* nouv_cell = (CellNoeud*)malloc(sizeof(CellNoeud));

    if (!nouv_cell) {
        free(nouv_noeud);
        return NULL;
    }
    nouv_cell->nd = nouv_noeud;
    nouv_cell->suiv = H->T[index];
    H->T[index] = nouv_cell;

    // On ajoute le noeud dans la liste de noeud du reseau 
    nouv_cell->suiv = R->noeuds;
    R->noeuds = nouv_cell;

    return nouv_noeud;
}
