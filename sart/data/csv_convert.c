#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BUFF_SIZE 60


void strip_ext(char *fname)
{
    char *end = fname + strlen(fname);

    while (end > fname && *end != '.') {
        --end;
    }

    if (end > fname) {
        *end = '\0';
    }
}

long getKeyVal(

	char* buffer,
	long N,
	long offset,
	const char* key,
	char* val,
	const char delim

){

	long i,j;

	int keyLen = strlen(key);

	int found=0;


	for (i=offset;i<N-keyLen;i++)
	{

		if(strncmp(key,buffer+i,keyLen)==0)
		{
			found =1;
			//key match!
			i=i+keyLen; //skip over key -> lands on a ':'

			i=i+1;   //skip over ':'

			//read values until ',' or '}'
			j=0;

			while( !(buffer[i+j]==delim || buffer[i+j]=='}') && (i+j)<N)
			{
				val[j]= buffer[i+j];
				j++;
			}

			val[j]='\0';

			i=i+j+1; //past the delim or }

			break;
		}
	}

	if(found)
		return i;  //returns starting point for next key

	return -1;  //nothing found
}



int main( int argc, char *argv[] ) {

	FILE* fptr;

	long sz,i,k,j,n;

	char filename[100];
	char outputfilename[100];
	char* filebuffer = NULL;

	char val[BUFF_SIZE];
	char symbol[BUFF_SIZE];
	char reaction[BUFF_SIZE];


	//keys..
	const char* user = "\"pid\"";
	const char* group = "\"group\"";
	const char* state = "\"state\"";  //values: ctl exp
	const char* date = "\"date\"";
	const char* omit = "\"omit\"";
	const char* block = "\"block\"";  //fp,tp,tn,fn
	const char* spacer = "\"spacer\""; //sym, react_ms

	const char* sym = "\"sym\"";
	const char* rt = "\"rt\"";

	const char* fp = "\"fp\"";
	const char* tp = "\"tp\"";
	const char* tn = "\"tn\"";
	const char* fn = "\"fn\"";


	//targets
		const char* t1 = "\"star\"";
	  const char* t2 = "\"blue\"";
	 	const char* t3 = "\"c\"";
		const char* t4 = "\"3\"";


	if(argc==2)
	{

		printf("\n\t Filename: %s\n",argv[1]);

		sprintf(filename,"%s",argv[1]);


		fptr = fopen(filename, "r");  //append?
		if(!fptr)
		{
			printf("\n\t Could not open file\n");
			return 0;
		}

		strip_ext(filename);
		sprintf(outputfilename,"%s.csv",filename);

		fseek(fptr, 0L, SEEK_END);
			sz = ftell(fptr);
		fseek(fptr, 0L, SEEK_SET);


		filebuffer =  (char*)calloc(sz,1);

		fread(filebuffer,sz,1,fptr); //copy file to buffer

		fclose(fptr);  //close original file

		printf("\n\t File size %lu bytes\n",sz);

		printf("\n----------------------------- \n");
		printf("----------------------------- \n");


		//write output to temp file
	//	fptr = fopen("temp.csv","w");
			fptr = fopen(outputfilename,"w");



		i=0;

		getKeyVal(filebuffer,sz, i, user, val,',');
		printf("\n PID:\t%s\n",val);

		fprintf(fptr,"%s,%s\n", user,val);

		getKeyVal(filebuffer,sz, i, group, val,',');
		printf("\n Group:\t%s\n",val);

		fprintf(fptr,"%s,%s\n", group,val);

		getKeyVal(filebuffer,sz, i, date, val,',');
		printf("\n Timestamp:\t%s\n",val);

		fprintf(fptr,"%s,%s\n\n\n", date,val);



		printf("\n----------------------------- \n");
		printf("----------------------------- \n");

		//skip meta data
		i  = getKeyVal(filebuffer,sz,i,spacer,val,',');
		printf("\n Spacer:\t%s, found at i:%ld\n",val,i);



		do{
				i =	getKeyVal(filebuffer,sz,i,block,val,','); //get a block
				long count =0;
				double rsum = 0;
				long gap = 0;
				long max_gap=0;
				long min_gap=20000;

				if(i>0)
				{

					printf("block %s found at %ld\n",val,i);
					fprintf(fptr,"\n\n%s,%s\n", block,val);

					i = getKeyVal(filebuffer,sz, i, state, val,',');
					printf("\n State:\t%s\n",val);
					fprintf(fptr,"%s,%s\n", state,val);

					//i = getKeyVal(filebuffer,sz, i, omit, val,'}');
					i = getKeyVal(filebuffer,sz, i, omit, val,']');  //excludes closing bracket, so opening bracket can be ditched too!
					printf("\n Omit:\t%s\n",val+1);
					//remove closing brackets...
					fprintf(fptr,"%s,%s\n", omit,val+1);  //+1 skip 1st character


					k = getKeyVal(filebuffer,sz,i,fp,val,','); //get near end location of block
			//		printf("fp found at %ld\n",k);

		/*for column-wise write*/
		/*
				fprintf(fptr,"Symbol,React(ms)\n");

					j=1;
					do{
							i = getKeyVal(filebuffer,sz,i,rt,reaction,',');
							if(i>0 && i<k){
								i = getKeyVal(filebuffer,sz,i,sym,symbol,',');
								printf("\n %s:\t%s:\n",symbol,reaction);
								fprintf(fptr,"%s,%s\n", symbol,reaction);
							}else{j=0;}
					}while(j);
*/

		/*row -wise write*/
				j=i;  //original start, looking for symbols...
				fprintf(fptr,"symbol");
				printf("Symbol");
				i = getKeyVal(filebuffer,sz,i,sym,symbol,',');
				while(i>0 && i<k){
								printf(",%s",symbol);
								fprintf(fptr,",%s", symbol);
								i = getKeyVal(filebuffer,sz,i,sym,symbol,',');



								gap++;

								if(strcmp(symbol,t1)==0 || strcmp(symbol,t2)==0 || strcmp(symbol,t3)==0 || strcmp(symbol,t4)==0 ){
								/*found a target symbol*/
										gap--; //gap is index-1
										rsum = rsum + (double)(gap-rsum)/(count+1);

										if(gap<=min_gap)
												min_gap =gap;

										if(gap>=max_gap)
												max_gap = gap;

										gap=0;
										count++;
								}


				}



				i=j; //reset i to start, looking for react(ms)...
				fprintf(fptr,"\nreact(ms)");
				printf("\nReact(ms)");
				i = getKeyVal(filebuffer,sz,i,rt,reaction,',');
				while(i>0 && i<k){
								printf(",%s",reaction);
								fprintf(fptr,",%s", reaction);
								i = getKeyVal(filebuffer,sz,i,rt,reaction,',');
				}

				fprintf(fptr,"\n");
				printf("\n");
				i=k; /*skip to end of block (just before fp,tp,tn,fn)*/


				printf("\nAverage gap %f",rsum);
				fprintf(fptr,"%s,%f\n", "Gap(avg)",rsum);

				printf("\nMin gap %lu",min_gap);
				fprintf(fptr,"%s,%lu\n", "Gap(min)",min_gap);

				printf("\nMax gap %lu",max_gap);
				fprintf(fptr,"%s,%lu\n", "Gap(max)",max_gap);



			/*	getKeyVal(filebuffer,sz, i, fp, val,',');*/ //already stored in val
				printf("\n fp:\t%s\n",val);
				fprintf(fptr,"%s,%s\n", fp,val);

				getKeyVal(filebuffer,sz, i, fn, val,',');
				printf("\n fn:\t%s\n",val);
				fprintf(fptr,"%s,%s\n", fn,val);


				getKeyVal(filebuffer,sz, i, tp, val,',');
				printf("\n tp:\t%s\n",val);
				fprintf(fptr,"%s,%s\n", tp,val);

				getKeyVal(filebuffer,sz, i, tn, val,',');
				printf("\n tn:\t%s\n",val);
				fprintf(fptr,"%s,%s\n", tn,val);




			}
		}while(i>0); //blocks

		fclose(fptr);

		if(filebuffer!=NULL)
			free(filebuffer);

	}//if(argc==2)

    return 0;
}
