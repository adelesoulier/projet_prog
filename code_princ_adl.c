// Code C projet prog, version Maëlle
// test bcs pbs de commit

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
bool readCsvGen(char * filename, double * values, int sizeX, int sizeY, int skip_x, int skip_y) {
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

///////////////////////////
// FONCTIONS DE CONVERSIONS
///////////////////////////

// Fonction qui fait la conversion des coordonnées de lat, longi en i 
int ll2i(int lat, int longi){ 
    return longi*360 + lat; 
} // ON L'UTILISE ?????????????????????????????????

// Conversion des lati
int exactocasei(double lat, double longi){
	//Permet de déterminer dans quelle case de courrant nous sommes. (cases de 0.5° par 0.5°)
	
    lat = (lat + 89.75)*2; //conversion de la latitude en la colonne (donne un entier pour tous les .25 et .75)
    longi = (longi + 179.75)*2; // idem, mais de longi en ligne
    lat = (int) floor(lat + 0.5); // arrondir à l'entier le plus proche (il n'y a que floor qui existe,
    // mais si on fait + 0.5 revient au même que d'arrondir à l'entier le plus proche)
    longi = (int) floor(longi + 0.5);
    return longi*360 + lat; // on convertit en i
}

// Conversion de kg en nombre de plastique (moyenne pondérée) (FAIRE HEAVY TAIL SI POSSIBLE!!!!!!!!!!!!!!!!!!!!!!)
int kg2nb(double kg){

    double densiteMega = 0.0762;    // [nb/kg]
    double densiteMacro = 41.05;    // [nb/kg]
    double masseMega = 0.667*kg;    // [kg]
    double masseMacro = 0.323*kg;   // [kg]

    int nbMega = (int) floor(masseMega*densiteMega + 0.5); // on arrondit à l'entier le plus proche
    int nbMacro = (int) floor(masseMacro*densiteMacro + 0.5); // idem

    return nbMega + nbMacro;
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

        // Composante aléatoire du déplacement (vent, poisson ect...)
        double alat = randomNumber(30) - 15; //
        double along = randomNumber(30) - 15; // COMMENT DETERMINER SI C'EST UN BON ALEA?????????????????
        // essayer, on est obligées de passer par visualisation des courants et trajectoire de notre plastique.....................

        // Déplacer potentiel du plastique
        latExact += dlat + alat;
        longExact += dlong + along;
        
        //Cas d'un plastique se trouvant à une longitude de -180 ou 180:
        //Les cases vont de -179.75 à 179.75, nous avons fait le choix dans la foncrion exactocasei que 
        // lorsqu'on se trouve à la frontière d'une case notre plastique va:
        //-si c'est une frontière verticale: on passe à la case à droite de cette frontière
        //-si c'est une frontière horizontale: on passe à la case au dessus de cette frontière
        //Cela est du au fait que la fonction qui arrondi un nombre à l'entier le plus proche arrondis 
        //à l'entier supperieur une valeur ayant une partie décimale égale à 0.5 .

        if (longExact >= 180){
			longExact=-180+(longExact-180);}
		if (longExact < -180){	
			longExact=180+(longExact+180); }
        
        
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


    }}
    


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
    

    

    ////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////    PARTIE ADÈLE       /////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	
	//Ouverture des fichiers de données :
	
	double*gps_inputs=malloc(48*7 * sizeof(double)); //48 points GPS avec 7 infos dessus (colones) 
    readCsvGen("gps_inputs.csv", gps_inputs, 7, 48);
   
    //X=colones ET Y=lignes 

	//??????????QUESTION: QUELLES LIGNES NEGLIGER DANS LES FICHIERS (NOMS DE PAYS?) ET COMMENT ?
	
	//INDEXATION DES TABLEAUX:
	// - gps_inputs[0+7c]=>longi , gps_inputs[1+7c]=>lat , gps_inputs[2+7c]=>facteur pour ÷ population costale / nb de villes 
	// - gps_inputs[3+7c]=>pop du pays en 2021, gps_inputs[4+7c]=> % pop in pacific coast, 
	// - gps_inputs[5+7c]=>wate generation rate, gps_inputs[6+7c]=> pars des déchets tt qui finissent dans l'eau
	
	// avec c (city) l'indice de la ville de 0 à 48 (exclus)
	// Les pays sont triés par ordre alphabétique et de 1 à 3 villes sont considérés dans chaque pays.
	
	
	
	//INTERFACE UTILISATEUR:
	printf(" Les taux de croissances que vous allez entrer fonctionnent de la manière suivante: \n\n" );
	printf(" Chaque année durant la période qui sera entrée, la population et le taux de production de dechet augmenteront ou diminueront selon les valeurs entrées. \n\n" );
	printf(" Les formats attendus sont les suivants: \n" );
	printf("-taux de croissance annuel en pourcentage de la population mondiale : double  \n" );
	printf("-taux de croissance annuel en pourcentage de la production de déchet mondiale : double\n");
	printf("-durée de simulation : int (années entières) \n\n");

	double taux_croiss_pop;
	double taux_croiss_dechets;
	int duree;
	
	printf("Entrez un taux de croissance annuel en pourcentage de la population mondiale  : \n\n" );
	scanf("%lf", &taux_croiss_pop);
	printf("Entrez un taux de croissance annuel en pourcentage de la production de déchet mondiale (format attendu: double): \n\n");
	scanf("%lf",&taux_croiss_dechets);
	printf("Entrez une durée de simulation (années entières)(format attendu: int): \n\n");

	
	
	
	//inputs des déchets à partir des données rentrées:
	
	//durée simulation duree [années] * 365 * 24 car actualisation du système toutes les heures:
	//Considérer les 48 points GPS (villes côtières du pacifique / points pertinants) qui relachent des déchets dans l'océean:
		
	
	for (int y=0; y<duree; y++){
		for(int t =0; t<365*24; t++){
			for (int c=0; c<48; c++){
			
				//Coordonnée GPS considérées:
				double lat= gps_inputs[1+7*c];
				double longi= gps_inputs[0+7*c];
			
				// waste rate generation [kg/person/day]
				double actual_rate= gps_inputs[5+7*c];
				double new_rate = actual_rate* pow([1+taux_croiss_dechets/100],y)
				
			
				double actual_pop= (gps_inputs[3+7*c]*gps_inputs[4+7c]/100)/gps_inputs[2+7c] ; //pop tt pays * % habitant sur la cote pacifique / nb villes considérées dans le pays
				double new_pop= actual_pop*pow([1+taux_croiss_pop/100],y);
			
				double kg_platique_produit = new_rate*new_pop/24 ;
				double kg_plastique_ocean = kg_plastique*gps_inputs[6+7c];
				int nombre_plastique_ocean= kg2nb(kg_plastique_ocean) ;
				
			//Déplacement des plastiques émis dans l'océan:
				for (int p=0; p<nombre_plastique_ocean; p++){            
					plastique(lat,longi);}

					}}}


    free(Cases);
    ////////////////////////    PARTIE ADÈLE       /////////////////////////////////////////
    ////////////////////////   REGARDER LES NOTES!!!!!!      /////////////////////////////////////////
    free(gps_inputs);
    return 0;
}


// À FAIRE: QUEL ALÉATOIRE CHOISIR
