// Code C projet prog, version Maëlle

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// On définit les structures:

// celle pour les courants (propriétés de chaque case)
struct courants {
    double compoN;
    double compoE;
    unsigned long compteur;
};

// celle pour chaque paquet de plastiques (propriétés de chaque paquet de plastiques)
struct paquet {
    double lat;
    double longi;
    int i;
};

// On définit le tableau de structures (tableau malloc) qui
// contiendra toutes les cases
// Le fait qu'on définit ici le tableau nous permet de ne pas
// devoir la rentrer en argument à chaque fois qu'on veut l'utiliser
// dans une fonction.
struct courants * Cases = NULL;

///////////////////////////////////
// FONCTIONS D'ÉCRITURE DE FICHIERS
///////////////////////////////////

int writeCsv(char * filename, int*tableau, int duree) {

  int y = duree;
  
  FILE *fichier = fopen(filename, "w+"); 	 	
  if (fichier == NULL) {
    printf("File %s can't be created.", filename);
    return 0;
    }

    for (int i=0; i<y*360*720; i++){
	    int fillvalue = tableau[i];
	  
	    if ((i+1)%(360*720)==0){
	    fprintf(fichier," %d\n",fillvalue);
	    continue;
        }
	    fprintf(fichier," %d,",fillvalue);
    }
    fclose(fichier);     
    return 0;
}

///////////////////////////////////
// FONCTIONS DE LECTURE DE FICHIERS
///////////////////////////////////

// Fonction pour lire le fichier CSV des courants et les mettre dans notre structure
bool readCsvCourantsNVEL(char * filenameNVEL, int sizeLong, int sizeLat) {
    FILE * fileN = fopen(filenameNVEL, "r");

    if (fileN == NULL) {
        fprintf(stderr, "File %s not found.\n", filenameNVEL);
        return false;
    }

    int y = 0;
    char buffer[1000000];
    while (fgets(buffer, 1000000, fileN) != NULL) {
        int x = 0;
        char * start = buffer;
        while (true) {
            Cases[y * sizeLong + x].compoN = atof(start);
            start = strchr(start, ',');
            if (start == NULL) break;
            start += 1;

            x += 1;
            if (x >= sizeLong) break;
        }

        y += 1;
        if (y >= sizeLat) break;
    }
    fclose(fileN);
    
    return true;
}

bool readCsvCourantsEVEL(char * filenameEVEL, int sizeLong, int sizeLat) {
    FILE * fileE = fopen(filenameEVEL, "r");

    if (fileE == NULL) {
        fprintf(stderr, "File %s not found.\n", filenameEVEL);
        return false;
    }

    int y = 0;
    char buffer[1000000];
    while (fgets(buffer, 1000000, fileE) != NULL) {
        int x = 0;
        char * start = buffer;
        while (true) {
            Cases[y * sizeLong + x].compoE = atof(start);

            start = strchr(start, ',');
            if (start == NULL) break;
            start += 1;

            x += 1;
            if (x >= sizeLong) break;
        }

        y += 1;
        if (y >= sizeLat) break;
    }
    fclose(fileE);
    
    return true;
}

// Fonction pour lire les fichiers CSV générale
bool readCsvGen(char * filename, double * values, int sizeX, int sizeY, int skip_x, int skip_y) {
    FILE * file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "File %s not found.\n", filename);
        return false;
    }
	
    int y = 0;
    char buffer[1000000];
    while (fgets(buffer, 1000000, file) != NULL) {
		if (y>=skip_y){
			int x = 0;
			char * start = buffer;
			while (true) {
				if(x>=skip_x){
				values[(y-skip_y) * sizeX + (x-skip_x)] = atof(start);}
				start = strchr(start, ',');
				if (start == NULL) break;
				start += 1;

				x += 1;
				if (x >= sizeX+skip_x) break;
			}}

			y += 1;
			if (y >= sizeY+skip_y) break;
		}

    fclose(file);
    return true;
}

/////////////////////////
// FONCTIONS POUR RANDOM
/////////////////////////

double randomDomaine = RAND_MAX + 1.0;

double randomNumber(double max) {
    return random() / randomDomaine * max;
    // on crée une fonction qui return un nb aléat.
    // entre 0 et max non compris (on est en double)
}

///////////////////////////
// FONCTIONS DE CONVERSIONS
///////////////////////////

// Fonction qui fait la conversion des coordonnées de lat, longi en terme de place dans un tableau
// 2D en i 
int ll2i(int longi, int lat){ 
    int i = lat*720 + longi;
    return i; 
}

// Conversion des latitudes et longitudes exactes (pas forcément .25 ou .75, longi et lat selon le référentiel
// géographique) en index i du tableau
unsigned long exactocasei(double longi, double lat){
	//Permet de déterminer dans quelle case de courant nous sommes. (cases de 0.5° par 0.5°)
	
    lat = (lat + 89.75)*2; //conversion de la latitude en la ligne (donne un entier pour tous les .25 et .75)
    longi = (longi + 179.75)*2; // idem, mais de longi en colonne
    lat = (int) floor(lat + 0.5); // arrondir à l'entier le plus proche (pas de fonction qui le fait directement,
    // mais si on fait + 0.5 revient au même que d'arrondir à l'entier le plus proche)
    // quand on arrive sur une valeur .5, on arrondit à la valeur du DESSUS (choix délibéré)
    longi = (int) floor(longi + 0.5);
    printf("longi i: %f\n", longi);
    printf("lat i: %f\n", lat);
    unsigned long i = lat*720 + longi;
    return i; // on convertit en i
}


// Conversion quand on rentre une longitude en la case correspondante dans un tableau 1D d'uniquement des longitudes
int long2i1D(double longi){
    longi = (longi + 179.75)*2;
    longi = (int) floor(longi + 0.5);
    return longi;  // on a déjà i car ici l'index de latitude correspond directement à i (de 0 à 719)
}

// Conversion quand on rentre une latitude en la case correspondante dans un tableau 1D d'uniquement des latitudes
int lat2i1D(double lat){
    lat = (lat + 89.75)*2;
    lat = (int) floor(lat + 0.5);
    return lat;
}

// Conversion de kg en nombre de plastiqueS (moyenne pondérée), on ne considère ici que les macro- et les megaplastiques
unsigned long kg2nb(double kg, int plast_par_paquet){

    double densiteMega = 0.0762;    // [nb/kg]
    double densiteMacro = 41.05;    // [nb/kg]
    double masseMega = 0.5357*kg;    // [kg], le 53.57% correspond à la masse de megaplastiques dans le continent par rapport
    // à la masse totale de plastiques (ici on prend en compte les micro- et les mésoplastiques car ils sont tout de même émis)
    // cependant de ne prendre que les macro- et les megaplastiques nous donnent une idée de combien chaque pays produit
    // proporitonnellement aux autres. Prendre les micro- et mésoplastiques en compte nous prendrait beaucoup trop de place sur
    // l'ordinateur (pas assez de RAM), car le ratio nombre/poids est énorme.
    double masseMacro = 0.2551*kg;   // [kg]

    unsigned long nbMega = (unsigned long) (masseMega*densiteMega/plast_par_paquet +0.5); // on arrondit à l'entier le plus proche
    unsigned long nbMacro = (unsigned long) (masseMacro*densiteMacro/plast_par_paquet +0.5); // idem

    return nbMega + nbMacro;
}

////////////////////////////////////////////
// INITIALISATION DU CONTINENT DE PLASTIQUES
////////////////////////////////////////////

void fctLatInit(double * latInit){
    int I1 = lat2i1D(25); // 25°N est la coordonnée à partir de laquelle notre gradient de déchets va commencer
    int I2 = lat2i1D(28); // à 28°N on est à saturation
    int I3 = lat2i1D(36); // à partir de là on va commencer à diminuer en concentration
    int I4 = lat2i1D(40); // on finit notre gradient de pollution à 40°
    double concentration = 0;
    double penteMontee = (double) 1/(I2-I1);
    double penteDescente = (double) -1/(I4-I3);
    for (int i = 0; i < 360; i++){
        if (i < I1 || i > I4) latInit[i] = 0;
        // si on est au-delà des limites extérieures, la concentration initiale en plastiques est nulle
        // dans ce sens car est 0 en-bas et augmente en montant
        else if (i >= I1 && i <= I2){
            // si on est entre I1 et I2, on va de bas en haut, le gradient de plastiques augmente
            concentration = penteMontee*(i-I1);
            // on n'arrondit pas encore car on arrondit après (dans la fonction où l'on
            // joint nos deux fonctions d'initialisation 1D) afin de gagner un peu en précision
            latInit[i] = concentration;
        } else if (i > I2 && i < I3) latInit[i] = 1; // on est dans la partie dense (saturée)
        else if (i >= I3 && i <= I4){
            // si on se situe en-dessus de la partie saturée, le gradient de pollution diminue
            concentration = 1 + penteDescente*(i-I3);
            latInit[i] = concentration;
        }
    }
}

void fctLongInit(double * longInit){
    // cette fonction est très similaire à fctLongInit, s'y référer pour des explications
    int I1 = long2i1D(-160); // est la limite à gauche de notre continent de plastique
    int I2 = long2i1D(-144); // frontière gauche de partie dense du continent de plastiques
    int I3 = long2i1D(-134);
    int I4 = long2i1D(-130);
    double concentration = 0;
    double penteMontee = (double) 1/(I2-I1);
    double penteDescente = (double) -1/(I4-I3);
    for (int i = 0; i < 720; i++){
        if (i < I1 || i > I4) longInit[i] = 0;
        // dans ce sens car est 0 à gauche et augmente en allant à droite
        else if (i >= I1 && i <= I2){
            concentration = penteMontee*(i-I1);
            longInit[i] = concentration;
        } else if (i > I2 && i < I3) longInit[i] = 1;
        else if (i >= I3 && i <= I4){
            concentration = 1 + penteDescente*(i-I3);
            longInit[i] = concentration;
        }
    }
}

void GPGPinit(double * longInit, double * latInit, int saturation){
    // on remplit la partie "compteur" de notre tableau de structures pour l'initialiser
    for (int longi = 0; longi < 720; longi++){
        for (int lat = 0; lat < 360; lat++){
            int i = ll2i(longi, lat);
            Cases[i].compteur = (int) floor(saturation*latInit[lat]*longInit[longi]);
        }
    }
}

//////////////////////////////////////
// DEFINITION DE LA ZONE DE SATURATION
//////////////////////////////////////

int saturationzone(double longi, double lat){
    int SZ = 0;
    if ((longi >= 170 || (longi >= -180 && longi <= -127)) && (lat >= -5 && lat <= 43)){
        SZ = 1;
    }
    // Pour remédier au problème du fait que proche des côtes les émissions de plastiques sont très denses
    // (car nous avons compacté en un à trois point(s) les émissions d'un pays et que les plastiques n'avancent
    // pas très vite), nous définissons une zone autour du GPGP de saturation, à 30° à gauche et en-dessous de ce
    // dernier car il n'y a pas de terres dans ces zones, mais à 3° en-dessus et à droite car la côte des Etats-Unis
    // est très proche de ce côté.

    /////////////////////////////////ATTENTION SI ON CHANGE LES DEGRES LIMITATION, IL FAUT CHANGER LE COMMENTAIRE
    
    return SZ;
}

//////////////////////////////
// PROPAGATION DES PLASTIQUES
//////////////////////////////

void plastique(struct paquet * paquet, int saturation, int show){
   
    unsigned long indexCase = exactocasei(paquet->longi, paquet->lat);
    unsigned long prevIndexCase = exactocasei(paquet->longi, paquet->lat);
        
    if ( show == 1 ){
        printf("PrevIndexCase: %lu\n", prevIndexCase);
        printf("lat: %f\n", paquet->lat);
        printf("long: %f\n", paquet->longi);
        printf("i: %d\n", paquet->i);
    }
    // Explication des i:
    // - i = 0 : si le paquet vient d'arriver sur la case actuelle (n'y était pas à l'itération d'avant)
    // - i = 1 : si le paquet était déjà sur la case actuelle (à l'itération d'avant)
    // - i = 2 : le paquet est à côté d'une case saturée ou sur un continent/île

    // Est-ce qu'on doit laisser le paquet immobile? (i == 2)
    if (paquet->i == 2) return;


    // Courants marins: nous allons lire dans les fichiers csv les composantes
    // correspondant à la case dans laquelle se trouve notre plastique au temps t.

    // Est-ce que le plastique était déjà dans la case ou non? (Si non: i == 0 avant)
    if (paquet->i == 0){ 
        Cases[indexCase].compteur += 1;
        paquet->i = 1;
    }
        
    // Composante du courant [°/h]
    double dlong = Cases[indexCase].compoE*24;
    double dlat = Cases[indexCase].compoN*24;


    // Composante aléatoire du déplacement (vents, poissons ect...)
    double alat = randomNumber(2e-4) - 1e-4;
    double along = randomNumber(2e-4) - 1e-4;
    // pour trouver cet aléa, nous avons regardé les valeurs moyennes des courants, et en regardant quel était les ordres de
    // grandeur les plus fréquents, nous avons estimé cet aléa en nous disant qu'il était raisonnable: aux endroits où le courant
    // est très faible, il y a beaucoup d'aléa, et aux endroits où il est plus fort, l'aléa a un rôle moins important.

    // Déplacement potentiel du plastique (si cela n'implique pas une condition break)
    double longitemp = paquet->longi + dlong + along;
    double lattemp = paquet->lat + dlat + alat;
        
    // Cas d'un plastique se trouvant à une longitude de -180 ou 180:
    // Les cases vont de -179.75 à 179.75, nous avons fait le choix dans la fonction exactocasei que 
    // lorsqu'on se trouve à la frontière d'une case notre plastique va:
    // - si c'est une frontière verticale: on passe à la case à droite de cette frontière
    // - si c'est une frontière horizontale: on passe à la case au dessus de cette frontière
    // Cela est dû au fait que la fonction qui arrondit un nombre à l'entier le plus proche arrondit 
    // à l'entier supérieur une valeur ayant une partie décimale égale à 0.5 .
    if (longitemp >= 180){
		longitemp = -180 + (longitemp-180);}
        // Quand on est à 180 pile, on a également un problème d'arrondi (arrondit à la case 720, qui n'existe
        // pas dans notre tableau).
	if (longitemp < -180){ 
		longitemp = 180 + (longitemp+180);}
        // Alors que quand on est à -180 pile, exactocasei le convertit en 0, ce qui n'est pas un problème par 
        // rapport à notre tableau et est cohérent avec nos "hypothèses de frontières" énoncées ci-dessus.
        
    int sz = saturationzone(longitemp, lattemp);
    indexCase = exactocasei(longitemp, lattemp);

	if ( show == 1 ){
        printf("Déplacement potentiel: %lu\n", indexCase);
        printf("future lat: %f\n", lattemp);
        printf("future long: %f\n", longitemp);
        printf("i: %d\n", paquet->i);
    }

    // On vérifie si on va rester dans la même case ou non
    if (prevIndexCase != indexCase){
        if ( show == 1 ){
            printf("\n\n Veut se déplacer.\n");
        }
        if (isnan(Cases[indexCase].compoN) || isnan(Cases[indexCase].compoE)){
            Cases[prevIndexCase].compteur -= 1; // on enlève le paquet de la case précédente dans laquelle il était
              paquet->longi = longitemp;
		      paquet->lat = lattemp;
		       Cases[indexCase].compteur += 1; 
               // le plastique s'est échoué, on l'ajoute sur la case sur laquelle il est (un nan)
               paquet->i = 2;
               if ( show == 1 ){
                printf("Se déplace sur un nan\n");
                }
               return;
        }
        if (sz == 1){ // si nous sommes dans la zone de saturation, il faut prendre la saturation en compte:
            if ( show == 1 ){
                printf("Est dans la sz\n");
            }
            if (Cases[indexCase].compteur < saturation){
                Cases[prevIndexCase].compteur -= 1;
                paquet->i = 0; // si la case n'est pas pleine, on peut le déplacer
                // il vient donc d'arriver sur une nouvelle case 
                if ( show == 1 ){
                printf("Et peut se déplacer\n");
                printf("Compteur case précédente: %lu\n", Cases[prevIndexCase].compteur);
                printf("Compteur case sur laquelle il va se déplacer (vérif prochaine itération): %lu\n", Cases[indexCase].compteur);
                }
                }
           
            else {
            paquet->i = 2;
            return; }}
            
        else {
            Cases[prevIndexCase].compteur -= 1;
            paquet->i = 0;
            if ( show == 1 ){
                printf("N'est pas dans la sz\n");
                printf("Compteur case précédente: %lu\n", Cases[prevIndexCase].compteur);
                printf("Compteur case sur laquelle il va se déplacer (vérif prochaine itération): %lu\n", Cases[indexCase].compteur);
            }
        }
        // sinon, donc si on a Cases[indexCase].compteur >= saturation, on sort de la fonction,
        // et le compteur de la case prevIndexCase a toujours le plastique en mémoire
        // si la case dans laquelle on va n'est pas à saturation, on peut avancer donc on peut enlever 1 au compteur
        // de la case d'avant. Si le plastique s'échoue sur une terre/une île (un nan), on enlève le plastique du
        // compteur de la case dans l'eau et on l'ajoute à celui de la terre sur laquelle il arrive (on le fait après)
    }

    // Conditions d'arrêt:
    //  1) la case d'à côté est saturée
    //  2) on arrive sur un nan -> on l'enlève du compteur de la case dans l'eau et on ajoute à la case
    //      nan (ce qu'on a bien fait au-dessus dans la condition)
   
    paquet->longi = longitemp;
    paquet->lat = lattemp;
    
}


int main(int argc, char * argv[]) {
    // on initialise le temps, sera utile pour aléatoire de propagation d'un plastique
    srandom(time(NULL));
    
    // On réserve la place pour notre tableau de cases
    Cases = calloc(720*360, sizeof (struct courants));

    // Notre grille de courants est de la forme (lat, longi) :
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
    readCsvCourantsNVEL("NVELmoyenne.csv", 720, 360);
    readCsvCourantsEVEL("EVELmoyenne.csv", 720, 360);
    // nos fichiers sont en °/h (vitesse du courant).

    // et on initialise notre continent de plastiques en mettant à jour les "compteurs" du tableau de structures:
    double * longInit = malloc(720 * sizeof(double));
    double * latInit = malloc(360 * sizeof(double));
    fctLongInit(longInit);
    fctLatInit(latInit);
    GPGPinit(longInit,latInit, saturation);
    free(latInit);
    free(longInit);

    // Ouverture des fichiers de données :

	// Pour le tableau contenant aussi les villes (et leurs points GPS):
	double * gps_inputs = malloc(48*7 * sizeof(double)); //48 points GPS avec 7 infos dessus (colonnes) 
    readCsvGen("gps_inputs.csv", gps_inputs, 7, 48, 2, 1);
	// INDEXATION DU TABLEAU:
	// - gps_inputs[0+7*c]=>longi , gps_inputs[1+7*c]=>lat , gps_inputs[2+7*c]=>facteur pour diviser la population costale par nb de villes 
	// - gps_inputs[3+7*c]=>pop du pays en 2021, gps_inputs[4+7*c]=> % pop in pacific coast, 
	// - gps_inputs[5+7*c]=>waste generation rate, gps_inputs[6+7*c]=> part des déchets qui finissent dans l'eau [kg/person/day]
	
	// avec c (city) l'indice de la ville de 0 à 48 (exclus)
	// Les pays sont triés par ordre alphabétique et de 1 à 3 villes sont considérées dans chaque pays.

    // TABLEAU CONTENANT LES DONNÉES RELATIVES AUX PAYS SEULS:
    double * pays_inputs = malloc(26*5 * sizeof(double));
    readCsvGen("pays_inputs.csv", pays_inputs, 5, 26, 1, 1);
    //INDEXATION DU TABLEAU:
    // - pays_inputs[0+5*c]=> nombre de villes dans pays , pays_inputs[1+5*c]=> population pays 2021,
    //   pays_inputs[2+5*c]=> % pop in pacific coast, pays_inputs[3+5*c]=> waste generation rate,
    //   pays_inputs[4+5*c]=> part des déchets qui finissent dans l'eau [kg/person/day]


    // On fixe les taux maximaux d'augmentation de production et de population, ainsi que le nombre d'années
    int anneesMax = 3;
    double prodMax = 50;
    double popMax = 10;

    //INTERFACE UTILISATEUR:
	printf(" Les taux de croissances que vous allez entrer fonctionnent de la manière suivante: \n\n" );
	printf(" Chaque année durant la période qui sera entrée, la population et le taux de production de déchêt augmenteront ou diminueront selon les valeurs entrées. \n\n" );
	printf(" Les formats attendus sont les suivants: \n" );
	printf("-taux de croissance annuel en pourcentage de la population mondiale : double  \n" );
    printf(" Valeur maximale: %0.0f.\n", popMax );
	printf("-taux de croissance annuel en pourcentage de la production de dechet mondiale : double\n");
    printf(" Valeur maximale: %0.0f.\n", prodMax );
	printf("-durée de simulation : int (années entières)\n");
    printf(" Valeur maximale: %d.\n\n", anneesMax );

	double taux_croiss_pop;
	double taux_croiss_dechets;
	int duree;

	printf("Entrez un taux de croissance annuel en pourcentage de la population mondiale  : \n" );
	scanf("%lf", &taux_croiss_pop);
    while (taux_croiss_pop > popMax) {
        printf("Votre valeur dépasse la valeur demandée. Veuillez en rentrer une nouvelle.\n");
	    scanf("%lf", &taux_croiss_pop);
    }
    printf("\n");

	printf("Entrez un taux de croissance annuel en pourcentage de la production de déchet mondiale (format attendu: double): \n");
	scanf("%lf",&taux_croiss_dechets);
    while (taux_croiss_dechets > prodMax) {
        printf("Votre valeur dépasse la valeur demandée. Veuillez en rentrer une nouvelle.\n");
	    scanf("%lf", &taux_croiss_dechets);
    }

    printf("\n");
	printf("Entrez une durée de simulation (années entières)(format attendu: int): \n");
    scanf("%d",&duree);
    while (duree > anneesMax) {
        printf("Votre valeur dépasse la valeur demandée. Veuillez en rentrer une nouvelle.\n");
	    scanf("%d", &duree);
    }
    printf("\n");

    // Calcul de la taille des tableaux malloc de chaque pays selon les inputs de l'utilisateur.ice:
    ////////////////////////////
    //METTRE UN CALLOC






    unsigned long * longueur_tableaux = calloc(26, sizeof(unsigned long));









    // On définit le tableau du nombre de paquet de plastique en temps réel émis par chaque pays:
    unsigned long * longueur_reelle_tableaux = calloc(26 , sizeof(unsigned long));
    
    //TABLEAU FINAL CONTENANT LES COMPTEURS DE PLASTIQUES DE CHAQUE ANNÉE:
    int * CSV_output = calloc(360*720*(duree+1), sizeof(int));
    //INDEXATION DE CSV_OUTPUT:
    //1 ligne = 1 année
    //chaque colone = valeur du compteur de plastique pour une case 
    //année * 360*720 + colone (=i)


    // On définit les mallocs pour chaque pays
    struct paquet * Perou = NULL;


	// INPUTS ET DÉPLACEMENT DES DÉCHETS DANS L'OCÉAN:
	
	// durée simulation duree [années] * 365 car nous actualisons notre système tous les jours:
	// Nous considérons 48 points GPS (villes côtières du pacifique ou autres points pertinants) qui relâchent des déchets dans l'océan:
    // vu que nous avons un nombre de villes par pays différent à chaque fois, nous avons décidé de faire un tableau par pays.
    // Cependant, nous aurions aussi pu faire un seul tableau malloc, avec une indexation bien définie, qui prend en compte le nombre
    // changeant de villes par pays. Le code aurait été plus court, l'indexation plus compliquée.
    
    //On écrit dans notre tableau des valeurs du compteur pour chaque année les valeurs intitiales avant de lancer la simmulation: (GPGP initial)
    
    for (int i=0;i<360*720;i++){
		 CSV_output[i]+=Cases[i].compteur;}

    //////////////////////////////////////////
    /////////////////DEBOGAGE/////////////////
    //////////////////////////////////////////
    //int count = 1;
    //////////////////////////////////////////
    /////////////////DEBOGAGE/////////////////
    //////////////////////////////////////////

	for (int a=0; a<duree; a++){
		for (int j = 0; j < 365; j++){
            int nb_villes_parcourues = 0;
            int pays_parcourus = 0;
            
            
            int show = 0;
            //if ( (a == 0 && j >= 360) || (a == 1 && j < 100)) show = 1;
            //if ( (a == 0 && j < 100)) show = 1;
            if ( (a == 2 && j >= 300) ) show = 1;

            
            ////// PEROU //////


            pays_parcourus = 20;
            nb_villes_parcourues = 35;



            int nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            double actual_rate= pays_inputs[4+5*pays_parcourus];
            double new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            double actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            double new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);
            //////////////////////////////////////////
            /////////////////DEBOGAGE/////////////////
            //////////////////////////////////////////
            if (show == 1){
            printf("*******************************\n");
            printf("Jour: %d, année: %d\n", j, a);
            printf("*******************************\n");
            }
            //////////////////////////////////////////
            /////////////////DEBOGAGE/////////////////
            //////////////////////////////////////////
            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); 
 
                if (a == 0 && j == 0 && c == 0){
                    for (int a = 0; a<duree; a++){
                        double new_rate_l = actual_rate* pow(1+taux_croiss_dechets/100,a);
                        double new_pop_l = actual_pop*pow(1+taux_croiss_pop/100,a);
                        double kg_plastique_emis_l = new_pop_l*new_rate_l/nb_villes*1;
                        unsigned long nb_paquets_emis_l = kg2nb(kg_plastique_emis_l,plast_par_paquet);
                        //longueur_tab += nb_paquets_emis_l*nb_villes*365;
                        longueur_tableaux[pays_parcourus] += nb_paquets_emis_l*nb_villes*365;                   
                    }
                    //longueur_tableaux[pays_parcourus] = longueur_tab;
                    Perou = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                    printf("Longueur tableau: %lu\n\n", longueur_tableaux[pays_parcourus]);
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    //indexation : indice ligne * indice colone * nb total de colone
                    Perou[p].lat=lat;
                    Perou[p].longi=longi;
                    Perou[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){           
					if (p == 0){
                        if (show == 1){
                        printf("-------------------------\n");}
                    }
                    else break;
                    //printf("Paquet %d\n", p); 
                    plastique(&Perou[p],saturation, show);
                }
                    if (show == 1){
                        printf("longueur_reelle_tableaux: %lu\n", longueur_reelle_tableaux[pays_parcourus]);
                    }
                
                
            }
            if (a == duree-1 && j == 364){
                    printf("\n\nDERNIER JOUR : année %d, jour %d\n", a, j);
                    printf("pays parcourus:%d\n", pays_parcourus);
                    printf("longueur totale tableau: %lu\n", longueur_tableaux[pays_parcourus]);
                    printf("longueur réelle tableau: %lu\n", longueur_reelle_tableaux[pays_parcourus]);
                }
                nb_villes_parcourues+=1;
           
        }
        //On update notre tableau final on y mettant les valeurs des compteurs obtenus pour l'année en cours    
        for (int i=0;i<360*720;i++){
		CSV_output [(a+1)*360*720+i]=Cases[i].compteur;}
    }
    //On écrit le CSV final:
    char * filename = "actualisationGPGPtest1.csv";
    writeCsv(filename,CSV_output,(duree+1));

    free(Cases);
    free(gps_inputs);
    free(pays_inputs);
    free(longueur_tableaux);
    free(longueur_reelle_tableaux);
    free(Perou);
    return 0;
}