#include <arpa/inet.h>  //inet_ntop,inetpton
#include <unistd.h>     //read,write,close
#include <stdio.h>      //printf,scanf
#include <stdlib.h>     //atoi,exit
#include <string.h>     //strlen,memset,memcmp,memmove

#define BUFF 256
#define SA struct sockaddr 
#define SERVER_IP "10.0.2.15"

//Ahmed Khalid Elhousseiny 117190
//Huthaifa Mohammed Qashou 122898

int main( int argc, char **argv ) //SERVER
{
	//Variables
		int        	e = 1, listenfd, connfd;
		struct     	sockaddr_in servaddr, cliaddr;
		char       	recvline[BUFF], answer[BUFF];
		socklen_t	addrlen;


	//Arguments
		if (argc !=2){
			fprintf( stderr, "Usage: server Port_number\n");
			exit(1);
		}

		//Server Port number
		int        	server_port	= atoi(argv[1]);
		if (server_port < 45001 || server_port > 65535) {
			fprintf(stderr, "Server: Port_number is Invalid\n" );
			exit(1);
		}


	//Socket file descriptor
		//IPv4 TCP
		if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			fprintf(stderr,"Server: Can't open a stream socket\n" );
			exit(1);
		}


	//Server Info
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family		= AF_INET;
		servaddr.sin_port		= htons(server_port);
		inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr.s_addr);


	//Binding local addresses
		if (bind(listenfd, (SA*)&servaddr,
			sizeof(servaddr) ) < 0) {
			fprintf(stderr,"Server: Binding error\n");
			exit(1);
		}


	//Listening on port
		if (listen(listenfd, 5) < 0) {	//backlog = 5
			fprintf(stderr,"Server: Listen error\n");
			exit(1);
		}


	//Accepting connections into a new file descriptor
		if ((connfd = accept(listenfd, (SA*)&cliaddr, &addrlen)) < 0) {
			fprintf(stderr,"Server: Can't accept error\n");
			exit(1);
		}


	//Get the client info
		if (getpeername(connfd, (SA*)&cliaddr, &addrlen)) {
			fprintf(stderr,"Server: Can't get the client info\n");
			exit(1);
		}


	for (;;) {

		printf("Server: Waiting for client messages.\n");
		
		//Empty the buffers
		memset(recvline, 0, sizeof(recvline));
		memset(answer, 0, sizeof(answer));
			
		//Receiving the equation from the client
		e = read(connfd, recvline, sizeof(recvline));
			if(e == -1) {
				fprintf(stderr,"Server: Read error\n");
				exit(1);
			}

		//Print the received line buffer
		printf	("%s%s%s","Server: Received \"", recvline,
				 "\" from the client");

		//Print the client information
		inet_ntop(AF_INET, &cliaddr.sin_addr, answer, sizeof(answer));
			printf	(" (%s, %d)\n",answer, ntohs(cliaddr.sin_port));
			memset(answer, 0, sizeof(answer));

		//Exit if "exit" was received
		if (memcmp("exit", recvline, 4) == 0) { break;}

		//Solve the Equation
		int 	x,y;
		char 	op = ' ';

		//Transform the received line
			//get the firt operant
			sscanf 	(recvline,"%d", &x);
			//Search for the operator
			while(op != '+' && op != '-' && op != '*' && 
				  op != '/' && op != '\0' ) {	
				sscanf (recvline,"%c%d", &op ,&y);
				memmove (recvline, recvline + 1, BUFF);
			}
		
		//Solving the Equation
		if (op == '+') {
			sprintf(answer, "%d", x + y);
		}
		else if (op == '-') {
			sprintf(answer, "%d", x - y);
		}
		else if (op == '*') {
			sprintf(answer, "%d", x * y);
		}
		else if (op != '/') {
			if(y == 0)
				sprintf(answer, "%d", x / y);
			else
				sprintf(answer, "%s", "Why would you divid by Zero");
		}
		else
			sprintf(answer, "%s", "What are you doing to me T_T");

		//Send the Solution 		
		printf	("%s%s%s\n", "Server: Sending \"", answer, 
				 "\" to the client");
		e = write(connfd, answer, sizeof(answer));
			if(e == -1) {
				fprintf(stderr,"Server: Can't Write\n");
				exit(1);
			}

	}
	//Close the Socket
		printf	("Server: Exiting\n");
		close(connfd);

	exit(0);
}
