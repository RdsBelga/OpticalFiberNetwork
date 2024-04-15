#include <stdio.h>
#include <stdlib.h>
#include "Chaine.h"
#include "SVGwriter.h"
#include <math.h> // Pour sqrt et pow

Chaines* lectureChaines(FILE *fichier) {
    Chaines *chaines = (Chaines*)malloc(sizeof(Chaines));
    if (!fichier || !chaines) {
        return NULL; // Gestion simplifiée des erreurs
    }

    fscanf(fichier, "NbChain: %d\nGamma: %d\n", &chaines->nbChaines, &chaines->gamma);
    chaines->chaines = NULL;

    CellChaine *derniereChaine = NULL;
    for (int i = 0; i < chaines->nbChaines; i++) {
        CellChaine *nouvelleChaine = (CellChaine*)malloc(sizeof(CellChaine));
        if (!nouvelleChaine) {
            // Gestion d'erreur simplifiée, considérer de libérer la mémoire allouée précédemment
            return NULL;
        }

        // Initialisation des champs de nouvelleChaine
        nouvelleChaine->points = NULL;
        nouvelleChaine->suiv = NULL;
        fscanf(fichier, "%d", &nouvelleChaine->numero);

        if (!chaines->chaines) {
            chaines->chaines = nouvelleChaine;
        } else {
            derniereChaine->suiv = nouvelleChaine;
        }
        derniereChaine = nouvelleChaine;

        // À ajuster selon le format exact de votre fichier
        int nbPoints; // Supposer que vous avez un moyen de connaître nbPoints
        fscanf(fichier, "%d", &nbPoints); // Hypothétique lecture du nombre de points
        
        CellPoint *dernierPoint = NULL;
        for (int j = 0; j < nbPoints; j++) {
            CellPoint *nouveauPoint = (CellPoint*)malloc(sizeof(CellPoint));
            if (!nouveauPoint) {
                // Gestion d'erreur simplifiée
                return NULL;
            }

            // Initialisation et lecture des coordonnées x, y
            nouveauPoint->suiv = NULL;
            fscanf(fichier, "%lf %lf", &nouveauPoint->x, &nouveauPoint->y);

            if (!nouvelleChaine->points) {
                nouvelleChaine->points = nouveauPoint;
            } else {
                dernierPoint->suiv = nouveauPoint;
            }
            dernierPoint = nouveauPoint;
        }
    }
    return chaines;
}

void ecrireChaines(Chaines *C, FILE *f) {
    if (C == NULL || f == NULL) {
        return; // Retourne si la structure Chaines ou le fichier est NULL
    }

    // Écriture de l'entête avec NbChain et Gamma
    fprintf(f, "NbChain: %d\nGamma: %d\n", C->nbChaines, C->gamma);

    // Parcours de chaque CellChaine dans la structure Chaines
    for (CellChaine *chaineCourante = C->chaines; chaineCourante != NULL; chaineCourante = chaineCourante->suiv) {
        // Comptage du nombre de points dans la chaîne courante pour le formatage correct
        int nbPoints = 0;
        for (CellPoint *pointCourant = chaineCourante->points; pointCourant != NULL; pointCourant = pointCourant->suiv) {
            nbPoints++;
        }

        // Écriture du numéro de la chaine et du nombre de points
        fprintf(f, "%d %d ", chaineCourante->numero, nbPoints);

        // Parcours de chaque CellPoint dans la CellChaine courante
        for (CellPoint *pointCourant = chaineCourante->points; pointCourant != NULL; pointCourant = pointCourant->suiv) {
            // Écriture des coordonnées de chaque point
            fprintf(f, "%.2lf %.2lf ", pointCourant->x, pointCourant->y);
        }

        fprintf(f, "\n");
    }
}

void freeChaines(Chaines *C) {
    if (C == NULL) {
        return; // Si C est déjà NULL, il n'y a rien à libérer
    }

    // Parcourir chaque CellChaine de la liste
    CellChaine *chaineCourante = C->chaines;
    while (chaineCourante != NULL) {
        CellChaine *chaineASupprimer = chaineCourante; // Sauvegarder la référence de la chaine à supprimer
        chaineCourante = chaineCourante->suiv; // Avancer dans la liste

        // Parcourir et libérer chaque CellPoint de la chaine courante
        CellPoint *pointCourant = chaineASupprimer->points;
        while (pointCourant != NULL) {
            CellPoint *pointASupprimer = pointCourant; // Sauvegarder la référence du point à supprimer
            pointCourant = pointCourant->suiv; // Avancer dans la liste

            free(pointASupprimer); // Libérer le CellPoint
        }

        free(chaineASupprimer); // Libérer la CellChaine après avoir libéré tous ses points
    }

    free(C); // Libérer l'objet Chaines lui-même après avoir libéré toutes ses chaines
}

void afficheChainesSVG(Chaines *C, char* nomInstance){
    // int i;
    double maxx=0,maxy=0,minx=1e6,miny=1e6;
    CellChaine *ccour;
    CellPoint *pcour;
    double precx,precy;
    SVGwriter svg;
    ccour=C->chaines;
    while (ccour!=NULL){
        pcour=ccour->points;
        while (pcour!=NULL){
            if (maxx<pcour->x) maxx=pcour->x;
            if (maxy<pcour->y) maxy=pcour->y;
            if (minx>pcour->x) minx=pcour->x;
            if (miny>pcour->y) miny=pcour->y;  
            pcour=pcour->suiv;
        }
    ccour=ccour->suiv;
    }
    SVGinit(&svg,nomInstance,500,500);
    ccour=C->chaines;
    while (ccour!=NULL){
        pcour=ccour->points;
        SVGlineRandColor(&svg);
        SVGpoint(&svg,500*(pcour->x-minx)/(maxx-minx),500*(pcour->y-miny)/(maxy-miny)); 
        precx=pcour->x;
        precy=pcour->y;  
        pcour=pcour->suiv;
        while (pcour!=NULL){
            SVGline(&svg,500*(precx-minx)/(maxx-minx),500*(precy-miny)/(maxy-miny),500*(pcour->x-minx)/(maxx-minx),500*(pcour->y-miny)/(maxy-miny));
            SVGpoint(&svg,500*(pcour->x-minx)/(maxx-minx),500*(pcour->y-miny)/(maxy-miny));
            precx=pcour->x;
            precy=pcour->y;    
            pcour=pcour->suiv;
        }
        ccour=ccour->suiv;
    }
    SVGfinalize(&svg);
}

double longueurChaine(CellChaine *c) {
    if (c == NULL || c->points == NULL) {
        return 0.0;
    }

    double longueur = 0.0;
    CellPoint *pointActuel = c->points;
    CellPoint *pointSuivant = pointActuel->suiv;

    while (pointSuivant != NULL) {
        double dx = pointSuivant->x - pointActuel->x;
        double dy = pointSuivant->y - pointActuel->y;
        longueur += sqrt(dx * dx + dy * dy);

        pointActuel = pointSuivant;
        pointSuivant = pointSuivant->suiv;
    }

    return longueur;
}

double longueurTotale(Chaines *C) {
    if (C == NULL || C->chaines == NULL) {
        return 0.0;
    }

    double longueurTotale = 0.0;
    CellChaine *chaineCourante = C->chaines;

    while (chaineCourante != NULL) {
        longueurTotale += longueurChaine(chaineCourante);
        chaineCourante = chaineCourante->suiv;
    }

    return longueurTotale;
}

int comptePointsTotal(Chaines *C) {
    if (C == NULL) {
        return 0; // Retourne 0 si la structure Chaines est NULL
    }

    int totalPoints = 0;
    CellChaine *chaineCourante = C->chaines;

    while (chaineCourante != NULL) {
        CellPoint *pointCourant = chaineCourante->points;
        while (pointCourant != NULL) {
            totalPoints++; // Incrémente le compteur pour chaque point trouvé
            pointCourant = pointCourant->suiv;
        }
        chaineCourante = chaineCourante->suiv;
    }

    return totalPoints;
}
    