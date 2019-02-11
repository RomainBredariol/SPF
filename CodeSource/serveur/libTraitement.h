//delUser permet de supprimer un utilisateur
int delUser(char *donnee);

//ajoute un utilisateur a la liste
int addUser(char *donnee);

//un client veut televerser un fichier 
int televerser(char *donnee);

//permet de lister les fichiers telechargeble
int lister();

// fonction qui supprimer un fichier du serveur
int supprimerFichier(char *donnee);

// fonction qui modifier le nom d'un fichier du serveur
int renommerFichier(char *donnee);

// fonction qui permet d'autoriser un utilisateur à telecharger un fichier
int addDroits(char *donnee);

//permet d'envoyer un fichier a un client
int telecharger(char *donnee);
