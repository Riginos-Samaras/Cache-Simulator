
#include <stdio.h>
#include <conio.h>
#include <time.h>
int random_number(int x);//Returns a number >=0 and <x
char cacheAction();//Returns a random character from the list
int list_Size(char []);//Returns the size of a char list
void fillDataBase(FILE*);
struct Cache
{
 int Tag;
 int Index;
 int BlockOffSet;
 struct Cache *head;
 struct Cache *tail;
};

int main(){
    struct Cache addresses;
    srand(time(NULL));
	FILE *DataBase;                 //a file variable
	DataBase= fopen("Data.txt","w");
	fillDataBase(DataBase);//Fills Data.txt with random cache action and random numbers  "R 10","W 9" , "M 5"
	return 1;
}	//end of main()


int random_number(int num){
    return rand()%num;
    }

int list_Size(char A[]){
    int SizeOfList=0;
    while(1){
        if(A[SizeOfList]!=0)
            SizeOfList++;
        else break;
    }
    return SizeOfList;
}
char cacheAction(){
    char Rand_c;
    int Rand_i;
    char List[]="RWM";
    Rand_i=random_number(list_Size(List));
    Rand_c=List[Rand_i];
    return Rand_c;
}
void fillDataBase(FILE *DataBase){
        int i=0;
        char x;
        int y;
        if(DataBase!=NULL){
             while(i<10){
                x=cacheAction();
                y=random_number(10);
                fprintf(DataBase,"%c %d\n",x,y);
                i++;
             }
             fclose(DataBase);
        }
        else
            printf("File does not exists");

}
