// auteurs : Poussard Sébastien, Bredariol Romain
//delUser permet de supprimer un utilisateur
// supprimer le dossier (son environnemnt) mais aussi le supprime de
// la liste des utilisateurs (son login/ mot de passe)
int delUser(char *donnee);

// auteurs : Poussard Sébastien, Bredariol Romain
//ajoute un utilisateur a la liste et créer son environemnt (dossier contenant ses fichiers +
// fichier d'autorisations
int addUser(char *donnee);

// auteur : Romain Bredariol
//un client veut televerser un fichier 
int televerser(char *donnee);

// auteur : Poussard Sébastien
//permet de lister les fichiers telechargeble
int lister();

// auteur : Poussard Sébastien
// fonction qui supprimer un fichier du serveur
int supprimerFichier(char *donnee);

// auteur : Poussard Sébastien
// fonction qui modifier le nom d'un fichier du serveur
int renommerFichier(char *donnee);

// auteur : Poussard Sébastien
// fonction qui permet d'autoriser un utilisateur à telecharger un fichier
int addDroits(char *donnee);

// auteur : Bredariol Romain
//permet d'envoyer un fichier a utilisateur
int telecharger(char *donnee);

// Auteurs : Poussard Sébastien, Bredariol Romain
// fonction qui permet d'enlever les autorisation d'un fichier à un utilisateur
int delDroits(char *donnee);

