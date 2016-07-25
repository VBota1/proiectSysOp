#include <string.h>

#define CMDLISTSIZE 255
#define NEWLINE '\n'

int gListIndex = 0 ;
char *gCommandList [ CMDLISTSIZE+1 ] ;

void fCMDHLIBpush ( char *command );
int fCMDHLIBindexOfCh ( char *string, char delimiter  );

void fCMDHLIBpush ( char *command )
{
	char *localcommand;
	int loopcount;
	int delimiter;

	localcommand = (char *) malloc ( strlen ( command ) );
	memcpy ( localcommand, command, strlen ( command ) );

	delimiter = fCMDHLIBindexOfCh( localcommand, NEWLINE );
	if ( -1!=delimiter )
	{
		localcommand [ delimiter ] = '\0';
		gCommandList [gListIndex] = (char*) malloc( sizeof(char)* delimiter );
		memcpy ( gCommandList [gListIndex], localcommand, sizeof(char)* delimiter );

		if ( CMDLISTSIZE > gListIndex )
				gListIndex ++;
		else
			for ( loopcount=0; loopcount<CMDLISTSIZE; loopcount++ )
			{
				//TODO realloc memeory
				gCommandList [loopcount] = gCommandList [loopcount+1];
			}
	}
}

//returns -1 if delimiter not found
//return position of delimiter if found
int  fCMDHLIBindexOfCh ( char *string, char delimiter )
{
	char *localstring;
	int retcode = -1; //delimiter not found 
	char *tmpchr;

	localstring = (char *) malloc ( strlen ( string ) );
	memcpy ( localstring, string, strlen ( string ) );

	tmpchr = strchr ( localstring, delimiter );
	if ( NULL != tmpchr )
		retcode = tmpchr - localstring;

	return retcode;
}
