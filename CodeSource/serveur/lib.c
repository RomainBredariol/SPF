#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "serveur.h"

//definit la structure d'un utilisateur 
struct user{
	char login [100];
	char mdp[100];
};

//le super utilisateur
struct user su = {"admin", "AdM!n"};

//verifie si l'utilisateur existe
int is_user(char * login_mdp){
	//login et mdp sont ceux du client qui essaye de se connecter
	//login_test et mdp_test sont ceux recupere 
	//aupres du fichier de log "user_list"
	char login[100], login_test[100];
	char mdp[100], mdp_test[100];

	//decoupe le login et mdp
	sscanf(login_mdp,"%s %s", login, mdp);

	//teste le login et mdp par rapport au Su
	if(strcmp(su.login, login)==0){
		if(strcmp(su.mdp, mdp)==0){
			//confirme que le su est identifie
			return 5;
		}else{
			printf("Mauvais mdp\n");
			return 0;
		}
	}else{
		//teste le login et mdp par rapport a un utilisateur lambda
		FILE * user_list = fopen("users", "r");
		if(user_list == NULL){
			printf("Erreur lecture du ficher user\n");
			return -1;
		}

		while(fscanf(user_list, "%s %s",login_test, mdp_test) != EOF){ 

			if(strcmp(login_test, login)==0){
				if(strcmp(login_test, login)==0){
					//confirme qu'un utilisateur est identifie
					return 4;
				}else{
					printf("Mauvais mdp\n");
				}
			}
		}
	}
	
	return 0;
}

//ajoute un utilisateur a la liste
int addUser(char *donnee){
	int ecode; //error code

	FILE * user_list = fopen("users", "a");

	if(user_list == NULL){
		printf("Erreur fopen users\n");
		return -1;
	}

	strcat(donnee, "\n");
	
	ecode = fprintf(user_list, "%s", donnee);
	if(ecode < 0){
		printf("Erreur ecriture de donnee\n");
		return -1;
	}
	
	fclose(user_list);

	Emission("L'utilisateur a ete cree\n");
	return 0;
}

// fonction qui lance le serveur en demandant le port d'ouverture 
// à l'utilisateur
int lancerServeur() {
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
	return 0;
}

//delUser permet de supprimer un utilisateur
int delUser(char *donnee){
	char login_mdp[203];

	strcat(donnee, "\n");

	FILE * user_list = fopen("users", "r");
	FILE * user_list_tmp = fopen("users.tmp", "w");

	if(user_list == NULL){
		printf("Erreur fopen users\n");
		return -1;
	}

	if(user_list_tmp == NULL){
		printf("Erreur fopen users.tmp\n");
		return -1;
	}

	while(fgets(login_mdp, sizeof(login_mdp), user_list)){
		if(strcmp(login_mdp, donnee)==0){
			continue;
		}else{
			fputs(login_mdp, user_list_tmp);
		}
	}
	
	fclose(user_list);
	fclose(user_list_tmp);

	rename("users.tmp", "users");

	Emission("L'utilisateur a ete supprime\n");
	return 0;
}
