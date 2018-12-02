#include "aracne.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

string parseHttp (string bufferRequest) {

	FILE * request;
	char c, hostURL[150];
	char * hostPosition;

	request = fopen("request.txt","w+");

	//fputs(&bufferRequest[0], request);

	hostPosition = strstr(&bufferRequest[0], "www.");

	while(*(hostPosition) != '\n' && *(hostPosition) != '\0')
	{
		strncat(hostURL, hostPosition, 1);
		hostPosition++;
	}

	fputs(&hostURL[0], request);

	fclose(request);

    return bufferRequest;
}