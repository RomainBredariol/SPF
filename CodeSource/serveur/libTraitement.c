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

// auteurs : Poussard Sébastien, Bredariol Romain
//delUser permet de supprimer un utilisateur
// supprimer le dossier (son environnemnt) mais aussi le supprime de
// la liste des utilisateurs (son login/ mot de passe)
int delUser(char *donnee){
	char login_mdp[203];	//chaine de caractere contenant login/mdp

	strcat(donnee, "\n");
	// auteur : Bredariol Romain
	// supprimer l'utilisateur de la liste
	// ouverture de 2 fichier : l'original et le temporaire
	FILE * user_list     = fopen("users", "r");
	FILE * user_list_tmp = fopen("users.tmp", "w");

	if(user_list == NULL){
		printf("Erreur fopen users\n");
		Emission("202\n");	// envoyer au client un message d'erreur en cas d'échec
		return -1;
	}

	if(user_list_tmp == NULL){
		printf("Erreur fopen users.tmp\n");
		Emission("202\n");	// envoyer au client un message d'erreur en cas d'échec
		return -1;
	}
	// parcourir le fichier original et le recopier dans le temporaire 
	// sauf si la ligne correspond à l'utilisateur à supprimer
	while(fgets(login_mdp, sizeof(login_mdp), user_list)){
		if(strcmp(login_mdp, donnee)==0){
			continue;
		}else{
			fputs(login_mdp, user_list_tmp);
		}
	}
	// fermer les 2 fichier et écraser le fichier original avec le temporaire
	fclose(user_list);
	fclose(user_list_tmp);

	rename("users.tmp", "users");

	// auteur : Poussard Sébastien
	// suppression du dossier de l'utilisateur et de son contenu
	char user[50];		// chaine contenant l'utilisateur
	char command[100];	// chaine contenant la commande a executer
	memset(user,0,50);
	memset(command,0,100);

	// forger la commande a executer
	sscanf(donnee,"%[^ ]",user);
	strcpy(command,"rm -rf depot/");
	strcat(command,user);

	// executer la commande
	system(command);
	printf("Suppression de l'utilisateur de la liste des utilisateurs et suppression de son environnement\n");

	supprimerFichierListe(user);

	// envoyer la confirmation au client
	Emission("007 L'utilisateur a ete supprime\n");

	return 0;
}

// auteurs : Poussard Sébastien, Bredariol Romain
//ajoute un utilisateur a la liste et créer son environemnt (dossier contenant ses fichiers +
// fichier d'autorisations
int addUser(char *donnee){
	int ecode; //error code
	char user[50];			// chaine contenant le nom d'utilisateur a creer
	char addresseDossierUser[80];	//chaine contenant un chemin

	memset(user,0,50);
	memset(addresseDossierUser,0,80);
	// auteur : Bredariol Romain
	// ajouter l'utilisateur à la liste des utilisateur (login/mot de passe
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

	// auteur :  Poussard Sébastien
	//créer le dossier d'utilisateur et le fihier autorisations
	// forger le nom de dossier
	sscanf(donnee,"%[^ ]",user);
	strcpy(addresseDossierUser,"depot/");
	strcat(addresseDossierUser,user);

	// creer le dossier de l'utilisateur
	if (mkdir(addresseDossierUser,0777) == 0) {	
		printf("création du dossier d'utilisateur\n");
		strcat(addresseDossierUser,"/autorisations");
	}	

	// creer le fichier d'autorisations de l'utilisateur
	if (fopen(addresseDossierUser,"w") != NULL) {
			printf("création du fichier d'autorisations de l'utilisateur\n");
	}
	
	Emission("007 L'utilisateur a ete cree\n");
	return 0;
}

// auteur : Romain Bredariol
//un client veut televerser un fichier 
int televerser(char *donnee){
	char *fileName,	//contient le nom du fichier 
	     *contenu;	//contient le contenu du fichier
	int size;	//la taille du fichier

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
// auteur : Bredariol Romain
//permet d'envoyer un fichier a utilisateur
int telecharger(char *donnee){
	char *path = malloc(MAX_PATH),		//contient le chemin pour acceder au fichier 
	     *contenu = malloc(MAX_PATH);	//le contenu du fichier
	int size;				//la taille du fichier

	//si le fichirt a telecharger appartient a l'utilisateur ou s'il est partage
	if(strstr(donnee, "/") == NULL){
		sprintf(path, "depot/%s/%s", nomUser, donnee);
	}else{
		sprintf(path, "depot/%s", donnee);
	}

	//ouverture du fichier a envoyer
	FILE *fichier = fopen(path, "r");
	if(fichier == NULL){
		printf("Erreur fopen du fichier %s\n", path);
		return -1;
	}

	//recuperation de la taille du fichier
	size = longueur_fichier(path);

	//on extrait le nom du fichier
	strcpy(donnee, path);
	extraitNomFichier(donnee);

	//on envoie l'entete 
	sprintf(contenu, "200 %s %i\n",donnee, size);
	Emission(contenu);
	free(contenu);

	//on lit le contenu et on l'envoie au client 
	contenu = malloc(size);
	lireContenuFichier(path, contenu, size);
	EmissionBinaire(contenu, size);
	return 0;
}

// auteur : Poussard Sébastien
//permet de lister les fichiers telechargeble
int lister(){

	// lister les fichier du dossier utilisateur
	DIR *d;				// pointeur vers un dossier
     	struct dirent *dir;		// strucutre dirent
	char cheminUser[50];		// chemin
	char reponse[1000];		// reponse envoyé au client

	memset(reponse,0,1000);
	memset(cheminUser,0,50);

	// forger le chemin
	strcpy(cheminUser,"depot/");
	strcat(cheminUser,nomUser);

	// ouvrir le dossier
     	d = opendir(cheminUser);
     	if (d) {
		// parcourir tous les fichier dans le dossier
        	while ((dir = readdir(d)) != NULL){
			// si le nom du fichier n'est pas "." ".." ou "autorisations"
			if (strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,"..") != 0 && strcmp(dir->d_name,"autorisations")){
				// ajouter a la reponse "#" : symbole pour marquer que ce qui suit est un fichier 
				// de l'utilisateur
				strcat(reponse,"#");
				// ajouter à la reponse le nom du fichier
				strcat(reponse,dir->d_name);
				// ajouter a la reponse ":" : symbole pour marquer la fin d'un fichier
				strcat(reponse,":");
			} 	
        	}
        closedir(d);
	
	// lister les fichiers partagés à l'utilisateur
	// forger le nouveau chemin pour acceder au fichier d'autorisations
	strcat(cheminUser,"/autorisations");

	// l'ouvrir
	FILE *f = fopen(cheminUser,"r");
	char partageur[50];		//chaine contenant la personne qui partage un fichier avec l'utilisateur
	char fichierPartage[50];	// chaine contenant le nom du fichier partagé

	memset(partageur,0,50);
	memset(fichierPartage,0,50);
	
	// parcourir le fichier d'autorisations (les fichiers partagés avec l'utilisateur
	while(fscanf(f,"%[^/]/%[^\n]\n",partageur,fichierPartage) != EOF) {
		// ajouter a la reponse le marquer "?" : indique que la suite est le nom 
		// de l'utilisateur qui partage du contenu avec l'utilisateur
		strcat(reponse,"?");

		// ajouter a la reponse le nom de l'utilisateur qui partage 
		strcat(reponse,partageur);

		// ajouter a la reponse le marquer "!" : indique que la suite est le nom
		// du fichier qui est partagé avec l'utilisateur 
		strcat(reponse,"!");

		// ajouter a la reponse le nom du fichier
		strcat(reponse,fichierPartage);

		// ajouter a la reponse le marqueur ":" : indique la fin d'un fichier partagé
		strcat(reponse,":");
	}

	// envoi de la réponse
	strcat(reponse,"\n");
	Emission(reponse);
     	}

	return 0;	
}

// auteur : Poussard Sébastien
// fonction qui supprimer un fichier du serveur
int supprimerFichier(char *donnee) {
	char command[50];		// chaine qui contient la commande qui sera executer sur le serveur
	memset(command,0,50);

	// forger la commande de suppression du fichier
	strcpy(command,"rm depot/");
	strcat(command,nomUser);
	strcat(command,"/");
	strcat(command,donnee);

	//verifier que le fichier existe
	DIR *d;			// pointeur vers un dossier
     	struct dirent *dir;	// structure dirent
	char cheminUser[50];	// chemin d'acces
	int ok = 0;		// ok 0 si le fichier n'existe pas 1 si le fichier à été trouvé

	// forger le chemin d'acces au fichiers
	memset(cheminUser,0,50);
	strcpy(cheminUser,"depot/");
	strcat(cheminUser,nomUser);

	// ouvrir le dossier 
     	d = opendir(cheminUser);
     	if (d) {
		// parcourir les fichiers dans le dossier
        	while ((dir = readdir(d)) != NULL){
			if (strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,"..") != 0 && strcmp(dir->d_name,"autorisations")){
				// si le fichier indiqué par l'utilisateur existe indiqué qu'il à été trouvé
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
		// si le fichier n'a pas été trouvé, envoyer un message d'erreur au client
		Emission("202\n");
	}
	return 0;
}

// auteur : Poussard Sébastien
// fonction qui modifier le nom d'un fichier du serveur
int renommerFichier(char *donnee) {
	// forger la commande de modificiation du nom du fichier
	char command[50];	// commande qui sera executé
	char fichierInit[50];		//chaine contenant le nom du fichier initial
	char fichierFin[50];		//chaine contenant le nom du fichier final

	memset(fichierInit,0,50);
	memset(fichierFin,0,50);
	memset(command,0,50);

	// forger la commande
	strcpy(command,"mv depot/");
	strcat(command,nomUser);
	strcat(command,"/");
	sscanf(donnee,"%[^ ] %[^ ]",fichierInit,fichierFin);
	strcat(command,fichierInit);
	strcat(command," depot/");
	strcat(command,nomUser);
	strcat(command,"/");
	strcat(command,fichierFin);

	//verifier que le fichier existe
	DIR *d;				//pointeur vers un dossier
     	struct dirent *dir;		// structure dirent
	char cheminUser[50];		// chemin d'acces
	int ok = 0;			// ok 0 si le fichier n'existe pas 1 si le fichier à été trouvé
		
	// forger le chemin
	memset(cheminUser,0,50);
	strcpy(cheminUser,"depot/");
	strcat(cheminUser,nomUser);
     	d = opendir(cheminUser);
	// ouvrir le dossier
     	if (d) {
        	while ((dir = readdir(d)) != NULL){
			// parcourir le contenu du dossier
			if (strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,"..") != 0 && strcmp(dir->d_name,"autorisations")){
				// si aucun fichier porte le nom alors valider la requete
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
		// sinon envoyer un message d'erreur au client
		Emission("202\n");
	}
	return 0;
}
// auteur : Poussard Sébastien
// fonction qui permet d'autoriser un utilisateur à telecharger un fichier
int addDroits(char *donnee) {
		
	char utilisateur[50];		// chaine contenant le nom d'utilisateur a autoriser
	char fichier[50];		// chaine contenant le nom du fichier à autoriser	

	memset(utilisateur,0,50);
	memset(fichier,0,50);

	// séparer utilisateur et nom de fichier de la requete
	sscanf(donnee,"%[^ ] %[^ ]",fichier,utilisateur);

	// verifier que le fichier existe
	DIR *d;                    // pointeur vers un dossier
	struct dirent *dir;        // structure dirent
	char cheminUser[MAX_PATH]; // chemin d'accés
	char cheminAuto[MAX_PATH]; // chemin d'accés
	int ok = 0;                // ok 0 si le fichier n'existe pas 1 si le fichier à été trouvé
	
	// forger la chemin
	memset(cheminUser,0,MAX_PATH);
	memset(cheminAuto,0,MAX_PATH);
	strcpy(cheminUser,"depot/");
	strcat(cheminUser,nomUser);
	// ouvrir le dossier
     	d = opendir(cheminUser);
     	if (d) {
		// parcourir les elements du dossier
        	while ((dir = readdir(d)) != NULL){
			if (strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,"..") != 0 && strcmp(dir->d_name,"autorisations")){
				// si le nom du fichier existe alors valider la requete
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
	// verifier si l'utilisateur à autoriser existe (si son dossier existe)
	memset(cheminUser,0,MAX_PATH);
	// forger le nouveau chemin
	sprintf(cheminUser,"depot/%s",utilisateur);
	if (opendir(cheminUser) == NULL) {
		// si l'utilisateur n'existe pas envoyer un message d'erreur
		Emission("202\n");
		return 1;
	}

	// autoriser l'utilisateur
	// ouvrir le fichier
	strcat(cheminUser,"/autorisations");
	FILE *f = fopen(cheminUser,"a+");

	// ecrire dans le fichier
	memset(cheminUser,0,MAX_PATH);

	sprintf(cheminUser,"%s/%s",nomUser,fichier);

	while(fscanf(f, "%s", cheminAuto) != EOF){
		if((strcmp(cheminUser, cheminAuto)) == 0){
			Emission("202 l'autorisation a ce fichier existe deja\n");
			return 0;
		}
	}
	
	strcat(cheminUser, "\n");
	fputs(cheminUser,f);
	fclose(f);

	// envoyer un message pour indiquer au client le succés de la requete
	Emission("007\n");
	return 0;
}
// Auteurs : Poussard Sébastien, Bredariol Romain
// fonction qui permet d'enlever les autorisation d'un fichier à un utilisateur
int delDroits(char *donnee) {
	// auteur : Poussard Sébastien	
	char utilisateur[50];		// chaine contenant le nom de l'utilisateur dont on doit supprimer les droits
	char fichier[50];		// chaine contenant le nom du fichier dont on doit retirer les droits
	char chemin[MAX_PATH];		// chemin d'accés
	char chemin_tmp[MAX_PATH];	// chemin d'accés temporaire
	char fileAuto[MAX_PATH];	// chaine recupere dans le fichier autorisation
	int ecode;			// error code

	memset(utilisateur,0,50);
	memset(fichier,0,50);
	memset(chemin,0,MAX_PATH);
	memset(chemin_tmp,0,MAX_PATH);
	memset(fileAuto,0,MAX_PATH);

	// séparer utilisateur et nom de fichier de la requete
	sscanf(donnee,"%[^ ] %[^ ]",utilisateur,fichier);

	// verifier que l'utilisateur existe sinon envoyer un message d'erreur

	// forger le chemin
	sprintf(chemin,"depot/%s",utilisateur);
	// si le dossier ne peut pas être ouvert, envoyer un message d'erreur au client
	if (opendir(chemin) == NULL) {
		Emission("202\n");
		return 1;
	}

	// auteur : Bredariol Romain
	//verifier que l'autorisation existe et suppression de celle-ci si elle existe
	// ouverture du fichier contenant les autorisations
	strcat(chemin,"/autorisations");
	FILE *f = fopen(chemin,"r");
	if (f == NULL) {
		Emission("202\n");
		return 1;
	}

	// creation d'un nouveau fichier
	sprintf(chemin_tmp, "%s.tmp", chemin);
	FILE *f_tmp = fopen(chemin_tmp,"w");
	if (f_tmp == NULL) {
		Emission("202\n");
		return 1;
	}

	//on formate les donnees a tester
	memset(donnee, 0, strlen(donnee));
	sprintf(donnee, "%s/%s", nomUser, fichier);

	//tant qu'il reste une ligne dans le fichier autorisation
	while(fscanf(f, "%s\n", fileAuto) != EOF){
		if((strcmp(fileAuto, donnee)) != 0){
			//si on trouve une ligne différente on l'ajoute au fichier temporaire 
			strcat(fileAuto, "\n");
			ecode = fputs(fileAuto, f_tmp);
			if(ecode == EOF){
				printf("Erreur ecriture de donnee dans autorisation.tmp\n");
				return -1;
			}
		}
	}

	fclose(f);
	fclose(f_tmp);

	//on renomme le fichier tmp
	rename(chemin_tmp, chemin);

	// envoyer un message pour pour indiquer le succés de la requete
	Emission("007\n");
	return 0;
}

// auteur : Poussard Sébastien
// cette fonction retourne au client l'état de l'environnement de l'utilisateur (nombre de fichiers,
// taille consommés, taille disponible ... 
int etat(char *donnee) {

	char reponse[1000];		// reponse envoyé au client
	int taille = placeDisponible();		// taille en kb disponible
	
	// compter le nombre de fichier dans le dossier de l'utilisateurs
	char cmd[MAX_PATH];	// commande a executer
	memset(cmd,0,MAX_PATH);

	// executer la commande pour obtenir la place en kb
	sprintf(cmd,"ls -l depot/%s | wc -l > tmp",nomUser);
	system(cmd);

	
	// ouvrir le fichier contenant le resultat de la commande
	FILE *f = fopen("tmp","r");
	
	// recuperer le nb de fichiers
	int nb;			// nombre de fichiers
	fscanf(f,"%d",&nb);
	nb = nb-2;		// corriger le nombre
	
	// supprimer le fichier temporaire
	system("rm tmp");
	
	// forger la reponse
	sprintf(reponse,"%d:%d\n",taille,nb);

	// envoyer la reponse au client
	Emission(reponse);
	return 0;
}

// auteur : Poussard Sébastien
// cette fonction retourne la place disponible en kb dans l'environnement de l'utilisateur
// pour le stockage de ses fichiers
int placeDisponible() {

	char cmd[MAX_PATH];	// commande a executer
	memset(cmd,0,MAX_PATH);

	// executer la commande pour obtenir la place en kb
	sprintf(cmd,"du depot/%s | cut -c1 > tmp",nomUser);
	system(cmd);

	
	// ouvrir le fichier contenant le resultat de la commande
	FILE *f = fopen("tmp","r");
	
	// recuperer la chaine contenant la taille 
	
	int taille;
	fscanf(f,"%d",&taille);
	
	// supprimer le fichier temporaire
	system("rm tmp");
	
	// renvoyer la taille en kb disponible
	// par defaut l'utilisateur à 100Mo de place disponible
	return (100000-taille);
}
