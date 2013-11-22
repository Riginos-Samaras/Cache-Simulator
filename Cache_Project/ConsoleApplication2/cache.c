
/*****LIBRARIES*****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include<conio.h>
#include <malloc.h>
#include <time.h>
errno_t error;
/*****DEFINES*****/
typedef int bool;
#define true 1
#define false 0

/*****PARAMETERS*****/
int RAM_SIZE=0;
int	L1_CACHE_SIZE=0;
int L2_CACHE_SIZE=0;
int L1_ADDER=0;
int L2_ADDER=0;
int VICTIM_CACHE_SIZE=0;
bool VICTIM_CACHE=false;
bool L2_CACHE =false;
bool CACHE_MODE =false;	
int WORD_SIZE=0;
int WORDS_PER_BLOCK=0;
bool FULL_ASSOCIATIVE=false;
bool SET_ASSOCIATIVE=false;
bool DIRECT_MAPPED=false;
int DATA_FILL_NUMBER=0;
int SET_ASSOCIATIVE_WAYS=0;
int MAX_FLUSHES=0;
bool WRITE_BACK=false;
bool WRITE_THROUGH=false;
bool WRITE_ALLOCATE=false;
char POLICY[10]={NULL};
int READ_FROM_CACHE=0;
int READ_FROM_RAM=0;
int WRITE_TO_RAM=0;
int WRITE_TO_CACHE=0;


/*****FILES*****/
FILE *DataBase_w;               //Write to Data.txt
FILE *DataBase_r;				//Read from Data.txt
FILE *Parameter_r;				//Reads the Parameters from Parameter.txt 
FILE *Output_w;					//Writes the Output
FILE *Results_w;

/*****FUNCTIONS*****/
int random_number(int x);	//Returns a number >=0 and <x
char cacheAction();			//Returns a random characters from the list
int stringSize(char []);	//Input:a string Output: the number of the characters in string 
void fillDataBase(FILE*);	//Fills Data.txt with random cache action and random numbers  "R 10","W 9", "M 5"
void fillOutput(FILE*);		//Input:a FILE Output in file: puts random action and random number in this file
void decToInt(int);			//Input:a decimal number Output: binaryNumber[10] gets the binary numnber of the input
int getLine(FILE*);			//Gets a line from a FILE and puts this line into char FilesLine 
void equalStrings(char *str1,char *str2);//Takes 2 strings and equals str2 to str1
void printString(char *);				 //Input:a string Output: prints that String
void printBinaryNumber(int,int,int);
void parseWord(char*,int);				 //Input: an integer n and a line of characters and returns the n'th word
int log_2(int);							 //Input:a deciman number Output: the log of this number base 2
void findTagIndexBlockOffset(int);		     //Based on the Parameters finds the actual tag index and block offset of the current cache 
void findErrors();		//find the errors of Parameters
int isLog_2(int);		//returns 1 if an integer is log of 2 else returns 0
void getParameters(FILE*);
void guessParameter(char *,char *);
int isStringEqual(char *,char *);
int BintoInt(int *);
void enqueueCache(int ,int,struct Cache);
void printQueue(struct Cache);
void createCache(int,struct Cache *);
void createVictim(int,struct Cache *);
void initializer();
int enqueuePlace(int,int,int,int,int,int,struct Cache);
int guessCycles(int,int);
void free_Cache (struct Cache *chain);
int enqueueVictim( int,int,int,int,int);

struct Cache
{
 bool Valid;
 int Tag;
 int Index;
 int BlockOffSet;
 int Age;
 int Way;
 int Touched;
 int Dirty;
 struct Cache *head;
 struct Cache *tail;
};



int BinaryNumber[50];		//the binary number saved by the function decToInt
char FilesLine[100]={NULL};	//each line of the DataBase is saved in this String
char WordInLine[50]={NULL};	//the word taken each time from FilesLine
int LinesInDataBase=0;	//The number of lines filled in files
int MAX_NUMBER=0;		//The maximum number of address reference is put in DataBase
int Ram_bits=0;			//Ram bits are saved by that global variable 
int Cache_bits=0;		//Cache bits are saved by that global variable 
int Tag_bits=0;			//Tag bits are saved by that global variable 
int Index_bits=0;		//Index bits are saved by that global variable 
int BlockOffset_bits=0;	//Block offset bits are saved by that global variable 
int Way=0;				//The way in set assosiative that address is refered to
int full_associative_place;

char Action;
int Tag_cache;
int Index_cache;
int BlockOffset_cache;

/*****RESULT VARIABLES*****/
int Hits=0;
int Misses=0;
int ReadMiss=0;
int WriteMiss=0;
int ModifyMiss=0;
int ReadHit=0;
int WriteHit=0;
int ModifyHit=0;
int Cycles=0;
int compulsory=0;
int conflict=0;
int capacity=0;

/*****L1 AND L2 CACHE POINTERS*****/
struct Cache L1_Head;
struct Cache L1_Tail;

struct Cache L2_Head;
struct Cache L2_Tail;

struct Cache Victim_Head;
struct Cache Victim_Tail;

int main(int argc, char ** argv){
	int i;
	int flushNo=1;
	int Adder_1;
	int Adder_2;
	float performance;
	struct Cache *Temp1;
	struct Cache *Temp2;
	struct Cache *TempVictim;

	Temp1=(struct Cache*)malloc((sizeof(struct Cache)));//refers to L1_Tail
	Temp2=(struct Cache*)malloc((sizeof(struct Cache)));//refers to L2_Tail
	TempVictim=(struct Cache*)malloc((sizeof(struct Cache)));//refers to L2_Tail

	L1_Head.head=&L1_Tail;
	L1_Head.tail=NULL;
	L1_Head.BlockOffSet=-1;L1_Head.Index=-1;L1_Head.Tag=-1;


	L1_Tail.head=NULL;
	L1_Tail.tail=&L1_Head;
	L1_Tail.BlockOffSet=-1;L1_Tail.Index=-1;L1_Tail.Tag=-1;

	L2_Head.head=&L2_Tail;
	L2_Head.tail=NULL;
	L2_Head.BlockOffSet=-1;L2_Head.Index=-1;L2_Head.Tag=-1;


	L2_Tail.head=NULL;
	L2_Tail.tail=&L2_Head;
	L2_Tail.BlockOffSet=-1;L2_Tail.Index=-1;L2_Tail.Tag=-1;

	Victim_Head.head=&Victim_Tail;
	Victim_Head.tail=NULL;
	Victim_Head.BlockOffSet=-1;Victim_Head.Index=-1;Victim_Head.Tag=-1;


	Victim_Tail.head=NULL;
	Victim_Tail.tail=&Victim_Head;
	Victim_Tail.BlockOffSet=-1;Victim_Tail.Index=-1;Victim_Tail.Tag=-1;

	srand(time(NULL));			//helps to generate random number with rand()
	error=fopen_s(&Parameter_r,"Parameters.txt","r+");//opens Data.txt for filling
	printf("PARAMETERS:\n==========\n");
	getParameters(Parameter_r);
	
	printf("\nTotal Cycles:%d %d",L1_CACHE_SIZE);
	printf("\n\n");
	findErrors();
	error=fopen_s(&Results_w,"Results.txt","w+");//opens Data.txt for filling
	error=fopen_s(&DataBase_w,"Data.txt","w");//opens Data.txt for filling
	Adder_1=L1_ADDER;
	Adder_2=L2_ADDER;
	printf("\n\nRESULTS:\n");
	 
	while (Adder_1<=L1_CACHE_SIZE)
	{	
		while (Adder_2<=L2_CACHE_SIZE){
			findTagIndexBlockOffset(Adder_1);  //finds Tag Index and Block Offset of the current cache 
			fillDataBase(DataBase_w);				   //Random cache action and random numbers in Data.txt
			error=fopen_s(&DataBase_r,"Data.txt","r");//opens Data.txt for parsing
			error=fopen_s(&Output_w,"Output.txt","w+");//opens Output.txt for filling
			TempVictim=Victim_Head.head;
			createVictim(VICTIM_CACHE_SIZE,TempVictim);
			Temp1=L1_Head.head;
			createCache(Adder_1,Temp1);
			Temp2=L2_Head.head;
			createCache(L2_CACHE_SIZE,Temp2);
			//printQueue(L1_Head);
			//printQueue(L2_Head);
			for(i=0;i<LinesInDataBase;i++){//loops until the end of Data.txt
				fillOutput(Output_w);		//fills Output w
				if(Action=='R'||Action=='M'||Action=='W'){
					//createAddress();
					enqueueCache(i,Adder_1,L1_Head);
				}
				else{
					//printf(" \n");
					//printf("\nOUR CACHE BEFORE #%d FLUSH:\n",flushNo);
					//printf("======================");
					//printQueue(L1_Head);
					free_Cache(L1_Head.head);
					L1_Head.head=&L1_Tail;
					L1_Tail.tail=&L1_Head;
					flushNo++;
					createCache(Adder_1,Temp1);
				}
			}
			//printQueue();
			//printf("\n\nOUR CACHE AFTER INPUT:\n");
			//printf("=====================");
			//printQueue(L1_Head);

			printf("\n\n\n\nL1_CACHE_SIZE:%d L2_CACHE_SIZE:%d",Adder_1,Adder_2);
			printf("\t(T:%d,IN:%d,BO:%d)\n",Tag_bits,Index_bits,BlockOffset_bits);
			printf("================================");
			printf("\nreadmisses:%d readhits:%d\nwritemisses:%d writehits:%d\nmodifymisses:%d modifyhits:%d",ReadMiss,ReadHit,WriteMiss,WriteHit,ModifyMiss,ModifyHit);
			printf("\n\nTotal Misses:%d  Total Hits:%d",Misses,Hits);
			performance=(float)Hits/((float)Misses+(float)Hits)*100;
			printf("\nPerformance:%f %%\nFlushes:%d",performance,flushNo-1);
			printf("\nTotal Cycles:%d",Cycles);
			printf("\nMISSES:\tCompulsory:%d\n\tCapacity:%d\n\tConflict:%d",compulsory,capacity,conflict);

			fprintf(Results_w,"\n\n\n\nL1_CACHE_SIZE:%d L2_CACHE_SIZE:%d",Adder_1,Adder_2);
			fprintf(Results_w,"\t(T:%d,IN:%d,BO:%d)\n",Tag_bits,Index_bits,BlockOffset_bits);
			fprintf(Results_w,"============================");
			fprintf(Results_w,"\nTAG:%d Index:%d BlockOffSet:%d\n",Tag_bits,Index_bits,BlockOffset_bits);
			fprintf(Results_w,"\nreadmisses:%d readhits:%d\nwritemisses:%d writehits:%d\nmodifymisses:%d modifyhits:%d",ReadMiss,ReadHit,WriteMiss,WriteHit,ModifyMiss,ModifyHit);
			fprintf(Results_w,"\n\nTotal Misses:%d  Total Hits:%d",Misses,Hits);
			fprintf(Results_w,"\nPerformance:%f %%\nFlushes:%d",performance,flushNo-1);
			fprintf(Results_w,"\nTotal Cycles:%d",Cycles);
			fprintf(Results_w,"\nMISSES:\tCompulsory:%d\n\tCapacity:%d\n\tConflict:%d",compulsory,capacity,conflict);
			printQueue(L1_Head);
			printQueue(Victim_Head);
			free_Cache(L1_Head.head);
			
			Adder_2+=L2_ADDER;
			fclose(Output_w);

			L1_Head.head=&L1_Tail;
			L1_Tail.tail=&L1_Head;
			
			L2_Head.head=&L2_Tail;
			L2_Tail.tail=&L2_Head;
			
			Victim_Head.head=&Victim_Tail;
			Victim_Tail.tail=&Victim_Head;

			initializer();
			flushNo=1;
			fclose(DataBase_r);
			fclose(DataBase_w);
		}//end of while(Adder_2<=L2_CACHE_SIZE)
		Adder_1+=L1_ADDER;
		Adder_2=L2_ADDER;
	}//end of while(Adder_1<=L1_CACHE_SIZE)
	

	fclose(Parameter_r);
	fclose(Results_w);  
	_getch();	
	exit(0);	//exits 0 when everything gone right
}	//end of main()

//** Returns a number >=0 and <x **//
int random_number(int num){
    return rand()%num;//returns a random number from 0 to num-1
    }//end of random_number(int num)
//** Returns a random characters from the list **//
char cacheAction(){
    char Rand_c;
    int Rand_i;
    char List[]="RWMF";//gets a list with the actions
    Rand_i=random_number(stringSize(List));//gets a random n number from 0-3
    Rand_c=List[Rand_i];	//takes the character on the n'th place of the list
    return Rand_c;	//returns that character
}//end of cacheAction()
//** Input:a string Output: the number of the characters in string  **//
int stringSize(char A[]){
    int SizeOfList=0;
    while(1){//loops until it finds the last character
        if(A[SizeOfList]!='\0')
            SizeOfList++;//if its not a \0 gets to the next character
        else 
			break;
    }
    return SizeOfList;//returns the variable size of list 
}//end of stringSize(char A[])
//** Fills Data.txt with random cache action and random numbers  "R 10","W 9", "M 5" **//
void fillDataBase(FILE *DataBase){
        int i=0;				//variable used in for
		int flush_constant=0;	//that constant gets values 1,2 or 3
        char ch;				//variable used for characters
        int y;					
		int flush_max=MAX_FLUSHES;//max flush into Data.txt will be 1/6'th of total lines
		int flush_times=0;
		srand(time(NULL));	
        if(DataBase!=NULL){
             while(i<LinesInDataBase){
                ch=cacheAction();			//randomly gets an action from Write Read Modify and Flush
                y=random_number(MAX_NUMBER);//randomly gets a number from 0 to Max Number-1
				
				if(flush_times>=flush_max||flush_max<=0)	//if flush time is more LinesInDataBase/6 
					while(ch=='F')			//gets an other cache action
						ch=cacheAction();	

				if(ch=='F'){			//if its a flush ('F')
					flush_constant++;	//increases the constant
					if(flush_constant==(LinesInDataBase/MAX_FLUSHES/4))//that constant gets values 
						flush_constant=0;
					if(flush_constant==0){// 2/3 of the times it will write 'F' into Data.txt
						flush_times++;
						fprintf(DataBase,"%c \n",ch,flush_times);
					}
					else {			 // 1/3 of the times 
						while(ch=='F')					 // will change that action 
							ch=cacheAction();			 // to another action
						fprintf(DataBase,"%c %d\n",ch,y);// and write it into Data.txt
					}//end of if(flush_constant==3)
				}//end of if(ch=='F')

				else
					fprintf(DataBase,"%c %d\n",ch,y);
                i++;
             }//end of while(i<LinesInDataBase) 
             fclose(DataBase);
        }
        else
            printf("File does not exists");
}//end of fillDataBase(FILE *DataBase)
//** Input:a FILE Output in file: puts random action and random number in this file **//
void fillOutput(FILE *Output_w){
	int num,num1,i=0;
	int TAG[20]={9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9};
	int INDEX[20]={9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9};
	int BLOCKOFFSET[20]={9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9};

	for(num=0;num<=0;num++){	//makes one loop in order to let me use continue and break		
			getLine(DataBase_r);	//gets the num'th line
			parseWord(FilesLine,1);	//takes the second word which is a string 
			decToInt(atoi(WordInLine));//and makes it an integer
			parseWord(FilesLine,0);		//takes the 1st word which shows the action


			//***ACTION***//
			if(WordInLine[0]=='R'){		//prints READ if its a Read action 
				fprintf(Output_w,"READ\t:");
				Action='R';
			}
			if(WordInLine[0]=='W'){		//prints WRITE if its a Write action 
				fprintf(Output_w,"WRITE\t:");
				Action='W';
			}
			if(WordInLine[0]=='M'){		//prints MODIFY if its a Modify action 
				fprintf(Output_w,"MODIFY\t:");
				Action='M';
			}
			if(WordInLine[0]=='F'){		//prints FLUSH if its a Flush action 
				fprintf(Output_w,"===============================FLUSH===============================\n");
				Action='F';
				continue;
			}
			parseWord(FilesLine,1);		//Takes again the second word


			//***DECIMAL***//
			fprintf(Output_w," %d\t",atoi(WordInLine));



			//***TAG***//
			fprintf(Output_w,"  TAG: ");//prints the Tag number
			for(num1=0;num1<Tag_bits;num1++){
				fprintf(Output_w,"%d",BinaryNumber[num1]);
				TAG[num1]=BinaryNumber[num1];
			}
			Tag_cache=BintoInt(TAG);


			//***INDEX***//
			if(Index_bits>0){
				fprintf(Output_w,"\tINDEX: ");//prints the Index number
			}
			for(num1=0;num1<Index_bits;num1++){
				fprintf(Output_w,"%d",BinaryNumber[num1+Tag_bits]);
				INDEX[num1]=BinaryNumber[num1+Tag_bits];
			}
			Index_cache=BintoInt(INDEX);


			//***BLOCK OFFSET***//
			if(BlockOffset_bits>0)
				fprintf(Output_w,"  BO: ");//prints the Block Offset number
			for(num1=0;num1<BlockOffset_bits;num1++){
				fprintf(Output_w,"%d",BinaryNumber[num1+Tag_bits+Index_bits]);
				BLOCKOFFSET[num1]=BinaryNumber[num1+Tag_bits+Index_bits];
			}
			BlockOffset_cache=BintoInt(BLOCKOFFSET);



	}
}//end of fillOutput(FILE *Output_w)
//** Input:a decimal number Output: binaryNumber[10] gets the binary numnber of the input **//
void decToInt(int decimalNumber){
    int i=0,j=0;
	int k=0;
    for(i=Ram_bits-1;i>=0;i--){//loops as many times as the number of ram bits
		j=decimalNumber>>i;		//shift the decimal Number in order to get each bit of it
		if (j & 1)
			BinaryNumber[k]=1;//puts each bit into the global array BinaryNumber
		else
		    BinaryNumber[k]=0;
		k++;
    }
	 BinaryNumber[k]='\0';//shows the end of that global array
}//end of decToInt(int decimalNumber)
//** Gets a line from a FILE and puts this line into char FilesLine  **//
int getLine(FILE *DataBase){
	int i=0;
	char ch=fgetc(DataBase);
	while((ch!='\n')&&(ch!=EOF)){//get characters from Data.txt until it finds \n or EOF
		FilesLine[i]=ch;		//and puts them into global array FilesLine
		ch=fgetc(DataBase);	
		i++;
		if(i>100){
						printf("Errors:Words in Database's shouldnt be bigger than 100 characters ");
						_getch();
						exit(11);
					}
	}
	FilesLine[i]='\0';
	if (ch==EOF)
		return 1;
	else
		return 0;
}
//** Takes 2 strings and equals str2 to str1 **//
void equalStrings(char *str1,char *str2){
	int n=0;
	while(1){//loops until str2 ends
		if(str2[n]=='\0'){//if its null it breaks
			str1[n]='\0';//and puts a null at the end of str1
			break;
		}
		else{
			str1[n]=str2[n];//equals each place
			n++;			
		}
	}
}
//** Input:a string Output: prints that String **//
void printString(char *String){
	int num=0;
	while(1){
		if(String[num]=='\0')//loops untill string ends
			break;
		else{
			printf("%c",String[num]);//prints the string
			num++;
		}	
	}
}
//** Prints the Replaced number to Output file **//
void printBinaryNumber(int tagg,int indexx,int blockoffsets){
	int total=0,index=indexx,tag=tagg,bo=blockoffsets;
	tag=tagg<<BlockOffset_bits+Index_bits;
	if(Index_bits==0)
		index=0;
	else
		index=indexx<<BlockOffset_bits;

	if(BlockOffset_bits==0)
		bo=0;
	else
		bo=blockoffsets;
	total=tag+index+bo;
	fprintf(Output_w,"%d",total);
}
//** Input: an integer n and a line of characters and returns the n'th word **//
void parseWord(char *characterLine,int i){
	char ch1[50]={0};
	int j=0;
	int k=0;
	int n=0;
	for(k=0;k<stringSize(characterLine);k++){//loops as many times as the size of characterLine
			if((characterLine[k]==' '||characterLine[k]==',')&&(characterLine[k+1]!=' '&&characterLine[k+1]!=',')){//if the current character is space and the next is not knows that it went to the next word
					i--;									   //minus 1 from i which is the pointer to the current word
					k++;									   //starts the new word
			}
			if(i==0){						//if its the i'th word it starts putting this word int the ch1
					ch1[j]=characterLine[k];
					if(j>49){
						printf("Errors:Words in Database's shouldnt be bigger than 50 characters ");
						exit(11);
					}
					j++;
			}
	}
	ch1[j]='\0';
	equalStrings(WordInLine,ch1);//equals ch1 to global variable WordInLine
}
//** Input:a deciman number Output: the log of this number base 2 **//
float log_2(float n){
	float p=0;
	p=log(n)/log(2);//this is the formula for base 2
	return p;
}
//** Based on the Parameters finds the actual tag index and block offset of the current cache  **//
void findTagIndexBlockOffset(int CACHE_SIZE){
	Ram_bits=log_2(RAM_SIZE/WORD_SIZE);		//gets the number of ram bits into the global variable Ram_bits
	Cache_bits=ceil(log_2((float)CACHE_SIZE/(float)WORD_SIZE)); //gets the number of Cache bits into the global variable Cache_bits
	MAX_NUMBER=pow(2,Ram_bits)-1;			//gets the maximum into the global variable MAX_NUMBER
	LinesInDataBase=(MAX_NUMBER+1)*DATA_FILL_NUMBER;//puts the total lines of Data.txt into the global variable LinesInDataBase
	
	if(DIRECT_MAPPED==1&&!SET_ASSOCIATIVE&&!FULL_ASSOCIATIVE){//Its Direct Mapped so it find the considering results TAG INDEX BLOCK OFFSET
		BlockOffset_bits=log_2(WORDS_PER_BLOCK); 
		Index_bits=Cache_bits-BlockOffset_bits;
		Tag_bits=Ram_bits-Index_bits-BlockOffset_bits;
	}
	else if(!DIRECT_MAPPED&&SET_ASSOCIATIVE==1&&!FULL_ASSOCIATIVE){//Its Set Associative so it find the considering results TAG INDEX BLOCK OFFSET
		BlockOffset_bits=log_2(WORDS_PER_BLOCK);
		Index_bits=log_2(CACHE_SIZE/WORD_SIZE/SET_ASSOCIATIVE_WAYS)-BlockOffset_bits;
		Tag_bits=Ram_bits-Index_bits-BlockOffset_bits;
	}
	else if(!DIRECT_MAPPED&&!SET_ASSOCIATIVE&&FULL_ASSOCIATIVE==1){//Its Full Associative so it find the considering results TAG INDEX BLOCK OFFSET
		BlockOffset_bits=log_2(WORDS_PER_BLOCK);
		Tag_bits=Ram_bits-BlockOffset_bits;
	}
	else{	//If the Parameters are not proper shows the Error
		printf("Error:DIRECT_MAPPED SET_ASSOCIATIVE FULL_ASSOCIATIVE only one must be true");
		_getch();
		exit(1);
	}
}
//** find the errors of Parameters
void findErrors(){
	//Parameters are passed in local variables
	int cachesize=L1_CACHE_SIZE;
	int cachesize2=L2_CACHE_SIZE;
	int ramsize=RAM_SIZE;
	int wordsize=WORD_SIZE;
	int wordsperblock=WORDS_PER_BLOCK;
	int i=0;
	int iserror=0;
	if(READ_FROM_CACHE<=0){//cache size cant be bigger than Ram size
				printf("Error:READ_FROM_CACHE should be bigger than zero\n");
				iserror=1;
	}
	if(READ_FROM_RAM<=0){//cache size cant be bigger than Ram size
				printf("Error:READ_FROM_RAM should be bigger than zero\n");
				iserror=1;
	}
	if(WRITE_TO_CACHE<=0){//cache size cant be bigger than Ram size
				printf("Error:WRITE_TO_CACHE should be bigger than zero\n");
				iserror=1;
	}
	if(WRITE_TO_RAM<=0){//cache size cant be bigger than Ram size
				printf("Error:WRITE_TO_RAM should be bigger than zero\n");
				iserror=1;
	}

	if(cachesize>=ramsize){//cache size cant be bigger than Ram size
				printf("Error:RAM_SIZE should be bigger than L1_CACHE_SIZE\n");
				iserror=1;
	}
	if(cachesize2>=ramsize){//cache size cant be bigger than Ram size
				printf("Error:RAM_SIZE should be bigger than L2_CACHE_SIZE\n");
				iserror=1;
	}
	if(!isLog_2(cachesize2)){//cannot have cache size which is not a power of 2
				printf("Error:L2_CACHE_SIZE should be a power of 2\n");
				iserror=1;
	}
	if(!isLog_2(cachesize)){//cannot have cache size which is not a power of 2
				printf("Error:L1_CACHE_SIZE should be a power of 2\n");
				iserror=1;
	}
	if(!isLog_2(L2_ADDER)){//cannot have cache size which is not a power of 2
				printf("Error:L2_CACHE_SIZE should be a power of 2\n");
				iserror=1;
	}
	if(!isLog_2(VICTIM_CACHE_SIZE)&&(VICTIM_CACHE==true)){//cannot have cache size which is not a power of 2
				printf("Error:VICTIM_CACHE_SIZE should be a power of 2\n");
				iserror=1;
	}
	if(!isLog_2(L1_ADDER)){//cannot have cache size which is not a power of 2
				printf("Error:L1_CACHE_SIZE should be a power of 2\n");
				iserror=1;
	}
	if(L1_ADDER>cachesize){
				printf("Error:L1_CACHE_SIZE should be BIGGER than L1_ADDER\n");
				iserror=1;
	}
	if(L2_ADDER>cachesize2){
				printf("Error:L2_CACHE_SIZE should be BIGGER than L2_ADDER\n");
				iserror=1;
	}
	if(cachesize2<cachesize){
				printf("Error:L2_CACHE_SIZE should be bigger than L2_CACHE_SIZE\n");
				iserror=1;
	}
	if(!isLog_2(ramsize)){//cannot have Ram size which is not a power of 2
				printf("Error:RAM_SIZE should be a power of 2\n");
				iserror=1;
	}
	if(!isLog_2(wordsperblock)){//cannot have words per block which are not a power of 2
				printf("Error:WORDS_PER_BLOCK should be a power of 2\n");
				iserror=1;
	}
	if(!isLog_2(SET_ASSOCIATIVE_WAYS)&&SET_ASSOCIATIVE){//cannot have words per block which are not a power of 2
				printf("Error:SET_ASSOCIATIVE_WAYS should be a power of 2\n");
				iserror=1;
	}
	if(SET_ASSOCIATIVE&&cachesize<(WORDS_PER_BLOCK*SET_ASSOCIATIVE_WAYS*WORD_SIZE)){//data should fit into cache size
				printf("Error:CACHE_SIZE should be bigger than WORDS_PER_BLOCK*SET_ASSOCIATIVE_WAYS*WORD_SIZE\n");
				iserror=1;;
	}
	else if(cachesize<(WORDS_PER_BLOCK*WORD_SIZE)){
				printf("Error:CACHE_SIZE should be bigger than WORDS_PER_BLOCK*WORD_SIZE\n");
				iserror=1;
	}
	if(SET_ASSOCIATIVE&&L1_ADDER<(WORDS_PER_BLOCK*SET_ASSOCIATIVE_WAYS*WORD_SIZE)){//data should fit into cache size
				printf("Error:L1_CACHE_SIZE should be bigger than WORDS_PER_BLOCK*SET_ASSOCIATIVE_WAYS*WORD_SIZE\n");
				iserror=1;;
	}
	else if(L1_ADDER<(WORDS_PER_BLOCK*WORD_SIZE)){
				printf("Error:L1_CACHE_SIZE should be bigger than WORDS_PER_BLOCK*WORD_SIZE\n");
				iserror=1;
	}
	if(!isLog_2(WORD_SIZE)){//cannot have words per block which are not a power of 2
				printf("Error:WORDS_SIZE should be a power of 2\n");
				iserror=1;
	}
	if(WRITE_BACK!=(0|1)&&WRITE_THROUGH!=(1|0)){//cannot have words per block which are not a power of 2
				printf("Error:WRITEBACK_THROUGH SHOULD BE \"0\" or \"1\"\n");
				iserror=1;
	}
	if(WRITE_ALLOCATE!=0&&WRITE_ALLOCATE!=1){//cannot have words per block which are not a power of 2
				printf("Error:WRITE_ALLOCATE SHOULD BE \"0\" or \"1\"\n");
				iserror=1;
	}
	if(WRITE_BACK==WRITE_THROUGH){//write back and through cant be true or faulse at the same time
				printf("Error:WRITEBACK_THROUGH SHOULDNT BE \"0\" or \"1\" AT THE SAME TIME\n");
				iserror=1;
	}
	if(!isStringEqual(POLICY,"RANDOM")&&!isStringEqual(POLICY,"FIFO")&&!isStringEqual(POLICY,"LRU")){
				printf("Error:POLICY SHOULD BE \"RANDOM\" or \"FIFO\" or \"LRU\"\n");
				iserror=1;
	}
	if(MAX_FLUSHES<0){
				printf("Error:MAX FLUSHES SHOULDNT BE LESS THAN ZERO\n");
				iserror=1;
	}
	if(iserror){
		_getch();
		exit(1);
	}
}
//** returns 1 if an integer is log of 2 else returns 0  **//
int isLog_2(int parameter){
	int power;
	if(parameter==0)
		return 0;
	for ( power = 1; power > 0; power = power << 1)
    {
        // this for loop used shifting for powers of 2, meaning
        // that the value will become 0 after the last shift
        // (from binary 1000...0000 to 0000...0000) then, the for
        // loop will break out

        if (power == parameter)
            return 1;
        if (power > parameter)
            return 0;
    }
    return 0;
}
//** parses the Parameter File and gets all the parameters **//
void getParameters(FILE *Parameter){
	char Param_str[25]={NULL};
	char Param_num[50]={NULL};
	char Param_num2[50]={NULL};
	char Param_Line[100]={NULL};
	int LastLine=0;
	getLine(Parameter);
	while(1){
		equalStrings(Param_Line,FilesLine);
		parseWord(Param_Line,0);
		equalStrings(Param_str,WordInLine);
		parseWord(Param_Line,1);
		equalStrings(Param_num,WordInLine);
		parseWord(Param_Line,2);
		equalStrings(Param_num2,WordInLine);
		guessParameter(Param_str,Param_num,Param_num2);	
		printf("\n");
		
		if(LastLine==1)break;
		if(getLine(Parameter)==1)
			LastLine=1;
	}

}
//** Helps getParameter to get the right parameter **//
void guessParameter(char *String,char *Number,char *Number2){
	if(isStringEqual(String,"RAM_SIZE"))
		RAM_SIZE=atoi(Number);
	if(isStringEqual(String,"L1_CACHE_SIZE")){
		L1_CACHE_SIZE=atoi(Number);
		L1_ADDER=atoi(Number2);
	}
	if(isStringEqual(String,"L2_CACHE_SIZE")){
		L2_CACHE_SIZE=atoi(Number);
		L2_ADDER=atoi(Number2);
	}

	if(isStringEqual(String,"VICTIM_CACHE_SIZE")){
		VICTIM_CACHE_SIZE=atoi(Number);
	}	
	if(isStringEqual(String,"VICTIM_CACHE")){
		if(isStringEqual(Number,"ENABLE"))
			VICTIM_CACHE=true;
		else
		if(isStringEqual(Number,"DISABLE"))
			VICTIM_CACHE=false;
		else
			{
			printf("Error:VICTIM_CACHE_SIZE should be ENABLE OR DISABLE\n");
			_getch();
			exit(1);
			}
	}
	if(isStringEqual(String,"L2_CACHE")){
		if(isStringEqual(Number,"ENABLE"))
			L2_CACHE=true;
		else
		if(isStringEqual(Number,"DISABLE"))
			L2_CACHE=false;
		else
			{
			printf("Error:L2_CACHE should be ENABLE OR DISABLE\n");
			_getch();
			exit(1);
			}
	}
	if(isStringEqual(String,"CACHE_MODE")){
		if(isStringEqual(Number,"INCLUSIVE"))
			CACHE_MODE=true;
		else
		if(isStringEqual(Number,"EXCLUSIVE"))
			CACHE_MODE=false;
		else
			{
			printf("Error:CACHE should be INCLUSIVE OR EXCLUSIVE\n");
			_getch();
			exit(1);
			}
	}
	if(isStringEqual(String,"WORD_SIZE"))
		WORD_SIZE=atoi(Number);
	if(isStringEqual(String,"FULL_ASSOCIATIVE"))
		FULL_ASSOCIATIVE=atoi(Number);
	if(isStringEqual(String,"SET_ASSOCIATIVE"))
		SET_ASSOCIATIVE=atoi(Number);
	if(isStringEqual(String,"DIRECT_MAPPED"))
		DIRECT_MAPPED=atoi(Number);
	if(isStringEqual(String,"DATA_FILL_NUMBER"))
		DATA_FILL_NUMBER=atoi(Number);
	if(isStringEqual(String,"SET_ASSOCIATIVE_WAYS"))
		SET_ASSOCIATIVE_WAYS=atoi(Number);
	if(isStringEqual(String,"WORDS_PER_BLOCK"))
		WORDS_PER_BLOCK=atoi(Number);
	if(isStringEqual(String,"MAX_FLUSHES"))
		MAX_FLUSHES=atoi(Number);
	if(isStringEqual(String,"WRITE_BACK"))
		WRITE_BACK=atoi(Number);
	if(isStringEqual(String,"WRITE_THROUGH"))
		WRITE_THROUGH=atoi(Number);
	if(isStringEqual(String,"WRITE_ALLOCATE"))
		WRITE_ALLOCATE=atoi(Number);
	if(isStringEqual(String,"POLICY"))
		equalStrings(POLICY,Number);
	if(isStringEqual(String,"READ_FROM_CACHE"))
		READ_FROM_CACHE=atoi(Number);
	if(isStringEqual(String,"READ_FROM_RAM"))
		READ_FROM_RAM=atoi(Number);
	if(isStringEqual(String,"WRITE_TO_RAM"))
		WRITE_TO_RAM=atoi(Number);
	if(isStringEqual(String,"WRITE_TO_CACHE"))
		WRITE_TO_CACHE=atoi(Number);

	printf("%s ",String);
	if(String[0]!=NULL){
		if(!isStringEqual(String,"POLICY"))
			printf("%d",atoi(Number));
		else
			printf("%s",Number);
	}
}
//** Checks if string 1 and string 2 are equal and returns 1 if so **//
int isStringEqual(char *Str1,char *Str2){
	int i=0;
	int isequal;
	while(1){
		if(Str1[i]==Str2[i]){
			isequal=1;
			if(Str1[i]=='\0'&&Str2[i]=='\0')
				break;
			i++;
		}
		else{
			isequal=0;
			break;
		}
	}
	return isequal;
}
//** Gets an integer array full of zeros and ones and converts it to a decimal number**//
int BintoInt(int *Bits){
	int num0=0;
	int num1=0;
	int Int=-1;
	int multiplier=0;
	while(1){
		//finds the number of places in Bits
		//finds how big the multiplier will be
		if((Bits[num0]!=0)&&(Bits[num0]!=1)){
			break;
		}
		else
			multiplier++;
		num0++;
	}
	for(num1=0;num1<num0;num1++){
			multiplier--;
			if(num1==0)
				Int=0;
			Int=Int+Bits[num1]*pow(2,multiplier);		
	}
	return Int;
}
//** prints a queue from head to tail **//
void printQueue(struct Cache Head){
	struct Cache *Temp;
	int i=0;
	int indexpower=pow(2,Index_bits);
	Temp= (struct Cache*)malloc((sizeof(struct Cache)));
	Temp=Head.head;
	if(FULL_ASSOCIATIVE)
		printf("\nFULL ASSOSIATIVE:\n");
	if(SET_ASSOCIATIVE)
		printf("\nSET_ASSOCIATIVE:");
	if(DIRECT_MAPPED)
		printf("\nDIRECT_MAPPED:\n");

	while(Temp->head!=NULL){
		if(i%WORDS_PER_BLOCK==0){
			printf("\n");}
		if(SET_ASSOCIATIVE==1){
			if(Temp->Way!=Temp->tail->Way){
				printf("\n(Way #%d)\n",Temp->Way);
			}
		}


		if(Temp->Index==-1)
			printf("%3d:%d",Temp->Tag,Temp->BlockOffSet);
		else
			printf("%3d:%d:%d",Temp->Tag,Temp->BlockOffSet,Temp->Index);

			printf(" ");
		Temp=Temp->head;
		i++;
	}


}
//** Creates the Cache depenting on the parameters **//
void createCache(int CACHE_SIZE,struct Cache *Tail){
	struct Cache *newCache;
	int i=0;
	int tag;
	int bo=0;
	int AddressesPerWay;
	int index=0;
	int wayno=0;
	int waycount=0;
	int CACHEBITS=log_2(CACHE_SIZE/WORD_SIZE);
	int indexpower=pow(2,Index_bits+BlockOffset_bits);
	int bopower=pow(2,BlockOffset_bits);
	int size=CACHE_SIZE/WORD_SIZE;
	if(SET_ASSOCIATIVE)
		AddressesPerWay=CACHE_SIZE/SET_ASSOCIATIVE_WAYS/WORDS_PER_BLOCK/WORD_SIZE;
	for(i=0;i<size;i++){
		/*finds tag*/
		tag=i>>(Index_bits+BlockOffset_bits);
		/*finds Index*/
		if(Index_bits==0)
			index=-1;
		else
			index=(i%indexpower)>>BlockOffset_bits;
		/*finds Block offset*/
		if(BlockOffset_bits==0)
			bo=-1;
		else
			bo=i%bopower;

		newCache=(struct Cache*)malloc((sizeof(struct Cache)));
		newCache->Valid=false;
		newCache->Tag=tag;
		newCache->Index=index;
		newCache->BlockOffSet=bo;
		newCache->Age=-1;
		newCache->Touched=-1;
		newCache->Dirty=1;
		newCache->head=Tail;
		newCache->tail=Tail->tail;
		Tail->tail->head=newCache;
		Tail->tail=newCache;
		if(SET_ASSOCIATIVE==1){
			newCache->Way=wayno;
			if(newCache->Index!=newCache->tail->Index){
				if(waycount%(AddressesPerWay)==0)wayno++;
				newCache->Way=wayno;
				waycount++;
			}

		}
	}
}
//** enqueues a new address into Cache **//
void enqueueCache(int size,int L1_CACHE_SIZE,struct Cache Head){
	struct Cache *newCache;
	struct Cache *Temp;
	struct Cache *PLACE;
	int place=1;
	int notfull=0;
	int i=0;
	int dirty=1;
	int actionCycles=0;
	int read_write_hit_miss=0;
	Temp= (struct Cache*)malloc((sizeof(struct Cache)));
	Temp=Head.head;
	PLACE= (struct Cache*)malloc((sizeof(struct Cache)));
	PLACE=Head.head;
	newCache=(struct Cache*)malloc((sizeof(struct Cache)));
	newCache->Valid=true;
	newCache->Tag=Tag_cache;
	newCache->Index=Index_cache;
	newCache->BlockOffSet=BlockOffset_cache;
	newCache->Age=LinesInDataBase-size;
	place=enqueuePlace(newCache->Tag,newCache->Index,newCache->BlockOffSet,newCache->Age,newCache->Touched,L1_CACHE_SIZE,Head);
	for(i=0;i<place;i++){
		Temp=Temp->head;
	}
	//printf("%d,%d,%d->%d,%d,%d\n",newCache->Tag,newCache->Index,newCache->BlockOffSet,Temp->Tag,Temp->Index,Temp->BlockOffSet);
	if(Action=='R'){
		if(Temp->Tag==newCache->Tag&&Temp->Valid==true){
			ReadHit++;
			Hits++;
			Temp->Touched=newCache->Age;
			read_write_hit_miss=1;
			fprintf(Output_w,"\tHIT\t\t\t\t\t");
		}
		else{
			ReadMiss++;
			Misses++;
			read_write_hit_miss=2;
			fprintf(Output_w,"\tMISS:");
			if(Temp->Age==-1){
				fprintf(Output_w,"\tCOMPULSORY\t\t\t");
			}else{
				if(DIRECT_MAPPED){
					fprintf(Output_w,"\tCAPACITY\t\t\t");
				}
				if(SET_ASSOCIATIVE||FULL_ASSOCIATIVE){
					if(!isStringEqual(POLICY,"RANDOM")){
						fprintf(Output_w,"\tCONFLICT\t\t\t");
					}
					else{
						PLACE=Head.head;
						while(PLACE->head!=NULL){
							if(PLACE->Age==-1)
								notfull=1;
							PLACE=PLACE->head;
						}
						if(notfull){
							fprintf(Output_w,"\tCONFLICT\t\t\t");
						}
						else{
							fprintf(Output_w,"\tCAPACITY\t\t\t");
						}
					}
				}
			}
		}
	}
	if(Action=='W'){
		if(Temp->Tag==newCache->Tag&&Temp->Valid==true){
			WriteHit++;
			Hits++;
			read_write_hit_miss=3;
			fprintf(Output_w,"\tHIT\t\t\t\t\t");
			Temp->Age=newCache->Age;
			Temp->Touched=newCache->Age;
			if(WRITE_BACK){
				Temp->Dirty=0;
			}
		}
		else{
			enqueueVictim(Temp->Tag,Temp->Index,Temp->BlockOffSet,Temp->Age,Temp->Touched);
			fprintf(Output_w,"\tMISS:");
			if(Temp->Age==-1){
				fprintf(Output_w,"\tCOMPULSORY\tREPLACE:(UNKOWN)");
				compulsory++;
			}
			else{
				if(DIRECT_MAPPED){
					fprintf(Output_w,"\tCONFLICT\tREPLACE:(");
					printBinaryNumber(Temp->Tag,Temp->Index,Temp->BlockOffSet);
					fprintf(Output_w,")\t");
					conflict++;
				}
				if(SET_ASSOCIATIVE||FULL_ASSOCIATIVE){
					if(!isStringEqual(POLICY,"RANDOM")){
						fprintf(Output_w,"\tCAPACITY\tREPLACE:(");
						printBinaryNumber(Temp->Tag,newCache->Index,newCache->BlockOffSet);
						fprintf(Output_w,")\t");
						capacity++;
					}
					else{
						PLACE=Head.head;
						while(PLACE->head!=NULL){
							if(PLACE->Age==-1)
								notfull=1;
							PLACE=PLACE->head;
						}
						if(notfull){
							fprintf(Output_w,"\tCONFLICT\tREPLACE:(");
							printBinaryNumber(Temp->Tag,newCache->Index,newCache->BlockOffSet);
							fprintf(Output_w,")\t");
							conflict++;
						}
						else{
							fprintf(Output_w,"\tCAPACITY\tREPLACE:(");
							printBinaryNumber(Temp->Tag,Temp->Index,Temp->BlockOffSet);
							fprintf(Output_w,")\t");
							capacity++;
						}
					}
				}
			}
			Temp->Tag=newCache->Tag;
			Temp->Valid=true;
			Temp->Age=newCache->Age;
			Temp->Touched=newCache->Age;
			read_write_hit_miss=4;
			WriteMiss++;
			Misses++;
		}
	}
	if(Action=='M'){
		if(Temp->Tag==newCache->Tag&&Temp->Valid==true){
			ModifyHit++;
			Hits++;
			Temp->Touched=newCache->Age;
			read_write_hit_miss=1;
			fprintf(Output_w,"\tHIT\t\t\t\t\t");
		}
		else{
			ModifyMiss++;
			Misses++;
			read_write_hit_miss=2;
			fprintf(Output_w,"\tMISS");
			if(Temp->Age==-1){
				fprintf(Output_w,"\tCOMPULSORY\t\t\t");
			}else{
				if(DIRECT_MAPPED){
					fprintf(Output_w,"\tCAPACITY\t\t\t");
				}
				if(SET_ASSOCIATIVE||FULL_ASSOCIATIVE){
					if(!isStringEqual(POLICY,"RANDOM")){
						fprintf(Output_w,"\tCONFLICT\t\t\t");
					}	
					else{
						PLACE=Head.head;
						while(PLACE->head!=NULL){
							if(PLACE->Age==-1)
								notfull=1;
							PLACE=PLACE->head;
						}
						if(notfull){
							fprintf(Output_w,"\tCONFLICT\t\t\t");
						}
						else{
							fprintf(Output_w,"\tCAPACITY\t\t\t");
						}
					}
				}
			}
		}
	}
	if(Action=='W'){
		dirty=Temp->Dirty;
	}
	actionCycles=guessCycles(read_write_hit_miss,dirty);
	fprintf(Output_w,"\tCYCLES:%d\n",actionCycles);
	free((void*)newCache);
}
//** finds a place for new address  int the  into Cache **//
int enqueuePlace(int tag,int index,int bo,int age,int touch,int CACHE_SIZE,struct Cache Head){
	struct Cache *Temp;
	int writeplace=0;
	int i=0,j=0;
	int found=0;
	int number=0;
	int random=0;
	int decisionplace=0;
	Temp= (struct Cache*)malloc((sizeof(struct Cache)));
	Temp=Head.head;

	if(DIRECT_MAPPED){
		while (Temp->head!=NULL)
		{		
			if(Temp->Index==index&&Temp->BlockOffSet==bo){
				break;
			}
			else{
				Temp=Temp->head;
				writeplace++;
			}
		}
	}

	if(FULL_ASSOCIATIVE&&isStringEqual(POLICY,"FIFO")){	
		while(Temp->head!=NULL){
			if(Temp->Tag==tag&&Temp->Valid==true&&Temp->BlockOffSet==bo){
				found=1;
				writeplace=j;
			}
			j++;
			Temp=Temp->head;
		}
		if(found!=1){
			Temp=Head.head;
			while(Temp->head!=NULL){
				if(Temp->BlockOffSet==bo&&Temp->Age==-1){
					writeplace=number;
					found=2;
					break;
				}
				Temp=Temp->head;
				number++;
			}
			if(found!=2){
				number=0;
				Temp=Head.head;
				while(Temp->head!=NULL){
					if(Temp->BlockOffSet==bo&&age>=Temp->Age){
						age=Temp->Age;
						writeplace=number;
					}
				Temp=Temp->head;
				number++;
				}
			}
		}
	}

	if(FULL_ASSOCIATIVE&&isStringEqual(POLICY,"RANDOM")){	
		found=0;
		while(Temp->head!=NULL){
			if(Temp->Tag==tag&&Temp->Valid==true&&Temp->BlockOffSet==bo){
				found=1;
				writeplace=j;
			}
			j++;
			Temp=Temp->head;
		}
		if(found!=1){
			number=0;
			Temp=Head.head;
			random=random_number(CACHE_SIZE/WORDS_PER_BLOCK/WORD_SIZE);
			while(Temp->head!=NULL){
				if(Temp->BlockOffSet==bo&&random==0){
					writeplace=number;
					break;
				}
				if(Temp->BlockOffSet==bo){
					random--;
				}
				Temp=Temp->head;
				number++;

			}
		}
	}

	if(FULL_ASSOCIATIVE&&isStringEqual(POLICY,"LRU")){	
		found=0;
		while(Temp->head!=NULL){
			if(Temp->Tag==tag&&Temp->Valid==true&&Temp->BlockOffSet==bo){
				found=1;
				writeplace=j;
			}
			j++;
			Temp=Temp->head;
		}
		if(found!=1){
			Temp=Head.head;
			number=0;
			while(Temp->head!=NULL){
				if(Temp->BlockOffSet==bo&&Temp->Age==-1){
					writeplace=number;
					found=2;
					break;
				}
				Temp=Temp->head;
				number++;
			}
		}
		if(found!=2){
				number=0;
				Temp=Head.head;
				while(Temp->head!=NULL){
					if(Temp->BlockOffSet==bo&&(age>=Temp->Age&&touch>=Temp->Age)){
						age=Temp->Age;
						touch=Temp->Touched;
						writeplace=number;
					}
				Temp=Temp->head;
				number++;
				}
			}
	}

	if(SET_ASSOCIATIVE&&isStringEqual(POLICY,"FIFO")){
		while(Temp->head!=NULL){
			if(Temp->Tag==tag&&Temp->Valid==true&&Temp->BlockOffSet==bo&&Temp->Index==index){
				found=1;
				writeplace=j;
			}
			j++;
			Temp=Temp->head;
		}
		if(found!=1){
			Temp=Head.head;
			while(Temp->head!=NULL){
				if(Temp->BlockOffSet==bo&&Temp->Index==index&&Temp->Age==-1){
					writeplace=number;
					found=2;
					break;
				}
				Temp=Temp->head;
				number++;
			}
			if(found!=2){
				number=0;
				Temp=Head.head;
				while(Temp->head!=NULL){
					if(Temp->BlockOffSet==bo&&age>=Temp->Age&&Temp->Index==index){
						age=Temp->Age;
						writeplace=number;
					}
				Temp=Temp->head;
				number++;
				}
			}
		}
	}
	
	if(SET_ASSOCIATIVE&&isStringEqual(POLICY,"RANDOM")){	
		found=0;
		while(Temp->head!=NULL){
			if(Temp->Tag==tag&&Temp->Valid==true&&Temp->BlockOffSet==bo&&Temp->Index==index){
				found=1;
				writeplace=j;
			}
			j++;
			Temp=Temp->head;
		}
		if(found!=1){
			number=0;
			Temp=Head.head;
			random=random_number(CACHE_SIZE/WORDS_PER_BLOCK/WORD_SIZE);
			while(Temp->head!=NULL){
				if(Temp->BlockOffSet==bo&&random==0&&Temp->Index==index){
					writeplace=number;
					break;
				}
				if(Temp->BlockOffSet==bo){
					random--;
				}
				Temp=Temp->head;
				number++;

			}
		}
	}

	if(SET_ASSOCIATIVE&&isStringEqual(POLICY,"LRU")){	
		found=0;
		while(Temp->head!=NULL){
			if(Temp->Tag==tag&&Temp->Valid==1&&Temp->BlockOffSet==bo&&Temp->Index==index){
				found=1;
				writeplace=j;
			}
			j++;
			Temp=Temp->head;
		}
		if(found!=1){
			Temp=Head.head;
			number=0;
			while(Temp->head!=NULL){
				if(Temp->BlockOffSet==bo&&Temp->Age==-1&&Temp->Index==index){
					writeplace=number;
					found=2;
					break;
				}
				Temp=Temp->head;
				number++;
			}
		}
		if(found!=2){
				number=0;
				Temp=Head.head;
				while(Temp->head!=NULL){
					if(Temp->BlockOffSet==bo&&(age>=Temp->Age&&touch>=Temp->Age&&Temp->Index==index)){
						age=Temp->Age;
						touch=Temp->Touched;
						writeplace=number;
					}
				Temp=Temp->head;
				number++;
				}
			}
	}
	return writeplace;
}
//** Guesses how may penalty Cycles will we have for every instuction **//
int guessCycles(int read_write_hit_miss,int dirty){
	
	/*READ OR MODIFY HIT*/
	if(read_write_hit_miss==1){
			return READ_FROM_CACHE;
	}
	/*READ OR MODIFY MISS*/
	if(read_write_hit_miss==2){
		if(WRITE_THROUGH)
				Cycles+=READ_FROM_CACHE+READ_FROM_RAM;
				return READ_FROM_CACHE+READ_FROM_RAM;
		if(!WRITE_THROUGH){//
			Cycles+=READ_FROM_CACHE+READ_FROM_RAM+WRITE_TO_CACHE;
			return READ_FROM_CACHE+READ_FROM_RAM+WRITE_TO_CACHE;
		}
	}
	/*WRITE HIT*/
	if(read_write_hit_miss==3){
		if(WRITE_THROUGH){
				Cycles+=READ_FROM_CACHE;
				Cycles+=WRITE_TO_CACHE;
				Cycles+=WRITE_TO_RAM;
				return READ_FROM_CACHE+WRITE_TO_CACHE+WRITE_TO_RAM;
		}
		if(WRITE_BACK){//
			if(dirty==0){
				Cycles+=READ_FROM_CACHE;
				Cycles+=WRITE_TO_RAM;
				return WRITE_TO_RAM+READ_FROM_CACHE;
			}
			if(dirty==1){
				Cycles+=READ_FROM_CACHE;
				return READ_FROM_CACHE;
			}
		}
	}
	/*WRITE MISS*/
	if(read_write_hit_miss==4){
		if(WRITE_ALLOCATE){
				Cycles+=READ_FROM_CACHE;
				Cycles+=READ_FROM_RAM;
				Cycles+=WRITE_TO_CACHE;
				Cycles+=WRITE_TO_RAM;
				return WRITE_TO_RAM+WRITE_TO_CACHE+READ_FROM_RAM+READ_FROM_CACHE;
			}
		if(!WRITE_ALLOCATE){//
			Cycles+=READ_FROM_CACHE;
			Cycles+=READ_FROM_RAM;
			Cycles+=WRITE_TO_RAM;
			return WRITE_TO_RAM+READ_FROM_RAM+READ_FROM_CACHE;
		}
	}
}
/**Frees every single memory used by our List**/
void free_Cache (struct Cache *chain){
	while(chain->head!=NULL){
		chain=chain->head;
		free((void*)chain->tail);
	}
}
//**Initializes every variable needed for next loop**/
void initializer(){
Hits=0;
Misses=0;
ReadMiss=0;
WriteMiss=0;
ModifyMiss=0;
ReadHit=0;
WriteHit=0;
ModifyHit=0;
Cycles=0;
compulsory=0;
conflict=0;
capacity=0;
}

void createVictim(int CACHE_SIZE,struct Cache *Tail){
	struct Cache *newCache;
	int i=0;
	int CACHEBITS=log_2(CACHE_SIZE/WORD_SIZE);
	int size=CACHE_SIZE/WORD_SIZE;
	for(i=0;i<size;i++){
		newCache=(struct Cache*)malloc((sizeof(struct Cache)));
		newCache->Valid=false;
		newCache->Tag=-1;
		newCache->Index=-1;
		newCache->BlockOffSet=-1;
		newCache->Age=-1;
		newCache->Touched=-1;
		newCache->Dirty=1;
		newCache->Way=-1;
		newCache->head=Tail;
		newCache->tail=Tail->tail;
		Tail->tail->head=newCache;
		Tail->tail=newCache;
	}
}

int enqueueVictim(int tag,int index,int bo,int age,int touch){
	struct Cache *Temp;
	struct Cache *Temp1;
	int writeplace=0;
	int i=0,j=0;
	int found=0;
	int number=0;
	int random=0;
	int decisionplace=0;
	Temp= (struct Cache*)malloc((sizeof(struct Cache)));
	Temp=Victim_Head.head;
	Temp1= (struct Cache*)malloc((sizeof(struct Cache)));
	Temp1=Victim_Head.head;
		while(Temp->head!=NULL){
			if(Temp->Tag==tag&&Temp->Valid==true&&Temp->BlockOffSet==bo&&Temp->Index==index){
				found=1;
				writeplace=j;
			}
			j++;
			Temp=Temp->head;
		}
		if(found!=1){
			Temp=Victim_Head.head;
			while(Temp->head!=NULL){
				if(Temp->BlockOffSet==bo&&Temp->Age==-1&&Temp->Index==index){
					writeplace=number;
					found=2;
				}
				Temp=Temp->head;
				number++;
			}
			if(found!=1&&found!=2){
				number=0;
				Temp=Victim_Head.head;
				while(Temp->head!=NULL){
					if(Temp->BlockOffSet==bo&&age>=Temp->Age){
						age=Temp->Age;
						writeplace=number;
					}
				Temp=Temp->head;
				number++;
				}
			}
		}
	for(i=0;i<writeplace;i++){
	Temp1=Temp1->head;
	}
	Temp1->Tag=tag;
	Temp1->Valid=true;
	Temp1->BlockOffSet=bo;
	Temp1->Age=age;
	Temp1->Touched=touch;
	return writeplace;
}
