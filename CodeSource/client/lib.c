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

char nomUser[50];

// auteur : Poussard Sébastien
// connexion permet d'ouvrir une connexion avec le serveur, elle va demander l'ip et le port 
// à l'utilisateur, si celui de rentre rien, une IP par defaut sera utilisé (localhost) et un
// port par defaut sera utilisé (1337)
// renvoie 0 en cas de succés
int connexion() {
	char ip[16];		// ip de connexion au serveur
	char port[7];		// port de connexion au serveur
	char c;			// char pour vider le stdin
	int rep;		// retour de lireReponse()

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
	rep = lireReponse();
	return rep;
}

// auteur : Poussard Sébastien, Bredariol Romain
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

// auteur : Poussard Sebastien
// authentification permet à l'utilisateur de s'authentifier
int authentification(){
	char login[100];	// chaine utilisé pour le login
	char mdp[100];		// chaine utilisé pour le mot de passe
	char donnee[210]; 	//donnee sera la chaine formate envoye au serveur

	// initialisation des login et mdp
	memset(login,0,100);
	memset(mdp,0,100);

	// lecture des identifiants de l'utilisateur
	printf("Veuillez vous identifier\n");

	lecture_login_mdp(login, mdp);
	memset(nomUser,0,50);
	strcpy(nomUser,login);


	//formatage de la chaine avec le bon id de requete
	sprintf(donnee, "003 %s %s\n", login, mdp);

	//envoie de la chaine au serveur
	Emission(donnee);
	return lireReponse();
}

// auteur : Bredariol Romain
//affiche le menu en fonction du statut client (s'il est admin ou non)
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

// auteur : Poussard Sébastien
// execute le choix de l'utilisateur
// renvoie 1 si l'utilisateur choisit de quitter l'application
int choix_menu(int user){
	char choixChar[3];		// choix sous forme de chaine
	int choix;			// choix transformé en int
	char c; 			// char pour vider le stdin

	// lire le choix de l'utilisateur
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

	// executer le traitement en fonction du choix de l'utilisateur
	switch(choix) {
		case 0: 
			return 1;
		case 1:
			// televerser un fichier
			televerser();
			break;
		case 2:
			// telecharger un fichier
			telecharger();
			break;
		case 3:
			// acceder au menu d'autorisations (autoriser ou revoquer les autorisations
			// d'un  utilisateur a telecharger un fichier
			autorisations();
			break;
		case 4: 
			// acceder a l'état de l'environnement de l'utilisateur
			etat();
			break;
		case 5: 
			// acceder au menu pour modifier ou supprimer des fichiers
			gererFichiers();
			break;
		case 6:		
			// obtenir la liste des fichier telechargeable
			listeFichiers();
			break;
		case 7:
			// si l'utilisateur est un administrateur, acceder a la gestion des comptes
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

// auteur : Bredariol Romain
// Renvoie la taille du fichier demandé
unsigned long longueur_fichier(char *nomFichier){
	unsigned int taille;

	//ouverture du fichier demandé
	FILE *fichier = fopen(nomFichier, "r");

	//si l'ouverture échoue
	if(fichier == NULL){
		printf("Erreur fopen fichier\n");
		return -1;
	}

	//place le pointeur fichier en fin de fichier
	taille = fseek(fichier, 0, SEEK_END);

	//si le fseek échoue
	if(taille != 0){
		printf("Erreur fseek\n");
		return -1;
	}else{
		//renvoie la position en octets, ici la fin du fichier donc sa taille
		return ftell(fichier);
	}
}

// auteur : Bredariol Romain
//lit le contenu d'un fichier et le renvoie 
int lireContenuFichier(char *nomFichier, char *contenu, int taille){
	/*      contenu est le contenu du fichier binaire à envoyer au client
	 *                      taille est la longueur du fichier demande*/
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

// auteur : Bredariol Romain
//permet d'ecrire dans un fichier
int ecrireContenuFichier(char *nomFichier, char *contenu, int size){
	FILE *fichier = fopen(nomFichier, "w");
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

	printf("le fichier %s a bien ete telecharge\n", nomFichier);
	return 0;
}


