#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "client.h"
#include "lib.h"
#include <unistd.h>

// auteur : Bredariol Romain
// televerser permet d'envoyer un fichier sur le serveur
int televerser();

// auteur : Bredariol Romain
// telecharger permet de télécharger un fichier depuis le serveur
int telecharger();

// auteur : Poussard Sébastien
// autorisations afficher à l'utilisateur un menu de choix entre autoriser un utilisateur
// à telecharger un fichier chez lui et revoquer un utilisateur de ces droits de telechargement 
// sur un de ses fichiers
int autorisations();

// auteur : Poussard Sébastien
// etat permet de d'afficher l'état de l'environnement (dossier) de l'utilisateur
int etat();

// auteur : Poussard Sébastien
// gererFichiers  permets d'affichr un menu pour supprimer ou renommer un fichier de l'utilisateur 
int gererFichiers();

// auteur : Poussard Sébastien
// listeFichiers permet de lister les fichiers telechargeable (ceux de l'utilisateur et ceux partagés)
int listeFichiers();

// auteurs : Poussard Sébastien, Bredariol Romain
// gestionComptes affiche le menu pour que l'administrateur choisisse netre 
// ajouter un utilisateur, supprimer un utilisateur ou modifier le compte administrateur
int gestionComptes();

// auteur : Bredariol Romain
//addUser permet de saisir un nouvel utilisateur
int addUser();

// auteur : Bredariol Romain
//delUser permet de supprimer un utilisateur
int delUser();

// auteur : Bredariol Romain
//editSu permet de modifier les login et mdp du Su
int editSu();

// auteur : Bredariol Romain
//lecture_login_mdp permet de recuperer le login et le mdp dans le buffer stdin
int lecture_login_mdp(char *login, char *mdp);

// auteur : Poussard Sébastien
//lire permet de recuperer une chaine de caractere dans le buffer stdin
int lire(char *chaine, int size);

// auteur : Poussard Sébastien
// fonction qui permet a l'utilisateur de supprimer une fichier de son environnement
int supprimerFichier();

// auteur : Poussard Sébastien
// fonction qui permet à l'utilisateur de renomer un fichier de son environnement
int renommerFichier();

// auteur : Poussard Sébastien
// autoriser un utilisateur à pouvoir telecharger un de nos fichiers
int addDroits();

// auteur : Poussard Sébastien
// revoquer les droits d'un utilisateur à telecharger un fichier
int delDroits();

