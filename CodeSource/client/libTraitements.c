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



// auteur : Bredariol Romain
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

// auteur : Bredariol Romain
// telecharger permet de télécharger un fichier depuis le serveur
int telecharger() {
	char *fichier = malloc(MAX_PATH), *requete, *entete = malloc(MAX_PATH), *contenu;
	char nomFichier[100];
	int taille;

	system("clear");
	listeFichiers();	

	printf("Veuillez entrer le nom du fichier que vous voulez telecharger : \n\n");
	printf("(Si vous voulez telecharger un "VERT"fichier partage\n"RESET);
	printf(" Entrez le nom utilisateur et le nom fichier sous le format suivant : nomUtilisateur/nomFichier)\n\n");
	
	//on recupere le nom du fichier
	lire(fichier, MAX_PATH);

	requete = malloc(MAX_PATH+20);	
	sprintf(requete, "023 %s\n", fichier);
	
	//on envoie le nom du fichier a telecharger
	Emission(requete);

	//on recoit le nom du fichier et sa taille 
	entete = Reception();	
	sscanf(entete, "200 %s %i\n", nomFichier, &taille);

	contenu = malloc(taille);

	ReceptionBinaire(contenu, taille);

	ecrireContenuFichier(nomFichier, contenu, taille);

	free(fichier);
	free(requete);
	free(contenu);
	free(entete);
	
	return 0;
}

// auteur : Poussard Sébastien
// autorisations afficher à l'utilisateur un menu de choix entre autoriser un utilisateur
// à telecharger un fichier chez lui et revoquer un utilisateur de ces droits de telechargement 
// sur un de ses fichiers
int autorisations() {
	char choixChar[3];		// choix sous forme de chaine
	char c; 			// char pour vider le stdin
	int choix;			// choix transformé en int

	// afficher le menu
	system("clear");
	printf("=========================================\n");
	printf("1 - Autoriser un utilisateur à télécharger un fichier\n");
	printf("2 - Revoquer les droits d'un utilisateur à télécharger un fichier\n");
	printf("0 - Retour au menu principal\n");

	// recuperer la réponse de l'utilisateur
	printf("Veuillez entre le numero de l'option souhaitee :\n");
	fgets(choixChar, 3, stdin);

	// vider le stdin si l'utilisateur depasse
	if (choixChar[2] != '\0') {
		while ((c = getchar()) != '\n' && c != EOF) { }
	} else {
		//sinon supprimer le new line
		choixChar[1] = '\0';
	}
	// convertir le choix en int	
	choix = strtol(choixChar, NULL, 10);
	// executer le traitement en fonction du choix de l'utilisateur
	switch(choix){
		case 1:
			// autoriser un utilisateur à telecharger un fichier
			addDroits();
			break;
		case 2:
			// revoquer les droits d'un utilisateur a telecharger un fichier
			delDroits();
			break;
		default:
			break;
	}
	return 0;
}

// auteur : Poussard Sébastien
// etat permet de d'afficher l'état de l'environnement (dossier) de l'utilisateur
int etat() {
	return 0;
}

// auteur : Poussard Sébastien
// gererFichiers  permets d'affichr un menu pour supprimer ou renommer un fichier de l'utilisateur 
int gererFichiers() {
	char choixChar[3];		// choix sous forme de chaine
	char c; 			// char pour vider le stdin
	int choix;			// choix transformé en int

	// afficher le menu
	system("clear");
	printf("=========================================\n");
	printf("1 - Supprimer fichier\n");
	printf("2 - Renommer fichier\n");
	printf("0 - Retour au menu principal\n");

	// lire l'option choisis par l'utilisateur
	printf("Veuillez entre le numero de l'option souhaitee :\n");
	fgets(choixChar, 3, stdin);

	// vider le stdin si l'utilisateur depasse
	if (choixChar[2] != '\0') {
		while ((c = getchar()) != '\n' && c != EOF) { }
	} else {
		//sinon supprimer le new line
		choixChar[1] = '\0';
	}
	
	// convertir le choix en int
	choix = strtol(choixChar, NULL, 10);

	// executer le traitement adequat au choix de l'utilisateur
	switch(choix){
		case 1:
			// supprimer un fichier du dossier de l'utilisateur
			supprimerFichier();
			break;
		case 2:
			// renommer un fichier du dossier de l'utilisateur
			renommerFichier();
			break;
		default:
			break;
	}
	return 0;
}

// auteur : Poussard Sébastien
// listeFichiers permet de lister les fichiers telechargeable (ceux de l'utilisateur et ceux partagés)
int listeFichiers() {

	// envoi du code pour obtenir les infos sur les fichiers telechargeable au serveur
	Emission("10 \n");
	//recevoir les données
	char *rep;		// pointeur de chaine contenant la réponse du serveur
	rep = Reception();
	// afficher proprement la réponse obtenue
	printf("\n\nLISTE DES FICHIERS TELECHARGEABLES\n\nvos fichiers en ");
	// indiquer a l'utilisateur le code couleur utilisé
	printf(BLEU"bleu"RESET);
	printf(" les fichiers partagés en ");
	printf(VERT"vert"RESET);
	printf(" par d'autres ");
	printf(JAUNE"utilisateurs\n\n\n"RESET);
	// lire chaque caractere de la reponse
	for (int i = 0 ; i < strlen(rep); i++ ) {
		// si c'est un ":" soter une ligne
		if (rep[i] == ':') {
			printf("\n");
		// si c'est un "?" alors la suite correspondra au nom d'un utilisateur qui partage avec 
		// nous un fichier
		} else if (rep[i] == '?') {
			printf(RESET"l'utilisateur "JAUNE);
		// si c'est un "!" alors la suite correspondra au fichier partagé par l'utilisateur
		} else if (rep[i] == '!') {
			printf(RESET" partage le fichier "VERT);
		// si c'est un "#" alors la suite correspond a un fichier du repretoire de l'utilisateur
		} else if (rep[i] == '#') {
			printf(BLEU);
		} else {
			printf("%c",rep[i]);
		}
	}
	// reset le code couleur pour la suite du programme
	printf(RESET);
	
	return 0;
}

// auteurs : Poussard Sébastien, Bredariol Romain
// gestionComptes affiche le menu pour que l'administrateur choisisse netre 
// ajouter un utilisateur, supprimer un utilisateur ou modifier le compte administrateur
int gestionComptes() {
	// auteur : Bredariol Romain
	char choixChar[3];		// choix sous forme de chaine
	char c; 			// char pour vider le stdin
	int choix;			// choix transformé en int

	// afficher le menu de choix
	system("clear");
	printf("=========================================\n");
	printf("1 - Ajouter un utilisateur\n");
	printf("2 - Supprimer un utilisateur\n");
	printf("3 - Modifier les infos super utilisateur\n");
	printf("0 - Retour au menu principal\n");

	// lire le choix de l'utilisateur
	printf("Veuillez entre le numero de l'option souhaitee :\n");
	fgets(choixChar, 3, stdin);

	// auteur : Poussard Sébastien
	// vider le stdin si l'utilisateur depasse
	if (choixChar[2] != '\0') {
		while ((c = getchar()) != '\n' && c != EOF) { }
	} else {
		//sinon supprimer le new line
		choixChar[1] = '\0';
	}
	
	// transformer le choix en int
	choix = strtol(choixChar, NULL, 10);
	printf("utilisateur choisi : %d \n",choix);

	// executer le choix de l'utilisateur
	switch(choix){
		case 1:
			// ajouter un nouvel utilisateur
			addUser();
			break;
		case 2:
			// supprimer un utilisateur
			delUser();
			break;
		case 3:
			// editer le compte administrateur
			editSu();
			break;
		case 0: 
			return 0;
			break;
		default:
			break;
	}
	return 0;
}

// auteur : Bredariol Romain
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

// auteur : Bredariol Romain
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

// auteur : Poussard Sébastien
//lire permet de recuperer une chaine de caractere dans le buffer stdin
int lire(char *chaine, int size){
	char c;			// caractere utilisé pour vider le buffer
	// lire la reponse de l'utilisateur
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

// auteur : Bredariol Romain
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

// auteur : Bredariol Romain
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


// auteur : Poussard Sébastien
// fonction qui permet a l'utilisateur de supprimer une fichier de son environnement
int supprimerFichier() {
	// lister les fichier pour savoir quoi pouvoir supprimer
	listeFichiers();
	printf("\nQuel fichier voulez vous supprimer?\n\n");

	// lire la reponse de l'utilisateur
	char reponse[50];
	memset(reponse,0,50);
	lire(reponse,50);
	
	//forger la requete
	char requete[60];		// chaine contenant la requete a emettre au serveru
	memset(requete,0,60);
	strcpy(requete,"19 ");
	strcat(requete,reponse);
	strcat(requete,"\n");

	// envoyer la requete de suppression
	Emission(requete);

	// lire la reponse du serveur
	char *rep;	// pointeur vers une chaine contenant la reponse du serveur
	rep = Reception();

	// afficher reussite ou echec de la requete en fonction de ce qu'envoi le serveur
	if(strcmp(rep,"007\n") == 0) {
		printf("suppression du fichier réussie\n");
	} else {
		printf("suppresson impossible, nom de fichier incorrect\n");
	}
	
	return 0;
}

// auteur : Poussard Sébastien
// fonction qui permet à l'utilisateur de renomer un fichier de son environnement
int renommerFichier() {

	// lister les fichier pour savoir quoi pouvoir supprimer
	listeFichiers();
	printf("\nQuel fichier voulez vous renomer ?\n\n");

	// lire la reponse de l'utilisateur et commencer a forger la requete
	char reponse[50];
	memset(reponse,0,50);
	char requete[60];
	memset(requete,0,60);

	lire(reponse,50);
	strcpy(requete,"17 ");
	strcat(requete,reponse);

	// demander le nouveau nom de fichier et finir de forger la requete
	printf("Quel est le nom du nouveau fichier ?\n");
	memset(reponse,0,50);
	lire(reponse,50);
	strcat(requete," ");
	strcat(requete,reponse);
	strcat(requete,"\n");
	Emission(requete);

	// lire la reponse du serveur
	char *rep;
	rep = Reception();

	// afficher un message en fonction de la reponse du serveur (echec ou reussite)
	if(strcmp(rep,"007\n") == 0) {
		printf("fichier renommé avec succés\n");
	} else {
		printf("modification impossible, nom du fichier incorrect un fichier porte deja ce nom\n");
	}
	
	return 0;
}

// auteur : Poussard Sébastien
// autoriser un utilisateur à pouvoir telecharger un de nos fichiers
int addDroits() {

	// lister les fichier pour savoir quoi pouvoir supprimer
	listeFichiers();
	printf("\nSelectionner un fichier à partager\n\n");

	// lire la reponse de l'utilisateur et commencer a forger la requete
	char reponse[50];
	memset(reponse,0,50);
	char requete[60];
	memset(requete,0,60);

	lire(reponse,50);
	strcpy(requete,"11 ");
	strcat(requete,reponse);

	// demander le nouveau nom de fichier et finir de forger la requete
	printf("Quel est le nom de l'utilisateur ?\n");
	memset(reponse,0,50);
	lire(reponse,50);
	strcat(requete," ");
	strcat(requete,reponse);
	strcat(requete,"\n");
	Emission(requete);

	// lire la reponse du serveur
	char *rep;
	rep = Reception();

	// afficher un message en fonction de la reponse du serveur (echec ou reussite)
	if(strcmp(rep,"007\n") == 0) {
		printf("utilisateur autorisé avec succés\n");
	} else {
		printf("autorisation impossible (nom de fichier ou utilisateur inexistant)\n");
	}
	
	return 0;
}

// auteur : Poussard Sébastien
// revoquer les droits d'un utilisateur à telecharger un fichier
int delDroits() {

	char utilisateur[50];		// chaine contenant le nom d'un utilisateur
	memset(utilisateur,0,50);
	char fichier[50];		// chaine contenant le nom d'un fichier
	memset(fichier,0,50);

	// lire l'utilisateur dont on doit revoquer un droit
	printf("\nQuel utilisateur souhaitez vous revoquer les droits d'accés ?\n\n");
	lire(utilisateur,50);

	// lire le nom du fichier 
	printf("sur quel fichier voulez vous révoquer les droits de %s?\n",utilisateur);
	lire(fichier,50);

	// forger la requete et l'envoyer
	char requete[50];
	memset(requete,0,50);

	sscanf(requete,"13 %s %s\n",utilisateur,fichier);
	Emission(requete);

	// lire la reponse du serveur
	char *rep;
	rep = Reception();

	// afficher un message en fonction de la reponse du serveur (echec ou reussite)
	if(strcmp(rep,"007\n") == 0) {
		printf("utilisateur autorisé avec succés\n");
	} else {
		printf("autorisation impossible (nom de fichier ou utilisateur inexistant)\n");
	}
	
	return 0;
}
