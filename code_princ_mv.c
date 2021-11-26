// Code C projet prog, version Maëlle

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

// On définit le tableau de structures (tableau malloc) qui
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

///////////////////////////////////
// FONCTIONS DE LECTURE DE FICHIERS
///////////////////////////////////

// Fonction pour lire le fichier CSV des courants et les mettre dans notre structure
bool readCsvCourants(char * filenameEVEL, char * filenameNVEL, int sizeLong, int sizeLat) {
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

    while (fgets(buffer, 10000, fileE) != NULL) {
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
        fprintf(stderr, "File %s not found.", filename);
        return false;
    }
	
    int y = 0;
    char buffer[10000];
    while (fgets(buffer, 10000, file) != NULL) {
		if (y>=skip_y){
			int x = 0;
			char * start = buffer;
			while (true) {
				if(x>=skip_x){
				values[(y-skip_y) * sizeX + (x-skip_x)] = atof(start); ;}
                ///////////////// Y A PAS UN POINT VIRGULE DE TROP LA???????????????? (EN-DESSUS)
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

///////////////////////////
// FONCTIONS DE CONVERSIONS
///////////////////////////

// Fonction qui fait la conversion des coordonnées de lat, longi en i 
int ll2i(int longi, int lat){ 
    return lat*720 + longi; 
}

// Conversion des latitudes et longitudes exactes (pas forcément .25 ou .75) en index i du tableau
int exactocasei(double longi, double lat){
	//Permet de déterminer dans quelle case de courrant nous sommes. (cases de 0.5° par 0.5°)
	
    lat = (lat + 89.75)*2; //conversion de la latitude en la colonne (donne un entier pour tous les .25 et .75)
    longi = (longi + 179.75)*2; // idem, mais de longi en ligne
    lat = (int) floor(lat + 0.5); // arrondir à l'entier le plus proche (il n'y a que floor qui existe,
    // mais si on fait + 0.5 revient au même que d'arrondir à l'entier le plus proche)
    // quand on arrive sur une valeur.5, on arrondit à la valeur du DESSUS (choix délibéré)
    longi = (int) floor(longi + 0.5);
    return lat*720 + longi; // on convertit en i
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
int kg2nb(double kg){

    double densiteMega = 0.0762;    // [nb/kg]
    double densiteMacro = 41.05;    // [nb/kg]
    double masseMega = 0.667*kg;    // [kg]
    double masseMacro = 0.323*kg;   // [kg]

    int nbMega = (int) floor(masseMega*densiteMega + 0.5); // on arrondit à l'entier le plus proche
    int nbMacro = (int) floor(masseMacro*densiteMacro + 0.5); // idem

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

void plastique(double longi, double lat, int saturation){
    int i = 0;
    double longExact = longi;
    double latExact = lat;
    int indexCase = exactocasei(longExact, latExact);
    int prevIndexCase = exactocasei(longExact, latExact);


    //on fait premièrement simulation sur years années, avec avancée chaque heure
    for (int t = 0; t < 24*365*years){
        // Courant marin: nous allons lire dans les fichiers csv les composantes
        // correspondant à la case dans laquelle se trouve notre plastique au temps t.

        // Est-ce que le plastique était déjà dans la case ou non? (Si non: i == 0 avant)
        if (i == 0){
            Cases[indexCase].compteur += 1;
            i = 1;
        }
        
        // Composante du courant [deg/h]
        double dlong = Cases[indexCase].compoE*1;
        //je fais *1 pour rappeler qu'on fait *1h
        // COMMENTAIRE A SUPPRIMER DANS RENDU FINAL!!!!!!!!!!!!!!!!:
        // (si on change boucle ou que unités pas des /h -> changer
        double dlat = Cases[indexCase].compoN*1;
        

        // Composante aléatoire du déplacement (vents, poissons ect...)
        double alat = randomNumber(2e-5) - 1e-5;
        double along = randomNumber(2e-5) - 1e-5;
        // VOIR SI EST UN BON ALEA, POUR L'INSTANT ON MET CELA, EXPLIQUER POURQUOI

        // Déplacement potentiel du plastique (si cela n'implique pas une condition break)
        longExact += dlong + along;
        latExact += dlat + alat;
        
        // Cas d'un plastique se trouvant à une longitude de -180 ou 180:
        // Les cases vont de -179.75 à 179.75, nous avons fait le choix dans la fonction exactocasei que 
        // lorsqu'on se trouve à la frontière d'une case notre plastique va:
        // - si c'est une frontière verticale: on passe à la case à droite de cette frontière
        // - si c'est une frontière horizontale: on passe à la case au dessus de cette frontière
        // Cela est dû au fait que la fonction qui arrondit un nombre à l'entier le plus proche arrondit 
        // à l'entier supérieur une valeur ayant une partie décimale égale à 0.5 .
        if (longExact >= 180){
			longExact = -180 + (longExact-180);}
            // Quand on est à 180 pile, on a également un problème d'arrondi (arrondit à la case 720, qui n'existe
            // pas dans notre tableau).
		if (longExact < -180){	
			longExact = 180 + (longExact+180);}
            // Alors que quand on est à -180 pile, exactocasei le convertit en 0, ce qui n'est pas un problème par 
            // rapport à notre tableau et est cohérent avec nos "hypothèses de frontières" énoncées ci-dessus.
        
        
        indexCase = exactocasei(longExact, latExact);

        // On vérifie si on va rester dans la même case ou non
        if (prevIndexCase != indexCase){
            if (Cases[indexCase].compteur < saturation || Cases[indexCase].compoN == nan || Cases[indexCase].compoE == nan){
                // CONDITION EST JUSTE ??????????????????????????????????????? BCS 3 "OU" ET LES NAN OK?????????????????????
                Cases[prevIndexCase].compteur -= 1; 
                // EST JUSTE????????????? PARCE QU'AVANT C'ETAIT Cases[indexCase].compteur -= 1;
                i = 0; // si on sort de la case
            }
            // COMMENTAIRES A ENLEVER POUR LE RENDU FINAL:
            // si on a des trucs négatifs dans les compteurs vient de là,
            // mais si fonctionne est pas censé arriver

            // sinon, donc si on a Cases[indexCase].compteur >= saturation, on sort de la fonction,
            // et le compteur de la case prevIndexCase a toujours le plastique en mémoire
            // si la case dans laquelle on va n'est pas à saturation, on peut avancer donc on peut enlever 1 au compteur
            // de la case d'avant. Si le plastique s'échoue sur une terre/une île (un nan), on enlève le plastique du
            // compteur de la case dans l'eau et on l'ajoute à celui de la terre sur laquelle il arrive

        }

        // Conditions de terminaison:
        //  1) une case est saturée
        //  2) on arrive sur un nan -> on l'enlève du compteur de la case dans l'eau et on ajoute à la case
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
        if (Cases[indexCase].compoN == nan || Cases[indexCase].compoE == nan){
            Cases[indexCase].compteur += 1; 
            // le plastique s'est échoué, on l'ajoute sur la case sur laquelle il est (un nan)
            break;
        }
        prevIndexCase = indexCase;
    }
}
    


int main(int argc, char * argv[]) {
    // on initialise le temps, sera utile pour aléatoire de propagation d'un plastique
    srandom(time(NULL));

    // On réserve la place pour notre tableau de cases
    Cases = malloc(720*360 * sizeof (struct courants));
    // Notre grille de courants est de la forme (lat, longi) :
    // LONGITUDES = COLONNES (720) = x = compoE 
    // LATITUDES = LIGNES (360) = Y = compoN 
    // avec la conversion lat = (lat + 89.75)*2 pour convertir les latitudes en index du tableau
    // (pour plus de précisions, aller voir dans fonctions de conversions, lat2i1d ou exactocasei)
    // et pour les longitudes: longi = (longi + 179.75)*2 (voir long2i1D)
    
    // notre saturation de cases est (nombre de plastiques maximum admis par case, à cette valeur, la
    // case est "pleine")
    int saturation = 100 
    /////////////////// ON MET QUOI???????????????????

    // on remplit le tableau malloc Cases (compoN et compoE)
    readCsvCourants("NOM_DU_FICHIER_COURANTS_MARINS_EVEL.csv", "NOM_DU_FICHIER_COURANTS_MARINS_NVEL.csv", 720, 360); 
    // nos fichiers sont en °/h (vitesse du courant).

    // et on initialise notre continent de plastiques en mettant à jour les "compteurs" du tableau de structures
    fctLongInit(longInit);
    fctLatInit(latInit);
    GPGPinit(longInit,latInit, saturation);
    free(latInit);
    free(longInit);
    /////////////EST-CE QU'ON REUTILISE CES TABLEAUX APRES?????????????????????????????

    ////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////    PARTIE ADÈLE       /////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	
	//Ouverture des fichiers de données :
	
	double*gps_inputs = malloc(48*7 * sizeof(double)); //48 points GPS avec 7 infos dessus (colones) 
    readCsvGen("gps_inputs.csv", gps_inputs, 7, 48);
    /////////////// IL FAUT ACTUALISER CETTE FONCTION!!!!!!!!!! ici on n'a pas encore le skip x et skip y
   
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
				
			
				double actual_pop= (gps_inputs[3+7*c]*gps_inputs[4+7*c]/100)/gps_inputs[2+7*c] ;
                //pop tt pays * % habitant sur la cote pacifique / nb villes considérées dans le pays
				double new_pop= actual_pop*pow([1+taux_croiss_pop/100],y);
			
				double kg_platique_produit = new_rate*new_pop/24 ;
				double kg_plastique_ocean = kg_plastique*gps_inputs[6+7*c];
				int nombre_plastique_ocean= kg2nb(kg_plastique_ocean) ;
				
			//Déplacement des plastiques émis dans l'océan:
				for (int p=0; p<nombre_plastique_ocean; p++){            
					plastique(longi,lat);
                    // ON A UN PROBLEME ICI!!!!!!!!!! DEJA IL DEVRAIT Y AVOIR YEARS ET SATURATION DEDANS MAIS FERAIT
                    // UNE BOUCLE D'ANNEES DANS UNE BOUCLE D'ANNEES, NE VA PAS!!!!!!!!!!!!!!!!!!!!
                }

           }
        }
    }   


    free(Cases);
    ////////////////////////    PARTIE ADÈLE       /////////////////////////////////////////
    free(gps_inputs);
    return 0;
}


// À FAIRE: QUEL ALÉATOIRE CHOISIR
