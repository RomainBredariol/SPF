#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "serveur.h"
#include "lib.h"

int main() {

	char *message = NULL;
	int ecode;

	//lancer le serveur en demandant le port
	lancerServeur();


	while(1) {
		int fini = 0;

		//attend qu'un client se connecte
		AttenteClient();
		//confirme la connexion au client
		Emission("001 connexion etablie\n");

		//demande l'authentification du client
		// se repete tant que le code d'authentifiation est mauvais
		Emission("002 demande d'authentification\n");
		do {
			ecode = authentification();
		} while (ecode == 0);
		
		// executer les requetes reçues
		while(fini != 1){
			//message recu du client
			message = Reception();
			printf("j'ai recu : %s\n", message);
			
			if(message != NULL){
				//execute la demande du client
				executerRequete(message);
			}else{
				fini = 1;
			}
		}
		//on ferme la connection
		TerminaisonClient();
	}

	return 0;
}
