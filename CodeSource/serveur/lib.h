// is_user vérifie sir l'utilisateur existe et si son 
// mot de passe est correcte
int is_user(char * login_mdp);

//ajoute un utilisateur a la liste
int addUser(char *donnee);

// cette fonction ouvre les socket sur le port indiqué par l'utilisateur
// si aucun port n'est spécifié, la fonction ouvre sur le port par defaut
// ( port 1337 )
int lancerServeur();
