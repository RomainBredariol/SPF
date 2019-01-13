#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
		FILE * user_list = fopen("user_list", "r");
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




