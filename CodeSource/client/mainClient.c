#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "client.h"

int main() {
	char *message;		// un message echange entre le client et le serveur
	char ip[16];		// ip de connexion au serveur
	char port[6];		// port de connexion au serveur
	char c;			// char pour vider le stdin


	//connexion a un serveur lecture de l'ip et du port, instancie des valeurs par defaut si rien n'est précisé
	printf("Bienvneue sur le client SPF\n\n");
	printf("Veuillez indiquer l'ip du serveur (appuyer sur entree pour localhost par defaut)\n");
	fgets(ip, 15, stdin);
	if (strcmp(ip, "\n") == 0) { 
		strcpy(ip,"localhost");
	} else {
		while ((c = getchar()) != '\n' && c != EOF) { }
	}
	printf("Veuillez indiquer le port (appuyer sur entree pour 1337 par defaut\n");
	fgets(port, 5, stdin);
	if (strcmp(port, "\n") == 0 ) {
		strcpy(port,"1337");
	} else {
		while ((c = getchar()) != '\n' && c != EOF) { }
	}
	printf("\n\nConnexion à %s:%s...\n\n",ip,port);

	//demande une connexion sur l'adresse localhost
	if(Initialisation("localhost") != 1) {
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
				choix_menu();
			}

			//si le serveur confirme l'authentification du Su
			if(strcmp(message, "005")==0){
				printf("Bienvenue Su\n");
				afficher_menu(5);
				choix_menu();
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
