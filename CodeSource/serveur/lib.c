#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "serveur.h"
#include "lib.h"
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_PATH 260

char nomUser[100];

//definit la structure d'un utilisateur 
struct user{
	char login [100];
	char mdp[100];
};

//le super utilisateur
struct user su = {"ad", "ad"};

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

//delUser permet de supprimer un utilisateur
int delUser(char *donnee){
	char login_mdp[203];

	strcat(donnee, "\n");

	FILE * user_list     = fopen("users", "r");
	FILE * user_list_tmp = fopen("users.tmp", "w");

	if(user_list == NULL){
		printf("Erreur fopen users\n");
		return -1;
	}

	if(user_list_tmp == NULL){
		printf("Erreur fopen users.tmp\n");
		return -1;
	}

	while(fgets(login_mdp, sizeof(login_mdp), user_list)){
		if(strcmp(login_mdp, donnee)==0){
			continue;
		}else{
			fputs(login_mdp, user_list_tmp);
		}
	}

	fclose(user_list);
	fclose(user_list_tmp);

	rename("users.tmp", "users");

	// suppression du dossier de l'utilisateur et de son contenu
	char user[50];
	char command[100];
	memset(user,0,50);
	memset(command,0,100);
	sscanf(donnee,"%[^ ]",user);
	printf("user = %s\n", user);
	strcpy(command,"rm -rf depot/");
	strcat(command,user);
	printf("command = %s\n",command);
	system(command);
	printf("Suppression de l'utilisateur de la liste des utilisateurs et suppression de son environnement\n");

	Emission("007 L'utilisateur a ete supprime\n");
	return 0;
}

//ajoute un utilisateur a la liste
int addUser(char *donnee){
	int ecode; //error code
	char user[50];
	char addresseDossierUser[80];

	memset(user,0,50);
	memset(addresseDossierUser,0,80);
	

	FILE * user_list = fopen("users", "a");

	if(user_list == NULL){
		printf("Erreur fopen users\n");
		return -1;
	}

	strcat(donnee, "\n");
	
	ecode = fprintf(user_list, "%s", donnee);
	if(ecode < 0){
		printf("Erreur ecriture de donnee\n");
		return -1;
	}
	
	fclose(user_list);

	// verifier que le fichier data/liste existe sinon le créer
	sscanf(donnee,"%[^ ]",user);
	strcpy(addresseDossierUser,"depot/");
	strcat(addresseDossierUser,user);

	printf("adresseDossierUser = %s \n",addresseDossierUser);
	if (mkdir(addresseDossierUser,0777) == 0) {	
		printf("création du dossier d'utilisateur\n");
	strcat(addresseDossierUser,"/autorisations");
	}	
	if (fopen(addresseDossierUser,"w") != NULL) {
			printf("création du fichier d'autorisations de l'utilisateur\n");
	}
	
	Emission("007 L'utilisateur a ete cree\n");
	return 0;
}

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

//un client veut televerser un fichier 
int televerser(char *donnee){
	char *fileName, *contenu, *test;
	int size;

	fileName = malloc(MAX_PATH);
	test = malloc(MAX_PATH);

	sscanf(donnee, "%s %i", fileName, &size); 		

	strcpy(test, extraitNomFichier(fileName));	

	contenu = malloc(size);

	ReceptionBinaire(contenu, size);
	ecrireContenuFichier(fileName, contenu, size);	

	free(fileName);
	free(contenu);
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
	Emission("007 le fichier a bien ete televerse\n");
	return 0;
}

//permet de lister les fichiers telechargeble
int lister(){

	DIR *d;
     	struct dirent *dir;
	char cheminUser[50];
	char reponse[1000];
	memset(reponse,0,1000);
	memset(cheminUser,0,50);
	strcpy(cheminUser,"depot/");
	strcat(cheminUser,nomUser);
     	d = opendir(cheminUser);
     	if (d) {
        	while ((dir = readdir(d)) != NULL){
			if (strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,"..") != 0 && strcmp(dir->d_name,"autorisations")){
				strcat(reponse,dir->d_name);
				strcat(reponse,":");
			} 	
        	}
        closedir(d);
	strcat(reponse,"\n");
	Emission(reponse);
     	}

	return 0;	
}

// fonction qui supprimer un fichier du serveur
int supprimerFichier(char *donnee) {
	// forger la commande de suppression du fichier
	char command[50];
	memset(command,0,50);
	strcpy(command,"rm depot/");
	strcat(command,nomUser);
	strcat(command,"/");
	strcat(command,donnee);
	// executer la suppression
	system(command);

	return 0;
}
