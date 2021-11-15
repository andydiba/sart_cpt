#ifndef MSG_HANDLER_H
#define MSG_HANDLER_H


#define ROOT_TEMPLATE "/home/pi/Documents/sart/templates"

#define DATA_TEMPLATE "/home/pi/Documents/sart/data/"

#define REQ_BUFFSIZE 256  	//buffer initial size and incremental growth
#define MAX_URL 2048

#define MAX_POST 50000


#define GET 0
#define POST 1

struct response{
	char header[512];     //header
	unsigned int headLen; //store the lenght of header
	char* data;			  //data/response (raw)
	unsigned int dataLen; //data length
};

struct request{
	char* BUFF;
	unsigned long buffSize;
	char url[MAX_URL];
	
	char * post;
	//char post[MAX_POST];
};

void clearResponse(struct response* res);
void clearRequest(struct request* req);


struct response setResponse(struct request , void (*callback)(char*,char*) );

void getRequest(int new_socket, struct request* req);
int getURL(char* buff, char* url);

unsigned long loadFile(char* rootpath, struct response* res, int mode);


unsigned int setHeader(char* url, char* header, unsigned int dataL);
const char* get_filename_ext(const char *filename);


long getPostData(char* buff, unsigned int N);
//int getPostData(char* post, char* buff, unsigned int N);

#endif
