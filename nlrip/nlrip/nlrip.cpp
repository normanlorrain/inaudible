// nlrip.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CAudioCD.h"
#include "AudioCD_Helpers.h"


void cderror(void)
{
	DWORD errorMessageID = GetLastError();
	LPSTR messageBuffer = nullptr;

	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
	fprintf(stderr, "CD error %s\n", messageBuffer);



	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);


	

}

int main(int argc, char ** argv)
{
	if (argc != 3)
	{
		fprintf(stderr,  "Usage: nlrip <drive letter> <outputfile>\n");
		return 1;
	}
	CAudioCD AudioCD;
	if (!AudioCD.Open(argv[1][0]))
	{
		fprintf(stderr, "Cannot open cd-drive!\n");
		cderror();
		return 2;
	}
	

	AudioCD.ExtractAllTracks(argv[2]);

	if (!AudioCD.UnlockCD())
	{
		fprintf(stderr, "unable to unlock CD\n");
		cderror();
		return 3;
	}
	if (!AudioCD.EjectCD())
	{
		fprintf(stderr, "unable to eject CD");
		cderror();
		return 4;
	}


	return 0;
}

