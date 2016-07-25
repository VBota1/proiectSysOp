
#define CMDLISTSIZE 255
#define NEWLINE '\n'

int gListIndex = 0 ;
char *gCommandList [ CMDLISTSIZE+1 ] ;


void fPush ( char *command );
int fIndexOfChar ( char *string, char delimiter  );

void fPush ( char *command )
{
	int loopcount;
	int delimiter;

	delimiter = fIndexOfChar( command, NEWLINE );
	command [ delimiter ] = '\0';
	gCommandList [gListIndex] = (char*) calloc(1, sizeof(char)* delimiter);
	gCommandList [gListIndex] = command;
	
	if ( CMDLISTSIZE > gListIndex )
			gListIndex ++;
	else
		for ( loopcount=0; loopcount<CMDLISTSIZE; loopcount++ )
		{
			gCommandList [loopcount] = gCommandList [loopcount+1];
		}
}

//returns -1 if delimiter not found
//return position of delimiter if found
int  fIndexOfChar ( char *string, char delimiter )
{
	int retcode = -1; //delimiter not found 
	int loopcount;
	
	for ( loopcount=0; loopcount<=sizeof(string); loopcount++ )
		if ( delimiter==string[loopcount] )
		{
			retcode = loopcount;
			break;
		}
	
	return retcode;
}