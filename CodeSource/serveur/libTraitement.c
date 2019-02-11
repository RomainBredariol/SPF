#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "serveur.h"
#include "lib.h"
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_PATH 260



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


//un client veut televerser un fichier 
int televerser(char *donnee){
	char *fileName, *contenu;
	int size;

	fileName = malloc(MAX_PATH);

	//on scanne les donnees qui contiennent le nom de fichier et sa size
	sscanf(donnee, "%s %i", fileName, &size); 		

	//on extrait le nom du fichier dans le cas ou c'est un chemin absolue
	extraitNomFichier(fileName);	

	contenu = malloc(size);

	//on recoit le fichier
	ReceptionBinaire(contenu, size);

	//on ecrit le fichier 
	ecrireContenuFichier(fileName, contenu, size);	

	//on libere les pointeurs
	free(fileName);
	free(contenu);
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
	//verifier que le fichier existe
	DIR *d;
     	struct dirent *dir;
	char cheminUser[50];
	int ok = 0;
	memset(cheminUser,0,50);
	strcpy(cheminUser,"depot/");
	strcat(cheminUser,nomUser);
     	d = opendir(cheminUser);
     	if (d) {
        	while ((dir = readdir(d)) != NULL){
			if (strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,"..") != 0 && strcmp(dir->d_name,"autorisations")){
				if (strcmp(dir->d_name,donnee) == 0) {
					ok = 1;	
				}
			} 	
        	}
	}
        closedir(d);

	// executer la suppression si elle est possible
	if (ok) {
		system(command);
		Emission("007\n");
	} else {
		Emission("202\n");
	}
	return 0;
}

// fonction qui modifier le nom d'un fichier du serveur
int renommerFichier(char *donnee) {
	// forger la commande de modificiation du nom du fichier
	char command[50];
	memset(command,0,50);
	strcpy(command,"mv depot/");
	strcat(command,nomUser);
	strcat(command,"/");
	char fichierInit[50];
	char fichierFin[50];
	memset(fichierInit,0,50);
	memset(fichierFin,0,50);
	sscanf(donnee,"%[^ ] %[^ ]",fichierInit,fichierFin);
	strcat(command,fichierInit);
	strcat(command," depot/");
	strcat(command,nomUser);
	strcat(command,"/");
	strcat(command,fichierFin);
	//verifier que le fichier existe
	DIR *d;
     	struct dirent *dir;
	char cheminUser[50];
	int ok = 0;
	memset(cheminUser,0,50);
	strcpy(cheminUser,"depot/");
	strcat(cheminUser,nomUser);
     	d = opendir(cheminUser);
     	if (d) {
        	while ((dir = readdir(d)) != NULL){
			if (strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,"..") != 0 && strcmp(dir->d_name,"autorisations")){
				if (strcmp(dir->d_name,fichierInit) != 0 && strcmp(dir->d_name,fichierFin) != 0) {
					ok = 1;	
				}
			} 	
        	}
	}
        closedir(d);

	// executer la suppression si elle est possible
	if (ok) {
		system(command);
		Emission("007\n");
	} else {
		Emission("202\n");
	}
	return 0;
}
// fonction qui permet d'autoriser un utilisateur à telecharger un fichier
int addDroits(char *donnee) {
	
	// séparer utilisateur et nom de fichier de la requete
	char utilisateur[50];
	char fichier[50];
	memset(utilisateur,0,50);
	memset(fichier,0,50);
	sscanf(donnee,"%[^ ] %[^ ]",fichier,utilisateur);
	//verifier que le fichier existe
	DIR *d;
     	struct dirent *dir;
	char cheminUser[50];
	int ok = 0;
	memset(cheminUser,0,50);
	strcpy(cheminUser,"depot/");
	strcat(cheminUser,nomUser);
     	d = opendir(cheminUser);
     	if (d) {
        	while ((dir = readdir(d)) != NULL){
			if (strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,"..") != 0 && strcmp(dir->d_name,"autorisations")){
				if (strcmp(dir->d_name,fichier) == 0) {
					ok = 1;	
				}
			} 	
        	}
	}
        closedir(d);

	// si le fichier n'existe pas envoyer un message d'erreur
	if (!ok) {
		Emission("202\n");
		return 1;
	}
	// verifier si l'utilisateur existe (si son dossier existe)
	memset(cheminUser,0,50);
	sprintf(cheminUser,"depot/%s",utilisateur);
	if (opendir(cheminUser) == NULL) {
		// si l'utilisateur n'existe pas envoyer un message d'erreur
		Emission("202\n");
		return 1;
	}
	// autoriser l'utilisateur
	// ouvrir le fichier
	strcat(cheminUser,"/autorisations");
	FILE *f = fopen(cheminUser,"a");
	if (f != NULL) { printf("fichier ouvert %s\n",cheminUser); }
	// ecrire dans le fichier
	memset(cheminUser,0,50);
	sprintf(cheminUser,"../%s/%s\n",nomUser,fichier);
	fputs(cheminUser,f);
	fclose(f);
	// envoyer un message pour OK
	Emission("007\n");
	return 0;
}
