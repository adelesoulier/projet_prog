// Code C projet prog, version Maëlle

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

// Je définis le tableau de structures (tableau malloc) qui
// contiendra toutes les cases

struct grille {
    double compoN;
    double compoE;
    int compteur;
};

// Le fait que je définis ici ma structure me permet de ne pas
// devoir la rentrer en argument à chaque fois que je veux l'utiliser
// dans une fonction.
struct grille * Cases = NULL;


// Fonction pour lire le fichier CSV des courants et les mettre dans notre structure
bool readCsv(char * filenameNVEL, char * filenameEVEL, int sizeLat, int sizeLong) {
    FILE * fileN = fopen(filenameNVEL, "r");
    FILE * fileE = fopen(filenameEVEL, "r");
    if (fileN == NULL) {
        fprintf(stderr, "File %s not found.", filenameN);
        return false;
    }
    if (fileE == NULL) {
        fprintf(stderr, "File %s not found.", filenameE);
        return false;
    }

    int y = 0;
    char buffer[260000]; // euhh est juste?? bcs le notre est 260'000 cases environ mais était écrit 10'000????????????????????????'
    while (fgets(buffer, 260000, fileN) != NULL) {
        int x = 0;
        char * start = buffer;
        while (true) {
            Cases[y * sizeLat + x].compoN = atof(start);
            start = strchr(start, ',');
            if (start == NULL) break;
            start += 1;

            x += 1;
            if (x >= sizeLat) break;
        }

        y += 1;
        if (y >= sizeLong) break;
    }
    fclose(fileN);

    while (fgets(buffer, 260000, fileE) != NULL) {
        int x = 0;
        char * start = buffer;
        while (true) {
            Cases[y * sizeLat + x].compoE = atof(start);
            start = strchr(start, ',');
            if (start == NULL) break;
            start += 1;

            x += 1;
            if (x >= sizeLat) break;
        }

        y += 1;
        if (y >= sizeLong) break;
    }
    fclose(fileE);

    
    return true;
}

// Fonction pour lire les fichiers CSV générale
bool readCsvGen(char * filename, double * values, int sizeX, int sizeY) {
    FILE * file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "File %s not found.", filename);
        return false;
    }

    int y = 0;
    char buffer[10000];
    while (fgets(buffer, 10000, file) != NULL) {
        int x = 0;
        char * start = buffer;
        while (true) {
            values[y * sizeX + x] = atof(start);
            start = strchr(start, ',');
            if (start == NULL) break;
            start += 1;

            x += 1;
            if (x >= sizeX) break;
        }

        y += 1;
        if (y >= sizeY) break;
    }

    fclose(file);
    return true;
}


// Fonction qui fait la conversion des coordonnées de lat, longi en i 
int ll2i(int lat, int longi){
    return longi*360 + lat; //C'EST JUSTE?????????????????????????
} // ON L'UTILISE ?????????????????????????????????

// Conversion des lati
int exactocasei(int lat, int longi){
    // CONVERSIONS DES COORDONNEES PRECISES EN INDEX DE CASES
    return longi*360 + lat; //C'EST JUSTE?????????????????????????
}



//FAIRE LA CONVERSION DE m EN ° POUR LAT ET LONG, DIFF POUR CHAQUE (UNE FONCTION?)

void plastique(double lat, double longi){
    int i = 0;
    double latExact = lat;
    double longExact = longi;
    int indexCase = exactocasei(latExact, longExact);
    int prevIndexCase = exactocasei(latExact, longExact);
    int saturation = 100; // A MODIFIER EN FONCTION DE LA CAPACITÉ QU'ON VEUT!!


    //on fait premièrement simulation sur 1 an, avec avancée chaque heure
    for (int t = 0; t < 24*365){
        // Courant marin: nous allons lire dans les fichiers csv les composantes
        // correspondant à la case dans laquelle se trouve notre plastique au temps t.
        // est-ce que c'est déjà les bonnes unités pour les fichiers csv?????????????????

        // Est-ce que le plastique était déjà dans la case ou non? (Si non: i == 0 avant)
        if (i == 0){
            Cases[indexCase].compteur += 1;
            i = 1;
        }
        
        // Composante du courant (J'ASSUME QUE C'EST DEJA EN DEGRES/H)
        double dlat = Cases[indexCase].compoN*1; //je fais *1 pour rappeler qu'on fait *1h
        // (si on change boucle ou que unités pas des /h -> changer!!)
        double dlong = Cases[indexCase].compoE*1;

        // Aléa
        double alat = randomNumber(30) - 15;
        double along = randomNumber(30) - 15;

        // Déplacer le plastique
        latExact += dlat + alat;
        longExact += dlong + along;
        indexCase = exactocasei(latExact, longExact);

        // On vérifie si on va rester dans la même case ou non
        if (prevIndexCase != indexCase) i = 0; // si on sort de la case

        // Conditions de terminaison:
        //  1) une case est saturée
        //  2) on est sur un nan???????????????????
        //  3) autres???????????????????

        //  1) Case saturée
        if (Cases[indexCase].compteur >= saturation) break; // le > est nécessaire pour si
        // plusieurs plastiques arrivent simultanément sur la case
        // le = est là car nous venons d'arriver sur cette case (sinon on n'aurait pas effectué
        // de déplacement car cette condition aurait été remplie à l'itération précédente)
        // donc le compteur de la case sur laquelle nous arrivons n'ayant pas encore été mis à jour,
        // (pour éviter d'avoir des problèmes sur les bords), il faut que le compteur ne soit pas
        // >= à la saturation (si est égal ou plus, on s'arrête là (donc dans prevIndexCase, car
        // la saturation dans indexCase est déjà atteinte))

        prevIndexCase = indexCase;


    }
    


int main(int argc, char * argv[]) {
    // on initialise le temps, sera utile pour aléatoire de propagation d'un plastique
    srandom(time(NULL));

    // On réserve la place pour notre tableau de cases
    Cases = malloc(720*360 * sizeof (struct grille));
    // Notre grille est de la forme (long, lat) :
    // LONGITUDES = LIGNES (720) = y = compoE ???????????????????C'EST JUSTE???????????????
    // LATITUDES = COLONNES (360) = x = compoN ???????????????????C'EST JUSTE???????????????
    // avec la conversion ??????????????????????????

    // on remplit le tableau malloc Cases
    readCsvCases(("NOM_DU_FICHIER_COURANTS_MARINS.csv", 360, 720); 

    
    

    free(Cases);
    return 0;
}