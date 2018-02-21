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

#include "header.h"

int main(int argc, char *argv[]){  //Prelevo parametri da shell

       //////////////////////////////////////

        int socket_id, client_len, n_byte;
        int i;
       char buffer[GENERAL_SIZE+1];
       char indirizzo_server[GENERAL_SIZE+1];

        pthread_t thread_lettura;
       
        struct hostent *hp;
        struct sockaddr_in addr_client;
        
        //////////////////////////////////////
          
        socket_id=socket(AF_INET, SOCK_STREAM, 0);  //Inizializzo la socket
        
                if(socket_id==-1){
                
                        perror("Errore inizializzazione socket.\n");
                        exit(EXIT_FAILURE);
                
                }
        
        //Riempio la struttura prelevando dall'opportuna lista l'indirizzo a cui connettermi
        
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

         pthread_create(&thread_lettura, NULL, funzione_thread_lettura, (void *)&socket_id);  //Creo il thread di lettura
         
         printf("Credenziali inserite.\n");
      
         Menu(socket_id); 
     
     
       ///////////////////////////////////////////////
     
        pthread_cancel(thread_lettura); //Cancello il thread di lettura

        close(socket_id); //Chiudo la socket
         
        exit(EXIT_SUCCESS);

}
