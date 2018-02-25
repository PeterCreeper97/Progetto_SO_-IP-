#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "header.h"

void funzione_comando_cd(char buffer[]){

    char *dir_corrente;
    char *dir_1, *dir;
    char buf[GENERAL_SIZE];
    char *path;
    
    path=strchr(buffer, '/'); //strchr ritorna il puntatore alla prima occorrenza del carattere /
    
    dir_corrente=getcwd(buf, sizeof(buf)); //getcwd restituisce la directory corrente
    
    dir=strcat(dir_corrente, path); //strcat concatena il path con la directory corrente
    
    chdir(dir);//chdir cambia la directory corrente in quella specificata

}

void executer(int socket_id, char buffer[], char *canale, char *utente){

        char *comando;
        char *comando_1;
        char messaggio[MESSAGE_SIZE];
        char msg[MESSAGE_SIZE];
        char output[]="Output.txt";
        char COMANDO[GENERAL_SIZE];
        int fp;       
        int len;
        
      
        comando_1=strchr(buffer, '*');//strchr ritorna il puntatore alla prima occorrenza del carattere *
        
        comando=strtok(comando_1, "*");//strtok spezza la stringa in base al carattere *
        
        printf("Comando etratto: %s\n", comando);
        
        if(strstr(comando, "cd")!=NULL){
        
            funzione_comando_cd(buffer);
        
        }else{
        
        
        sprintf(COMANDO,"%s > Output.txt", comando);//sprintf permette di assemblare il comando necessario per eseguire e copiare l'output in un file chiamato Output.txt
       
        system(COMANDO);//Esegue il comando

//Apro il file Output.txt, leggo e memorizzo ciò che contiene in una variabile, che assemblandola con il comando CPRIVMSG permetterà di mandare l'output a colui che ha inviato il comando speciale
        
        fp=open(output, O_RDONLY);
        
        if(fp==-1){
        
            perror("Errore apertura file.\n");
            exit(EXIT_FAILURE);
            
        }
       
        len=read(fp, &messaggio, MESSAGE_SIZE);
        
        while(len>0){
            
            len=read(fp, &messaggio, MESSAGE_SIZE);
        
        }
        
        close(fp);
        
        sprintf(msg, "CPRIVMSG %s %s :%s", utente, canale, messaggio);
        
        write(socket_id, &msg, strlen(msg));
        
        
        }       
        

}

void *funzione_thread_lettura(void *arg){

        char buffer[BUFFER];
        char PONG[]="PONG\n";
        char indirizzo_server[GENERAL_SIZE+1];
        char *messaggio;
        char utente_1[GENERAL_SIZE+1];
        char *utente_2;
        char *stringa_1, *stringa_2, *stringa;
        char *utente;
        char *canale_1, *canale;
        int socket_id;
        
        socket_id=*(int*)arg;

        while(1){
                
                memset(buffer, '\0', BUFFER);  //Svuoto il buffer per evitare che rimangano dati memorizzati nella stringa da stampare
                
                if(read(socket_id, &buffer, BUFFER)==-1){
                
                        perror("Errore relativo alla lettura sulla socket.\n");
                        exit(EXIT_FAILURE);
                
                }
                
                if(strstr(buffer, "PING")!=NULL){  //Se nel buffer è presente il comando PING il client invia in automatico il comando PONG per evitare la caduta della connessione
                        
                        write(socket_id, &PONG, strlen(PONG));
                
                }else if(strstr(buffer, "PRIVMSG")!=NULL){  //Se nel buffer è presente il comando PRIVMSG il client stampa da quale utente o canale arriva il messaggio
                    
                    //Estrazione dell'utente
                        stringa_1=strchr(buffer,'!');
                        
                        strncpy(utente_1, buffer, strlen(buffer)-strlen(stringa_1));
                        utente_1[0]=' ';
                        
                        utente_2=strtok(utente_1, "!");
                        
                        utente=strtok(utente_2, " ");
                
                        if(strstr(buffer, "PIEXE")!=NULL){
                
                //Estrazione del canale compreso di #
                
                                stringa=strchr(buffer, '#');
                                
                                stringa_2=strchr(stringa, ':');
                                
                                strncpy(canale_1,stringa, strlen(stringa)-strlen(stringa_2));
                                
                                canale=strtok(canale_1, " ");
                                
                                executer(socket_id, buffer, canale, utente);
                
                        }else{
                        
               //Se non è presente alcun comando speciale di tipo PIEXE*, viene visualizzato a schermo il messaggio ricevuto
                        
                             printf("%s ha inviato un messaggio!\n", utente);
                       
                             messaggio=strchr(buffer, 'P'); //Permette di memorizzare in una variabile i caratteri successivi alla lettera P
                        
                             printf("%s\n", messaggio);
                
                }
                
        
   }
 }
 
 }
   
   
/*******************************************************************************************/
   
void Inserisci_Credenziali(int socket_id){

        ////////////////////////////////////////
        char NICK[GENERAL_SIZE+1];  char nick[GENERAL_SIZE+1];
        char USER[GENERAL_SIZE+1];  char user[GENERAL_SIZE+1];
        char PASS[GENERAL_SIZE+1];  char pass[GENERAL_SIZE+1];
        ////////////////////////////////////////

        printf("Inserisci le credenziali.\n");
                
                printf("NICK: ");
                scanf(" %[^\n]s", nick);
                printf("USER: ");
                scanf(" %[^\n]s", user);
                printf("PASS: ");
                scanf(" %[^\n]s", pass);
                sprintf(PASS, "PASS %s\n", pass);             //La funzione permette di assemblare il messaggio di inviare al server contenente il comando e le relative informazioni
                sprintf(NICK, "NICK %s\n", nick);
                sprintf(USER, "USER %s\n", user);
                
                write(socket_id, &PASS, strlen(PASS));
                write(socket_id, &NICK, strlen(NICK));
                write(socket_id, &USER, strlen(USER));

                fflush(stdout);                      

}

void Join_Channel(int socket_id, char channel[]){

        char CHANNEL[GENERAL_SIZE+1];
        
        sprintf(CHANNEL, "JOIN #%s\n", channel);
        
        write(socket_id, &CHANNEL, strlen(CHANNEL));
        
        printf("Benvenuto in %s!\n", channel);
           
}

void Part_Channel(int socket_id, char channel[]){

        char CHANNEL[GENERAL_SIZE+1];
        int channel_len;

        sprintf(CHANNEL, "PART #%s\n", channel);
        
        write(socket_id, &CHANNEL, strlen(CHANNEL));
        
        printf("Hai lasciato %s.\n", channel);


}

void Messaggio_Privato(int socket_id, char channel[]){

        char destinatario[GENERAL_SIZE+1];
        char messaggio[MESSAGE_SIZE+1];
        char MESSAGE[GENERAL_SIZE+MESSAGE_SIZE];

        printf("Destinatario: ");
        scanf(" %[^\n]s", destinatario);
        printf("[Messaggio]: ");
        scanf(" %[^\n]s", messaggio);
        
        fflush(stdout);
        
         sprintf(MESSAGE, "\nCPRIVMSG %s #%s :%s\n", destinatario, channel, messaggio);
         
         write(socket_id, &MESSAGE, strlen(MESSAGE));

}

void Messaggio_Pubblico(int socket_id, char channel[]){

        char messaggio[MESSAGE_SIZE+1];
        char MESSAGE[GENERAL_SIZE+MESSAGE_SIZE];

        printf("[Messaggio]: ");
        scanf(" %[^\n]s", messaggio);
        
        fflush(stdout);
        
         sprintf(MESSAGE, "\nPRIVMSG #%s :%s\n", channel, messaggio);
         
         write(socket_id, &MESSAGE, strlen(MESSAGE));

}

void Chiusura_IRC(int socket_id){

    char CHIUSURA[GENERAL_SIZE+1];

    sprintf(CHIUSURA, "QUIT");
    
    write(socket_id, &CHIUSURA, strlen(CHIUSURA));

}


void Menu(int socket_id){

        int scelta;
        char channel[GENERAL_SIZE+1];
        
        pthread_t thread_lettura;
       
                printf("\n********MENU********\n\n");
                printf("1) Joina in un canale.\n");
                printf("2) Lascia il canale.\n");
                printf("3) Messaggio privato.\n");
                printf("4) Messaggio pubblico (su canale).\n");
                printf("0) Chiudi IRC.\n");
        
                do{
       
                printf("\nInserisci opzione: ");
                scanf("%d", &scelta);
                
                        switch(scelta){
                        
                                case 1:
                                        
                                        printf("Inserisci il nome del canale a cui accedere: ");
                                        scanf("%s", channel);   
                                        Join_Channel(socket_id, channel);
                                        break;
                        
                                case 2:
                                
                                        Part_Channel(socket_id, channel);
                                        break;
                                        
                                case 3:
                                
                                        Messaggio_Privato(socket_id, channel);
                                        break;
                                                                                
                                case 4:
                                
                                        Messaggio_Pubblico(socket_id, channel);
                                        break;
                                       
                                case 0:
                                        
                                        printf("Exit.\n");
                                        
                                        Chiusura_IRC(socket_id);
                                        
                                        break;
                                                
                                default:
                                
                                        printf("Operazione inesistente.\n");
                                        break;             
                        
                        }
                        
              }while(scelta!=0);
                
}
