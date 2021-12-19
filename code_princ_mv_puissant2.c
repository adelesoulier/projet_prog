#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// On définit les structures:

// Celle pour une case de 0.5° x 0.5° :
// composition longitudinale et transversale du courant dedans et compteur de plastique.
struct courants
{
    double compoN;
    double compoE;
    unsigned long compteur;
};

// Celle pour un paquet de plastiques :
// Latitude, Longitude et indice i
// Explication de l'indice i:
// - i = 0 : si le paquet vient d'arriver sur la case actuelle (n'y était pas à l'itération d'avant)
// - i = 1 : si le paquet était déjà sur la case actuelle (à l'itération d'avant)
// - i = 2 : le paquet est à côté d'une case saturée ou sur un continent/île (= immobile)
struct paquet
{
    double lat;
    double longi;
    int i;
};

// On définit le tableau de structures (tableau malloc) qui
// contiendra toutes les cases. (structure globale du tableau expliquée dans le main)
// Le fait qu'on définit ici le tableau nous permet de ne pas
// devoir la rentrer en argument à chaque fois qu'on veut l'utiliser
// dans une fonction.
struct courants *Cases = NULL;

////////////////////////////////////////////
// FONCTIONS D'ÉCRITURE DU FICHIER DE SORTIE
////////////////////////////////////////////

int writeCsv(char *filename, unsigned long *tableau, int duree)
{

    int y = duree;

    FILE *fichier = fopen(filename, "w+");
    if (fichier == NULL)
    {
        printf("File %s can't be created.", filename);
        return 0;
    }

    for (int i = 0; i < y * 360 * 720; i++)
    {
        unsigned long fillvalue = tableau[i];

        if ((i + 1) % (360 * 720) == 0)
        {
            fprintf(fichier, " %lu\n", fillvalue);
            continue;
        }
        fprintf(fichier, " %lu,", fillvalue);
    }
    fclose(fichier);
    return 0;
}

///////////////////////////////////
// FONCTIONS DE LECTURE DE FICHIERS
///////////////////////////////////

// Fonction pour lire le fichier CSV des courants et les mettre dans notre structure
bool readCsvCourantsNVEL(char *filenameNVEL, int sizeLong, int sizeLat)
{
    FILE *fileN = fopen(filenameNVEL, "r");

    if (fileN == NULL)
    {
        fprintf(stderr, "File %s not found.\n", filenameNVEL);
        return false;
    }

    int y = 0;
    char buffer[1000000];
    while (fgets(buffer, 1000000, fileN) != NULL)
    {
        int x = 0;
        char *start = buffer;
        while (true)
        {
            Cases[y * sizeLong + x].compoN = atof(start);
            start = strchr(start, ',');
            if (start == NULL)
                break;
            start += 1;

            x += 1;
            if (x >= sizeLong)
                break;
        }

        y += 1;
        if (y >= sizeLat)
            break;
    }
    fclose(fileN);

    return true;
}

bool readCsvCourantsEVEL(char *filenameEVEL, int sizeLong, int sizeLat)
{
    FILE *fileE = fopen(filenameEVEL, "r");

    if (fileE == NULL)
    {
        fprintf(stderr, "File %s not found.\n", filenameEVEL);
        return false;
    }

    int y = 0;
    char buffer[1000000];
    while (fgets(buffer, 1000000, fileE) != NULL)
    {
        int x = 0;
        char *start = buffer;
        while (true)
        {
            Cases[y * sizeLong + x].compoE = atof(start);

            start = strchr(start, ',');
            if (start == NULL)
                break;
            start += 1;

            x += 1;
            if (x >= sizeLong)
                break;
        }

        y += 1;
        if (y >= sizeLat)
            break;
    }
    fclose(fileE);

    return true;
}

// Fonction pour lire les fichiers CSV générale
bool readCsvGen(char *filename, double *values, int sizeX, int sizeY, int skip_x, int skip_y)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "File %s not found.\n", filename);
        return false;
    }

    int y = 0;
    char buffer[1000000];
    while (fgets(buffer, 1000000, file) != NULL)
    {
        if (y >= skip_y)
        {
            int x = 0;
            char *start = buffer;
            while (true)
            {
                if (x >= skip_x)
                {
                    values[(y - skip_y) * sizeX + (x - skip_x)] = atof(start);
                }
                start = strchr(start, ',');
                if (start == NULL)
                    break;
                start += 1;

                x += 1;
                if (x >= sizeX + skip_x)
                    break;
            }
        }

        y += 1;
        if (y >= sizeY + skip_y)
            break;
    }

    fclose(file);
    return true;
}

/////////////////////////
// FONCTIONS POUR RANDOM
/////////////////////////

double randomDomaine = RAND_MAX + 1.0;

double randomNumber(double max)
{
    return random() / randomDomaine * max;
    // on crée une fonction qui return un nb aléat.
    // entre 0 et max non compris (on est en double)
}

//////////////////////////////
// FONCTIONS DE CONVERSIONS //
//////////////////////////////

// Fonction qui fait la conversion des coordonnées (lat,longi) en indice de tableau 1D
// dimension du tableau : (360*720) <=> (latitude x longitude) (tableau représentant les case de 0.5° x 0.5°)
int ll2i(int longi, int lat){ // cette fonction est nécessaire pour GPGPinit
    int i = lat * 720 + longi;
    return i;}

// Conversion des latitudes et longitudes exactes (pas forcément .25 ou .75, longi et lat selon le référentiel
// géographique) en index i du tableau, c'est à dire dans quelle case de 0.5°x0.5° nous sommes :
int exactocasei(double longi, double lat){
    //conversion de la latitude exacte en l'indice  de la ligne correspondant (donne un entier pour tous les .25 et .75):
    lat = (lat + 89.75) * 2;
    // idem, mais de longi en colonne :
    longi = (longi + 179.75) * 2;
    // arrondir à l'entier le plus proche (pas de fonction qui le fait directement,
    // mais si on fait + 0.5 revient au même que d'arrondir à l'entier le plus proche)
    // une valeur ayany une partie décimale =0.5 est arrondie au DESSUS (choix délibéré)
    lat = (int)floor(lat + 0.5);
    longi = (int)floor(longi + 0.5);
    // convertion en indice i du tableau:
    int i = lat * 720 + longi;
    return i;}


// Conversion quand on rentre une longitude en la case correspondante dans un tableau 1D d'uniquement des longitudes
int long2i1D(double longi){ // cette fonction est nécessaire dans fctLongInit
    longi = (longi + 179.75) * 2;
    longi = (int)floor(longi + 0.5);
    return longi;} // on a déjà i car ici l'index de latitude correspond directement à i (de 0 à 719)

// Conversion quand on rentre une latitude en la case correspondante dans un tableau 1D d'uniquement des latitudes
int lat2i1D(double lat){ // cette fonction est nécessaire dans fctLatInit
    lat = (lat + 89.75) * 2;
    lat = (int)floor(lat + 0.5);
    return lat;}

// Conversion des kg de plastique en nombre de plastiques :
// On effectue une moyenne pondérée, en ne considèrant  que les macro- et  megaplastiques.
unsigned long kg2nb(double kg, int plast_par_paquet){
    double densiteMega = 0.0762;     // [nb/kg]
    double densiteMacro = 41.05;     // [nb/kg]
    double masseMega = 0.5357 * kg;  // [kg]
    double masseMacro = 0.2551 * kg; // [kg]
    // les valeurs 53.57% et 25.51% correspondent à la masse de la catégorie de plastique coonsidérée dans le continent
    // par rapport à la masse totale de plastiques
    //(ici on prend en compte les micro- et les mésoplastiques car ils sont tout de même émis)
    // cependant de ne prendre que les macro- et les megaplastiques nous donne une idée de combien chaque pays produit
    // proporitonnellement aux autres.
    //Prendre les micro- et mésoplastiques en compte nous prendrait beaucoup trop de place sur
    // l'ordinateur (pas assez de RAM), car le ratio nombre/poids est énorme.

    unsigned long nbMega = (unsigned long)(masseMega * densiteMega / plast_par_paquet + 0.5);    // on arrondit à l'entier le plus proche
    unsigned long nbMacro = (unsigned long)(masseMacro * densiteMacro / plast_par_paquet + 0.5); // idem

    return nbMega + nbMacro;}


////////////////////////////////////////////
// INITIALISATION DU CONTINENT DE PLASTIQUES
////////////////////////////////////////////

void fctLatInit(double *latInit){
    int I1 = lat2i1D(25); // 25°N est la coordonnée à partir de laquelle notre gradient de déchets va commencer
    int I2 = lat2i1D(28); // à 28°N on est à saturation
    int I3 = lat2i1D(36); // à partir de là on va commencer à diminuer en concentration
    int I4 = lat2i1D(40); // on finit notre gradient de pollution à 40°
    double concentration = 0;
    double penteMontee = (double)1 / (I2 - I1);
    double penteDescente = (double)-1 / (I4 - I3);
    for (int i = 0; i < 360; i++)  {
        if (i < I1 || i > I4) latInit[i] = 0;
        // si on est au-delà des limites extérieures, la concentration initiale en plastiques est nulle
        // dans ce sens car est 0 en-bas et augmente en montant
        else if (i >= I1 && i <= I2)
        {
            // si on est entre I1 et I2, on va de bas en haut, le gradient de plastiques augmente
            concentration = penteMontee * (i - I1);
            // on n'arrondit pas encore car on arrondit après (dans la fonction où l'on
            // joint nos deux fonctions d'initialisation 1D) afin de gagner un peu en précision
            latInit[i] = concentration;
        }
        else if (i > I2 && i < I3)
            latInit[i] = 1; // on est dans la partie dense (saturée)
        else if (i >= I3 && i <= I4)
        {
            // si on se situe en-dessus de la partie saturée, le gradient de pollution diminue
            concentration = 1 + penteDescente * (i - I3);
            latInit[i] = concentration;
        }
    }
}

void fctLongInit(double *longInit)
{
    // cette fonction est très similaire à fctLatInit définit au dessus, s'y référer pour des explications
    int I1 = long2i1D(-160); // est la limite à gauche de notre continent de plastique
    int I2 = long2i1D(-144); // frontière gauche de partie dense du continent de plastiques
    int I3 = long2i1D(-134);
    int I4 = long2i1D(-130);
    double concentration = 0;
    double penteMontee = (double)1 / (I2 - I1);
    double penteDescente = (double)-1 / (I4 - I3);
    for (int i = 0; i < 720; i++)
    {
        if (i < I1 || i > I4)
            longInit[i] = 0;
        // dans ce sens car est 0 à gauche et augmente en allant à droite
        else if (i >= I1 && i <= I2)
        {
            concentration = penteMontee * (i - I1);
            longInit[i] = concentration;
        }
        else if (i > I2 && i < I3)
            longInit[i] = 1;
        else if (i >= I3 && i <= I4)
        {
            concentration = 1 + penteDescente * (i - I3);
            longInit[i] = concentration;
        }
    }
}

void GPGPinit(double *longInit, double *latInit, int saturation)
{
    // on remplit l'attribut  "compteur" pour chaque case de 0.5x0.5 du tableau de structure "Cases" définit dans le main,
    // suivant le gradient de saturation en plastique du contient définit plus haut.
    for (int longi = 0; longi < 720; longi++)
    {
        for (int lat = 0; lat < 360; lat++)
        {
            int i = ll2i(longi, lat);
            Cases[i].compteur = (int)floor(saturation * latInit[lat] * longInit[longi]);
        }
    }
}

//////////////////////////////////////
// DEFINITION DE LA ZONE DE SATURATION
//////////////////////////////////////

int saturationzone(double longi, double lat)
{
    // Pour remédier au problème que, proche des côtes, les émissions de plastiques sont très denses
    // ( comme précisé dans le rapport nous avons condensé en un à trois point(s) les émissions d'un pays et les plastiques n'avancent
    // pas très vite), nous définissons une zone autour du GPGP où la saturation est effective:
    // à 30° à gauche et en-dessous de ce dernier car il n'y a pas de terres dans ces zones,
    // et à 3° en-dessus et à droite car la côte des Etats-Unis est très proche de ce côté.

    int SZ = 0;
    if ((longi >= 170 || (longi >= -180 && longi <= -127)) && (lat >= -5 && lat <= 43)) SZ = 1;
    return SZ;
}

//////////////////////////////
// PROPAGATION DES PLASTIQUES
//////////////////////////////

//Fonction permettant d'effectuer le déplacement d'un paquet de plastique.
void plastique(struct paquet *paquet, int saturation)
{

    int indexCase = exactocasei(paquet->longi, paquet->lat);
    int prevIndexCase = exactocasei(paquet->longi, paquet->lat);

    // Est-ce qu'on doit laisser le paquet immobile? (i == 2)
    if (paquet->i == 2)
        return;

    // Est-ce que le plastique était déjà dans la case ou non? (Si non: i == 0 avant puis i est actualisé)
    if (paquet->i == 0)
    {
        Cases[indexCase].compteur += 1;
        paquet->i = 1;
    }

    // Courants marins: nous allons lire dans les fichiers csv les composantes
    // correspondant à la case dans laquelle se trouve notre plastique au temps t.
    // Composante du courant [°/h]
    double dlong = Cases[indexCase].compoE * 24;
    double dlat = Cases[indexCase].compoN * 24;

    // Composante aléatoire du déplacement (vents, poissons ect...)
    double alat = randomNumber(2e-4) - 1e-4;
    double along = randomNumber(2e-4) - 1e-4;
    // pour trouver cet aléa, nous avons regardé les valeurs moyennes des courants, et en regardant quel était les ordres de
    // grandeur les plus fréquents, nous avons estimé cet aléa en nous disant qu'il était raisonnable: aux endroits où le courant
    // est très faible, il y a beaucoup d'aléa, et aux endroits où il est plus fort, l'aléa a un rôle moins important.

    // Déplacement potentiel du plastique (si cela n'implique pas une condition break)
    double longitemp = paquet->longi + dlong + along;
    double lattemp = paquet->lat + dlat + alat;

    // Cas d'un plastique se trouvant à une longitude de -180° ou 180° de longitude:
    // Les cases vont de -179.75 à 179.75, nous avons fait le choix dans la fonction exactocasei que
    // lorsqu'on se trouve à la frontière d'une case notre plastique va:
    // - si c'est une frontière verticale: on passe à la case à droite de cette frontière
    // - si c'est une frontière horizontale: on passe à la case au dessus de cette frontière
    // Cela est dû au fait que la fonction qui arrondit un nombre à l'entier le plus proche arrondit
    // à l'entier supérieur une valeur ayant une partie décimale égale à 0.5 .
    if (longitemp >= 180)
    {
        longitemp = -180 + (longitemp - 180);
    }
    // Quand on est à 180 pile, on a également un problème d'arrondi (arrondit à la case 720, qui n'existe
    // pas dans notre tableau).
    if (longitemp < -180)
    {
        longitemp = 180 + (longitemp + 180);
    }
    // Alors que quand on est à -180 pile, exactocasei le convertit en 0, ce qui n'est pas un problème par
    // rapport à notre tableau et est cohérent avec nos "hypothèses de frontières" énoncées ci-dessus.

    //On regarde si on se trouve dans la zone de saturation effective:
    int sz = saturationzone(longitemp, lattemp);
    indexCase = exactocasei(longitemp, lattemp);

    // On vérifie si le plastique va rester dans la même case ou non:
    if (prevIndexCase != indexCase)
    {
        //Cas où le plastique s'échoue sur un contient : les composantes du courrant sont des Nan
        if (isnan(Cases[indexCase].compoN) || isnan(Cases[indexCase].compoE))
        {
            // on enlève le paquet de la case précédente dans laquelle il était
            Cases[prevIndexCase].compteur -= 1;
            // on actualise ses coordonnés:
            paquet->longi = longitemp;
            paquet->lat = lattemp;
            // on actualise le compteur de plastique de la nouvelle case:
            Cases[indexCase].compteur += 1;
            // actualisation de son index i pour ne plus avoir à le déplacer après (il est bloqué/arrêté/échoué)
            paquet->i = 2;
            return;
        }

        // cas où le plastiqu est dans la zone de saturation effective, il faut prendre cette dernière en compte:
        if (sz == 1)
        {
            if (Cases[indexCase].compteur < saturation)
            {
                // si la case n'est pas saturée en plastique, on peut l'y déplacer
                //et enlever 1 au compteur de sa préccédente case
                Cases[prevIndexCase].compteur -= 1;
                // on actualise son i à 0 car il vient d'arriver sur une nouvelle case
                paquet->i = 0;
            }
            else
            {
                // Si la case est saturée le plastique ne peut s'y déplacer et reste bloqué sur sa case actuelle.
                paquet->i = 2;
                return;
            }
        }
        else
        {
            //Si nous ne sommes pas dans cette zone de saturation le plastique peut se déplacer dans sa nouvelle
            //case dans tous les cas
            Cases[prevIndexCase].compteur -= 1;
            paquet->i = 0;
        }
    }

    // Résumé des conditions d'arrêt:
    //  1) la case d'à côté est saturée
    //  2) on arrive sur une terre (courrant = nan)

    //Si rien ne bloque le déplacement du plastique on le déplace en actualisant ses coordonnées géographiques
    paquet->longi = longitemp;
    paquet->lat = lattemp;
}

/////////////////
////  MAIN  ////
/////////////////

int main(int argc, char *argv[])
{

    // on initialise le temps, sera utile pour aléatoire de propagation d'un plastique
    srandom(time(NULL));

    // On réserve la mémoire pour notre tableau de cases de 0.5° x 0.5°
    Cases = calloc(720 * 360, sizeof(struct courants));

    // Comme énnoncé précédement, le planisphère mondial est découpé en cases de 0.5° x 0.5°  (lat, longi) :
    // LONGITUDES = COLONNES (720) = x = compoE
    // LATITUDES = LIGNES (360) = Y = compoN
    // avec la conversion lat = (lat + 89.75)*2 pour convertir les latitudes en index du tableau
    // (pour plus de précisions, aller voir dans fonctions de conversions, lat2i1d ou exactocasei)
    // et pour les longitudes: longi = (longi + 179.75)*2 (voir long2i1D)

    // nombre de plastiques par paquet propagé
    int plast_par_paquet = 10000;
    // notre saturation de cases est (nombre de plastiques maximum admis par case, à cette valeur, la
    // case est "pleine"), valeur calculée à partir de l'étude sur laquelle nous nous basons, nous avons
    // pris saturation = 55 kg/km² et fait la conversion pour le nombre de plastiques dans une case de
    // 0.5°x0.5° et arrondi vers le haut
    int saturation = 200;

    // on remplit le tableau malloc Cases (compoN et compoE)
    bool read1 = readCsvCourantsNVEL("NVELmoyenne.csv", 720, 360);
    bool read2 = readCsvCourantsEVEL("EVELmoyenne.csv", 720, 360);
    // nos fichiers sont en °/h (vitesse du courant).

    // et on initialise notre continent de plastiques en mettant à jour les "compteurs" du tableau de structures:
    double *longInit = malloc(720 * sizeof(double));
    double *latInit = malloc(360 * sizeof(double));
    fctLongInit(longInit);
    fctLatInit(latInit);
    GPGPinit(longInit, latInit, saturation);
    free(latInit);
    free(longInit);

    //////////////////////////////////
    //   OUVERTURE DES DONNÉES :    //
    //////////////////////////////////

    // Pour le tableau contenant les point GPS d'input des déchets :

    //48 points GPS avec 7 infos dessus (colonnes)
    double *gps_inputs = malloc(48 * 7 * sizeof(double));
    bool read3 = readCsvGen("gps_inputs.csv", gps_inputs, 7, 48, 2, 1);
    // INDEXATION DU TABLEAU:
    // - gps_inputs[0+7*c]=>longitude du point GPS
    // - gps_inputs[1+7*c]=>latitude du point gps
    // - gps_inputs[2+7*c]=>nombre de point GPS considéré dans le pays
    // - gps_inputs[3+7*c]=>pop du pays en 2021
    // - gps_inputs[4+7*c]=> % de la popluation totale du pays vivant sur la côte pacifique (population à considérer),
    // - gps_inputs[5+7*c]=> waste generation rate (non utilisé dans ce code)
    // - gps_inputs[6+7*c]=> part des déchets finissant dans l'océan [kg/person/day]
    // avec c (city) l'indice de la ville de 0 à 48 (exclus)
    // Les pays sont triés par ordre alphabétique.

    // TABLEAU CONTENANT LES DONNÉES RELATIVES AUX PAYS SEULS:
    double *pays_inputs = malloc(26 * 5 * sizeof(double));
    bool read4 = readCsvGen("pays_inputs.csv", pays_inputs, 5, 26, 1, 1);
    //INDEXATION DU TABLEAU:
    // - pays_inputs[0+5*c]=> nombre de villes dans pays
    // - pays_inputs[1+5*c]=> population pays 2021
    // - pays_inputs[2+5*c]=> % de la popluation totale du pays vivant sur la côte pacifique (population à considérer)
    // - pays_inputs[3+5*c]=> waste generation rate,
    // - pays_inputs[4+5*c]=> part des déchets qui finissent dans l'eau [kg/person/day]

    // Gestion d'erreur: on vérifie si les fichiers ont tous bien été lus:
    if (read1 == false || read2 == false || read3 == false || read4 == false)
    {
        printf("\n");
        printf("La simulation ne peut pas commencer car les fichiers n'ont pas tous été trouvés.\n");
        printf("Veuillez vérifier les chemins d'accès aux fichiers NVELmoyenne.csv, EVELmoyenne.csv, gps_inputs.csv et pays_inputs.csv.\n");
        return 0;
    }

    // On fixe les taux maximaux d'augmentation de production et de population,
    //ainsi que le nombre d'années maximal pour la simmulation (des entrées plus grandes ne seraient pas
    // gérables et entraîneraient des segmentation fault)
    int anneesMax = 9;
    double prodMax = 50;
    double popMax = 10;

    //////////////////////////////////
    //     INTERFACE UTILISATEUR.ICE:   //
    //////////////////////////////////

    printf(" \n \n");
    printf(" Projet de Programmation: Simulation du continent de plastique , the GPGP (the Great Pacific Garbage Patch . \n\n\n");
    printf(" Les taux de croissances que vous allez entrer fonctionnent de la manière suivante: \n\n");
    printf(" Chaque année durant la période qui sera entrée, la population et le taux de production de déchêt augmenteront ou diminueront selon les valeurs entrées. \n\n");
    printf(" Les formats attendus sont les suivants: \n\n");
    printf("-taux de croissance annuel en pourcentage de la population mondiale : double  \n");
    printf(" Valeur maximale: %0.0f.\n\n", popMax);
    printf("-taux de croissance annuel en pourcentage de la production de dechet mondiale : double\n");
    printf(" Valeur maximale: %0.0f.\n\n", prodMax);
    printf("-durée de simulation : int (années entières)\n");
    printf(" Valeur maximale: %d.\n\n\n", anneesMax);

    double taux_croiss_pop = 10;
    double taux_croiss_dechets = 50;
    int duree = 8;

    printf("Entrez un taux de croissance annuel en pourcentage de la population mondiale  : \n\n");
    //scanf("%lf", &taux_croiss_pop);
    //gestion d'erreur:
    if (taux_croiss_pop > popMax)
    {
        printf("Votre valeur dépasse la valeur demandée. Veuillez en rentrer une nouvelle.\n");
        //scanf("%lf", &taux_croiss_pop);
    }
    printf("\n");

    printf("Entrez un taux de croissance annuel en pourcentage de la production de déchet mondiale (format attendu: double): \n\n");
    //scanf("%lf", &taux_croiss_dechets);
    //gestion d'erreur:
    if (taux_croiss_dechets > prodMax)
    {
        printf("Votre valeur dépasse la valeur demandée. Veuillez en rentrer une nouvelle.\n");
        //scanf("%lf", &taux_croiss_dechets);
    }
    printf("\n");
    printf("Entrez une durée de simulation (années entières)(format attendu: int): \n\n");
    //scanf("%d", &duree);
    //gestion d'erreur:
    if (duree > anneesMax)
    {
        printf("Votre valeur dépasse la valeur demandée. Veuillez en rentrer une nouvelle.\n");
        //scanf("%d", &duree);
    }
    printf("\n\n");
    printf("Démarrage de la Simulation.\n\n");
    printf("Simulation en cours...\n");
    printf("Cela peut prendre plusieurs dizaines de minutes selon les paramètres entrés...\n\n");

    // Calcul de la taille des tableaux malloc de chaque pays selon les inputs de l'utilisateur.ice:
    unsigned long *longueur_tableaux = calloc(26, sizeof(unsigned long));

    // On définit le tableau du nombre de paquet de plastique en temps réel émis par chaque pays:
    unsigned long *longueur_reelle_tableaux = calloc(26, sizeof(unsigned long));

    //TABLEAU FINAL RENVOYÉ À L'UTILISATEUR.ICE CONTENANT LES COMPTEURS DE PLASTIQUES DE CHAQUE ANNÉE:
    unsigned long *CSV_output = calloc(360 * 720 * (duree + 1), sizeof(unsigned long));
    //ORGANISATION:
    //1 ligne = 1 année
    //chaque colone = valeur du compteur de plastique pour une case
    //indexation car tableau 1D: année * 360*720 + colone (=i)

    // On définit les mallocs pour chaque pays
    //Comme chaque pays a un nombre différent de point GPGS d'intérêt (de 1 à 3), des population et des taux
    //de production de déchet différent il était plus judicieux de créer un tableau pour chaque pays.
    // Cependant, nous aurions aussi pu faire un seul tableau malloc, avec une indexation bien définie, qui prend en compte le nombre
    // changeant de villes par pays. Le code aurait été plus court, l'indexation plus compliquée.
    struct paquet *Australie = NULL;
    struct paquet *Canada = NULL;
    struct paquet *Chili = NULL;
    struct paquet *Chine = NULL;
    struct paquet *Colombie = NULL;
    struct paquet *CostaRica = NULL;
    struct paquet *Ecuador = NULL;
    struct paquet *Salvador = NULL;
    struct paquet *Guatemala = NULL;
    struct paquet *Honduras = NULL;
    struct paquet *HongKong = NULL;
    struct paquet *Indonesie = NULL;
    struct paquet *Japon = NULL;
    struct paquet *CoreeDuNord = NULL;
    struct paquet *CoreeDuSud = NULL;
    struct paquet *Malaisie = NULL;
    struct paquet *Mexique = NULL;
    struct paquet *NouvelleCaledonie = NULL;
    struct paquet *Nicaragua = NULL;
    struct paquet *Panama = NULL;
    struct paquet *Perou = NULL;
    struct paquet *Philippines = NULL;
    struct paquet *Russie = NULL;
    struct paquet *Singapour = NULL;
    struct paquet *USA = NULL;
    struct paquet *Vietnam = NULL;

    //////////////////////////////////////////////////
    // INPUTS ET DÉPLACEMENT DES DÉCHETS DANS L'OCÉAN:
    //////////////////////////////////////////////////

    // durée totale de la simulation: duree[années]*365 car nous actualisons notre système tous les jours:
    // Nous considérons 48 points GPS (villes côtières du pacifique ou autres points pertinants) qui relâchent des déchets dans l'océan:

    //On écrit dans notre tableau final les valeurs intitiales avant de lancer la simmulation: (GPGP initial)
    for (int i = 0; i < 360 * 720; i++)
    {
        CSV_output[i] += Cases[i].compteur;
    }

    //Actualisation journalière du système:
    for (int a = 0; a < duree; a++)
    {   
    ////////////////////////////////////////
    ////////////////////////////////////////
    ////////////////////////////////////////
    //////////// Où on se situe/////////////
    printf("année: %d\n", a);
    fflush(stdout);
    ////////////////////////////////////////
    ////////////////////////////////////////
    ////////////////////////////////////////
        for (int j = 0; j < 365; j++)
        {
            printf("jour: %d\n", j);
            fflush(stdout);

            int nb_villes_parcourues = 0;
            int pays_parcourus = 0;

            /////////Australie: /////////

            //nombre de ville à considérer dans ce pays:
            int nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // taux de production de déchet en [kg plastique dans l'ocean /person/day]:
            double actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            double new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);
            // augmentation de la population:
            double actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            double new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            //parcourt de chaque ville/point GPS d'intérêt:
            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];
                //émission des déchets:
                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                // on remplit le malloc pour l'Australie à la première itération seulement
                if (a == 0 && j == 0 && c == 0)
                {   
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Australie = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    Australie[p].lat = lat;
                    Australie[p].longi = longi;
                    Australie[p].i = 0;
                }
                //Prise en compte des nouveaux paquets émits:
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens émis durant les ittérations précédentes dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Australie[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// Canada //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Canada = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    Canada[p].lat = lat;
                    Canada[p].longi = longi;
                    Canada[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Canada[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// CHILI //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Chili = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    Chili[p].lat = lat;
                    Chili[p].longi = longi;
                    Chili[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Chili[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// CHINE //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Chine = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    Chine[p].lat = lat;
                    Chine[p].longi = longi;
                    Chine[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Chine[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// COLOMBIE //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Colombie = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    Colombie[p].lat = lat;
                    Colombie[p].longi = longi;
                    Colombie[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Colombie[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// COSTARICA //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    CostaRica = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    CostaRica[p].lat = lat;
                    CostaRica[p].longi = longi;
                    CostaRica[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&CostaRica[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// ECUADOR //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Ecuador = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    Ecuador[p].lat = lat;
                    Ecuador[p].longi = longi;
                    Ecuador[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Ecuador[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// SALVADOR //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Salvador = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    Salvador[p].lat = lat;
                    Salvador[p].longi = longi;
                    Salvador[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Salvador[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// GUATEMALA //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Guatemala = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    Guatemala[p].lat = lat;
                    Guatemala[p].longi = longi;
                    Guatemala[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Guatemala[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// HONDURAS //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Honduras = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    Honduras[p].lat = lat;
                    Honduras[p].longi = longi;
                    Honduras[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Honduras[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// HONGKONG //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    HongKong = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    HongKong[p].lat = lat;
                    HongKong[p].longi = longi;
                    HongKong[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&HongKong[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// INDONESIE //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Indonesie = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    Indonesie[p].lat = lat;
                    Indonesie[p].longi = longi;
                    Indonesie[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Indonesie[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// JAPON //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    double new_rate_max = actual_rate * pow(1 + taux_croiss_dechets / 100, duree);
                    double new_pop_max = actual_pop * pow(1 + taux_croiss_pop / 100, duree);
                    double kg_plastique_emis_max = new_pop_max * new_rate_max / nb_villes * 1;
                    unsigned long nb_paquets_emis_max = kg2nb(kg_plastique_emis_max, plast_par_paquet);
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis_max * nb_villes * 365 * duree;
                    Japon = malloc(longueur_tableaux[pays_parcourus] * sizeof(struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    Japon[p].lat = lat;
                    Japon[p].longi = longi;
                    Japon[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Japon[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// COREE DU NORD //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    CoreeDuNord = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    CoreeDuNord[p].lat = lat;
                    CoreeDuNord[p].longi = longi;
                    CoreeDuNord[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&CoreeDuNord[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// COREE DU SUD //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    double new_rate_max = actual_rate * pow(1 + taux_croiss_dechets / 100, duree);
                    double new_pop_max = actual_pop * pow(1 + taux_croiss_pop / 100, duree);
                    double kg_plastique_emis_max = new_pop_max * new_rate_max / nb_villes * 1;
                    unsigned long nb_paquets_emis_max = kg2nb(kg_plastique_emis_max, plast_par_paquet);
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis_max * nb_villes * 365 * duree;
                    CoreeDuSud = malloc(longueur_tableaux[pays_parcourus] * sizeof(struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    CoreeDuSud[p].lat = lat;
                    CoreeDuSud[p].longi = longi;
                    CoreeDuSud[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&CoreeDuSud[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// MALAISIE //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Malaisie = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    Malaisie[p].lat = lat;
                    Malaisie[p].longi = longi;
                    Malaisie[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Malaisie[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// MEXIQUE //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Mexique = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    Mexique[p].lat = lat;
                    Mexique[p].longi = longi;
                    Mexique[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Mexique[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// NOUVELLE CALEDONIE //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    NouvelleCaledonie = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    NouvelleCaledonie[p].lat = lat;
                    NouvelleCaledonie[p].longi = longi;
                    NouvelleCaledonie[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&NouvelleCaledonie[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// NICARAGUA //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Nicaragua = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    Nicaragua[p].lat = lat;
                    Nicaragua[p].longi = longi;
                    Nicaragua[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Nicaragua[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// PANAMA //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Panama = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    Panama[p].lat = lat;
                    Panama[p].longi = longi;
                    Panama[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Panama[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// PEROU //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Perou = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    Perou[p].lat = lat;
                    Perou[p].longi = longi;
                    Perou[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Perou[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// PHILIPPINES //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Philippines = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    Philippines[p].lat = lat;
                    Philippines[p].longi = longi;
                    Philippines[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Philippines[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// RUSSIE //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Russie = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    Russie[p].lat = lat;
                    Russie[p].longi = longi;
                    Russie[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Russie[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// SINGAPOUR //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Singapour = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    Singapour[p].lat = lat;
                    Singapour[p].longi = longi;
                    Singapour[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Singapour[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// USA //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    USA = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    USA[p].lat = lat;
                    USA[p].longi = longi;
                    USA[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&USA[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;

            ////// VIETNAM //////
            nb_villes = pays_inputs[0 + 5 * pays_parcourus];

            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate = pays_inputs[4 + 5 * pays_parcourus];
            new_rate = actual_rate * pow(1 + taux_croiss_dechets / 100, a);

            actual_pop = (pays_inputs[1 + 5 * pays_parcourus] * pays_inputs[2 + 5 * pays_parcourus] / 100);
            new_pop = actual_pop * pow(1 + taux_croiss_pop / 100, a);

            for (int c = 0; c < nb_villes; c++)
            {
                //Coordonnée GPS considérées:
                double lat = gps_inputs[1 + 7 * nb_villes_parcourues];
                double longi = gps_inputs[0 + 7 * nb_villes_parcourues];

                double kg_plastique_emis = new_pop * new_rate / nb_villes * 1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis, plast_par_paquet);

                if (a == 0 && j == 0 && c == 0)
                {
                    for (int a = 0; a<duree; a++)
                    {
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Vietnam = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for (int p = 0 + longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis + longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    //indexation : indice ligne * indice colone * nb total de colone
                    Vietnam[p].lat = lat;
                    Vietnam[p].longi = longi;
                    Vietnam[p].i = 0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
                for (int p = 0; p < longueur_reelle_tableaux[pays_parcourus]; p++)
                {
                    plastique(&Vietnam[p], saturation);
                }
                nb_villes_parcourues += 1;
            }
            pays_parcourus += 1;
        }

        //On update notre tableau final on y mettant les valeurs des compteurs obtenus pour l'année en cours
        for (int i = 0; i < 360 * 720; i++)
        {
            CSV_output[(a + 1) * 360 * 720 + i] = Cases[i].compteur;
        }
        char filename[100];
        sprintf(filename, "annee%d.csv", a);
        writeCsv(filename, CSV_output, (a+1));

    }

    //Écriture des données de chaque années dans le CSV final:
    char *filename = "actualisationGPGP.csv";
    writeCsv(filename, CSV_output, (duree + 1));

    //Libération de la mémoire:
    free(Cases);
    free(gps_inputs);
    free(pays_inputs);
    free(longueur_tableaux);
    free(longueur_reelle_tableaux);
    free(Australie);
    free(Canada);
    free(Chili);
    free(Chine);
    free(Colombie);
    free(CostaRica);
    free(Ecuador);
    free(Salvador);
    free(Guatemala);
    free(Honduras);
    free(HongKong);
    free(Indonesie);
    free(Japon);
    free(CoreeDuNord);
    free(CoreeDuSud);
    free(Malaisie);
    free(Mexique);
    free(NouvelleCaledonie);
    free(Nicaragua);
    free(Panama);
    free(Perou);
    free(Philippines);
    free(Russie);
    free(Singapour);
    free(USA);
    free(Vietnam);

    printf("\n\n");
    printf("Simulation terminée. Veuillez récupérer le fichier %s généré.\n", filename);

    return 0;
}
