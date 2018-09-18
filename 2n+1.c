#include<stdio.h>
int main(int argc, char const *argv[]){
	printf("input a number\n");
	int a,b;
	scanf("%d",&a);
 	b=single(a);
 	printf("value=%d\n",b);
	return 0;
}
int single(int a){
	int b=0;
	for(int i=1;i<a;i+=2){
	b+=i;
	}
	return b;
}
