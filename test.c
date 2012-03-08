#include<stdio.h>
#include<string.h>
#include<stdlib.h>

void send(char*, int length);

int main(){

/*	int array[3];
	array[0] = 1;
	array[1] = 2;
	array[2] = 3;

	char* message1 = malloc(sizeof(char)*strlen("hello")+1);
	char* message2 = malloc(sizeof(char)*strlen("world")+1);
	strcpy(message1, "hello");
	strcpy(message2, "world");
	int len = strlen(message1) + strlen(message2) + 1;
	char* newmessage = malloc(len*sizeof(char));

	void *temp = malloc(sizeof(int));
	strcpy(temp, "%d", array[0]);

	printf("temp=%s\n", temp);
*/

	char integer_str[13];
	int integer = 60;
	int num2=60;
	int num3 = 60;

	sprintf(integer_str, " %d %d %d", integer, num2, num3);

	printf("integer_str=%s\n", integer_str);

	int len = strlen("hello")+1;
	len+=strlen(integer_str);

	char *other_str = malloc(sizeof(char)*(6+strlen(integer_str)));
	strcpy(other_str, "hello");

	strcat(other_str, integer_str);
	printf("other_str=%s\n", other_str);
	
	int n1=0, n2=0,n3=0;

	char message[6];
	//char newmessage[12] = "hello 10 20 30";
	sscanf(other_str, "%s %d %d %d", message, &n1, &n2, &n3);
	//sscanf(newmessage, "%s %d %d %d", message, &n1, &n2, &n3);

	printf("n1=%d, n2=%d, n3=%d, str=%s\n", n1, n2 , n3, message);



/*
	strcpy(newmessage,message1);
	char* pos = newmessage +strlen(message1);
	strcpy(pos,message2);

	printf("newmessage=%s\n", newmessage);
*/
	return 0;
}
