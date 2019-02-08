// is_user vérifie sir l'utilisateur existe et si son 
// mot de passe est correcte
int is_user(char * login_mdp);

//ajoute un utilisateur a la liste
int addUser(char *donnee);

// cette fonction ouvre les socket sur le port indiqué par l'utilisateur
// si aucun port n'est spécifié, la fonction ouvre sur le port par defaut
// ( port 1337 )
int lancerServeur();

//supprime un utilisateur 
int delUser();

//edition du super utilisateur
int editSu(char *donnee);

/*Demande l'authentification au client*/
int authentification();

//un client veut televerser un fichier 
int televerser(char *donnee);

//permet d'ecrire dans un fichier
int ecrireContenuFichier(char* nomFichier, char *contenu, int size);

//permet de lister les fichiers telechargeble
int lister();
