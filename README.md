# Protocol MaxInt

## Description du projet

Vous devez écrire un serveur et deux clients pour le protocole de messagerie maxint décrit
ci-dessous.  
Le protocole fonctionnera de la façon suivante :  

- le serveur accepte plusieurs connexions TCP de clients en parallèle ;  
- lorsqu’un client se connecte au serveur, il peut envoyer au serveur un entier positif ou lui
demander de lui envoyer l’entier le plus grand parmi ceux qui lui ont été envoyés.  
Dans la suite MAX_NAME 10 et les noms d’utilisateurs sont codés en ASCII.

***

## Le protocole maxint côté client

Lorsque le client se connecte, il envoie immédiatement au serveur le message « <pseudo> » où <pseudo> est le nom de l’utilisateur d’exactement MAX_NAME caractères.  
Il attend ensuite la réponse du serveur de la forme « HELLO␣<pseudo> ».  
Le client peut alors envoyer deux types de messages au serveur :

- un message « INT␣<val> » où <val> est un entier non signé positif codé sur deux octets en big-endian

- un message « MAX » pour demander le plus grand entier reçu par le serveur jusqu’à présent.  
À la réception de la réponse à ce message, le client devra de plus afficher le le nom de
l’utilisateur, son adresse ip au format classique et la valeur contenue dans la réponse du
serveur.  
Ensuite le client se déconnecte.

***

## Le protocole maxint côté serveur

Après avoir salué le client avec le message « HELLO␣<pseudo> », le serveur répond aux requêtes
du client de la façon suivante :  

- s’il reçoit un message de type « INT »:
  Il renvoie le message « INTOK » (Message OK) au
client pour dire qu’il a reçu son message,  
- s’il reçoit un message de type « MAX »:
  il envoie au client la valeur maximale de l’entier
parmi les entiers reçus.  
Il envoie donc « REP<pseudo><ip><val> » où <pseudo> est le nom
de l’utilisateur ayant envoyé le plus grand entier, <ip> est son adresse ip codée sur 4 octets
en big-endian, et <val> est la valeur du plus grand entier reçu par le serveur codé en
big-endian. Il n’y a pas d’espace entre le pseudo, l’adresse ip et les données.  
Si le serveur
n’a pas encore reçu d’entier, il répond par « NOP ».  
Si plusieurs clients ont envoyé la valeur
la plus grande, le serveur renverra l’identité du dernier client ayant envoyé cette valeur.  
Attention, veillez à bien gérer les accès concurrents pour les valeurs sauvegardées par le serveur
