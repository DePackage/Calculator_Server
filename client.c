#include <arpa/inet.h>  //inet_addr,inet_ntop,inetpton
#include <unistd.h>     //read,write, close
#include <stdio.h>      //printf,getchar
#include <stdlib.h>     //atoi,exit
#include <string.h>     //memset,strlen

#define BUFF 256
#define CLIENT_PORT 55190
#define SA struct sockaddr

//Ahmed Khalid Elhousseiny 117190
//Huthaifa Mohammed Qashou 122898

int main(int argc, char **argv)	//CLIENT
{
	//Variables
		int        	sockfd;
		in_addr_t  	server_ip;
		struct     	sockaddr_in servaddr, cliaddr;
		char       	recvline[BUFF], expression[BUFF], c;


	//Arguments
		if (argc != 3){
			fprintf(stderr,"Usage: Client IP_address Port_number\n");
			exit(1);
		}

		//Server IP adress
		int	e = 	inet_pton(AF_INET, argv[1], &server_ip);
			if (e == 0) {
			fprintf(stderr,"Client: IP_address is Invalid\n");
			exit(1);
			}

		//Server Port number
		int        	server_port = atoi(argv[2]);
		if (server_port < 45000 || server_port > 65535){
			fprintf(stderr,"Client: Port_number is Invalid\n");
			exit(1);
		}


	//Socket File Descriptor
		//IPv4 TCP
		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			fprintf(stderr,"Client: Couldn't open stream socket\n");
			exit(1);
		}


	//Client Info
		memset(&cliaddr, 0, sizeof(cliaddr));
		cliaddr.sin_family      = AF_INET;
		cliaddr.sin_port        = htons	(CLIENT_PORT);
		cliaddr.sin_addr.s_addr = htonl (INADDR_ANY);
		//IP from kernal on connect


	//Binding local addresses
		if (bind(sockfd, (SA*)&cliaddr, sizeof(cliaddr)) < 0) {
			fprintf(stderr,"Client: Couldn't Bind\n");
			exit(1);
		}
	
	//Server Info
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family      = AF_INET;
		servaddr.sin_port        = htons(server_port);
		servaddr.sin_addr.s_addr = server_ip;


	//Connect to Server
		if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) < 0) {
			fprintf(stderr,"Client: Couldn't Connect\n");
			exit(1);
		}


	for (;;) {

		//Empty the buffers
		memset(recvline, 0, BUFF);
		memset(expression, 0, BUFF);

		//Get the Input
		printf	("Client: Please enter your expression:\n");
		e = 0;
		while ((c = getchar()) != '\n'){ expression[e++] = c; }
			printf	("%s%s%s\n", "Client: Sending \"", expression,
					 "\" to the server");

		//Send the Input to the Server
		e = write(sockfd, expression, sizeof(expression));
			if (e == -1) {
				fprintf(stderr,"Client: Couldn't write.\n");
				exit(1);
			}

			//Send exit to the Server
			if (memcmp("exit", expression, 4) == 0){ break;}

		//Read the Reply
		e = read(sockfd, recvline, sizeof(recvline));
			if(e == -1) {
				fprintf(stderr,"Client: Couldn't Read\n");
				exit(1);
			}
			printf	("%s = %s", expression, recvline);	

		//Print Server Info
		printf	(" (%s,%d).\n",inet_ntop(AF_INET, &servaddr.sin_addr,
			expression, sizeof(expression)), ntohs(servaddr.sin_port));
	}

	//Close the Socket
		printf	("Client: Exiting\n");
		close	(sockfd);

	exit(0);
}
