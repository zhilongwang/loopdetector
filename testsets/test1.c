#include <stdio.h>
#include <stdlib.h>
int main(){

	int a, b, c;
	for (int i =0; i<=10; i++){
		if (a>0){
			b = 20;
		}else{
			b = 30;
		}
		while(b){
			c += a*b;
			if (c%8 == 0){
				printf("%d\n", c);
			}
			b--;
		}
	}
	return 0;
}
