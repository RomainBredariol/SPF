extern char nomUser[50]; // nom de l'utilisateur accessible partout

// auteur : Poussard Sébastien
// connexion permet d'ouvrir une connexion avec le serveur, elle va demander l'ip et le port 
// à l'utilisateur, si celui de rentre rien, une IP par defaut sera utilisé (localhost) et un
// port par defaut sera utilisé (1337)
// renvoie 0 en cas de succés
int connexion();

// auteur : Poussard Sebastien
// authentification permet à l'utilisateur de s'authentifier
int authentification();

// auteur : Bredariol Romain
//affiche le menu en fonction du statut client (s'il est admin ou non)
int afficher_menu(int user);

// auteur : Poussard Sébastien
// execute le choix de l'utilisateur
// renvoie 1 si l'utilisateur choisit de quitter l'application
int choix_menu(int user);

// auteur : Poussard Sébastien, Bredariol Romain
// fonction qui lie la reponse donné par le serveur. 
// renvoie un integer qui correspond au code message
int lireReponse();

// auteur : Bredariol Romain
//lit le contenu d'un fichier et le renvoie 
int lireContenuFichier(char *nomFichier, char *contenu, int taille);

// auteur : Bredariol Romain
// Renvoie la taille du fichier demandé
unsigned long longueur_fichier(char *nomFichier);

// auteur : Bredariol Romain
//permet d'ecrire dans un fichier
int ecrireContenuFichier(char *nomFichier, char *contenu, int size);
