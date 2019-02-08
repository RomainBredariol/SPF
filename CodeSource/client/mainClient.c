#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "client.h"
#include "lib.h"

// codes couleurs pour printf
#define ROUGE "\x1b[31m"
#define VERT "\x1b[32m"
#define JAUNE "\x1b[33m"
#define BLEU "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"


int main() {
//	char *message;		// un message echange entre le client et le serveur
	int codeReponse;	// code de la reponse envoyé par le serveur	
	int compteurConnexion = 3;	// 3 tentatives de connexions possible
	int privileges;		// si utilisateur normal = 4 si admin = 5
	int continuer; 		// si l'utilisateur souhaite continuer dans le programme

	// etablir une connexion avec le serveur interactivement avec l'utilisateur
	codeReponse = connexion();
	if (codeReponse != 1) {
		printf("erreur a la connexion \n");
		return -1;
	}
		// rq : si serveur repond pas erreur 103, implementer ??

	// authentification
	codeReponse = lireReponse();
	if (codeReponse != 2) {
		printf("delais d'attente de demande d'authentificaiton depassée\n");
	}
	do {
		codeReponse = authentification();
		compteurConnexion--;
		if (codeReponse == 104 || codeReponse == 105) {
			printf(JAUNE"\nLogin / mdp incorrect \n"RESET);
		}
	} while (compteurConnexion != 0 && (codeReponse == 105 || codeReponse == 104));
	if (codeReponse == 105 || codeReponse == 104) {
		printf("echec d'authentification au bout de 3 essais \n");
		return -1;
	}
	privileges = codeReponse;
	// souhaiter la bienvenue (nous ne sommes pas des malotrues)
	printf("\n\n"JAUNE" Bienvenue "RESET);
	if (privileges == 5) {
	printf(CYAN" super utilisateur"RESET);
	}
	printf("\n");

	// tant que l'utilisateur ne souhaite pas quitter l'application
	// afficher le menu et traiter le choix qu'il fait
	do {
	afficher_menu(privileges);

	continuer = choix_menu(privileges);
	
	//attente de 3 sec pour afficher d'eventuels messages a l'utilisateur
	sleep(3);
	system("clear");
	} while (continuer != 1);
		
	// terminer la connexion
	printf(VERT"\n\n\n On se quitte parfois pour mieux se retrouver ensuite.\n\n\n\n"RESET);
	Terminaison();
	return 0;
}
