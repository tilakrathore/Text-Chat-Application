
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "../include/global.h"
#include "../include/logger.h"

#define TRUE 1
#define MSG_SIZE 256
#define BUFFER_SIZE 256


int connect_to_host(char *server_ip, int server_port);
char* get_ip(int client_port, char ip[100]);
 /**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */



struct node
{
	int list_id;
	char hostname[1024];
	char ip_add[INET_ADDRSTRLEN];
	char port_no[20];
	struct node* next;
};
char *get_ip(int client_port, char ip[100])
{	
	int sockfd;
	struct sockaddr_in udp_addr,client_addr;				
	char client_ip[100];
	
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0)
	{
	cse4589_print_and_log("ERROR in socket creation\n");
	}
				
	memset(&udp_addr,0, sizeof udp_addr);	
	udp_addr.sin_family = AF_INET;
    	udp_addr.sin_addr.s_addr =inet_addr("8.8.8.8");// htonl(INADDR_ANY);
    	udp_addr.sin_port = htons(client_port);    
	
	connect(sockfd, (struct sockaddr *)&udp_addr, sizeof(udp_addr));
	int addr_len = sizeof client_addr;

	if(!(getsockname(sockfd,(struct sockaddr *)&client_addr,&addr_len )))
	{
		inet_ntop(client_addr.sin_family, &client_addr.sin_addr, client_ip, sizeof client_ip);//taken from beej guide
		strcpy(ip,client_ip);
		return (char *)ip;
	}
	
}


bool isValidIpAddress(char *ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result != 0;
}

int client(int client_port)
{
/*	if(argc != 3) {
		printf("Usage:%s [ip] [port]\n", argv[0]);
		exit(-1);
	}*/
	
	char ip[100] ="";
	char *client_ip	= get_ip(client_port, ip);

	struct node* head;
    	head = (struct node *) malloc( sizeof(struct node) );
    	head = NULL;	
	int server;
	

	int flag = 0;
	while(TRUE)
	{
		//printf("\n[PA1-Client@CSE489/589]$ ");
		//fflush(stdout);
		

		char *msg = (char*) malloc(sizeof(char)*MSG_SIZE);
    		memset(msg, '\0', MSG_SIZE);
		if(fgets(msg, MSG_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to msg
			exit(-1);
		
		int len=strlen(msg);	
			
	
		if(msg[len-1]=='\n')
			msg[len-1] = '\0';
		
		if(!strncmp(msg,"AUTHOR", strlen("AUTHOR")))
		{
			cse4589_print_and_log("[%s:SUCCESS]\n", msg);
			cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "trathore");
			cse4589_print_and_log("[%s:END]\n", msg);
		}
		
		if(!strncmp(msg,"IP", strlen("IP")))
		{

			cse4589_print_and_log("[%s:SUCCESS]\n", msg);
			cse4589_print_and_log("IP:%s\n", client_ip);
			cse4589_print_and_log("[%s:END]\n", msg);

		}
		

		if(!strncmp(msg,"PORT",strlen("PORT")))
		{
			cse4589_print_and_log("[%s:SUCCESS]\n", msg);
			cse4589_print_and_log("PORT:%d\n", client_port);
			cse4589_print_and_log("[%s:END]\n", msg);
		}
		
		/* Initialize buffer to receieve response */
        	char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
        	memset(buffer, '\0', BUFFER_SIZE);

		char *command = "LOGIN";
		//if(!strncmp(msg,"LOGIN",strlen("LOGIN")))
		if(strstr(msg, command) != NULL)
		
		{	flag=1;	
			char *m[3];
			int n=0;
			char *s1, *save_ptr;			
			char *ip = msg;
  			s1 = strtok_r(ip," ", &save_ptr);
			while (s1 != NULL) 
			{
				m[n] = s1;
				s1 = strtok_r(NULL, " ", &save_ptr);
				n++;
			}						
		
			
			char *server_ip;
			server_ip = (char *) malloc ( 100 );
			strcpy(server_ip, m[1]);
			int server_port = atoi(m[2]);

			//Checking exception for ip address
			if (!isValidIpAddress(server_ip))
			{
				perror("Invalid IP");			
			}
			
			//Checking exception for server port
			struct sockaddr_in server_addr;
			server_addr.sin_family = AF_INET;
    			server_addr.sin_addr.s_addr = inet_addr(server_ip);
    			server_addr.sin_port = htons(server_port);
			if (bind(server, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) 
    			{
    				perror("Invalid Port");
    			}			
			

				
			//printf("server_ip %s, server_port %d\n",server_ip,server_port);
			server = connect_to_host(server_ip, server_port);
			//printf("\nSENDing it to the remote server ... ");
			
   		
			char* tmp_string = (char*) malloc(sizeof(char)*BUFFER_SIZE);//malloc(20* sizeof(char)); /*enough room for the biggest integer*/
			snprintf(tmp_string, BUFFER_SIZE,"%d",client_port); /*can I avoid using 20 here*/

			send(server, tmp_string, strlen(tmp_string), 0);
			//	printf("Port No Sent: %s\n", tmp_string);
			//fflush(stdout);
			/*if(send(server, msg, strlen(msg), 0) == strlen(msg))
				printf("Command Sent!\n");
			fflush(stdout);*/
		
			

			if(recv(server, buffer, BUFFER_SIZE, 0) >= 0)
			{
				printf("Server buFFER 1 responded: %s\n", buffer);
				
				printf("Server buFFER SIZE responded: %lu\n", sizeof buffer);
				fflush(stdout);
	
				
				
				char *pch1, *pch2, *save_ptr1, *save_ptr2;
  				char *str = buffer;
  				pch1 = strtok_r(str,"+", &save_ptr1);
 			 	
  				while (pch1 != NULL) 
				{	
					struct node *cl_node= (struct node*) malloc(sizeof(struct node));
					char *a[3];
					int j=0;
   					pch2 = strtok_r(pch1, " ", &save_ptr2);
    					while (pch2 != NULL) 
					{	                                                                               
						a[j] = pch2;
      						//printf ("PCH2%s\n",pch2);
      						pch2 = strtok_r(NULL, " ", &save_ptr2);
						j++;
    					}
					
					strcpy(cl_node->hostname, a[0]);
					strcpy(cl_node->ip_add, a[1]);
					strcpy(cl_node->port_no, a[2]);
					cl_node->next = NULL;

		                	struct node *temp=head;
                			if(head==NULL)
	        	        		head = cl_node;
						//printf("Case of head null\n");}
					else
	                		{
        	        			while(temp->next!=NULL)
	                				temp = temp->next;

		                		temp->next = cl_node;
						temp = cl_node;
					//printf("Not null case");
	                		}

					//cse4589_print_and_log("%-5d%-35s%-20s%-8d\n",j , cl_node->hostname, cl_node->ip_add, atoi(cl_node->port_no));

    					pch1 = strtok_r(NULL, "+", &save_ptr1);
					//printf ("PCH1%s\n", pch1);
  				}		
				

			}	memset(buffer, 0, BUFFER_SIZE);
		}

		
		if(!strncmp(msg,"LIST",strlen("LIST")))
		{	
			if (flag==1)
			{	
			int i=0;
			struct node* current;
			current = (struct node *) malloc( sizeof(struct node) );
			current = head;
			cse4589_print_and_log("[%s:SUCCESS]\n", msg);
			while(current!=NULL)
			{
				i++;
				cse4589_print_and_log("%-5d%-35s%-20s%-8d\n",i , current->hostname, current->ip_add, atoi(current->port_no));
				current = current->next; 
			}
			cse4589_print_and_log("[%s:END]\n", msg);
		}	}

		if(!strncmp(msg,"REFRESH",strlen("REFRESH")))		
		{	if (flag ==1){
			//memset(head, 0,sizeof(struct node) );
			send(server, msg, strlen(msg), 0);
		
			/* Initialize buffer to receieve response */
        		//char *buffer1 = (char*) malloc(sizeof(char)*BUFFER_SIZE);
        		//memset(buffer1, '\0', BUFFER_SIZE);
			

			if(recv(server, buffer, BUFFER_SIZE, 0) >= 0)
			{
				printf("Server responded:  %s\n", buffer);
				fflush(stdout);
			}

		}	}
		
	} 
}
int connect_to_host(char *server_ip, int server_port)
{
    int fdsocket, len;
    struct sockaddr_in remote_server_addr;

    fdsocket = socket(AF_INET, SOCK_STREAM, 0);
    if(fdsocket < 0)
       perror("Failed to create socket");

    bzero(&remote_server_addr, sizeof(remote_server_addr));
    remote_server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, server_ip, &remote_server_addr.sin_addr);
    remote_server_addr.sin_port = htons(server_port);
		
    if(connect(fdsocket, (struct sockaddr*)&remote_server_addr, sizeof(remote_server_addr)) < 0)
        perror("Connect failed");

    return fdsocket;
}
