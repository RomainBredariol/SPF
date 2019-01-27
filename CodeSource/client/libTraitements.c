#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "client.h"
#include "lib.h"
#include "libTraitements.h"
#include <unistd.h>

// televerser permet d'envoyer un fichier sur le serveur
int televerser() {
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
	return 0;
}

// listeFichiers
int listeFichiers() {
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
	//scanf("%s", choix);
	
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
	char c; 			// char pour vider le stdin
	char donnee[300];

	// lecture des identifiants de l'utilisateur a jouter
	printf("Veuillez saisir le login et mdp de l'utilisateur a ajouter\n");
	printf("login :\n");
	
	//recuperation du login
	fgets(login, 100, stdin);
	
	// vider le stdin si l'utilisateur depasse
	if (login[99] != '\0') {
		while ((c = getchar()) != '\n' && c != EOF) { }
	} else {
		//sinon supprimer le new line
		login[strlen(login)-1] = '\0';
	}
	
	printf("password :\n");
	//recuperation du mdp
	fgets(mdp, 100, stdin);

	// vider le stdin si l'utilisateur depasse
	if (mdp[99] != '\0') {
		while ((c = getchar()) != '\n' && c != EOF) { }
	} else {
		//sinon supprimer le new line
		mdp[strlen(mdp)-1] = '\0';
	}

	sprintf(donnee,"205 7.1 %s %s\n", login, mdp);

	Emission(donnee);
	return 0;
}

//delUser permet de supprimer un utilisateur
int delUser(){
	char login[100];	// chaine utilisé pour le login
	char mdp[100];		// chaine utilisé pour le mot de passe
	char c; 			// char pour vider le stdin
	char donnee[300];

	// lecture des identifiants de l'utilisateur a jouter
	printf("Veuillez saisir le login et le password l'utilisateur a supprimer\n");
	printf("login :\n");
	
	//recuperation du login
	fgets(login, 100, stdin);
	
	// vider le stdin si l'utilisateur depasse
	if (login[99] != '\0') {
		while ((c = getchar()) != '\n' && c != EOF) { }
	} else {
		//sinon supprimer le new line
		login[strlen(login)-1] = '\0';
	}

	printf("password :\n");
	//recuperation du mdp
	fgets(mdp, 100, stdin);

	// vider le stdin si l'utilisateur depasse
	if (mdp[99] != '\0') {
		while ((c = getchar()) != '\n' && c != EOF) { }
	} else {
		//sinon supprimer le new line
		mdp[strlen(mdp)-1] = '\0';
	}

	sprintf(donnee,"205 7.2 %s %s\n", login, mdp);

	Emission(donnee);
	return 0;
}

//editSu permet de modifier les login et mdp du Su
int editSu(){
	return 0;
}
