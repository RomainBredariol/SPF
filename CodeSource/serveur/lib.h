extern char nomUser[100]; // nom de l'utilisateur accessible partout

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

//extraitNomFichier permet de d'extraire le nom d'un fichier d'un chemin absolue
char* extraitNomFichier(char *fileName);


//on ecrit le nom du fichier et le nom de l'utilisateur a qui il appartient 
//dans le fichier liste qui recense l'ensemble des fichiers du serveur
int ajouterFichierListe(char* nomFichier);

// fonction qui supprimer un fichier du serveur
int supprimerFichier(char *donnee);
	
// fonction qui modifier le nom d'un fichier du serveur
int renommerFichier(char *donnee);

