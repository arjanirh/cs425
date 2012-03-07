#include<stdio.h>

void send(char*, int length);

int main(){

	int num1, num2;
	printf("Enter two numbers bitte\n");
	scanf("%d %d", &num1, &num2);
	int num3;
	num3=num1 + num2;
	printf("%d", num3);
	return 0;
}
