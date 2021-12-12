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
    unsigned long long compteur;
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
        return 0;}

  
 for (int i=0; i<y*360*720; i++){
	  
	 int fillvalue = tableau[i];
	  
	   if ((i+1)%(360*720)==0){
	  fprintf(fichier," %d\n",fillvalue);
	  continue;}
	  fprintf(fichier," %d,",fillvalue); }


	
  fclose(fichier);     
return 0;}

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
    ////////////////////// ON A BIEN VERIF QUE CETTE FONCTION EST OK??????????????????????
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

// Fonction qui fait la conversion des coordonnées de lat, longi en i 
int ll2i(int longi, int lat){ 
    int i = lat*720 + longi;
    return i; 
}

// Conversion des latitudes et longitudes exactes (pas forcément .25 ou .75) en index i du tableau
unsigned long exactocasei(double longi, double lat){
	//Permet de déterminer dans quelle case de courrant nous sommes. (cases de 0.5° par 0.5°)
	
    lat = (lat + 89.75)*2; //conversion de la latitude en la colonne (donne un entier pour tous les .25 et .75)
    longi = (longi + 179.75)*2; // idem, mais de longi en ligne
    lat = (int) floor(lat + 0.5); // arrondir à l'entier le plus proche (il n'y a que floor qui existe,
    // mais si on fait + 0.5 revient au même que d'arrondir à l'entier le plus proche)
    // quand on arrive sur une valeur.5, on arrondit à la valeur du DESSUS (choix délibéré)
    longi = (int) floor(longi + 0.5);
    unsigned long i = lat*720 + longi;
    return i; // on convertit en i
}

// Conversion quand on rentre une longitude en la case correspondante dans un tableau 1D d'uniquement des longitudes
int long2i1D(double longi){
    longi = (longi + 179.75)*2; // idem, mais de longi en ligne
    longi = (int) floor(longi + 0.5);
    return longi;
}

// Conversion quand on rentre une latitude en la case correspondante dans un tableau 1D d'uniquement des latitudes
int lat2i1D(double lat){
    lat = (lat + 89.75)*2; //conversion de la latitude en la colonne (donne un entier pour tous les .25 et .75)
    lat = (int) floor(lat + 0.5); // arrondir à l'entier le plus proche (il n'y a que floor qui existe,
    // mais si on fait + 0.5 revient au même que d'arrondir à l'entier le plus proche)
    return lat; // on a déjà i car ici l'index de latitude correspond directement à i (de 0 à 359)
}

// Conversion de kg en nombre de plastique (moyenne pondérée) (FAIRE HEAVY TAIL SI POSSIBLE!!!!!!!!!!!!!!!!!!!!!!)
unsigned long kg2nb(double kg, int plast_par_paquet){

    double densiteMega = 0.0762;    // [nb/kg]
    double densiteMacro = 41.05;    // [nb/kg]
    double masseMega = 0.5357*kg;    // [kg]
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
    int I2 = long2i1D(-144); // frontière gauche de partie dense du coninent de plastiques
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

//////////////////////////////
// PROPAGATION DES PLASTIQUES
//////////////////////////////

void plastique(struct paquet * paquet, int saturation){
    unsigned long indexCase = exactocasei(paquet->longi, paquet->lat);
    unsigned long prevIndexCase = exactocasei(paquet->longi, paquet->lat);

    // Explication des i:
    // - i = 0 : si la plastique vient d'arriver sur la case actuelle (n'y était pas à l'itération d'avant)
    // - i = 1 : si la plastique était déjà sur la case actuelle (à l'itération d'avant)
    // - i = 2 : le plastique à côté d'une case saturée ou sur un continent/île

    // Est-ce qu'on doit arrêter notre plastique? (i == 2)
    if (paquet->i == 2) return;
    // Courants marins: nous allons lire dans les fichiers csv les composantes
    // correspondant à la case dans laquelle se trouve notre plastique au temps t.

    // Est-ce que le plastique était déjà dans la case ou non? (Si non: i == 0 avant)
    if (paquet->i == 0){
        Cases[indexCase].compteur += 1;
        paquet->i = 1;
    }
        
    // Composante du courant [deg/h]
    double dlong = Cases[indexCase].compoE*24;
    double dlat = Cases[indexCase].compoN*24;


    // Composante aléatoire du déplacement (vents, poissons ect...)
    double alat = randomNumber(2e-5) - 1e-5;
    double along = randomNumber(2e-5) - 1e-5;
    // VOIR SI EST UN BON ALEA, POUR L'INSTANT ON MET CELA, EXPLIQUER POURQUOI

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
        
        
    indexCase = exactocasei(longitemp, lattemp);

    // On vérifie si on va rester dans la même case ou non
    if (prevIndexCase != indexCase){
        if (Cases[indexCase].compteur < saturation || isnan(Cases[indexCase].compoN) || isnan(Cases[indexCase].compoE)){
            // CONDITION EST JUSTE ??????????????????????????????????????? BCS 3 "OU" ET LES NAN OK?????????????????????
            Cases[prevIndexCase].compteur -= 1; 
            // EST JUSTE????????????? PARCE QU'AVANT C'ETAIT Cases[indexCase].compteur -= 1;
            if (Cases[indexCase].compteur < saturation) paquet->i = 0;
        }
        // COMMENTAIRES A ENLEVER POUR LE RENDU FINAL:
        // si on a des trucs négatifs dans les compteurs vient de là,
        // mais si fonctionne est pas censé arriver

        // A NE PAS ENLEVERR:
        // sinon, donc si on a Cases[indexCase].compteur >= saturation, on sort de la fonction,
        // et le compteur de la case prevIndexCase a toujours le plastique en mémoire
        // si la case dans laquelle on va n'est pas à saturation, on peut avancer donc on peut enlever 1 au compteur
        // de la case d'avant. Si le plastique s'échoue sur une terre/une île (un nan), on enlève le plastique du
        // compteur de la case dans l'eau et on l'ajoute à celui de la terre sur laquelle il arrive

    }

    // Conditions d'arrêt:
    //  1) une case est saturée
    //  2) on arrive sur un nan -> on l'enlève du compteur de la case dans l'eau et on ajoute à la case
    //      nan (ce qu'on a bien fait au-dessus dans la condition)
    //  3) autres???????????????????

    //  1) Case saturée
    if (Cases[indexCase].compteur >= saturation){
        paquet->i = 2;
        Cases[prevIndexCase].compteur += 1;
        return;
    } // le > est nécessaire pour si
    // plusieurs plastiques arrivent simultanément sur la case
    // le = est là car nous venons d'arriver sur cette case (sinon on n'aurait pas effectué
    // de déplacement car cette condition aurait été remplie à l'itération précédente)
    // donc le compteur de la case sur laquelle nous arrivons n'ayant pas encore été mis à jour,
    // (pour éviter d'avoir des problèmes sur les bords), il faut que le compteur ne soit pas
    // >= à la saturation (si est égal ou plus, on s'arrête là (donc dans prevIndexCase, car
    // la saturation dans indexCase est déjà atteinte))

    //  2) nan dans le futur index
    if ( isnan(Cases[indexCase].compoN) || isnan(Cases[indexCase].compoE) ){
        Cases[indexCase].compteur += 1; 
        // le plastique s'est échoué, on l'ajoute sur la case sur laquelle il est (un nan)
        paquet->i = 2;
        return;
    }
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
    int saturation = 2000000000/plast_par_paquet;
    /////////////////// CHANGER???????????????????

    // on remplit le tableau malloc Cases (compoN et compoE)
    readCsvCourantsNVEL("NVELmoyenneconverted_new.csv", 720, 360);
    readCsvCourantsEVEL("EVELmoyenneconverted_new.csv", 720, 360);

    //readCsvCourants("EVELmoyenneconverted_new.csv", "NVELmoyenneconverted_new.csv", 720, 360); 
    // nos fichiers sont en °/h (vitesse du courant).

    // et on initialise notre continent de plastiques en mettant à jour les "compteurs" du tableau de structures:
    
    double * longInit = malloc(720 * sizeof(double));
    double * latInit = malloc(360 * sizeof(double));
    fctLongInit(longInit);
    fctLatInit(latInit);
    GPGPinit(longInit,latInit, saturation);
    
    free(latInit);
    free(longInit);
    /////////////EST-CE QU'ON REUTILISE CES TABLEAUX APRES?????????????????????????????


    // Ouverture des fichiers de données :

	// Pour le tableau contenant aussi les villes (et leurs points GPS):
	double * gps_inputs = malloc(48*7 * sizeof(double)); //48 points GPS avec 7 infos dessus (colones) 
    readCsvGen("gps_inputs.csv", gps_inputs, 7, 48, 2, 1);
	// INDEXATION DU TABLEAU:
	// - gps_inputs[0+7*c]=>longi , gps_inputs[1+7*c]=>lat , gps_inputs[2+7*c]=>facteur pour ÷ population costale / nb de villes 
	// - gps_inputs[3+7*c]=>pop du pays en 2021, gps_inputs[4+7*c]=> % pop in pacific coast, 
	// - gps_inputs[5+7*c]=>waste generation rate, gps_inputs[6+7*c]=> part des déchets qui finissent dans l'eau [kg/person/day]
	
	// avec c (city) l'indice de la ville de 0 à 48 (exclus)
	// Les pays sont triés par ordre alphabétique et de 1 à 3 villes sont considérées dans chaque pays.

    // TABLEAU CONTENANT LES DONNÉES RELATIVES AUX PAYS SEULS:
    double * pays_inputs = malloc(26*5 * sizeof(double));
    readCsvGen("pays_inputs.csv", pays_inputs, 5, 26, 1, 1);
    //INDEXATION DU TABLEAU:
    // - pays_inputs[0+5*c]=>nombre de villes dans pays , pays_inputs[1+5*c]=>population pays 2021,
    //   pays_inputs[2+5*c]=> % pop in pacific coast, pays_inputs[3+5*c]=> waste generation rate,
    //   pays_inputs[4+5*c]=> part des déchets qui finissent dans l'eau [kg/person/day]


    // On fixe les taux maximaux d'augmentation de production et de population, ainsi que le nombre d'années
    int anneesMax = 15;
    double prodMax = 50;
    double popMax = 10; // VOIR SI ON DOIT CHANGER!!!!!!!!!

    //INTERFACE UTILISATEUR:
	printf(" Les taux de croissances que vous allez entrer fonctionnent de la manière suivante: \n\n" );
	printf(" Chaque année durant la période qui sera entrée, la population et le taux de production de dechet augmenteront ou diminueront selon les valeurs entrées. \n\n" );
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

    // TABLEAU CALCUL DE LA TAILLE DES TABLEAUX MALLOC DE CHAQUE PAYS SELON LES INPUTS DE L'UTILISATEUR:
    unsigned long * longueur_tableaux = malloc(26 * sizeof(unsigned long));
    

    // On définit le tableau du nombre de paquet de plastique en temps réel émis par chaque pays:
    unsigned long * longueur_reelle_tableaux = calloc(26 , sizeof(unsigned long));
    
    //TABLEAU FINAL CONTENANT LES COMPTEURS DE PLASTIQUES DE CHAQUE ANNÉE:
    int * CSV_output = calloc(360*720*(duree+1), sizeof(int));
    //INDEXATION DE CSV_OUTPUT:
    //1 ligne = 1 année
    //chaque colone = valeur du compteur de plastique pour une case 
    //année * 360*720 + colone (=i)

    // On définit les mallocs pour chaque pays
    struct paquet * Australie = NULL;
    struct paquet * Canada = NULL;
    struct paquet * Chili = NULL;
    struct paquet * Chine = NULL;
    struct paquet * Colombie = NULL;
    struct paquet * CostaRica = NULL;
    struct paquet * Ecuador = NULL;
    struct paquet * Salvador = NULL;
    struct paquet * Guatemala = NULL;
    struct paquet * Honduras = NULL;
    struct paquet * HongKong = NULL;
    struct paquet * Indonesie = NULL;
    struct paquet * Japon = NULL;
    struct paquet * CoreeDuNord = NULL;
    struct paquet * CoreeDuSud = NULL;
    struct paquet * Malaisie = NULL;
    struct paquet * Mexique = NULL;
    struct paquet * NouvelleCaledonie = NULL;
    struct paquet * Nicaragua = NULL;
    struct paquet * Panama = NULL;
    struct paquet * Perou = NULL;
    struct paquet * Philippines = NULL;
    struct paquet * Russie = NULL;
    struct paquet * Singapour = NULL;
    struct paquet * USA = NULL;
    struct paquet * Vietnam = NULL;

	//INPUTS ET DÉPLACEMENT DES DÉCHETS DANS L'OCÉAN:
	
	//durée simulation duree [années] * 365 car nous actualisons notre système tous les jours:
	//Nous considérons 48 points GPS (villes côtières du pacifique ou autres points pertinants) qui relâchent des déchets dans l'océan:
	//Nous sommes obligés de traiter chaque pays un par un car tout les pays n'ont pas le même nombre de villes
    //ni le même taux de production de déchet et la même population. Désolé de l'indigestion.
    
    
    //On écrit dans notre tableau des valeurs du compteur pour chaque année les valeurs intitiales avant de lancer la simmulation: (GPGP initial)
    
    for (int i=0;i<360*720;i++){
		 CSV_output[i]+=Cases[i].compteur;}
    
    
    
    
    
    //////////////////////////////////////////
    /////////////////DEBOGAGE/////////////////
    //////////////////////////////////////////
    int count = 1;
    //////////////////////////////////////////
    /////////////////DEBOGAGE/////////////////
    //////////////////////////////////////////

	for (int a=0; a<duree; a++){
		for (int j = 0; j < 365; j++){
            
            int nb_villes_parcourues = 0;
            int pays_parcourus = 0;


            /////////Australie: /////////
            int nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            double actual_rate= pays_inputs[4+5*pays_parcourus];
            double new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            double actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            double new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
                //////////////////////////////////////////
                /////////////////DEBOGAGE/////////////////
                //////////////////////////////////////////
                printf("itération %d\n", count);
                count +=1;
                //////////////////////////////////////////
                /////////////////DEBOGAGE/////////////////
                //////////////////////////////////////////
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis,plast_par_paquet); // int divisé par int donne int (division entière)
				
                // on remplit le malloc pour l'Australie
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Australie = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                } 
                
                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p = 0+longueur_reelle_tableaux[pays_parcourus]; p < nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus] ; p++){                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    Australie[p].lat=lat;
                    Australie[p].longi=longi;
                    Australie[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){         
					plastique(&Australie[p],saturation);
                }
                nb_villes_parcourues+=1;
                if (count==368){
                    printf("test");
                    printf("\n");
                }
            }
            pays_parcourus+=1;
			
			 
            ////// Canada //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis,plast_par_paquet); // int divisé par int donne int (division entière)
                
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Canada = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    Canada[p].lat=lat;
                    Canada[p].longi=longi;
                    Canada[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&Canada[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;
            
            ////// CHILI //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis,plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Chili = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    Chili[p].lat=lat;
                    Chili[p].longi=longi;
                    Chili[p].i=0;
                     
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;
		
                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&Chili[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;
            
            ////// CHINE //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);

            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis,plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Chine = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    Chine[p].lat=lat;
                    Chine[p].longi=longi;
                    Chine[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&Chine[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

            ////// COLOMBIE //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis,plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Colombie = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    Colombie[p].lat=lat;
                    Colombie[p].longi=longi;
                    Colombie[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&Colombie[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

            ////// COSTARICA //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis = kg2nb(kg_plastique_emis,plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    CostaRica = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    CostaRica[p].lat=lat;
                    CostaRica[p].longi=longi;
                    CostaRica[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&CostaRica[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

            ////// ECUADOR //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Ecuador = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    Ecuador[p].lat=lat;
                    Ecuador[p].longi=longi;
                    Ecuador[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&Ecuador[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

            ////// SALVADOR //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Salvador = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    Salvador[p].lat=lat;
                    Salvador[p].longi=longi;
                    Salvador[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&Salvador[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;
            
            ////// GUATEMALA //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Guatemala = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    Guatemala[p].lat=lat;
                    Guatemala[p].longi=longi;
                    Guatemala[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&Guatemala[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

            ////// HONDURAS //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Honduras = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    Honduras[p].lat=lat;
                    Honduras[p].longi=longi;
                    Honduras[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&Honduras[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

            ////// HONGKONG //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    HongKong = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    HongKong[p].lat=lat;
                    HongKong[p].longi=longi;
                    HongKong[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&HongKong[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;
            
            
            ////// INDONESIE //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Indonesie = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    Indonesie[p].lat=lat;
                    Indonesie[p].longi=longi;
                    Indonesie[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&Indonesie[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

            ////// JAPON //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Japon = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    Japon[p].lat=lat;
                    Japon[p].longi=longi;
                    Japon[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&Japon[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

            ////// COREE DU NORD //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    CoreeDuNord = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    CoreeDuNord[p].lat=lat;
                    CoreeDuNord[p].longi=longi;
                    CoreeDuNord[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&CoreeDuNord[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;


            ////// COREE DU SUD //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    CoreeDuSud = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    CoreeDuSud[p].lat=lat;
                    CoreeDuSud[p].longi=longi;
                    CoreeDuSud[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&CoreeDuSud[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

            ////// MALAISIE //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Malaisie = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    Malaisie[p].lat=lat;
                    Malaisie[p].longi=longi;
                    Malaisie[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&Malaisie[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

            ////// MEXIQUE //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Mexique = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    Mexique[p].lat=lat;
                    Mexique[p].longi=longi;
                    Mexique[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&Mexique[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

            
            ////// NOUVELLE CALEDONIE //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    NouvelleCaledonie = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    NouvelleCaledonie[p].lat=lat;
                    NouvelleCaledonie[p].longi=longi;
                    NouvelleCaledonie[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&NouvelleCaledonie[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

            ////// NICARAGUA //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Nicaragua = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    Nicaragua[p].lat=lat;
                    Nicaragua[p].longi=longi;
                    Nicaragua[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&Nicaragua[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

            
            ////// PANAMA //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Panama = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    Panama[p].lat=lat;
                    Panama[p].longi=longi;
                    Panama[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&Panama[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

            ////// PEROU //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Perou = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
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
                    
                    
                           
					plastique(&Perou[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

            
            ////// PHILIPPINES //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Philippines = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    Philippines[p].lat=lat;
                    Philippines[p].longi=longi;
                    Philippines[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&Philippines[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

            ////// RUSSIE //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Russie = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    Russie[p].lat=lat;
                    Russie[p].longi=longi;
                    Russie[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&Russie[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

            ////// SINGAPOUR //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Singapour = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    Singapour[p].lat=lat;
                    Singapour[p].longi=longi;
                    Singapour[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&Singapour[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

            ////// USA //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    USA = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    USA[p].lat=lat;
                    USA[p].longi=longi;
                    USA[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&USA[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

            ////// VIETNAM //////
            nb_villes=pays_inputs[0+5*pays_parcourus]; 
           
            //ACTUALISATION DES RATES (car augmentation annuelle):
            // rate en [kg plastique dans l'ocean /person/day]
            actual_rate= pays_inputs[4+5*pays_parcourus];
            new_rate = actual_rate* pow(1+taux_croiss_dechets/100,a);
       
            actual_pop= (pays_inputs[1+5*pays_parcourus]*pays_inputs[2+5*pays_parcourus]/100) ;
            new_pop= actual_pop*pow(1+taux_croiss_pop/100,a);

            for(int c=0;c<nb_villes;c++){
                //Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*nb_villes_parcourues];
				double longi= gps_inputs[0+7*nb_villes_parcourues]; 
                
                double kg_plastique_emis = new_pop*new_rate/nb_villes*1; //[kg], on a un jour donc *1
                unsigned long nb_paquets_emis= kg2nb(kg_plastique_emis, plast_par_paquet); // int divisé par int donne int (division entière)
 
                if (a == 0 && j == 0 && c == 0){
                    longueur_tableaux[pays_parcourus] = nb_paquets_emis*nb_villes*365*duree;
                    Vietnam = malloc( longueur_tableaux[pays_parcourus] * sizeof (struct paquet));
                }

                //Initialiser les structures des nouveaux paquets émis avec les coordonnées GPS de la ville:
                for(int p=0+longueur_reelle_tableaux[pays_parcourus];p<nb_paquets_emis+longueur_reelle_tableaux[pays_parcourus];p++){
                    
                    
                    
                    //indexation : indice ligne * indice colone * nb total de colone
                    Vietnam[p].lat=lat;
                    Vietnam[p].longi=longi;
                    Vietnam[p].i=0;
                }
                longueur_reelle_tableaux[pays_parcourus] += nb_paquets_emis;

                //Déplacement des nouveaux plastiques émis et anciens dans l'océan:
				for (int p=0; p<longueur_reelle_tableaux[pays_parcourus]; p++){     
                    
                    
                           
					plastique(&Vietnam[p],saturation);
                }
                nb_villes_parcourues+=1;
            }
            pays_parcourus+=1;

           
            }
        //On update notre tableau final on y mettant les valeurs des compteurs obtenus pour l'année en cours    
        for (int i=0;i<360*720;i++){
		CSV_output [(a+1)*360*720+i]=Cases[i].compteur;}
		 
    }
    
    //On écrit le CSV final:
    
    char * filename = "actualisationGPGP.csv";
    writeCsv(filename,CSV_output,(duree+1));
       
    free(Cases);
    free(gps_inputs);
    free(pays_inputs);
    free(longueur_tableaux);
    free(longueur_reelle_tableaux);
    free(CSV_output);
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
    
    return 0;
}
