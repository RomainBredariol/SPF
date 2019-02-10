#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "client.h"
#include "lib.h"
#include "libTraitements.h"
#include <unistd.h>
#define MAX_PATH 260


// codes couleurs pour printf
#define ROUGE "\x1b[31m"
#define VERT "\x1b[32m"
#define JAUNE "\x1b[33m"
#define BLEU "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"



// televerser permet d'envoyer un fichier sur le serveur
int televerser() {
	char *fileName, *donnee;
	int size;
	fileName = malloc(MAX_PATH);
	donnee = malloc(MAX_PATH+20);

	printf("Veuillez entrer le nom du fichier a televerser sur le serveur\n");
	printf("nom du fichier : ");
	lire(fileName, MAX_PATH);

	FILE * file = fopen(fileName, "r");
	if(file == NULL){
		printf("Erreur fopen %s\n", fileName);
		return -1;
	}
	size = longueur_fichier(fileName);		

	sprintf(donnee, "008 %s %i\n",fileName, size); 
	Emission(donnee);

	free(donnee);
	donnee = malloc(size);
	lireContenuFichier(fileName, donnee, size);

	EmissionBinaire(donnee, size);
	lireReponse();

	free(fileName);
	free(donnee);

	return 0;
}

// telecharger permet de télécharger un fichier depuis le serveur
int telecharger() {
	return 0;
}

// autorisations permet de ?????
int autorisations() {
	return 0;
}

// etat permet de ???? 
int etat() {
	return 0;
}

// gererFichiers  permets de ???? 
int gererFichiers() {
	char choixChar[3];		// choix sous forme de chaine
	char c; 			// char pour vider le stdin
	int choix;			// choix transformé en int

	system("clear");
	printf("=========================================\n");
	printf("1 - Supprimer fichier\n");
	printf("2 - Renommer fichier\n");
	printf("3 - ???\n");
	printf("0 - Retour au menu principal\n");

	printf("Veuillez entre le numero de l'option souhaitee :\n");
	fgets(choixChar, 3, stdin);

	// vider le stdin si l'utilisateur depasse
	if (choixChar[2] != '\0') {
		while ((c = getchar()) != '\n' && c != EOF) { }
	} else {
		//sinon supprimer le new line
		choixChar[1] = '\0';
	}
	
	choix = strtol(choixChar, NULL, 10);
	printf("utilisateur choisi : %d \n",choix);

	switch(choix){
		case 1:
			supprimerFichier();
			break;
		case 2:
			renommerFichier();
			break;
		case 3:
			// ???
			break;
		case 0: 
			return 0;
			break;
	}
	return 0;
}

// listeFichiers permet de lister les fichiers telechargeable
int listeFichiers() {

	Emission("10 \n");
	//recevoir les données
	char *rep;
	rep = Reception();
	printf("\n\nLISTE DES FICHIERS TELECHARGEABLES\n\n");
	for (int i = 0 ; i < strlen(rep); i++ ) {
		if (rep[i] != ':') {
			printf(BLEU"%c"RESET,rep[i]);
		} else {
			printf("\n");
		}	
	}
	
	return 0;
}

// gestionComptes permet de ????
int gestionComptes() {
	char choixChar[3];		// choix sous forme de chaine
	char c; 			// char pour vider le stdin
	int choix;			// choix transformé en int

	system("clear");
	printf("=========================================\n");
	printf("1 - Ajouter un utilisateur\n");
	printf("2 - Supprimer un utilisateur\n");
	printf("3 - Modifier les infos super utilisateur\n");
	printf("0 - Retour au menu principal\n");

	printf("Veuillez entre le numero de l'option souhaitee :\n");
	fgets(choixChar, 3, stdin);

	// vider le stdin si l'utilisateur depasse
	if (choixChar[2] != '\0') {
		while ((c = getchar()) != '\n' && c != EOF) { }
	} else {
		//sinon supprimer le new line
		choixChar[1] = '\0';
	}
	
	choix = strtol(choixChar, NULL, 10);
	printf("utilisateur choisi : %d \n",choix);

	switch(choix){
		case 1:
			addUser();
			break;
		case 2:
			delUser();
			break;
		case 3:
			editSu();
			break;
		case 0: 
			return 0;
			break;
	}
	return 0;
}

//addUser permet de saisir un nouvel utilisateur
int addUser(){
	char login[100];	// chaine utilisé pour le login
	char mdp[100];		// chaine utilisé pour le mot de passe
	char donnee[300];

	memset(login, 0, 100);
	memset(mdp, 0, 100);
	memset(donnee, 0, 300);

	// lecture des identifiants de l'utilisateur a jouter
	printf("Veuillez saisir le login et mdp de l'utilisateur a ajouter\n");
	lecture_login_mdp(login, mdp);

	sprintf(donnee,"6 %s %s\n", login, mdp);

	Emission(donnee);
	lireReponse();
	return 0;
}

//delUser permet de supprimer un utilisateur
int delUser(){
	char login[100];	// chaine utilisé pour le login
	char mdp[100];		// chaine utilisé pour le mot de passe
	char donnee[300];

	memset(login, 0, 100);
	memset(mdp, 0, 100);
	memset(donnee, 0, 300);


	// lecture des identifiants de l'utilisateur a jouter
	printf("Veuillez saisir le login et le password l'utilisateur a supprimer\n");
	lecture_login_mdp(login, mdp);
	
	sprintf(donnee,"25 %s %s\n", login, mdp);

	Emission(donnee);
	lireReponse();
	return 0;
}

//lire permet de recuperer une chaine de caractere dans le buffer stdin
int lire(char *chaine, int size){
	char c;
	fgets(chaine, size+1, stdin);

	// vider le stdin si l'utilisateur depasse
	if (chaine[size - 2] != '\0') {
		while ((c = getchar()) != '\n' && c != EOF) { }
	} else {
		//sinon supprimer le new line
		chaine[strlen(chaine) - 1] = '\0';
	}
	return 0;
}

//lecture_login_mdp permet de recuperer le login et le mdp dans le buffer stdin
int lecture_login_mdp(char *login, char *mdp){
	//recuperation login
	printf("login :\n");
	lire(login, 100);	

	//recuperation mdp
	printf("password :\n");
	lire(mdp, 100);	

	return 0;
}

//editSu permet de modifier les login et mdp du Su
int editSu(){
	char login[100];	// chaine utilisé pour le login
	char mdp[100];		// chaine utilisé pour le mot de passe
	char donnee[300];
	
	memset(login, 0, 100);
	memset(mdp, 0, 100);
	memset(donnee, 0, 300);

	// lecture des identifiants de l'utilisateur a jouter
	printf("Veuillez saisir votre nouveau login et nouveau mdp\n");
	lecture_login_mdp(login, mdp);

	sprintf(donnee,"27 %s %s\n", login, mdp);

	Emission(donnee);
	lireReponse();
	return 0;
}


// fonction qui permet a l'utilisateur de supprimer une fichier de son environnement
int supprimerFichier() {
	// lister les fichier pour savoir quoi pouvoir supprimer
	listeFichiers();
	printf("\nQuel fichier voulez vous supprimer?\n\n");
	// lire la reponse de l'utilisateur
	char reponse[50];
	memset(reponse,0,50);
	lire(reponse,50);
	// envoyer la requete de suppression
	char requete[60];
	memset(requete,0,60);
	strcpy(requete,"19 ");
	strcat(requete,reponse);
	strcat(requete,"\n");
	Emission(requete);
	// lire la reponse du serveur
	char *rep;
	rep = Reception();

	if(strcmp(rep,"007\n") == 0) {
		printf("suppression du fichier réussie\n");
	} else {
		printf("suppresson impossible, nom de fichier incorrect\n");
	}
	
	return 0;
}

// fonction qui permet à l'utilisateur de renomer un fichier de son environnement
int renommerFichier() {

	return 0;
}


