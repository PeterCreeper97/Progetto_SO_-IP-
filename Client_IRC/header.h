#define GENERAL_SIZE 50 //Dimensione generale delle stringhe utilizzate
#define MESSAGE_SIZE 1024 //Dimensione testo del messaggio scambiato con gli utenti
#define BUFFER 512 //Dimensione massima del buffer

/*PROTOTIPI*/

void *funzione_thread_lettura(void *arg); //Thread che permette la lettura dei messaaggi mandati dal server

void Inserisci_Credenziali(int socket_id); //Funzione per effettuare il login al server

void Join_Channel(int socket_id, char channel[]); //Funzione che permette di entrare in un canale

void Part_Channel(int socket_id, char channel[]); //Funzione che permette l'uscita da un canale

void Messaggio_Privato(int socket_id, char channel[]); //Funzione che permette di inviare un messaggio privato ad un utente presente nello stesso canale

void Messaggio_Pubblico(int socket_id, char channel[]); //Funzione che permette di inviare un messaggio a tutti gli utenti presenti in quel canale

void Chiusura_IRC(int socket_id); //Funzione che permette la chiusura della connessione al server IRC

void Menu(int socket_id); //Funzione che mostra un Menù a tendina delle operazioni disponibili
