#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main() {
	char* temp = "world";
	char* x = "hello";
	strcat(temp, x);
	printf("%s", temp);
	return 0;
}
