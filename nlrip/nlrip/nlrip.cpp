// nlrip.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CAudioCD.h"
#include "AudioCD_Helpers.h"


int main(int argc, char ** argv)
{
	if (argc != 2)
	{
		printf("Usage: nlrip <outputfile>");
	}
	CAudioCD AudioCD;
	if (!AudioCD.Open('D'))
	{
		printf("Cannot open cd-drive!\n");
		return 0;
	}
	

	AudioCD.ExtractAllTracks(argv[1]);

	AudioCD.UnlockCD();
	AudioCD.EjectCD();


	return 0;
}

