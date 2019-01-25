#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "client.h"
#include "lib.h"
#include <unistd.h>
#include "libTraitements.h"

// codes couleurs pour printf
#define ROUGE "\x1b[31m"
#define VERT "\x1b[32m"
#define JAUNE "\x1b[33m"
#define BLEU "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"

// connexion permet d'ouvrir une connexion avec le serveur, elle va demander l'ip et le port 
// à l'utilisateur, si celui de rentre rien, une IP par defaut sera utilisé (localhost) et un
// port par defaut sera utilisé (1337)
// renvoie 0 en cas de succés
int connexion() {
	char ip[16];		// ip de connexion au serveur
	char port[7];		// port de connexion au serveur
	char c;			// char pour vider le stdin
	int nb;

	// message de bienvenue
	printf("Bienvenue sur le client SPF\n\n");

	// demande de l'ip
	printf("Veuillez indiquer l'ip du serveur (appuyer sur entree pour localhost par defaut)\n");
	fgets(ip, 15, stdin);

	// si aucune ip n'est donné, utiliser la valeur par defaut
	if (strcmp(ip, "\n") == 0) { 
		strcpy(ip,"localhost");
	} else {
		// vider le stdin si l'utilisateur depasse
		if (ip[15] != '\0') {
			while ((c = getchar()) != '\n' && c != EOF) { }
		}

	//supprimer le new line
	ip[strlen(ip)-1] = '\0';
	}

	// demander le port
	printf("Veuillez indiquer le port (appuyer sur entree pour 1337 par defaut)\n");
	fgets(port, 7, stdin);

	// si aucune port n'est donné, utiliser le port par défaut
	if (strcmp(port, "\n") == 0 ) {
		strcpy(port,"1337");
	} else {

		// vider le stdin si l'utilisateur depasse
		if (port[6] != '\0') {
			while ((c = getchar()) != '\n' && c != EOF) { }
		}

	//supprimer le new line
	port[strlen(port)-1] = '\0';

	}

	// message pour indiquer la connexion en cours
	printf("\n\nConnexion à %s:%s...\n\n",ip,port);

	//demande une connexion sur l'adresse localhost
	if(InitialisationAvecService(ip,port) != 1) {
		printf("Erreur d'initialisation\n");
		return 1;
	}
	nb = lireReponse();
	return nb;
}

// fonction qui lie la reponse donné par le serveur. 
// renvoie un integer qui correspond au code message
int lireReponse() {
	char *message;		// un message echange entre le client et le serveur
	
	//boucle d'attente de lecture d'un message du serveur
	message = Reception();	
	if(message != NULL) {
		// afficher les message en rouge pour info
		printf(ROUGE"message recue du serveur : %s"RESET"\n", message);
		//recup l'identifiant de requete
		sscanf(message , "%[^ ]", message);
	}
	return strtol(message, NULL, 10);
}

// authentification permet à l'utilisateur de s'authentifier
int authentification(){
	char login[100];	// chaine utilisé pour le login
	char mdp[100];		// chaine utilisé pour le mot de passe
	char donnee[210]; 		//donnee sera la chaine formate envoye au serveur
	char c; 		// char pour vider le buffer

	// initialisation des login et mdp
	memset(login,0,100);
	memset(mdp,0,100);

	// lecture des identifiants de l'utilisateur
	printf("Veuillez vous identifier\n");
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
	fgets(mdp, 99, stdin);

	// vider le stdin si l'utilisateur depasse
	if (mdp[99] != '\0') {
		while ((c = getchar()) != '\n' && c != EOF) { }
	} else {
		//sinon supprimer le new line
		mdp[strlen(mdp)-1] = '\0';
	} 

	//formatage de la chaine avec le bon id de requete
	sprintf(donnee, "003 %s %s\n", login, mdp);

	//envoie de la chaine au serveur
	Emission(donnee);
	lireReponse();
	return 0;
}

//affiche le menu en fonction du statut client
int afficher_menu(int user){
	printf("========= Menu =========\n");
	printf("1 - Televerser\n");
	printf("2 - Telecharger\n");
	printf("3 - Autorisation users\n");
	printf("4 - Etat\n");
	printf("5 - Gerer fichiers\n");
	printf("6 - Liste fichiers telechargeable\n");
	
	//Si l'utilisateur est l'admin
	if(user == 5){
		printf("7 - Gestion des comptes\n");
	}
	printf("0 - Quitter\n\n");
	return 0;
}

// execute le choix de l'utilisateur
// renvoie 1 si l'utilisateur choisit de quitter l'application
int choix_menu(int user){
	char choixChar[3];		// choix sous forme de chaine
	int choix;			// choix transformé en int
	char *requete, *login, *mdp;
	char *donnee;
	char c; 			// char pour vider le stdin

	printf("Veuillez entre le numero de l'option souhaitee :\n");
	fgets(choixChar, 3, stdin);

	// vider le stdin si l'utilisateur depasse
	if (choixChar[2] != '\0') {
		while ((c = getchar()) != '\n' && c != EOF) { }
	} else {
		//sinon supprimer le new line
		choixChar[1] = '\0';
	}
	// convertir le choixChar en choix (int)
	choix = strtol(choixChar, NULL, 10);
	printf("utilisateur choisi : %d \n",choix);

	// executer le traitement en fonction du choix de l'utilisateur
	switch(choix) {
		case 0: 
			return 1;
		case 1:
			televerser();
			break;
		case 2:
			telecharger();
			break;
		case 3:
			autorisations();
			break;
		case 4: 
			etat();
			break;
		case 5: 
			gererFichiers();
			break;
		case 6:		
			listeFichiers();
			break;
		case 7:
			if(user == 5) {
				gestionComptes();
			} else {
				printf("impossible d'envoyer cette requete vous n'y etes pas autorise\n");
			}
			break;
		default:
			break;
	}
	return 0;
}
	/*

	refactorer ce code dans les fonctions dediées			


	if(user == 4){
		strcpy(requete, "204 ");
		if(strcmp(choix, "7")==0){
			return -1;
		}

		if(strcmp(choix,"0")==0){
			return 0;
		}
		return 1; 
	}else{
		strcpy(requete, "205 ");

		if(strcmp(choix,"0")==0){
			return 0;
		}

		if(strcmp(choix, "7")==0){
			strcat(requete, "7.");

			printf("=========================================\n");
			printf("Veuillez entre le numero de l'option souhaitee :\n");
			printf("1 - Ajouter un utilisateur\n");
			printf("2 - Supprimer un utilisateur\n");
			printf("3 - Modifier les infos super utilisateur\n");

			scanf("%s", choix);
			if(strcmp(choix, "1")==0){
				printf("Veuillez saisir le login et mdp de l'utilisateur a ajouter\n");
				printf("login :\n");
				scanf("%s", login); 
				printf("password :\n");
				scanf("%s", mdp);
				
				sprintf(donnee," %s %s", login, mdp);
			}
			return 1;
		}

	}
	
	
	strcat(requete, choix);
	strcat(requete, donnee);
	strcat(requete, "\n");

	Emission(requete);
	return 1;
}
*/

