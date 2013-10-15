
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
#define	RAM_SIZE 512
#define	CACHE_SIZE 128
#define WORD_SIZE 4
#define WORDS_PER_BLOCK 4
#define FULL_ASSOCIATIVE 1
#define SET_ASSOCIATIVE 0
#define DIRECT_MAPPED 0
#define DATA_FILL_NUMBER 3
#define SET_ASSOCIATIVE_WAYS 8

/*****FILES*****/
FILE *DataBase_w;               //Write to Data.txt
FILE *DataBase_r;				//Read from Data.txt
FILE *Parameter_r;				//Reads the Parameters from Parameter.txt 
FILE *Output_w;					//Writes the Output

/*****FUNCTIONS*****/
int random_number(int x);	//Returns a number >=0 and <x
char cacheAction();			//Returns a random characters from the list
int stringSize(char []);	//Input:a string Output: the number of the characters in string 
void fillDataBase(FILE*);	//Fills Data.txt with random cache action and random numbers  "R 10","W 9", "M 5"
void fillOutput(FILE*);		//Input:a FILE Output in file: puts random action and random number in this file
void decToInt(int);			//Input:a decimal number Output: binaryNumber[10] gets the binary numnber of the input
void getLine(FILE*);		//Gets a line from a FILE and puts this line into char FilesLine 
void equalStrings(char *str1,char *str2);//Takes 2 strings and equals str2 to str1
void printString(char *);	//Input:a string Output: prints that String
void parseWord(char*,int);		 //Input: an integer n and a line of characters and returns the n'th word
int log_2(int);				//Input:a deciman number Output: the log of this number base 2
void findTagIndexBlockOffset();	//Based on the Parameters finds the actual tag index and block offset of the current cache 
void findErrors();		//find the errors of Parameters
int isLog_2(int);		//returns 1 if an integer is log of 2 else returns 0
//void getParameters(FILE*);

/*
struct Cache
{
 int Tag;
 int Index;
 int BlockOffSet;
 struct Cache *head;
 struct Cache *tail;
};
enum cache_policy{
	#define _CRT_SECURE_NO_DEPRECATE
	//direct_map,nodirext
};*/

int BinaryNumber[20];		//the binary number saved by the function decToInt
char FilesLine[30]={NULL};	//each line of the DataBase is saved in this String
char WordInLine[10]={NULL};	//the word taken each time from FilesLine
int LinesInDataBase=0;	//The number of lines filled in files
int MAX_NUMBER=0;		//The maximum number of address reference is put in DataBase
int Ram_bits=0;			//Ram bits are saved by that global variable 
int Cache_bits=0;		//Cache bits are saved by that global variable 
int Tag_bits=0;			//Tag bits are saved by that global variable 
int Index_bits=0;		//Index bits are saved by that global variable 
int BlockOffset_bits=0;	//Block offset bits are saved by that global variable 
int Way=0;				//The way in set assosiative that address is refered to
int main(int argc, char ** argv){
	//struct Cache addresses;
	srand(time(NULL));			//helps to generate random number with rand()
	findErrors();
	findTagIndexBlockOffset();  //finds Tag Index and Block Offset of the current cache 
	error=fopen_s(&DataBase_w,"Data.txt","w+");//opens Data.txt for filling
	fillDataBase(DataBase_w);				   //Random cache action and random numbers in Data.txt
	error=fopen_s(&DataBase_r,"Data.txt","r+");//opens Data.txt for parsing
	error=fopen_s(&Output_w,"Output.txt","w+");//opens Output.txt for filling
	fillOutput(Output_w);		//fills Output w
	printf("TAG %d  INDEX %d BLOCK OFFSET %d",Tag_bits,Index_bits,BlockOffset_bits);
	//getParameters(FILE*);*/
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
		int flush_constant=1;	//that constant gets values 1,2 or 3
        char ch;				//variable used for characters
        int y;					
		int flush_max=LinesInDataBase/6;//max flush into Data.txt will be 1/6'th of total lines
		int flush_times=0;
        if(DataBase!=NULL){
             while(i<LinesInDataBase){
                ch=cacheAction();			//randomly gets an action from Write Read Modify and Flush
                y=random_number(MAX_NUMBER);//randomly gets a number from 0 to Max Number-1
				
				if(flush_times>=flush_max)	//if flush time is more LinesInDataBase/6 
					while(ch=='F')			//gets an other cache action
						ch=cacheAction();	
				if(ch=='F'){			//if its a flush ('F')
					flush_constant++;	//increases the constant
					if(flush_constant==4)//that constant gets values 1,2 or 3
						flush_constant=1;
					if(flush_constant==1||flush_constant==2){// 2/3 of the times it will write 'F' into Data.txt
						flush_times++;
						fprintf(DataBase,"%c \n",ch,flush_times);
					}
					else if(flush_constant==3){			 // 1/3 of the times 
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
	int num,num1=0;
	for(num=0;num<LinesInDataBase;num++){//loops until the end of Data.txt
			getLine(DataBase_r);	//gets the num'th line
			parseWord(FilesLine,1);	//takes the second word which is a string 
			decToInt(atoi(WordInLine));//and makes it an integer
			parseWord(FilesLine,0);		//takes the 1st word which shows the action
			if(WordInLine[0]=='R'){		//prints READ if its a Read action 
				printf("ACTION: READ\t",atoi(WordInLine));
				fprintf(Output_w,"ACTION: READ\t");
			}
			if(WordInLine[0]=='W'){		//prints WRITE if its a Write action 
				printf("ACTION: WRITE\t",atoi(WordInLine));
				fprintf(Output_w,"ACTION: WRITE\t");
			}
			if(WordInLine[0]=='M'){		//prints MODIFY if its a Modify action 
				printf("ACTION: MODIFY\t",atoi(WordInLine));
				fprintf(Output_w,"ACTION: MODIFY\t");
			}
			if(WordInLine[0]=='F'){		//prints FLUSH if its a Flush action 
				printf("ACTION: FLUSH\n",atoi(WordInLine));
				fprintf(Output_w,"ACTION: FLUSH\n");
				continue;
			}
			parseWord(FilesLine,1);		//Takes again the second word
			printf(" ",atoi(WordInLine));//prints the binary number
			printf("DECIMAL: %d\tBINARY: ",atoi(WordInLine));
			fprintf(Output_w,"DECIMAL: %d\tBINARY: ",atoi(WordInLine));
			for(num1=0;num1<Ram_bits;num1++){
				fprintf(Output_w,"%d",BinaryNumber[num1]);
				printf("%d",BinaryNumber[num1]);
			}
			printf("  TAG: ");//prints the Tag number
			fprintf(Output_w,"  TAG: ");
			for(num1=0;num1<Tag_bits;num1++){
				fprintf(Output_w,"%d",BinaryNumber[num1]);
				printf("%d",BinaryNumber[num1]);
			}
			printf("\tINDEX: ");//prints the Index number
			fprintf(Output_w,"\tINDEX: ");
			for(num1=0;num1<Index_bits;num1++){
				fprintf(Output_w,"%d",BinaryNumber[num1+Tag_bits]);
				printf("%d",BinaryNumber[num1+Tag_bits]);
			}
			printf("  BLOCK OFFSET: ");//prints the Block Offset number
			fprintf(Output_w,"  BLOCK OFFSET: ");
			for(num1=0;num1<BlockOffset_bits;num1++){
				fprintf(Output_w,"%d",BinaryNumber[num1+Tag_bits+Index_bits]);
				printf("%d",BinaryNumber[num1+Tag_bits+Index_bits]);
			}
			if(SET_ASSOCIATIVE==1){//if is set assosiative prints a random way to use
				Way=random_number(SET_ASSOCIATIVE_WAYS)+1;
				fprintf(Output_w,"\tWAY: %d",Way);
				printf("\tWAY: %d",Way);
			}
			fprintf(Output_w,"\n");
			printf("\n");
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
void getLine(FILE *DataBase){
	int i=0;
	char ch=fgetc(DataBase);
	while((ch!='\n')&&(ch!=EOF)){//get characters from Data.txt until it finds \n or EOF
		FilesLine[i]=ch;		//and puts them into global array FilesLine
		ch=fgetc(DataBase);	
		i++;
	}
	FilesLine[i]='\0';
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
//** Input: an integer n and a line of characters and returns the n'th word **//
void parseWord(char *characterLine,int i){
	char ch1[10]={0};
	int j=0;
	int k=0;
	int n=0;
	for(k=0;k<stringSize(characterLine);k++){//loops as many times as the size of characterLine
			if(characterLine[k]==' '&&characterLine[k+1]!=' '){//if the current character is space and the next is not knows that it went to the next word
					i--;									   //minus 1 from i which is the pointer to the current word
					k++;									   //starts the new word
			}
			if(i==0){						//if its the i'th word it starts putting this word int the ch1
					ch1[j]=characterLine[k];
					j++;
			}
	}
	ch1[j]='\0';
	equalStrings(WordInLine,ch1);//equals ch1 to global variable WordInLine
}

//** Input:a deciman number Output: the log of this number base 2 **//
int log_2(int n){
	int p=0;
	p=log(n)/log(2);//this is the formula for base 2
	return p;
}

//** Based on the Parameters finds the actual tag index and block offset of the current cache  **//
void findTagIndexBlockOffset(){
	Ram_bits=log_2(RAM_SIZE/WORD_SIZE);		//gets the number of ram bits into the global variable Ram_bits
	Cache_bits=log_2(CACHE_SIZE/WORD_SIZE); //gets the number of Cache bits into the global variable Cache_bits
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
	int cachesize=CACHE_SIZE;
	int ramsize=RAM_SIZE;
	int wordsize=WORD_SIZE;
	int wordsperblock=WORDS_PER_BLOCK;
	int i=0;
	if(cachesize>=ramsize){//cache size cant be bigger than Ram size
				printf("Error:RAM_SIZE should be bigger than CACHE_SIZE");
				_getch();
				exit(2);
	}
	if(!isLog_2(cachesize)){//cannot have cache size which is not a power of 2
				printf("Error:CACHE_SIZE should be a power of 2");
				_getch();
				exit(3);
	}
	if(!isLog_2(ramsize)){//cannot have Ram size which is not a power of 2
				printf("Error:RAM_SIZE should be a power of 2");
				_getch();
				exit(4);
	}
	if(!isLog_2(wordsperblock)){//cannot have words per block which are not a power of 2
				printf("Error:WORDS_PER_BLOCK should be a power of 2");
				_getch();
				exit(5);
	}
	if(cachesize<(WORDS_PER_BLOCK*SET_ASSOCIATIVE_WAYS*WORD_SIZE)){//data should fit into cache size
				printf("Error:CACHE_SIZE should be bigger that WORDS_PER_BLOCK*SET_ASSOCIATIVE_WAYS*WORD_SIZE");
				_getch();
				exit(6);
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

/*
void getParameters(FILE *Parameter){
	getLine(
}*/