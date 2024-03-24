#include <stdio.h>

int add(int i, int j) {
	
	printf("%s:%d return = %p, frame = %p\n", __FILE__, __LINE__, __builtin_return_address(0), __builtin_frame_address(0));
	printf("%s:%d return = %p, frame = %p\n", __FILE__, __LINE__, __builtin_return_address(1), __builtin_frame_address(1));
	return i + j;
}

int add_wrap(int i, int j) {
	printf("%s:%d return = %p, frame = %p\n", __FILE__, __LINE__, __builtin_return_address(0), __builtin_frame_address(0));
	return add(i, j);
}


// int main() {
// 	add_wrap(1, 2);
// 	return 0;
// }
