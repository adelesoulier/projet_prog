// Code C projet prog, version Maëlle

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

// Je définis le tableau de structures (tableau malloc) qui
// contiendra toutes les cases

struct courants {
    double compoN;
    double compoE;
    int compteur;
};

// Le fait que je définis ici ma structure me permet de ne pas
// devoir la rentrer en argument à chaque fois que je veux l'utiliser
// dans une fonction.
struct courants * Cases = NULL;


// Fonction pour lire le fichier CSV des courants et les mettre dans notre structure
bool readCsvCourants(char * filenameNVEL, char * filenameEVEL, int sizeLat, int sizeLong) {
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
    char buffer[10000]; // pas besoin de mettre 260'000 (nb de cases du tableau),
    // car ici c'est le nombre de lignes
    while (fgets(buffer, 10000, fileN) != NULL) {
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

    while (fgets(buffer, 10000, fileE) != NULL) {
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
    return longi*360 + lat; 
} // ON L'UTILISE ?????????????????????????????????

// Conversion des lati
int exactocasei(double lat, double longi){
    lat = (lat + 89.75)*2; //conversion de la latitude en la colonne (donne un entier pour tous les .25 et .75)
    longi = (longi + 179.75)*2; // idem, mais de longi en ligne
    lat = floor(lat + 0.5); // arrondir à l'entier le plus proche (il n'y a que floor qui existe,
    // mais si on fait + 0.5 revient au même que d'arrondir à l'entier le plus proche)
    longi = floor(longi + 0.5);
    return longi*360 + lat; // on convertit en i
}




void plastique(double lat, double longi, int years){
    int i = 0;
    double latExact = lat;
    double longExact = longi;
    int indexCase = exactocasei(latExact, longExact);
    int prevIndexCase = exactocasei(latExact, longExact);
    int saturation = 100; // A MODIFIER EN FONCTION DE LA CAPACITÉ QU'ON VEUT!!


    //on fait premièrement simulation sur 1 an, avec avancée chaque heure
    for (int t = 0; t < 24*365*years){
        // Courant marin: nous allons lire dans les fichiers csv les composantes
        // correspondant à la case dans laquelle se trouve notre plastique au temps t.

        // Est-ce que le plastique était déjà dans la case ou non? (Si non: i == 0 avant)
        if (i == 0){
            Cases[indexCase].compteur += 1;
            i = 1;
        }
        
        // Composante du courant [deg/h]
        double dlat = Cases[indexCase].compoN*1; //je fais *1 pour rappeler qu'on fait *1h
        // (si on change boucle ou que unités pas des /h -> changer!!)
        double dlong = Cases[indexCase].compoE*1;

        // Aléa
        double alat = randomNumber(30) - 15; //
        double along = randomNumber(30) - 15; // COMMENT DETERMINER SI C'EST UN BON ALEA?????????????????
        // essayer, on est obligées de passer par visualisation des courants et trajectoire de notre plastique.....................

        // Déplacer potentiel du plastique
        latExact += dlat + alat;
        longExact += dlong + along;
        indexCase = exactocasei(latExact, longExact);

        // On vérifie si on va rester dans la même case ou non
        if (prevIndexCase != indexCase){
            if (Cases[indexCase].compteur < saturation || Cases[indexCase].compoN == nan || Cases[indexCase].compoE == nan){
                // CONDITION EST JUSTE ??????????????????????????????????????? BCS 3 "OU" ET LES NAN OK?????????????????????
                Cases[indexCase].compteur -= 1;
                i = 0; // si on sort de la case
            }
            // si on a des trucs négatifs dans les compteurs vient de là,
            // mais si fonctionne est pas censé arriver

            // sinon, donc si on a Cases[indexCase].compteur >= saturation, on sort de la fonction,
            // et le compteur de la case prevIndexCase a toujours le plastique en mémoire

        }

        // Conditions de terminaison:
        //  1) une case est saturée
        //  2) on est sur un nan -> on l'enlève du compteur de la case dans l'eau et on ajoute à la case
        //      nan (ce qu'on a bien fait puisque la condition)
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

        //  2) nan dans le futur index
        if (Cases[indexCase].compoN == nan || Cases[indexCase].compoE == nan) break;

        prevIndexCase = indexCase;


    }


int main(int argc, char * argv[]) {
    // on initialise le temps, sera utile pour aléatoire de propagation d'un plastique
    srandom(time(NULL));

    // On réserve la place pour notre tableau de cases
    Cases = malloc(720*360 * sizeof (struct courants));
    // Notre grille de courants est de la forme (long, lat) :
    // LONGITUDES = LIGNES (720) = y = compoE ???????????????????C'EST JUSTE???????????????
    // LATITUDES = COLONNES (360) = x = compoN ???????????????????C'EST JUSTE???????????????
    // avec la conversion latIndex = (lat + 89.75)*2; longIndex = (longi + 179.75)*2

    // on remplit le tableau malloc Cases
    readCsvCourants(("NOM_DU_FICHIER_COURANTS_MARINS.csv", 360, 720); 

    // IL FAUT INITIALISER LE CONTINENT PLASTIQUES, CALCULER MOYPOND POUR NB PLASTIQUES/KG,
    // SATURATION, GRADIENT POUR CONTINENT PLASTIQUE AUTOUR: COMMENT DESCEND SATURATION AU FUR
    // ET A MESURES QU'ON S'ELOIGNE DU "COEUR" DU CONTINENT? ET INITIALISER LES AUTRES COMPTEURS
    // DE CASES À 0

    
    

    free(Cases);
    return 0;
}
