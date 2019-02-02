#ifndef __SERVEUR_H__
#define __SERVEUR_H__

/* Initialisation.
 * Creation du serveur.
 * renvoie 1 si �a c'est bien pass� 0 sinon
 */
int Initialisation();

/* Initialisation.
 * Creation du serveur en pr�cisant le service ou num�ro de port.
 * renvoie 1 si �a c'est bien pass� 0 sinon
 */
int InitialisationAvecService(char *service);


/* Attends qu'un client se connecte.
 * renvoie 1 si �a c'est bien pass� 0 sinon
 */
int AttenteClient();

/* Recoit un message envoye par le client.
 * retourne le message ou NULL en cas d'erreur.
 * Note : il faut liberer la memoire apres traitement.
 */
char *Reception();

/* Envoie un message au client.
 * Attention, le message doit etre termine par \n
 * renvoie 1 si �a c'est bien pass� 0 sinon
 */
int Emission(char *message);

/* Recoit des donnees envoyees par le client.
 * renvoie le nombre d'octets re�us, 0 si la connexion est ferm�e,
 * un nombre n�gatif en cas d'erreur
 */
int ReceptionBinaire(char *donnees, size_t tailleMax);

/* Envoie des donn�es au client en pr�cisant leur taille.
 * renvoie le nombre d'octets envoy�s, 0 si la connexion est ferm�e,
 * un nombre n�gatif en cas d'erreur
 */
int EmissionBinaire(char *donnees, size_t taille);

/*Extrait d'une requete HTTP GET le nom du fichier demand�*/
int extraitFichier(
	char *requete,
	char *tableauNomFichier,
	int tailleTableauNomFichier);

/*Renvoie la taille du fichier demand�*/
unsigned long longueur_fichier(char *nomFichier);

/*Envoie le contenu du fichier texte demand� au client*/
int envoyerContenuFichierTexte(char *nomFichier);

/*Envoi une Reponse HTML 200 OK*/
int envoyerReponse200HTML(char *nomFichier);

//envoie une reponse 404 HTML
int envoyerReponse404(char *nomFichier);

//envoie une reponse 500 HTML
int envoyerReponse500(char *message);

/*Envoie un r�ponse 200 pour les fichiers image jpg*/ 
int envoyerReponse200JPG(char *nomFichier);

/*Envoie un r�ponse 200 pour les fichiers ico*/ 
int envoyerReponse200ICO(char *nomFichier);

/* Ferme la connexion avec le client.
 */
void TerminaisonClient();

/* Arrete le serveur.
 */
void Terminaison();

//execute la requete demande si il en a le droit
int executerRequete(char * requete);

#endif
