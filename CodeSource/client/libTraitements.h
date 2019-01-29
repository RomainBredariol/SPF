#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "client.h"
#include "lib.h"
#include <unistd.h>

// televerser permet d'envoyer un fichier sur le serveur
int televerser();

// telecharger permet de télécharger un fichier depuis le serveur
int telecharger();

// autorisations permet de ?????
int autorisations();

// etat permet de ???? 
int etat();

// gererFichiers  permets de ???? 
int gererFichiers();

// listeFichiers
int listeFichiers();

// gestionComptes permet de ????
int gestionComptes();


//addUser permet de saisir un nouvel utilisateur
int addUser();

//delUser permet de supprimer un utilisateur
int delUser();

//editSu permet de modifier les login et mdp du Su
int editSu();

//lecture_login_mdp permet de recuperer le login et le mdp dans le buffer stdin
int lecture_login_mdp(char *login, char *mdp);

//lire permet de recuperer une chaine de caractere dans le buffer stdin
int lire(char *chaine, int size);
