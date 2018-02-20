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

#define GENERAL_SIZE 50
#define MESSAGE_SIZE 1024
#define BUFFER 512
#define DATI 3

/*************************************************************************************/

void *funzione_thread_lettura(void *arg){

        char buffer[BUFFER];
        char PONG[]="PONG\n";
        char indirizzo_server[GENERAL_SIZE+1];
        int socket_id;
        
        socket_id=*(int*)arg;

        while(1){
                
                if(read(socket_id, &buffer, BUFFER)==-1){
                
                        perror("Errore relativo alla lettura sulla socket.\n");
                        exit(EXIT_FAILURE);
                
                }
                
                if(strstr(buffer, "PING")!=NULL){
                        
                        write(socket_id, &PONG, strlen(PONG));
                
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
                sprintf(PASS, "PASS %s\n", pass);
                sprintf(NICK, "NICK %s\n", nick);
                sprintf(USER, "USER %s\n", user);
                
                write(socket_id, &PASS, strlen(PASS));
                write(socket_id, &NICK, strlen(NICK));
                write(socket_id, &USER, strlen(USER));

                fflush(stdout);                      

}

void Join_Channel(int socket_id, char channel[]){

        char CHANNEL[GENERAL_SIZE+1];
        int channel_len;
        
        memset(CHANNEL, '\0', GENERAL_SIZE+1);
        
        sprintf(CHANNEL, "JOIN #%s\n", channel);
        
        write(socket_id, &CHANNEL, strlen(CHANNEL));
           
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
        scanf("%s", destinatario);
        printf("[Messaggio]: ");
        scanf(" %[^\n]s", messaggio);
        
         sprintf(MESSAGE, "CPRIVMSG %s #%s :%s", destinatario, channel, messaggio);
         
         printf("%s", MESSAGE);
         
         write(socket_id, &MESSAGE, strlen(MESSAGE));

}

void Messaggio_Pubblico(int socket_id, char channel[]){

        char messaggio[MESSAGE_SIZE+1];
        char MESSAGE[GENERAL_SIZE+MESSAGE_SIZE];

        printf("[Messaggio]: ");
        scanf(" %[^\n]s", messaggio);
        
         sprintf(MESSAGE, "PRIVMSG #%s :%s\n", channel, messaggio);
         
         write(socket_id, &MESSAGE, strlen(MESSAGE));

}


void Menu(int socket_id){

        int scelta=10, controllo=0;
       char channel[GENERAL_SIZE+1];
        
        pthread_t thread_lettura;
        
                while(scelta!=0){
        
                printf("\n********MENU********\n\n");
                printf("1) Joina in un canale.\n");
                printf("2) Lascia il canale.\n");
                printf("3) Messaggio privato.\n");
                printf("4) Messaggio pubblico (su canale).\n");
                printf("0) Chiudi IRC.\n");
                printf("Inserisci opzione: ");
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
                                        
                                default:
                                
                                        printf("Operazione inesistente.\n");
                                        break;             
                        
                        }
                        
              }
                
}

int main(int argc, char *argv[]){

       //////////////////////////////////////

        int socket_id, client_len, n_byte;
        int i;
       char buffer[GENERAL_SIZE+1];
       char indirizzo_server[GENERAL_SIZE+1];

        pthread_t thread_lettura;
       
        struct hostent *hp;
        struct sockaddr_in addr_client;
        
        //////////////////////////////////////
          
        socket_id=socket(AF_INET, SOCK_STREAM, 0);
        
                if(socket_id==-1){
                
                        perror("Errore inizializzazione socket.\n");
                        exit(EXIT_FAILURE);
                
                }
        
         hp=gethostbyname(argv[2]);
         addr_client.sin_addr=*(struct in_addr *)*hp->h_addr_list;
         addr_client.sin_family=AF_INET;
         addr_client.sin_port=htons(atoi(argv[1])); 
         client_len=sizeof(addr_client);
      
        printf("Mi connetto a: %s.\n", argv[2]);
        
                if(connect(socket_id, (struct sockaddr *)&addr_client, client_len)==-1){
        
                        perror("Errore di connessione.\n");
                        exit(EXIT_FAILURE);
        
                }
                
          printf("Connesso.\n");
          
          
         Inserisci_Credenziali(socket_id);

         pthread_create(&thread_lettura, NULL, funzione_thread_lettura, (void *)&socket_id);
         
         printf("Credenziali inserite.\n");
      
         Menu(socket_id); 
     
     
       ///////////////////////////////////////////////
     
        pthread_cancel(thread_lettura);

         close(socket_id);
         
         exit(EXIT_SUCCESS);

}






