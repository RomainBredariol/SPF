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

//edition du super utilisateur
int editSu(char *donnee){
	char login[100], mdp[100];

	memset(login, 0, 100);
	memset(mdp, 0, 100);

	sscanf(donnee, "%s %s", login, mdp);

	strcpy(su.login, login);
	strcpy(su.mdp, mdp);

	Emission("007 Les modifications ont bien ete prises en compte\n");
	return 0;
}

//verifie si l'utilisateur existe
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
			printf("Mauvais mdp\n");
			return 0;
		}
	}else{
		//teste le login et mdp par rapport a un utilisateur lambda
		FILE * user_list = fopen("users", "r");
		if(user_list == NULL){
			printf("Erreur lecture du ficher user\n");
			return -1;
		}

		while(fscanf(user_list, "%s %s",login_test, mdp_test) != EOF){ 

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

// fonction qui lance le serveur en demandant le port d'ouverture 
// à l'utilisateur
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

/*Demande l'authentification au client*/
int authentification(){
	char * user;
	//l'ecode est un code d'erreur
	int ecode;
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

//extraitNomFichier permet de d'extraire le nom d'un fichier d'un chemin absolue
char* extraitNomFichier(char *fileName){
	char *ptr;
	while((ptr = strstr(fileName, "/"))){
		ptr = &ptr[1];
		strcpy(fileName, ptr);
	}
	return fileName;
}

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

//on supprime de la liste tous les fichiers qui appartiennent a un utilisateur
int supprimerFichierListe(char* userName){
	int ecode;
	char *userNameList;
	char liste_fichier[MAX_PATH];

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

	if((fscanf(fichier, "%[^\n]", liste_fichier)) != EOF){
		do{
			userNameList = strstr(liste_fichier, " ");
			userNameList = &userNameList[1];
			printf("userNameList : %s\n", userNameList);
			if((strcmp(userNameList, userName)) != 0){
				ecode = fwrite(liste_fichier, 1, strlen(liste_fichier), fichierTmp); 
				if(ecode == 0){
					printf("Erreur ecriture de donnee dans liste Tmp\n");
					return -1;
				}
			}
		}while((fscanf(fichier, "%[^\n]", liste_fichier)));
	}
	rename("liste.tmp", "liste");

	fclose(fichier);
	fclose(fichierTmp);
	return 0;
}

//on ecrit le nom du fichier et le nom de l'utilisateur a qui il appartient 
//dans le fichier liste qui recense l'ensemble des fichiers du serveur
int ajouterFichierListe(char* nomFichier){
	char contenu[strlen(nomFichier)+105];
	int ecode;
	char liste_fichier[MAX_PATH];

	FILE * fichier = fopen("depot/liste", "a+");
	if(fichier == NULL){
		printf("Erreur fopen liste\n");
		return -1;
	}

	sprintf(contenu, "%s %s", nomFichier, nomUser);

	if((fscanf(fichier, "%[^\n]", liste_fichier)) != EOF){
		do{
			if((strcmp(liste_fichier, contenu)) == 0){
				return 0;
			}
		}while((fscanf(fichier, "%[^\n]", liste_fichier)));
	}
	strcat(contenu, "\n");
	ecode = fwrite(contenu, 1, strlen(contenu), fichier); 
	if(ecode == 0){
		printf("Erreur ecriture de donnee dans liste\n");
		return -1;
	}

	fclose(fichier);
	return 0;
}

// executer une requete demandé
int executerRequete(char * requete){
	char choixChar[5];
	char donnee[200];
	int choix;

	memset(choixChar, 0, 5);
	memset(donnee, 0, 200);
	sscanf(requete, "%s %[^\n]", choixChar, donnee);
	
	// conversion de choixChar en int
	choix = strtol(choixChar, NULL, 10);

	// selection du choix
	switch(choix) {
		case 6:
			addUser(donnee);
			break;
		case 8:
			televerser(donnee);
			break;
		case 10:
			lister(donnee);
			break;
		case 11:
			addDroits(donnee);
			break;
		case 13:
			// enlever un utilisateur de la liste des utilisateurs autorisés 
			// a telecharger un fichier
			delDroits(donnee);
			break;
		case 15:
			// demande de l'état de l'espace de stockage
			break;
		case 17:
			renommerFichier(donnee);
			break;
		case 19:
			supprimerFichier(donnee);
			break;
		case 21:
			// liste des fichier telechargeable partagé par les autres
			break;
		case 23:
			// telecharger un fichier
			telecharger(donnee);			
			break;
		case 25:
			delUser(donnee);
			break; 
		case 27:
			editSu(donnee);
			break;
		default:
			break;
	}
	return 0;
}

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
