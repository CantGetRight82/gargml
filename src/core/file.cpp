#include "file.h"

#include <stdio.h>

char* newRead( const char* path) {
	FILE* fh = fopen( path, "rb");
	if(fh == NULL) {
		return NULL;
	}

	fseek(fh, 0, SEEK_END);
	int size = ftell(fh);
	fseek(fh, 0, SEEK_SET);


	char* buffer = new char[ size+1 ];
	fread(buffer, 1, size, fh);
	fclose(fh);
	buffer[size] = '\0';
	return buffer;

}
