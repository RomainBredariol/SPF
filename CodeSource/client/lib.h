

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