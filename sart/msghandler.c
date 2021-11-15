#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//#include <dirent.h>

//#include <time.h>

#include "msghandler.h"
#include "utilities.h"


void clearResponse(struct response* res){
	//reset buffer free memory
	res->header[0]='\0';
	res->headLen = 0;
	if(res->data!=NULL)
		free(res->data);

	res->dataLen= 0;
	res->data = NULL;

}
void clearRequest(struct request* req){

	req->url[0] ='\0';
	
	if(req->BUFF!=NULL)
	free(req->BUFF);

	req->BUFF = NULL;
	req->buffSize=0;
	
	
	req->post = NULL;
	
	//req->post[0]='\0';

}



//Note: does not cover all mime types (add extra features when required...)
unsigned int setHeader(char* url, char* header, unsigned int dataL)
{
	
	const char* ext = get_filename_ext(url);
	int pos;
	pos=0;
	
	if(!ext){	
		warningMsg("\n\tNo file extension");  //not likely to get to this point as file will not exist
	}else{
		
		printf("\tMIME: (%s)\n", ext);
		
		if(strcmp(ext,"png")==0){
			pos = sprintf(header,"HTTP/1.1 200 OK\r\nConnection: Close\r\nExpires: 0\r\n");//HTTP/1.1 100 Continue
			pos += sprintf(header+pos,"Content-Type: image/png; charset=UTF-8\r\n");
		}else{
			pos = sprintf(header,"HTTP/1.1 200 OK\r\nConnection: Close\r\nCache-Control: no-store, must-revalidate\r\nExpires: 0\r\n");//HTTP/1.1 100 Continue
		//pos += sprintf(header+pos,"Content-Type: text/%s; charset=UTF-8\r\n",ext);  //js->javascript
		
		if(strcmp(ext,"html")==0)
			pos += sprintf(header+pos,"Content-Type: text/html; charset=UTF-8\r\n");
		else if (strcmp(ext,"css")==0)
			pos += sprintf(header+pos,"Content-Type: text/css; charset=UTF-8\r\n");
		else if (strcmp(ext,"js")==0)
			pos += sprintf(header+pos,"Content-Type: text/javascript; charset=UTF-8\r\n");
		else//default assume plain text
			pos += sprintf(header+pos,"Content-Type: text/plain; charset=UTF-8\r\n");
		}
		
		pos += sprintf(header+pos,"Content-Length: %u\r\n\r\n",dataL);
			
	}
	return pos;
	
}



struct response setResponse(struct request req, void (*callback)(char*,char*) ){
	
	struct response res;
	res.data=NULL;//initialisation
	clearResponse(&res);
	
	char rootpath[MAX_URL+100];

	sprintf(rootpath,"%s%s",ROOT_TEMPLATE,req.url);
		
	colorMsg("(server):",MAGENTA_B);
	printf("\tFile path: %s\n",rootpath);


	res.dataLen = loadFile(rootpath,&res,1);
	
	if(res.dataLen==0)
	{
		errorMsg("File Not Found!");
		res.headLen = sprintf(res.header,"HTTP/1.1 404 Not Found\r\n\r\n");
		return res; 

	}else{
		
		  //set appropriate header based on mime type
		 res.headLen = setHeader(req.url, res.header, res.dataLen);
	
		 callback(req.url,req.post);
	
	}
		
	return res;
	
}



void getRequest(int new_socket, struct request* req){

	unsigned long nBytes=1;
	unsigned long total_read=0;
	int done = 0;
	
	
	clearRequest(req);

	while(!done)
	{
		req->BUFF = (char*)realloc(req->BUFF,total_read+REQ_BUFFSIZE);
		nBytes = read( new_socket ,req->BUFF+total_read, REQ_BUFFSIZE);


		total_read+=nBytes;

		if(nBytes<=0)
		{
			if(nBytes==0)
			done =1;
			else
			perror("read");
		}
		if(nBytes<REQ_BUFFSIZE)
			{
				printf("\nfinished on %lu\n",nBytes);
				done=1;//could try a non-blocking read again to see if eof?
			}
	}

		
	
	req->BUFF[total_read] ='\0';  //for binary files NULL may have already occurred in the data, can't treat as string!!
	req->buffSize = total_read;
	//printf("\n%s\n",req->BUFF);
	
	colorMsg("(client):",CYAN_B);
	
	long k=0;
	//get url...and check if post data exists
	if(getURL(req->BUFF,req->url)==POST)
	{
		
		//set post to start location in buffer
		
		
		//k=getPostData(req->post, req->BUFF,total_read);
		k=getPostData(req->BUFF,total_read);
		
		if(k>0)
		req->post = req->BUFF+k;
		
		if(req->post != NULL){
		
			if( (total_read -k) >50){
			//if(k>50){
				
				printf("\tPOST: %.25s\t...\t",req->post); 
			//	//printf("%s\n", req->post + k - 20);
				printf("%s\n", req->post +total_read -k - 20);
				
				//printf("\tPOST: %s\n",req->post);
				
			}else
			 printf("\tPOST: %s\n",req->post);	
		 
		}
	
	}
	
	printf("\tURL: %s \t (bytes):%lu\n",req->url, total_read);
	
	
					
}



/* Extracts and returns url with leading '/'. Assumes GET method  */
int getURL(char* buff, char* url)
{	
	
	unsigned int i,k;
	int http_found;
	
	int method=GET;
	
	i=4; //GET /      4 is index of first fwd-slash

	if(strncmp(buff,"POST",4) == 0)
	{i=5; method=POST;}
	
	k=0;
	
	//include space before HTTP, to fill with NULL
	http_found = ( ( strncmp(buff+i,"?",1)==0 ) || ( strncmp(buff+i," HTTP/1.1",8)==0 ) );

	while( !http_found  &&  k < MAX_URL-1) 
	{
			url[k] = buff[i];
			k++;
			i++;
		
		http_found = ( ( strncmp(buff+i,"?",1)==0 ) || ( strncmp(buff+i," HTTP/1.1",8)==0 ) );
	}
	

	if(http_found)
	{
		url[k]='\0';
  	}else{
		url[0]='\0';
	}
	
	return method;
}

long getPostData(char* buff, unsigned int N){
	
	long i;
	
	long k=0;
	
	for(i=0;i<N;i++)
	{	
		if( strncmp(buff+i,"\r\n\r\n",4)==0 )  //post data after double line break
		{	i=i+4;  //skip the line break
			k=i;
			return k;
		}
		
	}
	
	return k;
	
}
/*
int getPostData(char* post, char* buff, unsigned int N){
	
	unsigned int i;
	int copy =0;
	int k=0;
	
	for(i=0;i<N && N<MAX_POST;i++)
	{	
		if( strncmp(buff+i,"\r\n\r\n",4)==0 )
		{copy=1; i=i+4;}
		
		if(copy)
		{post[k]=buff[i];
			k++;
		}
			
	}
	post[k]='\0';
	
	return k;
	
}
*/

const char* get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}


unsigned long loadFile(char* rootpath, struct response* res, int mode){

	FILE* fp= NULL;
	unsigned long L;

	if(!mode) 						//0 = text, else binary
		fp = fopen(rootpath,"r");
	else
		fp = fopen(rootpath,"rb");

	if(fp!=NULL)
	{

		fseek(fp, 0, SEEK_END);  //goes to position of EOF (0-indexing)==number of actual data bytes
		L = ftell(fp);
		fseek(fp, 0, SEEK_SET);   //reset file pointer to start of file
		//assumes data=NULL
		res->data =(char *)realloc(res->data,L);	//sizeof(char) =1 so Lx1 will add null character at end
		fread(res->data, 1,L,fp);
		fclose(fp);
			
		
	}else{
		return 0;
	}

	return L;
}


