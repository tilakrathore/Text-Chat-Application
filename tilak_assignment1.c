/**
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/global.h"
#include "../include/logger.h"

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */

int server(int port_no);
int client(int client_port);

int main(int argc, char **argv)
{
	
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/*Clear LOGFILE*/
	fclose(fopen(LOGFILE, "w"));
	
	/*int i;
    	printf("argc %d\n",argc);

    	for(i=0;i<argc;i++)
    	{
        	printf("%s\n",argv[i]);
    	}*/
	

	/*char *server_ip;
	server_ip = (char *) malloc ( 100 );
	strcpy (server_ip, "192.168.1.187");*/
	///printf("%s\n", server_ip);

	/*Start Here*/
	int port_no;
	port_no = atoi(argv[2]);
	if(!strcmp(argv[1],"s"))		
		server(port_no);
		
	if(!strcmp(argv[1],"c"))

		client( port_no);

	return 0;
}
