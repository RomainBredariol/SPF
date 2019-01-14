#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "client.h"

int main() {
	char *message;
	
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
