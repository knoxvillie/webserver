
#include <iostream>
#include <unistd.h>
#include <sys/types.h>

int main(void) {

	char *filename = const_cast<char *>("helloworld.py");
	char *argv[] = {NULL, filename, NULL};
	
	if (execve(filename, argv, NULL) != 0)
		std::cout << "ERROR" << std::endl;
	return (0);
}