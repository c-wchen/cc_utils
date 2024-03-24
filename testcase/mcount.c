#include <stdio.h>

int i = 0;
void mcount() {
	printf("mcount start\n");
	i++;
}

int get_sum() {

	return i;
}
