#include<stdio.h>

void send(char*, int length);

int main(){

	char str[10];
	int seq=15;
	
	sprintf(str, "%d", seq);
	send(str,1);
	//printf("mess=%s\n", str);

	return 0;
}


void send(char* message, int length){

	if(message != NULL)
		printf("message=%s, len=%lu\n", message, sizeof(message));

	int num = atoi(message);

	printf("num=%d\n", num);
	num++;
	printf("num++=%d\n", num);


}
