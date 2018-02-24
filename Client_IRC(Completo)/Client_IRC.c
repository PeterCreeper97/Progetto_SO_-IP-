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

int main(){  //Prelevo parametri da shell

       //////////////////////////////////////

        int socket_id, client_len, n_byte;
        int i=0;
        char buffer[GENERAL_SIZE+1];
        char indirizzo_server[GENERAL_SIZE+1];

        pthread_t thread_lettura;
       
        struct hostent *hp;
        struct sockaddr_in addr_client;
        
        FILE *fp;
        
        char val[50+1];
        char indirizzo[50+1];
        char porta[50];
        
        fp=fopen("file_configurazione.txt", "r");
        
        while(!feof(fp)){
        
                fscanf(fp, "%s", val);
                
                if(i==0){
                
                        strcpy(indirizzo, val);
                       
                
                }else{
                
                        strcpy(porta, val);        
                
                }
                
                i++;
        
        }

        fclose(fp);
        
        /////////////////////////////////////////////////  
        socket_id=socket(AF_INET, SOCK_STREAM, 0);  //Inizializzo la socket
        
                if(socket_id==-1){
                
                        perror("Errore inizializzazione socket.\n");
                        exit(EXIT_FAILURE);
                
                }
        
        //Riempio la struttura prelevando dall'opportuna lista l'indirizzo a cui connettermi
        
         hp=gethostbyname(indirizzo);
         addr_client.sin_addr=*(struct in_addr *)*hp->h_addr_list;
         addr_client.sin_family=AF_INET;
         addr_client.sin_port=htons(atoi(porta)); 
         client_len=sizeof(addr_client);
      
        printf("Mi connetto a: %s.\n", indirizzo);
        
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
