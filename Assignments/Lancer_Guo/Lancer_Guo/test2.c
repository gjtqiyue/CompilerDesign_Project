#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main() { 
	int x = 0;
	float result = 0.000000;
	int n = 0;
	scanf("%d", &x);
	while ( ( n < 1000 ) ) 
	int exp = 1;
	int counter = 1;
	if ( ( n == 0 ) ) { 
		exp = 1;
	} 
	else { 
		while ( ( counter <= n ) ) 
		exp = exp * x;
		counter = counter + 1;
			} 
	} 
	int fact = 1;
	int iter = 1;
	if ( ( n == 0 ) ) { 
		fact = 1;
	} 
	else { 
		while ( ( iter <= n ) ) 
		fact = fact * iter;
		iter = iter + 1;
			} 
	} 
	result = result + (exp / fact);
	n = n + 1;
		} 
	printf("%s \n", "result of e^x with x=");
	return 0;
}
