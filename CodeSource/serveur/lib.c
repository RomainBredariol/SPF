#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "serveur.h"
#include "lib.h"
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include "libTraitement.h"

#define MAX_PATH 260

char nomUser[100];

//definit la structure d'un utilisateur 
struct user{
	char login [100];
	char mdp[100];
};

//le super utilisateur
struct user su = {"ad", "ad"};

// auteur : Bredariol Romain
//edition du super utilisateur
int editSu(char *donnee){
	char login[100], mdp[100]; //contient le nouveau login et nouveau mdp 

	memset(login, 0, 100);
	memset(mdp, 0, 100);

	//on recupere les nouveaux login et mdp
	sscanf(donnee, "%s %s", login, mdp);

	strcpy(su.login, login);
	strcpy(su.mdp, mdp);

	//on emet une reponse positive au client
	Emission("007 Les modifications ont bien ete prises en compte\n");
	return 0;
}

// auteur : Bredariol Romain
// is_user vérifie sir l'utilisateur existe et si son 
// mot de passe est correcte
int is_user(char * login_mdp){
	//login et mdp sont ceux du client qui essaye de se connecter
	//login_test et mdp_test sont ceux recupere 
	//aupres du fichier de log "user_list"
	char login[100], login_test[100];
	char mdp[100], mdp_test[100];

	//decoupe le login et mdp
	sscanf(login_mdp,"%s %s", login, mdp);

	//teste le login et mdp par rapport au Su
	if(strcmp(su.login, login)==0){
		if(strcmp(su.mdp, mdp)==0){
			//confirme que le su est identifie
			strcpy(nomUser, su.login);
			return 5;
		}else{
			//Mauvais mdp
			return 0;
		}
	}else{
		//teste le login et mdp par rapport a un utilisateur lambda
		FILE * user_list = fopen("users", "r");
		if(user_list == NULL){
			printf("Erreur lecture du ficher user\n");
			return -1;
		}

		//tant qu'il y a une ligne dans le fichier de log
		while(fscanf(user_list, "%s %s",login_test, mdp_test) != EOF){ 

			//on verifie que l'utilisateur existe
			if(strcmp(login_test, login)==0){
				if(strcmp(mdp_test, mdp)==0){
					//confirme qu'un utilisateur est identifie
					strcpy(nomUser, login);
					return 4;
				}else{
					printf("Mauvais mdp\n");
				}
			}
		}
		fclose(user_list);	
	}
	return 0;
}

// auteur : Poussard Sébastien
// fonction qui lance le serveur
// demande à l'utilisateur le port qu'il veut utiliser, sinon utilise 1337 par defaut
// crée toute les fichier necessaire au fonctionnement du serveur s'il ne sont pas présent
int lancerServeur() {
	char port[7];			// port sur lequel le serveur ecoute les requetes
	char c;				// char pour vider le stdin

	//demande sur quel port ouvrir le serveur
	printf("Sur quel port voulez vous que le serveur ecoute ? (appuyer sur entree pour utiliser le port par defaut : 1337)\n");
	fgets(port, 7, stdin);
	//si aucun port n'est donner utiliser le port par defaut : 1337
	if (strcmp(port, "\n") == 0) { 
		strcpy(port,"1337");
	
	} else {
		// sinon, si l'utilisateur depasse la taille du buffer, nettoyer stdin
		if (port[6] != '\0') {
			while ((c = getchar()) != '\n' && c != EOF) { }
		}
	// enlever le new line a la fin de la chaine de caractere
	port[strlen(port)-1] = '\0';
	} 

	//verifier que le fichier "users" contenant les login existe sinon le créer.
	if (fopen("users","r") == NULL) {
		printf("\nle fichiers d'identifiant n'existe pas...\ncréation du fichier...\n");
		if (fopen("users","w") != NULL) {
			printf("création du fichier users réussie\n");
		}else{
			printf("echec à la creation du fichiers users\n");
			return 1;
		}
	}
	// verifier que le dossier "data" existe, sinon le créer.
	if (opendir("depot") == NULL) {
		printf("\nle dossier de données \"depot\" n'existe pas...\ncréation du dossier...\n");
		if (mkdir("depot",0777) == 0) {
			printf("création du dossier depot réussie\n\n");
		}else{
			printf("echec à la creation du dossier depot\n\n");
			return 1;
		}
	}
	// verifier que le fichier data/liste existe sinon le créer
	if (fopen("depot/liste","r") == NULL) {
		printf("\nle fichiers data/liste (liste des fichiers et de leur propriétaires)n'existe pas...\ncréation du fichier...\n");
		if (fopen("depot/liste","w") != NULL) {
			printf("création du fichier liste réussie\n");
		}else{
			printf("echec à la creation du fichiers liste\n");
			return 1;
		}
	}
	
	// initialiser le serveur sur le port
	InitialisationAvecService(port);
	return 0;
}

// auteur : Bredariol Romain
//Demande l'authentification au client
int authentification(){
	char * user; 	//contient le login et mdp
	int ecode;	//l'ecode est un code d'erreur
	
	//recoit les donnees de connexion
	user = Reception();

	//decoupe le message recu 
	sscanf(user, "003 %[^\n]", user);

	//verifie que l'utilisateur existe
	ecode = is_user(user);

	//si c'est un utilisateur lambda
	if(ecode == 4){
		Emission("004 auth OK\n");
		return 1;
	}

	//si c'est le super utilisateur
	if(ecode == 5){
		Emission("005 auth su OK\n");
		return 1;
	}

	// si login ou mdp ne sont pas bon renvoyer le code pour echec d'authentification
	Emission("104 login et/ou mot de passe incorrect\n");

	return 0;
}

// auteur : Bredariol Romain
//extraitNomFichier permet de d'extraire le nom d'un fichier d'un chemin absolue
char* extraitNomFichier(char *fileName){
	char *ptr;	//pointe la chaine qui contient le nom du fichier

	while((ptr = strstr(fileName, "/"))){
		ptr = &ptr[1];
		strcpy(fileName, ptr);
	}
	return fileName;
}

// auteur : Bredariol Romain
//permet d'ecrire dans un fichier
int ecrireContenuFichier(char *nomFichier, char *contenu, int size){
	char path[MAX_PATH];
	sprintf(path, "depot/%s/%s", nomUser, nomFichier);

	FILE *fichier = fopen(path, "w");
	if(fichier == NULL){
		printf("Erreur fopen %s\n", nomFichier);
		return -1;
	}

	int ecode = fwrite(contenu, 1, size, fichier); 
        if(ecode == 0){
        	printf("Erreur ecriture de donnee dans %s\n", nomFichier);
        	return -1;
	}
	fclose(fichier);

	ajouterFichierListe(nomFichier);

	Emission("007 le fichier a bien ete televerse\n");
	return 0;
}

// auteur : Bredariol Romain
//on supprime de la liste tous les fichiers qui appartiennent a un utilisateur
int supprimerFichierListe(char* userName){
	int ecode;
	char liste_fichier[MAX_PATH], liste_user[100], fichier_nom[MAX_PATH+102];

	FILE * fichier = fopen("depot/liste", "r");
	if(fichier == NULL){
		printf("Erreur fopen liste\n");
		return -1;
	}

	FILE * fichierTmp = fopen("depot/liste.tmp", "w");
	if(fichierTmp == NULL){
		printf("Erreur fopen liste tmp\n");
		return -1;
	}

	while(fscanf(fichier, "%s %s", liste_fichier, liste_user) != EOF){
		if((strcmp(liste_user, userName)) != 0){
			sprintf(fichier_nom, "%s %s", liste_fichier, liste_user);
			ecode = fwrite(fichier_nom, 1, strlen(fichier_nom), fichierTmp); 
			if(ecode == 0){
				printf("Erreur ecriture de donnee dans liste Tmp\n");
				return -1;
			}
		}

	}

	rename("depot/liste.tmp", "depot/liste");

	fclose(fichier);
	fclose(fichierTmp);
	return 0;
}

// auteur : Bredariol Romain
//on ecrit le nom du fichier et le nom de l'utilisateur a qui il appartient 
//dans le fichier liste qui recense l'ensemble des fichiers du serveur
int ajouterFichierListe(char* nomFichier){
	char contenu[strlen(nomFichier)+105];
	int ecode;
	char liste_fichier[MAX_PATH], liste_user[100];

	FILE * fichier = fopen("depot/liste", "a+");
	if(fichier == NULL){
		printf("Erreur fopen liste\n");
		return -1;
	}

	while(fscanf(fichier, "%s %s", liste_fichier, liste_user) != EOF){
		if((strcmp(liste_fichier, nomFichier)) == 0){
			if(strcmp(liste_user, nomUser) == 0){
				return 0;
			}
		}
	}

	sprintf(contenu, "%s %s", nomFichier, nomUser); 
	strcat(contenu, "\n");
	ecode = fwrite(contenu, 1, strlen(contenu), fichier); 
	if(ecode == 0){
		printf("Erreur ecriture de donnee dans liste\n");
		return -1;
	}

	fclose(fichier);
	return 0;
}

// auteur : Poussard Sébastien
// cette fonction récupere la requete du client et execute en fonction du code
// utilisé la fonction adéquat
int executerRequete(char * requete){

	char choixChar[5];	// chaine contenant le code de la requete du client
	char donnee[200];	// donnee suivant le numéro de la requete
	int choix;		// code de la requete sous forme de int

	memset(choixChar, 0, 5);
	memset(donnee, 0, 200);
	// séparer de la requete le choix et les données
	sscanf(requete, "%s %[^\n]", choixChar, donnee);
	
	// conversion de choix de chaine de caractere vers un int
	choix = strtol(choixChar, NULL, 10);

	// selection du choix
	switch(choix) {
		case 6:
			// ajouter un nouvel utilisateur
			addUser(donnee);
			break;
		case 8:
			// televerser un fichier
			televerser(donnee);
			break;
		case 10:
			// lister les fichier telechargeable
			lister(donnee);
			break;
		case 11:
			// autoriser un utilisateur à telecharger un fichier
			addDroits(donnee);
			break;
		case 13:
			// enlever un utilisateur de la liste des utilisateurs autorisés 
			// a telecharger un fichier
			delDroits(donnee);
			break;
		case 15:
			etat(donnee);
			// demande de l'état de l'espace de stockage
			break;
		case 17:
			// renomer un fichier
			renommerFichier(donnee);
			break;
		case 19:
			// supprimer un fichier
			supprimerFichier(donnee);
			break;
		case 23:
			// telecharger un fichier
			telecharger(donnee);			
			break;
		case 25:
			// supprimer un utilisateur
			delUser(donnee);
			break; 
		case 27:
			// editer le compte administrateur
			editSu(donnee);
			break;
		default:
			break;
	}
	return 0;
}

// auteur : Bredariol Romain
//lit le contenu d'un ficher
int lireContenuFichier(char *nomFichier, char *contenu, int taille){
	/* contenu est le contenu du fichier binaire à envoyer au client
	 * taille est la longueur du fichier demande*/
	FILE *fichier = fopen(nomFichier, "r");
	//ouvre le fichier demande
	if(fichier == NULL){
		printf("Erreur ouverture fichier : %s\n", nomFichier);
		return -1;
	}

	//place le pointeur de fichier au debut du fichier
	fseek(fichier, 0, SEEK_SET);
	//lit le contenu du fichier
	fread(contenu, taille, 1, fichier);

	return 0;
}
