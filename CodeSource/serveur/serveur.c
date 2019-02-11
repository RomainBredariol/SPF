#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <errno.h>

#include "serveur.h"
#include "lib.h"

#define TRUE 1
#define FALSE 0
#define LONGUEUR_TAMPON 4096


#ifdef WIN32
#define perror(x) printf("%s : code d'erreur : %d\n", (x), WSAGetLastError())
#define close closesocket
#define socklen_t int
#endif

/* Variables cachees */

/* le socket d'ecoute */
int socketEcoute;
/* longueur de l'adresse */
socklen_t longeurAdr;
/* le socket de service */
int socketService;
/* le tampon de reception */
char tamponClient[LONGUEUR_TAMPON];
int debutTampon;
int finTampon;


/* Initialisation.
 * Creation du serveur.
 */
int Initialisation() {
	return InitialisationAvecService("13214");
}

/* Initialisation.
 * Creation du serveur en prŽcisant le service ou numŽro de port.
 * renvoie 1 si a c'est bien passŽ 0 sinon
 */
int InitialisationAvecService(char *service) {
	int n;
	const int on = 1;
	struct addrinfo	hints, *res, *ressave;

	#ifdef WIN32
	WSADATA	wsaData;
	if (WSAStartup(0x202,&wsaData) == SOCKET_ERROR)
	{
		printf("WSAStartup() n'a pas fonctionne, erreur : %d\n", WSAGetLastError()) ;
		WSACleanup();
		exit(1);
	}
	memset(&hints, 0, sizeof(struct addrinfo));
    #else
	bzero(&hints, sizeof(struct addrinfo));
	#endif

	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ( (n = getaddrinfo(NULL, service, &hints, &res)) != 0)  {
     		printf("Initialisation, erreur de getaddrinfo : %s", gai_strerror(n));
     		return 0;
	}
	ressave = res;

	do {
		socketEcoute = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (socketEcoute < 0)
			continue;		/* error, try next one */

		setsockopt(socketEcoute, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
#ifdef BSD
		setsockopt(socketEcoute, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
#endif
		if (bind(socketEcoute, res->ai_addr, res->ai_addrlen) == 0)
			break;			/* success */

		close(socketEcoute);	/* bind error, close and try next one */
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL) {
     		perror("Initialisation, erreur de bind.");
     		return 0;
	}

	/* conserve la longueur de l'addresse */
	longeurAdr = res->ai_addrlen;

	freeaddrinfo(ressave);
	/* attends au max 4 clients */
	listen(socketEcoute, 4);
	printf("Creation du serveur reussie sur %s.\n", service);

	return 1;
}

/* Attends qu'un client se connecte.
 */
int AttenteClient() {
	struct sockaddr *clientAddr;
	char machine[NI_MAXHOST];

	clientAddr = (struct sockaddr*) malloc(longeurAdr);
	socketService = accept(socketEcoute, clientAddr, &longeurAdr);
	if (socketService == -1) {
		perror("AttenteClient, erreur de accept.");
		return 0;
	}
	if(getnameinfo(clientAddr, longeurAdr, machine, NI_MAXHOST, NULL, 0, 0) == 0) {
		printf("Client sur la machine d'adresse %s connecte.\n", machine);
	} else {
		printf("Client anonyme connecte.\n");
	}
	free(clientAddr);
	/*
	 * Reinit buffer
	 */
	debutTampon = 0;
	finTampon = 0;

	return 1;
}

/* Recoit un message envoye par le serveur.
 */
char *Reception() {
	char message[LONGUEUR_TAMPON];
	int index = 0;
	int fini = FALSE;
	int retour = 0;
	while(!fini) {
		/* on cherche dans le tampon courant */
		while((finTampon > debutTampon) &&
			(tamponClient[debutTampon]!='\n')) {
			message[index++] = tamponClient[debutTampon++];
		}
		/* on a trouve ? */
		if ((index > 0) && (tamponClient[debutTampon]=='\n')) {
			message[index++] = '\n';
			message[index] = '\0';
			debutTampon++;
			fini = TRUE;
#ifdef WIN32
			return _strdup(message);
#else
			
			return strdup(message);
#endif
		} else {
			/* il faut en lire plus */
			debutTampon = 0;
			retour = recv(socketService, tamponClient, LONGUEUR_TAMPON, 0);
			if (retour < 0) {
				perror("Reception, erreur de recv.");
				return NULL;
			} else if(retour == 0) {
				fprintf(stderr, "Reception, le client a ferme la connexion.\n");
				return NULL;
			} else {
				/*
				 * on a recu "retour" octets
				 */
				finTampon = retour;
			}
		}
	}
	return NULL;
}

/* Envoie un message au client.
 * Attention, le message doit etre termine par \n
 */
int Emission(char *message) {
	int taille;
	if(strstr(message, "\n") == NULL) {
		fprintf(stderr, "Emission, Le message n'est pas termine par \\n.\n");
		return 0;
	}
	taille = strlen(message);
	if (send(socketService, message, taille,0) == -1) {
        perror("Emission, probleme lors du send.");
        return 0;
	}
	printf("Emission de %d caracteres : %s\n", taille+1, message);
	return 1;
}


/* Recoit des donnees envoyees par le client.
 */
int ReceptionBinaire(char *donnees, size_t tailleMax) {
	size_t dejaRecu = 0;
	int retour = 0;
	/* on commence par recopier tout ce qui reste dans le tampon
	 */
	while((finTampon > debutTampon) && (dejaRecu < tailleMax)) {
		donnees[dejaRecu] = tamponClient[debutTampon];
		dejaRecu++;
		debutTampon++;
	}
	/* si on n'est pas arrive au max
	 * on essaie de recevoir plus de donnees
	 */
	if(dejaRecu < tailleMax) {
		retour = recv(socketService, donnees + dejaRecu, tailleMax - dejaRecu, 0);
		if(retour < 0) {
			perror("ReceptionBinaire, erreur de recv.");
			return -1;
		} else if(retour == 0) {
			fprintf(stderr, "ReceptionBinaire, le client a ferme la connexion.\n");
			return 0;
		} else {
			/*
			 * on a recu "retour" octets en plus
			 */
			return dejaRecu + retour;
		}
	} else {
		return dejaRecu;
	}
}

/* Envoie des donnŽes au client en prŽcisant leur taille.
 */
int EmissionBinaire(char *donnees, size_t taille) {
	int retour = 0;
	retour = send(socketService, donnees, taille, 0);
	if(retour == -1) {
		perror("Emission, probleme lors du send.");
		return -1;
	} else {
		return retour;
	}
}

/*Extrait d'une requete HTTP GET le nom du fichier demandé*/
int extraitFichier(
	char *requete,
	char *tableauNomFichier,
	int tailleTableauNomFichier)
{
	//trouve la premiere occurence de "/" 
	//et renvoie un pointeur à cette adresse
	if((requete = strstr(requete, "/")) == NULL){
		printf("Erreur requete\n");
		return -1;
	}

	//pointe la chaine de caractere sans espace à partir de "/"
	if(sscanf(requete, "%[^ ]", requete) == 0){
		printf("Erreur sscanf\n");
		return -1;
	}

	//si il n'y a pas de nom explicite apres "/" 
	//le nom du fichier demandé est automatiquement "index.php"
	if(strcmp(requete , "/") == 0){
		requete = "/index.php";
	}

	//decaler de 1 le pointeur
	requete = &requete[1];

	strcpy(tableauNomFichier, requete);
	
	return 0;
}

/*Renvoie la taille du fichier demandé*/
unsigned long longueur_fichier(char *nomFichier)
{
	unsigned int taille;
	
	//ouverture du fichier demandé
	FILE *fichier = fopen(nomFichier, "r");

	//si l'ouverture échoue
	if(fichier == NULL){
		printf("Erreur fopen fichier\n");
		return -1;
	}

	//place le pointeur fichier en fin de fichier
	taille = fseek(fichier, 0, SEEK_END);

	//si le fseek échoue
	if(taille != 0){
		printf("Erreur fseek\n");
		return -1;
	}else{
		//renvoie la position en octets, ici la fin du fichier donc sa taille
		return ftell(fichier);
	}
}

/*Envoie le contenue du fichier texte demandé au client*/
int envoyerContenuFichierTexte(char *nomFichier){
	/*	contenu est le contenu du fichier texte
		taille est la longueur du fichier en octet
		format est l'extension du fichier*/
	char *contenu;
	unsigned long taille = longueur_fichier(nomFichier);
	char *format;

	contenu = malloc(taille);

	//on cherche la premiere occurence de "." dans nomFichier
	format = strstr(nomFichier, ".");
	
	//si le fichier est un fichier texte ou html
	if(strcmp(format, ".txt") == 0 || strcmp(format, ".html") == 0){

		FILE *fichier = fopen(nomFichier, "r");
		
		if(fichier == NULL){
			printf("Erreur ouverture fichier");
			return -1;
		}

		//place le pointeur de fichier au debut
		fseek(fichier, 0, SEEK_SET);

		//lit le contenu du fichier
		fread(contenu, taille, 1, fichier); 

		//emet le contenu au client
		Emission(contenu);
	}else{
		Emission("le fichier doit etre un .txt ou .html\n");
		return -1;
	}
	return 0;
}


/*Envoie le contenu du fichier binaire demandé*/
int envoyerContenuFichierBinaire(char *nomFichier){
	/*	contenu est le contenu du fichier binaire à envoyer au client
		taille est la longueur du fichier demande*/
	char *contenu;
	unsigned long taille = longueur_fichier(nomFichier);

	contenu = malloc(taille);
	
	FILE *fichier = fopen(nomFichier, "r");

	//ouvre le fichier demande
	if(fichier == NULL){
		printf("Erreur ouverture fichier");
		return -1;
	}

	//place le pointeur de fichier au debut du fichier
	fseek(fichier, 0, SEEK_SET);
	//lit le contenu du fichier
	fread(contenu, taille, 1, fichier); 
	//emet le contenu binaire du fichier
	EmissionBinaire(contenu, taille);
	
	return 0;
}
/* Ferme la connexion avec le client.
 */
void TerminaisonClient() {
	close(socketService);
}

/* Arrete le serveur.
 */
void Terminaison() {
	close(socketEcoute);
}
