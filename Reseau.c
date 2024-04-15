#include "Reseau.h"
#include "Chaine.h"
#include <stdlib.h>
#include "SVGwriter.h"

Noeud* rechercheCreeNoeudListe(Reseau *R, double x, double y) {
    CellNoeud *current = R->noeuds;
    while (current != NULL) {
        if (current->nd->x == x && current->nd->y == y) {
            return current->nd;
        }
        current = current->suiv;
    }

    Noeud *new_node = (Noeud*)malloc(sizeof(Noeud));
    if (!new_node) return NULL;

    new_node->num = ++R->nbNoeuds;
    new_node->x = x;
    new_node->y = y;
    new_node->voisins = NULL;

    CellNoeud *new_cell = (CellNoeud*)malloc(sizeof(CellNoeud));
    if (!new_cell) {
        free(new_node);
        return NULL;
    }
    
    new_cell->nd = new_node;
    new_cell->suiv = R->noeuds;
    R->noeuds = new_cell;
    
    return new_node;
}

int estDejaVoisin(Noeud* noeud, Noeud* voisinPotentiel) {
    CellNoeud* courant = noeud->voisins;
    while (courant != NULL) {
        if (courant->nd == voisinPotentiel) {
            return 1; // Vrai, ils sont déjà voisins
        }
        courant = courant->suiv;
    }
    return 0; // Faux, ils ne sont pas voisins
}

Reseau* reconstitueReseauListe(Chaines *C) {
    if (C == NULL) return NULL;

    Reseau *R = (Reseau*)malloc(sizeof(Reseau));
    if (R == NULL) return NULL;

    R->nbNoeuds = 0;
    R->gamma = C->gamma;
    R->noeuds = NULL;
    R->commodites = NULL;

    for (CellChaine *cc = C->chaines; cc != NULL; cc = cc->suiv) {
        CellPoint *cp = cc->points;
        Noeud *noeudPrecedent = NULL;

        while (cp != NULL) {
            Noeud *noeudActuel = rechercheCreeNoeudListe(R, cp->x, cp->y);

            if (noeudPrecedent != NULL && noeudActuel->num != noeudPrecedent->num) {
                if (!estDejaVoisin(noeudPrecedent, noeudActuel)) {
                    CellNoeud *voisin = (CellNoeud*)malloc(sizeof(CellNoeud));
                    voisin->nd = noeudActuel;
                    voisin->suiv = noeudPrecedent->voisins;
                    noeudPrecedent->voisins = voisin;
                    
                    // printf("Ajout liaison : %d -> %d\n", noeudPrecedent->num, noeudActuel->num);
                }

                if (!estDejaVoisin(noeudActuel, noeudPrecedent)) {
                    CellNoeud *voisin = (CellNoeud*)malloc(sizeof(CellNoeud));
                    voisin->nd = noeudPrecedent;
                    voisin->suiv = noeudActuel->voisins;
                    noeudActuel->voisins = voisin;
                    
                    // printf("Ajout liaison : %d -> %d\n", noeudActuel->num, noeudPrecedent->num);
                }
            }
            noeudPrecedent = noeudActuel;
            cp = cp->suiv;
        }

        if (cc->points != NULL && cc->points->suiv != NULL) {
            CellPoint *pointA = cc->points;
            while (pointA->suiv->suiv != NULL) {
                pointA = pointA->suiv;
            }
            CellPoint *pointB = pointA->suiv;
            Noeud *extrA = rechercheCreeNoeudListe(R, cc->points->x, cc->points->y);
            Noeud *extrB = rechercheCreeNoeudListe(R, pointB->x, pointB->y);
            
            CellCommodite *commod = (CellCommodite*)malloc(sizeof(CellCommodite));
            commod->extrA = extrA;
            commod->extrB = extrB;
            commod->suiv = R->commodites;
            R->commodites = commod;
        }
    }

    return R;
}

void freeReseau(Reseau *res) { // fonction pas dans l'énoncé 
    if (!res) return;

    CellNoeud *currNode = res->noeuds, *tmpNode;
    while (currNode) {
        tmpNode = currNode;
        currNode = currNode->suiv;
        free(tmpNode->nd); // on alloue de la mémoire pour chaque Noeud
        free(tmpNode);
    }

    CellCommodite *currCommod = res->commodites, *tmpCommod;
    while (currCommod) {
        tmpCommod = currCommod;
        currCommod = currCommod->suiv;
        free(tmpCommod);
    }

    free(res);
}

int nbCommodites(Reseau *R) {
    int compteur = 0;
    CellCommodite *commoditeActuelle = R->commodites;
    while (commoditeActuelle) {
        compteur++;
        commoditeActuelle = commoditeActuelle->suiv;
    }
    return compteur;
}

int nbLiaisons(Reseau *R) {
    int compteur = 0;
    CellNoeud *noeudActuel = R->noeuds;
    while (noeudActuel) {
        CellNoeud *voisinActuel = noeudActuel->nd->voisins;
        while (voisinActuel) {
            compteur++;
            voisinActuel = voisinActuel->suiv;
        }
        noeudActuel = noeudActuel->suiv;
    }
    // Chaque liaison est comptée deux fois (une fois par chaque noeud), donc on divise par 2 (on prut faire autrement aussi)
    return compteur / 2;
}

void ecrireReseau(Reseau *R, FILE *f) {
    if (R == NULL || f == NULL) {
        return;
    }

    fprintf(f, "NbNoeuds: %d\n", R->nbNoeuds);
    fprintf(f, "NbLiaisons: %d\n", nbLiaisons(R));
    fprintf(f, "NbCommodites: %d\n", nbCommodites(R));
    fprintf(f, "Gamma: %d\n", R->gamma);

    // Écriture des noeuds
    CellNoeud *parcoursNoeud = R->noeuds;
    while (parcoursNoeud != NULL) {
        fprintf(f, "v %d %.6f %.6f\n", parcoursNoeud->nd->num, parcoursNoeud->nd->x, parcoursNoeud->nd->y);
        parcoursNoeud = parcoursNoeud->suiv;
    }

    // Écriture des liaisons, en s'assurant de ne pas écrire deux fois la même liaison
    CellNoeud *parcoursLiaison;
    for (parcoursNoeud = R->noeuds; parcoursNoeud != NULL; parcoursNoeud = parcoursNoeud->suiv) {
        for (parcoursLiaison = parcoursNoeud->nd->voisins; parcoursLiaison != NULL; parcoursLiaison = parcoursLiaison->suiv) {
            if (parcoursNoeud->nd->num < parcoursLiaison->nd->num) { // Éviter les doublons
                fprintf(f, "l %d %d\n", parcoursNoeud->nd->num, parcoursLiaison->nd->num);
            }
        }
    }

    // Écriture des commodités
    CellCommodite *parcoursCommodite = R->commodites;
    while (parcoursCommodite != NULL) {
        fprintf(f, "k %d %d\n", parcoursCommodite->extrA->num, parcoursCommodite->extrB->num);
        parcoursCommodite = parcoursCommodite->suiv;
    }
}

void afficheReseauSVG(Reseau *R, char* nomInstance){
    CellNoeud *courN,*courv;
    SVGwriter svg;
    double maxx=0,maxy=0,minx=1e6,miny=1e6;

    courN=R->noeuds;
    while (courN!=NULL){
        if (maxx<courN->nd->x) maxx=courN->nd->x;
        if (maxy<courN->nd->y) maxy=courN->nd->y;
        if (minx>courN->nd->x) minx=courN->nd->x;
        if (miny>courN->nd->y) miny=courN->nd->y;
        courN=courN->suiv;
    }
    SVGinit(&svg,nomInstance,500,500);
    courN=R->noeuds;
    while (courN!=NULL){
        SVGpoint(&svg,500*(courN->nd->x-minx)/(maxx-minx),500*(courN->nd->y-miny)/(maxy-miny));
        courv=courN->nd->voisins;
        while (courv!=NULL){
            if (courv->nd->num<courN->nd->num)
                SVGline(&svg,500*(courv->nd->x-minx)/(maxx-minx),500*(courv->nd->y-miny)/(maxy-miny),500*(courN->nd->x-minx)/(maxx-minx),500*(courN->nd->y-miny)/(maxy-miny));
            courv=courv->suiv;
        }
        courN=courN->suiv;
    }
    SVGfinalize(&svg);


}
