extern char nomUser[50]; // nom de l'utilisateur accessible partout

// connexion permet d'ouvrir une connexion avec le serveur, elle va demander l'ip et le port 
// à l'utilisateur, si celui de rentre rien, une IP par defaut sera utilisé (localhost) et un
// port par defaut sera utilisé (1337)
// renvoie 0 en cas de succés
int connexion();

// authentification permet à l'utilisateur de s'authentifier
int authentification();

//affiche le menu en fonction du statut client
int afficher_menu(int user);

//recupere le choix du menu de l'utilisateur
int choix_menu(int user);

//permet de lire la reponse envoyé par le serveur
int lireReponse();

//lit le contenu d'un fichier et le renvoie 
int lireContenuFichier(char *nomFichier, char *contenu, int taille);

/*Renvoie la taille du fichier demandé*/
unsigned long longueur_fichier(char *nomFichier);

//permet d'ecrire dans un fichier
int ecrireContenuFichier(char *nomFichier, char *contenu, int size);
