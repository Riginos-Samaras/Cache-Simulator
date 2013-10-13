
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
#define	RAM_SIZE 1024 
#define	CACHE_SIZE 128
#define WORD_SIZE 4
#define BLOCK_SIZE 4
#define FULL_ASSOCIATIVE 1
#define SET_ASSOCIATIVE 0
#define DIRECT_MAPPED 0
#define DATA_FILL_NUMBER 3
#define SET_ASSOCIATIVE_WAYS 4

/*****FILES*****/
FILE *DataBase_w;               //Write to Data.txt
FILE *DataBase_r;				//Read from Data.txt
FILE *Parameter_r;				//Reads the Parameters from Parameter.txt 
FILE *Output_w;					//Writes the Output

/*****FUNCTIONS*****/
int random_number(int x);	//Returns a number >=0 and <x
char cacheAction();			//Returns a random character from the list
int stringSize(char []);	//Input:a string Output: the number of the characters in string 
void decToInt(int);			//Input:a decimal number Output: binaryNumber[10] gets the
void fillDataBase(FILE*);	//Fills Data.txt with random cache action and random numbers  "R 10","W 9", "M 5"
void getLine(FILE*);		//Gets a line from a FILE and puts this line into char FilesLine 
void getParameters(FILE*);
void parseWord(char*,int);		 //Input: an integer n and a line of characters and returns the n'th word
void equalStrings(char *str1,char *str2);//Takes 2 strings and equals str2 to str1
int log_2(int);				//Input:a deciman number Output: the log of this number base 2
void printString(char *);	//Input:a string Output: prints that String
void fillOutput(FILE*);		//Input:a FILE Output in file: puts random action and random number in this file
void findTagIndexBlockOffset();	//Based on the Parameters finds the actual tag index and block offset of the current cache 

struct word{
	char word[];
};
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
	findTagIndexBlockOffset();  //finds Tag Index and Block Offset of the current cache 
	error=fopen_s(&DataBase_w,"Data.txt","w+");//opens Data.txt for filling
	fillDataBase(DataBase_w);				   //Random cache action and random numbers in Data.txt
	error=fopen_s(&DataBase_r,"Data.txt","r+");//opens Data.txt for parsing
	error=fopen_s(&Output_w,"Output.txt","w+");//opens Output.txt for filling
	fillOutput(Output_w);		//fills Output w
	printf("tag %d  index %d",Tag_bits,Index_bits);
	//getParameters(FILE*);
	_getch();	
	exit(0);	//exits 0 when everything gone right
}	//end of main()


int random_number(int num){
    return rand()%num;
    }//end of random_number(int num)

int stringSize(char A[]){
    int SizeOfList=0;
    while(1){
        if(A[SizeOfList]!='\0')
            SizeOfList++;
        else 
			break;
    }
    return SizeOfList;
}

char cacheAction(){
    char Rand_c;
    int Rand_i;
    char List[]="RWMF";
    Rand_i=random_number(stringSize(List));
    Rand_c=List[Rand_i];
    return Rand_c;
}//end of cacheAction()

void fillDataBase(FILE *DataBase){
        int i=0;
        char ch;
        int y;
        if(DataBase!=NULL){
             while(i<LinesInDataBase){
                ch=cacheAction();
                y=random_number(MAX_NUMBER);
                fprintf(DataBase,"%c %d\n",ch,y);
                i++;
             }
             fclose(DataBase);
        }
        else
            printf("File does not exists");

}//end of fillDataBase(FILE *DataBase)

void fillOutput(FILE *Output_w){
	int num,num1=0;
	for(num=0;num<LinesInDataBase;num++){
			getLine(DataBase_r);
			parseWord(FilesLine,1);
			decToInt(atoi(WordInLine));
			printf("DECIMAL: %d\tBINARY: ",atoi(WordInLine));
			fprintf(Output_w,"DECIMAL: %d\tBINARY: ",atoi(WordInLine));
			for(num1=0;num1<Ram_bits;num1++){
				fprintf(Output_w,"%d",BinaryNumber[num1]);
				printf("%d",BinaryNumber[num1]);
			}
			printf("  TAG: ");
			fprintf(Output_w,"  TAG: ");
			for(num1=0;num1<Tag_bits;num1++){
				fprintf(Output_w,"%d",BinaryNumber[num1]);
				printf("%d",BinaryNumber[num1]);
			}
			printf("\tINDEX: ");
			fprintf(Output_w,"\tINDEX: ");
			for(num1=0;num1<Index_bits;num1++){
				fprintf(Output_w,"%d",BinaryNumber[num1+Tag_bits]);
				printf("%d",BinaryNumber[num1+Tag_bits]);
			}
			printf("  BLOCK OFFSET: ");
			fprintf(Output_w,"  BLOCK OFFSET: ");
			for(num1=0;num1<BlockOffset_bits;num1++){
				fprintf(Output_w,"%d",BinaryNumber[num1+Tag_bits+Index_bits]);
				printf("%d",BinaryNumber[num1+Tag_bits+Index_bits]);
			}
			if(SET_ASSOCIATIVE==1){
				Way=random_number(SET_ASSOCIATIVE_WAYS)+1;
				fprintf(Output_w,"\tWAY: %d",Way);
				printf("\tWAY: %d",Way);
			}
			fprintf(Output_w,"\n");
			printf("\n");
	}

}
void decToInt(int decimalNumber){
    int remainder,quotient;
    int i=0,j=0;
	int k=0;
	j=log_2(decimalNumber);
    for(i=Ram_bits-1;i>=0;i--){
		j=decimalNumber>>i;
		if (j & 1)
			BinaryNumber[k]=1;
		else
		    BinaryNumber[k]=0;
		k++;
    }
	 BinaryNumber[k]='\0';
}
void getLine(FILE *DataBase){
	int i=0;
	char ch=fgetc(DataBase);
	while((ch!='\n')&&(ch!=EOF)){
		FilesLine[i]=ch;
		ch=fgetc(DataBase);
		i++;
	}
	FilesLine[i]='\0';
}
void equalStrings(char *str1,char *str2){
	int n=0;
	while(1){
		if(str2[n]=='\0'){
			str1[n]='\0';
			break;
		}
		else{
			str1[n]=str2[n];
			n++;
		}
	}
}
void printString(char *String){
	int num=0;
	while(1){
		if(String[num]=='\0')
			break;
		else{
			printf("%c",String[num]);
			num++;
		}	
	}
}
void parseWord(char *characterLine,int i){
	char ch[10]={0};
	char ch1[10]={0};
	int j=0;
	int k=0;
	int n=0;
	equalStrings(ch,characterLine);
	for(k=0;k<stringSize(ch);k++){
			if(ch[k]==' '&&ch[k+1]!=' '){
					i--;
					k++;
			}
			if(i==0){
			ch1[j]=ch[k];
			j++;
			}
	}
	equalStrings(WordInLine,ch1);
}
int log_2(int n){
	int p=0;
	p=log(n)/log(2);
	return p;
}
void findTagIndexBlockOffset(){
	Ram_bits=log_2(RAM_SIZE/WORD_SIZE);
	Cache_bits=log_2(CACHE_SIZE/WORD_SIZE);
	MAX_NUMBER=pow(Ram_bits,2)-1;
	LinesInDataBase=(MAX_NUMBER+1)*DATA_FILL_NUMBER;
	if(DIRECT_MAPPED&&!SET_ASSOCIATIVE&&!FULL_ASSOCIATIVE){
		BlockOffset_bits=log_2(BLOCK_SIZE);
		Index_bits=Cache_bits-BlockOffset_bits;
		Tag_bits=Ram_bits-Index_bits-BlockOffset_bits;
	}
	else if(!DIRECT_MAPPED&&SET_ASSOCIATIVE&&!FULL_ASSOCIATIVE){
		BlockOffset_bits=log_2(BLOCK_SIZE);
		Index_bits=log_2(CACHE_SIZE/WORD_SIZE/SET_ASSOCIATIVE_WAYS)-BlockOffset_bits;
		Tag_bits=Ram_bits-Index_bits-BlockOffset_bits;
	}
	else if(!DIRECT_MAPPED&&!SET_ASSOCIATIVE&&FULL_ASSOCIATIVE){
		BlockOffset_bits=log_2(BLOCK_SIZE);
		Tag_bits=Ram_bits-BlockOffset_bits;
	}
	else{
		printf("Error:DIRECT_MAPPED SET_ASSOCIATIVE FULL_ASSOCIATIVE only one must be true");
		_getch();
		exit(1);
	}
}

/*
void getParameters(FILE *Parameter){
	getLine(
}*/