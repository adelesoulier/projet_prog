#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

int ll2i(int longi, int lat){ 
    return lat*720 + longi; ////////////////ETAIT FAUX!!!!!!!!!!!!!!!!!
} /////////////////////////////////////// OUI ON L'UTILISE!!


struct courants {
    double compoN;
    double compoE;
    int compteur;
};
struct courants * Cases = NULL;





int lat2i1D(double lat){
    lat = (lat + 89.75)*2; //conversion de la latitude en la colonne (donne un entier pour tous les .25 et .75)
    lat = (int) floor(lat + 0.5); // arrondir à l'entier le plus proche (il n'y a que floor qui existe,
    // mais si on fait + 0.5 revient au même que d'arrondir à l'entier le plus proche)
    return lat; // on a déjà i car ici l'index de latitude correspond directement à i (de 0 à 359)
}

int long2i1D(double longi){
    longi = (longi + 179.75)*2; // idem, mais de longi en ligne
    longi = (int) floor(longi + 0.5);
    return longi;
}



void fctLatInit(double * latInit){
    // des commentaires pour les deux fonctions qui suivent se trouvent dans le code principal
    int I1 = lat2i1D(25);
    int I2 = lat2i1D(28);
    int I3 = lat2i1D(36);
    int I4 = lat2i1D(40);
    double concentration = 0;
    double penteMontee = (double) 1/(I2-I1);
    double penteDescente = (double) -1/(I4-I3);
    printf("%d\n", I1);
    printf("%d\n", I2);
    printf("%d\n", I3);
    printf("%d\n", I4);
    printf("%f\n", concentration);
    printf("%f\n", penteMontee);
    printf("%f\n", penteDescente);
    printf("--------------------------\n");
    for (int i = 0; i < 360; i++){ // ON A BIEN LE (0,0) TOUT EN BAS À GAUCHE????????????????? oui
        if (i < I1 || i > I4) latInit[i] = 0; // dans ce sens car est 0 en-bas et augmente en montant
        else if (i >= I1 && i <= I2){
            concentration = penteMontee*(i-I1); // AVANT : concentration = (int) floor(penteMontee*(i-I1));
            latInit[i] = concentration;
        } else if (i > I2 && i < I3) latInit[i] = 1;
        else if (i >= I3 && i <= I4){
            concentration = 1 + penteDescente*(i-I3);
            latInit[i] = concentration;
        }
    }
    ///////////////printf("%f\n", latInit[I1-1]);
    ///////////////printf("%f\n", latInit[I1]);
    ///////////////printf("%f\n", latInit[I1+1]);
    ///////////////printf("%f\n", latInit[I2-1]);
    ///////////////printf("%f\n", latInit[I2]);
    ///////////////printf("%f\n", latInit[I2+1]);
    ///////////////printf("%f\n", latInit[I3-1]);
    ///////////////printf("%f\n", latInit[I3]);
    ///////////////printf("%f\n", latInit[I3+1]);
    ///////////////printf("%f\n", latInit[I4-1]);
    ///////////////printf("%f\n", latInit[I4]);
    ///////////////printf("%f\n", latInit[I4+1]);
    ///////////////printf("--------------------\n");
}

void fctLongInit(double * longInit){
    int I1 = long2i1D(-160);
    int I2 = long2i1D(-144);
    int I3 = long2i1D(-134);
    int I4 = long2i1D(-130);
    double concentration = 0;
    double penteMontee = (double) 1/(I2-I1);
    double penteDescente = (double) -1/(I4-I3);
    printf("%d\n", I1);
    printf("%d\n", I2);
    printf("%d\n", I3);
    printf("%d\n", I4);
    printf("%f\n", concentration);
    printf("%f\n", penteMontee);
    printf("%f\n", penteDescente);
    printf("--------------------------\n");
    for (int i = 0; i < 720; i++){ // ON A BIEN LE (0,0) TOUT EN BAS À GAUCHE?????????????????
        if (i < I1 || i > I4) longInit[i] = 0; // dans ce sens car est 0 à gauche et augmente en allant à droite
        else if (i >= I1 && i <= I2){
            concentration = penteMontee*(i-I1); // AVANT : concentration = (int) floor(penteMontee*(i-I1));
            longInit[i] = concentration;
        } else if (i > I2 && i < I3) longInit[i] = 1;
        else if (i >= I3 && i <= I4){
            concentration = 1 + penteDescente*(i-I3);
            longInit[i] = concentration;
        }
    }
    ///////////////printf("%f\n", longInit[I1-1]);
    ///////////////printf("%f\n", longInit[I1]);
    ///////////////printf("%f\n", longInit[I1+1]);
    ///////////////printf("%f\n", longInit[I2-1]);
    ///////////////printf("%f\n", longInit[I2]);
    ///////////////printf("%f\n", longInit[I2+1]);
    ///////////////printf("%f\n", longInit[I3-1]);
    ///////////////printf("%f\n", longInit[I3]);
    ///////////////printf("%f\n", longInit[I3+1]);
    ///////////////printf("%f\n", longInit[I4-1]);
    ///////////////printf("%f\n", longInit[I4]);
    ///////////////printf("%f\n", longInit[I4+1]);
    ///////////////printf("--------------------\n");
}

void GPGPinit(double * longInit, double * latInit, int saturation){
    for (int longi = 0; longi < 720; longi++){
        for (int lat = 0; lat < 360; lat++){
            int i = ll2i(longi, lat);
            Cases[i].compteur = (int) floor(saturation*latInit[lat]*longInit[longi]);
        }
    }
}



int main(int argc, char * argv[]) {

    // On crée nos deux tableaux pour l'initialisation: les lat et les long
    double * longInit = malloc(720 * sizeof (double));
    double * latInit = malloc(360 * sizeof (double));
    Cases = malloc(720*360 * sizeof (struct courants));

    //----- Notre grille de courants est de la forme (long, lat) :
    //----- LONGITUDES = LIGNES (720) = y = compoE ???????????????????C'EST JUSTE???????????????
    //----- LATITUDES = COLONNES (360) = x = compoN ???????????????????C'EST JUSTE???????????????
    //----- avec la conversion latIndex = (lat + 89.75)*2; longIndex = (longi + 179.75)*2
    //////////int test1 = lat2i1D(-89);
    ///////////////printf("%d\n", test1);
    fctLongInit(longInit);
    fctLatInit(latInit);
    ///////////////for (int i=0; i<=720; i++){
    ///////////////    printf("%f\n", longInit[i]);
    ///////////////}
    

    // On crée le fichier CSV pour l'initialisation du continent de plastiques
    GPGPinit(longInit,latInit, 50);
    FILE * file = fopen("GPGPinit.csv", "w");

    for (int lat = 359; lat >= 0 ; lat -= 1) { // comme cela car on a notre origine en-bas à
    // gauche mais on commence à écrire en-haut à gauche
    // A TEST SUR CARTOPY SI BIEN JUSTE
        for (int longi = 0; longi < 720; longi++) {
            if (longi > 0) fprintf(file, ", ");
            fprintf(file, "%d", Cases[ll2i(longi,lat)].compteur);
        }

        fprintf(file, "\n");
        // donne une matrice avec (y,x) = (lignes, colonnes),
        // ce qui correspond bien à une origine en-haut à gauche,
        // on descend et on va à droite.
    }

    fclose(file);
    

    //-----free(Cases);
    free(latInit);
    free(longInit);
    free(Cases);
    return 0;
}