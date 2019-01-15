#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "serveur.h"
#include "lib.h"

int main() {
	char *message = NULL;
	int ecode;
	char port[7];			// port sur lequel le serveur ecoute les requetes
	char c;				// char pour vider le stdin

	//demande sur quel port ouvrir le serveur
	printf("Sur quel port voulez vous que le serveur ecoute ? (appuyer sur entree pour utiliser le port par defaut : 1337)\n");
	fgets(port, 7, stdin);
	//si aucun port n'est donner utiliser le port par defaut : 1337
	if (strcmp(port, "\n") == 0) { 
		strcpy(port,"1337");
	
	} else {
		// sinon, si l'utilisateur depasse la taille du buffer, nettoyer stdin
		if (port[6] != '\0') {
			while ((c = getchar()) != '\n' && c != EOF) { }
		}
	// enlever le new line a la fin de la chaine de caractere
	port[strlen(port)-1] = '\0';
	} 
	// initialiser le serveur sur le port
	InitialisationAvecService(port);
	while(1) {
		int fini = 0;

		//attend qu'un client se connecte
		AttenteClient();
		//confirme la connexion au client
		Emission("001 connexion etablie\n");

		//demande l'authentification du client
		if((ecode = authentification()) != 0){
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
		}else{
			printf("Erreur d'authentification\n");
		}

		//on ferme la connection
		TerminaisonClient();
	}

	return 0;
}
