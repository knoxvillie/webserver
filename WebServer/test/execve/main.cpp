
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

int main(void) {

	char *filename = const_cast<char *>("../../var/www/cgibin/helloworld.py");
	char *argv[] = {NULL, filename, NULL};
	
	if (execve(filename, argv, NULL) != 0)
		std::cout << "ERROR: " << strerror(errno) << std::endl;
	return (0);
}