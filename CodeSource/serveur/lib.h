extern char nomUser[100]; // nom de l'utilisateur accessible partout
extern struct user su;	  // login administrateur

// auteur : Bredariol Romain
// is_user vérifie sir l'utilisateur existe et si son 
// mot de passe est correcte
int is_user(char * login_mdp);

// auteur : Bredariol Romain
//edition du super utilisateur
int editSu(char *donnee);

// auteur : Poussard Sébastien
// fonction qui lance le serveur
// demande à l'utilisateur le port qu'il veut utiliser, sinon utilise 1337 par defaut
// crée toute les fichier necessaire au fonctionnement du serveur s'il ne sont pas présent
int lancerServeur();

// auteur : Bredariol Romain
//Demande l'authentification au client
int authentification();

// auteur : Bredariol Romain
//permet d'ecrire dans un fichier
int ecrireContenuFichier(char* nomFichier, char *contenu, int size);

// auteur : Bredariol Romain
//extraitNomFichier permet de d'extraire le nom d'un fichier d'un chemin absolue
char* extraitNomFichier(char *fileName);

// auteur : Bredariol Romain
//on ecrit le nom du fichier et le nom de l'utilisateur a qui il appartient 
//dans le fichier liste qui recense l'ensemble des fichiers du serveur
int ajouterFichierListe(char* nomFichier);

//lit le contenu d'un ficher
int lireContenuFichier(char *nomFichier, char *contenu, int taille);

// auteur : Bredariol Romain
//on supprime de la liste tous les fichiers qui appartiennent a un utilisateur
int supprimerFichierListe(char* userName);

// auteur : Poussard Sébastien
// cette fonction récupere la requete du client et execute en fonction du code
// utilisé la fonction adéquat
int executerRequete(char * requete);

