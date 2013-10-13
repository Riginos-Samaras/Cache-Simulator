#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include<conio.h>
#include <malloc.h>
#include <time.h>
errno_t error;
#define _CRT_SECURE_NO_DEPRECATE
#define	RAM_SIZE 1024 
#define	CACHE_SIZE 128
#define WORD_SIZE 4
#define BLOCK_SIZE 4
#define FULL_ASSOCIATIVE 1
#define SET_ASSOCIATIVE 0
#define DIRECT_MAPPED 0
#define DATA_FILL_NUMBER 3

/*****FILES*****/
FILE *DataBase_w;               //Write to Data.txt
FILE *DataBase_r;				//Read from Data.txt
FILE *Parameter_r;				//Reads the Parameters from Parameter.txt 
FILE *Output_w;					//Writes the Output
/*****FUNCTIONS*****/
int random_number(int x);	//Returns a number >=0 and <x
char cacheAction();			//Returns a random character from the list
int stringSize(char []);		//Returns the size of a char list
int intListSize(int []);
void decToInt(int);
void fillDataBase(FILE*);	//Fills Data.txt with random cache action and random numbers  "R 10","W 9", "M 5"
void getLine(FILE*);
void getParameters(FILE*);
void parseWord(char*,int);		//Takes as input an integer n and a line of characters and returns the n'th word
void equalStrings(char *,char *);//Takes 2 strings and equals str1 to str2
int log_2(int);
void printString(char *);
void fillOutput(FILE*);
void findTagIndexBlockOffset();

struct word{
	char word[];
};

struct Cache
{
 int Tag;
 int Index;
 int BlockOffSet;
 struct Cache *head;
 struct Cache *tail;
};
//enum cache_policy{
	//direct_map,nodirext
//};

int binaryNumber[10];
int sizeOfBinaryNumber=0;
char line[30]={NULL};
char word[10]={NULL};
int linesInDataBase=20;
int Ram_bits=0;
int Cache_bits=0;
int MAX_NUMBER=0;
int Tag_bits=0;
int Index_bits=0;
int BlockOffset_bits=0;
int main(int argc, char ** argv){
	struct Cache addresses;
	int num=0;
	int num1=0;
	char num3[]="dsaddddd";
	char num4[]="sadsda";
	srand(time(NULL));
	findTagIndexBlockOffset();
	addresses.Index=Ram_bits-Cache_bits;
	addresses.Tag=Ram_bits-addresses.Index;
	error=fopen_s(&DataBase_w,"Data.txt","w+");
	fillDataBase(DataBase_w);//Random cache action and random numbers in Data.txt
	error=fopen_s(&DataBase_r,"Data.txt","r+");//opening Data.txt for parsing
	error=fopen_s(&Output_w,"Output.txt","w+");
	fillOutput(Output_w);
	
	printf("tag %d  index %d",addresses.Tag,addresses.Index);
	//printf("%d",atoi("112"));
	//getParameters(FILE*);

	_getch();
return 1;

}	//end of main()


int random_number(int num){
    return rand()%num;
    }

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
}
void fillDataBase(FILE *DataBase){
        int i=0;
        char ch;
        int y;
        if(DataBase!=NULL){
             while(i<linesInDataBase){
                ch=cacheAction();
                y=random_number(MAX_NUMBER);
                fprintf(DataBase,"%c %d\n",ch,y);
                i++;
             }
             fclose(DataBase);
        }
        else
            printf("File does not exists");

}
void fillOutput(FILE *Output_w){
	int num,num1=0;
	for(num=0;num<linesInDataBase;num++){
			getLine(DataBase_r);
			parseWord(line,1);
			printf("%d ",stringSize(line));
			decToInt(atoi(word));
			printf("DECIMAL: %d  BINARY:",atoi(word));
			fprintf(Output_w,"DECIMAL: %d  BINARY:",atoi(word));
			for(num1=0;num1<Ram_bits;num1++){
				fprintf(Output_w,"%d",binaryNumber[num1]);
				printf("%d",binaryNumber[num1]);
			}
			printf("  TAG: ");
			fprintf(Output_w,"  TAG: ");
			for(num1=0;num1<Tag_bits;num1++){
				fprintf(Output_w,"%d",binaryNumber[num1]);
				printf("%d",binaryNumber[num1]);
			}
			printf("  INDEX: ");
			fprintf(Output_w,"  INDEX: ");
			for(num1=0;num1<Index_bits;num1++){
				fprintf(Output_w,"%d",binaryNumber[num1+Tag_bits]);
				printf("%d",binaryNumber[num1+Tag_bits]);
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
		//printf("%d_",j);
		if (j & 1)
			binaryNumber[k]=1;
		else
		    binaryNumber[k]=0;
		k++;
    }
	 binaryNumber[k]='\0';
}
void getLine(FILE *DataBase){
	int i=0;
	char ch=fgetc(DataBase);
	while((ch!='\n')&&(ch!=EOF)){
		line[i]=ch;
		ch=fgetc(DataBase);
		i++;
	}
	line[i]='\0';
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
	equalStrings(word,ch1);
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
	linesInDataBase=(MAX_NUMBER+1)*DATA_FILL_NUMBER;
	if(DIRECT_MAPPED&&!SET_ASSOCIATIVE&&!FULL_ASSOCIATIVE){
		Index_bits=Ram_bits-Cache_bits;
		Tag_bits=Ram_bits-Index_bits;
	}
	else if(!DIRECT_MAPPED&&SET_ASSOCIATIVE&&!FULL_ASSOCIATIVE){
		Tag_bits=Ram_bits;
	}
	else if(!DIRECT_MAPPED&&!SET_ASSOCIATIVE&&FULL_ASSOCIATIVE){
		Tag_bits=Ram_bits-BlockOffset_bits;
	}
	else{
		printf("Error:DIRECT_MAPPED SET_ASSOCIATIVE FULL_ASSOCIATIVE only one must be true");
		_getch();
		exit(0);
	}
}

/*
void getParameters(FILE *Parameter){
	getLine(
}*/