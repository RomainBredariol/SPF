#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "serveur.h"

int main() {
	char *message = NULL;
	int ecode;

	//ouverture du serveur
	Initialisation();

	while(1) {
		int fini = 0;

		//attend qu'un client se connecte
		AttenteClient();
		//confirme la connexion au client
		Emission("001 connexion etablie\n");

		//demande l'authentification du client
		if((ecode = authentification()) != 0){
			while(!fini){
				//message recu du client
				message = Reception();
				printf("%s\n", message);
				
				if(message != NULL){
					//execute la demande du client
					executerRequete(message);
				}else{
					fini = 1;
				}
			}
		}else{
			printf("Erreur d'authentification\n");
		}

		//on ferme la connection
		TerminaisonClient();
	}

	return 0;
}
