/ Code C projet prog, version Maëlle

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

    ////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////    PARTIE ADÈLE       /////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	
	//Ouverture des fichiers de données :
	
	gps_inputs=malloc(48*7 * sizeof(double)); //48 points GPS avec 7 infos dessus (colones) 
    readCsvGen("gps_inputs.csv", gps_inputs, 48, 7);
   
    //?????????//QUESTION SUR CETTE FONCTION: X=LIGNES ET Y=COLONES ? ET LE DOUBLE * VALUES C'EST POUR LE TABLEAU A REMPLIR?
   

	//??????????QUESTION: QUELLES LIGNES NEGLIGER DANS LES FICHIERS (NOMS DE PAYS?) ET COMMENT ?
	
	//INDEXATION DES TABLEAUX:
	// - gps_inputs[0+7c]=>longi , gps_inputs[1+7c]=>lat , gps_inputs[2+7c]=>facteur pour ÷ population costale / nb de villes 
	// - gps_inputs[3+7c]=>pop du pays en 2021, gps_inputs[4+7c]=> % pop in pacific coast, 
	// - gps_inputs[5+7c]=>wate generation rate, gps_inputs[6+7c]=> pars des déchets tt qui finissent dans l'eau
	
	// avec c (city) l'indice de la ville de 0 à 48 (exclus)
	// Les pays sont triés par ordre alphabétique et de 1 à 3 villes sont considérés dans chaque pays.
	
	
	
	//INTERFACE UTILISATEUR:
	double taux_croiss_pop;
	double taux_croiss_dechets;
	int duree;
	printf("Entrez un taux de croissance [%] de la population mondiale (format attendu: double) : \n" );
	scanf("%f",taux_croiss_pop);
	printf("Entrez un taux de croissance [%] de la production de déchet mondiale (format attendu: double): \n");
	scanf("%f",taux_croiss_dechets);
	printf("Entrez une durée de simulation (années entières)(format attendu: int): \n");
	scanf("%d",duree);

	// ?????? INDIQUER CLAIREMENT qu'on fait l'hypothèse que une croissance de x% de la pop mondiale correspond à une croissance
	// de x% dans chaque pays ????? c'est juste de dire ça enfaite ??????
	
	
	//inputs des déchets à partir des données rentrées:
	
	//durée simulation duree [années] * 365 * 24 car actualisation du système toutes les heures:
	//Considérer les 48 points GPS (villes côtières du pacifique / points pertinants) qui relachent des déchets dans l'océean:
	
	// ???? QUEL EST LE MEILLEUR SENS PR LA BOUCLE IMBRIQUÉE H=>C OU CITY=>H???
	// ???? AU FINAL COMMENT ON ACTUALISE LE RATE DE PRODUCTION DE DÉHCET ET LA POP? RÉPARTIT OU D'UN COUP?
	
	for(int t =0; t<duree*365*24; t++){
		for (int c=0, c<48; c++){
			
			double lat= gps_inputs[1+7c];
			double longi= gps_inputs[0+7c];
			
			// rate generation [kg/person/day]
			double actual_rate= gps_inputs[5+7x]
			double new_rate = actual_rate + actual_rate*taux_croiss_dechets/100 //A MODIF SELON ACTUALISATION DU RATE, ici on l'actualise d'un coup
			
			double actual_pop= (gps_inputs[3+7x]*gps_inputs[4+7c]/100)/gps_inputs[2+7c]  //pop tt pays * % habitant sur la cote pacifique / nb villes considérées dans le pays
			double new_pop= actual_pop + actual_pop*taux_croiss_pop/100 
			
			double kg_platique_produit = new_rate*new_pop/24 
			double kg_plastique_ocean = kg_plastique*gps_inputs[6+7c]
			int nombre_plastique_ocean= //CONVERSION KG PLASTIQUE EN NB PLASTIQUE A METTRE !!!!!!!!!!!!!!!
			// !!!!!!!!!! NOMBRE PLASTIQUE DOIT ETRE UN INT, COMMENT ARRONDIR ? !!!!!!!!!!!!!
			
			//Déplacement des plastiques émis dans l'océan:
			for (int p=0; p<nombre_plastique_ocean; p++){            
				plastique(lat,longi);}

			
			


    free(Cases);
    ////////////////////////    PARTIE ADÈLE       /////////////////////////////////////////
    free(gps_inputs);
    return 0;
}
