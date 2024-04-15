#ifndef __HACHAGE_H__
#define __HACHAGE_H__	
#include <stdio.h>

typedef struct{
  int nbElement; 
  int tailleMax;
  CellNoeud** T;
} TableHachage ;

int fonction_h(double x, double y);
void test_fonction_h();
int key_hash(int m, double x, double y);
Noeud* rechercheCreeNoeudHachage(Reseau* R, TableHachage* H, double x, double y);


#endif
