
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <netdb.h>

#include "../include/global.h"
#include "../include/logger.h"
#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define CMD_SIZE 100
#define BUFFER_SIZE 256

struct node* SortedMerge(struct node* a, struct node* b);
void FrontBackSplit(struct node* source, struct node** frontRef, struct node** backRef);
void MergeSort(struct node** headRef);

  struct node{
		int list_id;
		char hostname[1025];
		char ip_add[INET_ADDRSTRLEN];
		char port_no[20];
		struct node* next;
	};

void MergeSort(struct node** headRef)
{
  	struct node* head = *headRef;
  	struct node* a;
  	struct node* b;

  	/* Base case -- length 0 or 1 */
  	if ((head == NULL) || (head->next == NULL))
    		return;

  	/* Split head into 'a' and 'b' sublists */
  	FrontBackSplit(head, &a, &b);

  	/* Recursively sort the sublists */
  	MergeSort(&a);
  	MergeSort(&b);
	
	/* answer = merge the two sorted lists together */
  	*headRef = SortedMerge(a, b);
    }

void FrontBackSplit(struct node* source, struct node** frontRef, struct node** backRef)
    {
  	struct node* fast;
  	struct node* slow;
  	if (source==NULL || source->next==NULL)
  	{
    		/* length < 2 cases */
    		*frontRef = source;
    		*backRef = NULL;
  	}
  	else
  	{
    		slow = source;
    		fast = source->next;
		/* Advance 'fast' two nodes, and advance 'slow' one node */
    		while (fast != NULL)
    		{
      			fast = fast->next;
      			if (fast != NULL)
      			{
        			slow = slow->next;
        			fast = fast->next;
      			}
    		}

    	/* 'slow' is before the midpoint in the list, so split it in two at that point. */
    		*frontRef = source;
    		*backRef = slow->next;
    		slow->next = NULL;
  	}
    }

    struct node* SortedMerge(struct node* a, struct node* b)
    {
  	struct node* result = NULL;

        /* Base cases */
  	if (a == NULL)
     		return(b);
  	else if (b==NULL)
     		return(a);

  	/* Pick either a or b, and recur */
  	if (a->port_no <= b->port_no)
  	{
     		result = a;
     		result->next = SortedMerge(a->next, b);
  	}
  	else
  	{
     		result = b;
     		result->next = SortedMerge(a, b->next);
  	}
  	return(result);
    }



int server(int port_no)
{
    int port, server_socket, head_socket, selret, sock_index, fdaccept=0, caddr_len;
    struct sockaddr_in server_addr, client_addr;
    fd_set master_list, watch_list;

    /* Socket */
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket < 0)
	perror("Cannot create socket");

    /* Fill up _in struct */
    port=port_no;
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    /* Bind */
    if(bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 )
    	perror("Bind failed");

    /* Listen */
    if(listen(server_socket, BACKLOG) < 0)
    	perror("Unable to listen on port");

    /* ---------------------------------------------------------------------------- */

    /* Zero select FD sets */
    FD_ZERO(&master_list);
    FD_ZERO(&watch_list);
    
    /* Register the listening socket */
    FD_SET(server_socket, &master_list);
    /* Register STDIN */
    FD_SET(STDIN, &master_list);

    head_socket = server_socket;

    struct node* head;
    head = (struct node *) malloc( sizeof(struct node) );
    head = NULL;

    while(TRUE){
        memcpy(&watch_list, &master_list, sizeof(master_list));

        //printf("\n[PA1-Server@CSE489/589]$ ");
	//fflush(stdout);
        /* select() system call. This will BLOCK */
        selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
        if(selret < 0)
            perror("select failed.");

        /* Check if we have sockets/STDIN to process */
        if(selret > 0){
            /* Loop through socket descriptors to check which ones are ready */
            for(sock_index=0; sock_index<=head_socket; sock_index+=1){

                if(FD_ISSET(sock_index, &watch_list)){

                    /* Check if new command on STDIN */
                    if (sock_index == STDIN){
                    	char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);

                    	memset(cmd, '\0', CMD_SIZE);
			if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to cmd
				exit(-1);
			
			//printf("\nI got: %s\n", cmd);
			
			int len=strlen(cmd);	
			//printf("Length = %d\n", len);			
	
			if(cmd[len-1]=='\n')
				cmd[len-1] = '\0';

			if(!strncmp(cmd,"AUTHOR", strlen("AUTHOR")))
			{
				cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
				cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "trathore");
				cse4589_print_and_log("[%s:END]\n", cmd);
			}
			


		    	if(!strncmp(cmd,"IP", strlen("IP")))
			{
				
				int sockfd;
				struct sockaddr_in udp_addr,res;				
				char ipstr[100];

				if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0)
				{
					cse4589_print_and_log("[%s:ERROR]\n", cmd);
					cse4589_print_and_log("[%s:END]\n", cmd);
				}
				
				
				memset(&udp_addr,0, sizeof udp_addr);
				udp_addr.sin_family = AF_INET; /* IPv4*/
				udp_addr.sin_addr.s_addr =inet_addr("8.8.8.8");
				udp_addr.sin_port = htons(port);
				
				
				connect(sockfd, (struct sockaddr*)&udp_addr, sizeof(udp_addr));
				int addr_len = sizeof res;

				if(!(getsockname(sockfd,(struct sockaddr *)&res,&addr_len )))
				{
				        inet_ntop(res.sin_family, &res.sin_addr, ipstr, sizeof ipstr);//taken from beej guide
					cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
					cse4589_print_and_log("IP:%s\n", ipstr);
					cse4589_print_and_log("[%s:END]\n", cmd);
				}
				

			}
			
			
			if(!strncmp(cmd,"PORT",strlen("PORT")))
			{
				cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
				cse4589_print_and_log("PORT:%d\n", port);
				cse4589_print_and_log("[%s:END]\n", cmd);
			}


			if(!strncmp(cmd,"LIST",strlen("LIST")))
			{
			int i=0;
			struct node* current;
			current = (struct node *) malloc( sizeof(struct node) );
			current = head;
			cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
			while(current!=NULL)
			{i++;
			cse4589_print_and_log("%-5d%-35s%-20s%-8d\n",i , current->hostname, current->ip_add, atoi(current->port_no));
			current = current->next; }
			cse4589_print_and_log("[%s:END]\n", cmd);
			}
			

		    free(cmd);
                    }
                    /* Check if new client is requesting connection */
                    else if(sock_index == server_socket)
		    {
                        caddr_len = sizeof(client_addr);
                        fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, &caddr_len);
                        if(fdaccept < 0)
                            perror("Accept failed.");
			
			//printf("\nRemote Host connected!\n");	
		
			char *cl_port = (char*) malloc(sizeof(char)*BUFFER_SIZE);
			
			char *port_buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                        memset(port_buffer, '\0', BUFFER_SIZE);

                        if(recv(fdaccept, port_buffer, BUFFER_SIZE, 0) <= 0){
                            printf("Client port not received!\n");
			    cse4589_print_and_log("ERROR\n");}

                        else {
                        	strcpy(cl_port,port_buffer);
			        printf("%s\n",port_buffer);
			}
                        /* Add to watched socket list */
                        FD_SET(fdaccept, &master_list);
                        if(fdaccept > head_socket) head_socket = fdaccept;	
			
			//Define a new node and allocate memory to it		
			//struct in_addr ipv4addr;
			char host[1024];
                	char service[20];
			
			struct node* newnode =(struct node*)malloc(sizeof(struct node));
			
			inet_ntop(AF_INET, &(((struct sockaddr_in *)&client_addr)->sin_addr), newnode->ip_add, INET_ADDRSTRLEN);
			//inet_pton(AF_INET, newnode->ip_add, &ipv4addr);			

			getnameinfo((struct sockaddr *)&client_addr, caddr_len, host, sizeof host, service, sizeof service, 0);
			strcpy( newnode->hostname , host);
			strcpy( newnode-> port_no, cl_port);
			newnode->next = NULL;

                	struct node *temp=head;
                	if(head==NULL)
	                	head = newnode;
	                else
	                {
        	        while(temp->next!=NULL)
	                	temp = temp->next;
	                temp->next = newnode;
			
	                }
                	
                	
	                MergeSort(&head);
                	
			//return list of clients logged in
			struct node* current;
                	current = (struct node *) malloc( sizeof(struct node) );
                	current = head;
			char cl_list[2048] = "";

                 	while(current!=NULL)
                	{
                	 	char s1[1024], s2[INET_ADDRSTRLEN], s3[20];

	                	 strcpy(s1,current->hostname);
       		         	 strcpy(s2,current->ip_add);
        	        	 strcpy(s3,current->port_no);
        	        	 strcat(s1, " ");
        	        	 strcat(s1, s2);
        	        	 strcat(s1, " ");
        	        	 strcat(s1, s3);
				 //printf("Token Message to be sent: %s\n", s1);
        	        	 
				strcat(s1,"+");
				strcat(cl_list, s1);		
				//printf("Complete Message to be sent: %s\n", cl_list);
				//int z= send(fdaccept, s1, sizeof s1, 0);
				//printf("Bytes to be sent: %lu\nBytes sent in actual: %d", sizeof (s1), z);
                    
        	 	       	current = current->next;
			}	
				int z= send(fdaccept, cl_list, sizeof cl_list, 0);
				//printf("Bytes to be sent: %lu\nBytes sent in actual: %d", sizeof (cl_list), z);
				memset(cl_list, 0, sizeof cl_list);;
                    }
                    /* Read from existing clients */
                    else{
                        /* Initialize buffer to receieve response */
                        char *buffer	 = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                        memset(buffer, '\0', BUFFER_SIZE);

                        if(recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0){
                            close(sock_index);
                            printf("Remote Host terminated connection!\n");

                            /* Remove from watched list */
                            FD_CLR(sock_index, &master_list);
                        }
                        else {
                        	//Process incoming data from existing clients here ...

                        	printf("\nClient sent me: %s\n", buffer);
				
				if(!strncmp(buffer,"REFRESH",strlen("REFRESH")))
				{	
					
					struct node* current;
					current = (struct node *) malloc( sizeof(struct node) );
					current = head;
					char cl_list[256] = "";

					cse4589_print_and_log("[%s:SUCCESS]\n", buffer);
					while(current!=NULL)
					{
						char s1[256], s2[INET_ADDRSTRLEN], s3[20];

						strcpy(s1,current->hostname);
       		         	 		strcpy(s2,current->ip_add);
        	        	 		strcpy(s3,current->port_no);
        	        	 		strcat(s1, " ");
        	        	 		strcat(s1, s2);
        	        	 		strcat(s1, " ");
        	        	 		strcat(s1, s3);
				 		//printf("Token Message to be sent: %s\n", s1);
        	        	 
						strcat(s1,"+");
						strcat(cl_list, s1);	

						current = current->next; 
					}
						printf("Client_list sent %s", cl_list);						
						int z= send(fdaccept, cl_list, sizeof cl_list, 0);
						
						printf("Bytes to be sent: %lu\nBytes sent in actual: %d", sizeof (cl_list), z);
						memset(cl_list, 0, sizeof cl_list);;						
						cse4589_print_and_log("[%s:END]\n", buffer);
						
				}
					
					
	
			

				//printf("ECHOing it back to the remote host ... ");
				//if(send(fdaccept, buffer, strlen(buffer), 0) == strlen(buffer))
				//	printf("Done!\n");
				//fflush(stdout);
                        }

                        free(buffer);
                    }
                }//buffer = strtok(buffer, "");
            }
        }
    }

    return 0;
}
