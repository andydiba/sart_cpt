// Server side C program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <poll.h>
#include <signal.h>


#include "msghandler.h"
#include "utilities.h"

#define PORT 8080  //Firewall: use sudo ufw allow 8080/tcp



void mycallback(char* url, char* post)
{
	static int array_on = 0;  //off(default)
	
	static pid_t child = -1;
	
	
	
	//Data posted
	if( strcmp(url,"/uploader.txt")==0)  //data in post
	{
		if(post==NULL)
		{
			errorMsg("no post");
			return;
		}
		
		printf("\tMY POST: %s\n",post);
		char *data = NULL;
		char *pid = NULL;
		/* get the first token; test scores are after the $ sign */
		pid = strtok(post,"$");
		printf("\tMY PID: %s\n",pid);
		
		
		
		
		if(pid)
		{
			data = strtok(NULL,"$");
			
			
   
			char datapath[MAX_URL+100];
			sprintf(datapath,"%s%s",DATA_TEMPLATE,pid);
			
			FILE* fp;
			
			fp = fopen(datapath, "w");  //append?
			if(fp) 
			{
				colorMsg("\tWriting output file", MAGENTA);
				fprintf(fp, "%s",data);
				fclose(fp);
			}else
				errorMsg("could not open file");
      
		}
	}
	
		
	if( (strcmp(url,"/array_on.txt")==0) && !array_on )
	{	
			array_on = 1;
			printf("\nswitching array on...");
			child = fork();
			
			if(child==0)
			{
					//char *args[] = {"./helloWorld",NULL};
					char *args[] = {"/home/pi/Documents/pigpio/pulse","50.0","40000","1200",NULL};
					execv(args[0],args);
					
					warningMsg("forked execv failed");
				
			}else if(child<0)
				errorMsg("fork failed!");
				
	
	}else if( (strcmp(url,"/array_off.txt")==0)  && array_on && child>0 )
	{
			array_on = 0;
			printf("\nshuting down array...");
			
			kill(child,SIGINT);
			
			child = -1;
		
	
	}else if( (strcmp(url,"/array_off.txt")==0) || (strcmp(url,"/array_on.txt")==0)){
			
			//array is already on/off...request ignored 
			warningMsg("Array request ignored");
	}
		
}	






// Add a new file descriptor to the set
void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size) {
        *fd_size *= 2; // Double it

        *pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read

    (*fd_count)++;
}

// Remove an index from the set
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count-1];

    (*fd_count)--;
}


int main(int argc, char const *argv[])
{

	int server_fd, new_socket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("In socket");
		exit(EXIT_FAILURE);
	}

	int enable = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    		perror("setsockopt(SO_REUSEADDR) failed");

	

	address.sin_family = AF_INET;  //IPv4
	address.sin_addr.s_addr = INADDR_ANY; //local host + network assigned IP;
	address.sin_port = htons( PORT );

	memset(address.sin_zero, '\0', sizeof address.sin_zero);

	//connect socket construct to address
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("In bind");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 10) < 0)//listen and keep queue of up to 10 requests
	{
		perror("In listen");
		exit(EXIT_FAILURE);
	}

	
	// Start off with room for 5 connections
    	// (We'll realloc as necessary)
   	int fd_count = 0;
    	int fd_size = 5;
    	struct pollfd *pfds = malloc(sizeof *pfds * fd_size);


	 // Add the listener to set
    	pfds[0].fd = server_fd;
		pfds[0].events = POLLIN; // Report ready to read on incoming connection

    	fd_count = 1; // For the listener



	while(1)
	{
		printf("\n\tWaiting for new connection on localhost:8080 ...fd_count:%d.\n\n",fd_count);

		int poll_count = poll(pfds, fd_count, -1);//no timeout
		

		if (poll_count == -1) 
		{
		    perror("poll");
		    exit(1);
		}

		// Run through the existing connections looking for data to read
        	for(int i = 0; i < fd_count; i++) 
			{
			 // Check if someone's ready to read
		    	if (pfds[i].revents & POLLIN) 
				{ // We got one!!

					if (pfds[i].fd == server_fd) 
					{
						// If listener is ready to read, handle new connection
						if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) //grab head of queue and copy into new_socket
						{
							perror("In accept");
							exit(EXIT_FAILURE);
						}else{
						
							add_to_pfds(&pfds, new_socket, &fd_count, &fd_size);
							printf("\tpoll server: new connection on socket %d..\n",new_socket);

						}
						
						
					}else {
						// If not a new connection from server_fd, we're just a regular client already connected

						//get client sent data...
						int sender_fd = pfds[i].fd;
					
						
						struct request Req;
						Req.BUFF = NULL;
						Req.post = NULL;

						struct response Res;
						

						getRequest(sender_fd,&Req);
						
						Res = setResponse(Req,mycallback);

						//callback will execute whatever needs doing...
					
						write(sender_fd, Res.header,Res.headLen);
						
						if(Res.dataLen!=0)
							write(sender_fd, Res.data,Res.dataLen);
					
					
						clearRequest(&Req);
						clearResponse(&Res);
					
					
						//Done? Close conection
						close(sender_fd);
						del_from_pfds(pfds, i, &fd_count);	
										
	
				}//handle client request
			}//check event
		}//eof for(i...pfds)
	}//eof while(1)

	close(server_fd);
	return (EXIT_SUCCESS);
}
