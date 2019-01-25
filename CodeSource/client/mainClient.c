#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "client.h"

int main() {
	char *message;		// un message echange entre le client et le serveur
	char ip[16];		// ip de connexion au serveur
	char port[7];		// port de connexion au serveur
	char c;			// char pour vider le stdin


	// message de bienvenue
	printf("Bienvenue sur le client SPF\n\n");
	// demande de l'ip
	printf("Veuillez indiquer l'ip du serveur (appuyer sur entree pour localhost par defaut)\n");
	fgets(ip, 15, stdin);
	// si aucune ip n'est donné, utilier la valeur par defaut
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

	//tant qu'il recoit des messages du serveur
	while((message = Reception()) != NULL ){
		if(message != NULL) {
			printf("%s\n", message);
			//recup l'identifiant de requete
			sscanf(message , "%[^ ]", message);

			//si le serveur demande l'authentification
			if(strcmp(message, "002")==0){
				authentification();
			}

			//si le serveur confirme l'authentification d'un utilisateur
			if(strcmp(message, "004")==0){
				printf("Bienvenue\n");
				afficher_menu(4);
				choix_menu(4);
			}

			//si le serveur confirme l'authentification du Su
			if(strcmp(message, "005")==0){
				printf("Bienvenue Su\n");
				afficher_menu(5);
				choix_menu(5);
			}

			free(message);
		} else {
			printf("Erreur de reception\n");
			return 1;
		}
	}

	Terminaison();

	return 0;
}
