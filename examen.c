//TODO: 1. The "diff" command. Parameters that need to be implemented are: -a -q
//TODO: 2. The "chmod" command. No parameters need to be implemented.
//TODO: 3. The "more" command. Parameters that need to be implemented are: -num -d -s
//TODO: 4. Your program must also support pipes in the commands, e.g. > ls -l | grep -e 'tmp' | wc -l (it must work with any type of command!)
//TODO: 5. Your program must also support redirection in the commands, e.g. > ls -l > out.txt (it must work with any type of command!)
//TODO: 6. Your program must support both pipes and redirection in the same command line, e.g. > ls -l | grep -e 'tmp' > out.txt (it must work with any type of command!)

#include <stdio.h>
#include <stdlib.h>
#include <termios.h> 
#include <unistd.h> 
#include "commandslibrary.c"

#define CMDHEAD "$>"

char *fGetCommand ();
int fInterpretCommand  ( char *command  );
void fClearConsoleLine ( int noOfChars );

void main ()
{
	static struct termios initialTerminalSetup, noFeedTerminalSetup;
	char *command;

	//record initial terminal settings
	tcgetattr( STDIN_FILENO, &initialTerminalSetup);
	noFeedTerminalSetup = initialTerminalSetup;

	//deactivate wait for \n to get command
	noFeedTerminalSetup.c_lflag &= ~(ICANON | ECHO);    
	tcsetattr( STDIN_FILENO, TCSANOW, &noFeedTerminalSetup);

	printf ("%s",CMDHEAD);
	do
	{
		command = fGetCommand ();
	}while (  fInterpretCommand ( command  )  );

	//revert to initial terminal settings
	tcsetattr( STDIN_FILENO, TCSANOW, &initialTerminalSetup);
}

//returns the read command
char *fGetCommand ()
{
	int localCommandIndex = gListIndex; //used to navigate through command history
	char *command=NULL;
	char tmpchar;
	int charcount = 1;

	command = (char*) malloc( sizeof(char) );
	do 
	{	
		tmpchar = getchar();

		if ( 127== tmpchar ) //treat backspace
		{
			charcount--;
			if ( 1<=charcount )
			{
				command[charcount-1]=' ';
				command[charcount]='\0';
				printf (	"\r%s%s\b", CMDHEAD, command );
			}
			else
				charcount = 1;
		}
		else if ( 27== tmpchar ) //treat arrows
		{
			getchar();
			tmpchar = getchar();
	
			if ( 65 == tmpchar  ) //upparrow
			{
				localCommandIndex--; //point to previous command in command hisotry
				if ( (0 <= localCommandIndex) &&  (gListIndex > localCommandIndex) )
				{
					fClearConsoleLine ( charcount-1 );
					charcount = strlen ( gCommandList[localCommandIndex] )+1; //get size of command
					command = realloc( command, charcount );
					memcpy ( command, gCommandList[localCommandIndex], charcount ); //copy indicated command from command history
					printf ( "%s", command  );
				}
				else
					localCommandIndex++; //point to next command in command hisotry
	
			}
			else if ( 66 == tmpchar  ) //downarrow
			{
				localCommandIndex++; //point to next command in command hisotry
				if ( (0 <= localCommandIndex) &&  (gListIndex > localCommandIndex) )
				{
					fClearConsoleLine ( charcount-1 );
					charcount = strlen ( gCommandList[localCommandIndex] )+1; //get size of command
					command = realloc( command, charcount );
					memcpy ( command, gCommandList[localCommandIndex], charcount ); //copy indicated command from command history
					printf ( "%s", command  ); 
				}
				else if ( gListIndex == localCommandIndex )
				{
					fClearConsoleLine ( charcount-1 );
					command=NULL; //clear command
					charcount = 1; //reset number of charaters in command
				}
				else
					localCommandIndex--; //point to previous command in command hisotry
			}
/*
			//else if ( 67 == tmpchar  ) //right arrow
			//else if ( 68 == tmpchar  ) //left arrow
			else
			{
				putchar( NEWLINE );
				command = NULL;
				command = realloc(command, sizeof(char)*11); 
				command = UNSUPPORTEDCMD; //set unsupported command
				tmpchar =  NEWLINE;
			}
*/
		}
		else if ( (NEWLINE == tmpchar) && ( 1==charcount ) )
		{
			putchar( NEWLINE );
			printf(CMDHEAD);
			tmpchar = '\0';
		}
		else
		{
			putchar(tmpchar);
			charcount++;
			if ( charcount >= CMDMAXSIZE )
			{
				putchar( NEWLINE );
				tmpchar =  NEWLINE;
			}
			else
			{
				command = realloc ( command, charcount);
				command[charcount-2]=tmpchar;
				command[charcount]='\0';
			}
		}
	}while (  NEWLINE  != tmpchar );

	return command;
}


//return 1 if exit command (EXITCODE) not recevied
//return 0 if exit command (EXITCODE) recevied
int fInterpretCommand ( char *command  )
{
	char *localcommand;
	int retcode = 1; //continue execution
	size_t delimiterpostion;

	localcommand = (char *) malloc ( strlen ( command ) );
	memcpy ( localcommand, command, strlen ( command ) );

	delimiterpostion=(size_t)fCMDHLIBindexOfCh( localcommand, NEWLINE );

	if ( -1 == delimiterpostion  )
	{
		fCMDLIBerrnohandle ( E2BIG );
		printf ( "%s", CMDHEAD );
	}
	else if ( 0==strncmp ( EXITCMD, localcommand, delimiterpostion ) && ( 0<delimiterpostion) )
			retcode = 0; //exit main loop
	else if ( 0 == fCMDLIBindexOfStr ( localcommand, CHMODCMD ) && ( 0<delimiterpostion) )
	{
		fCMDHLIBpush ( localcommand );  //record command
		fCMDLIBchmod ( localcommand ); //excute command
		printf ( "%s", CMDHEAD );
	}
	else if ( 0 == fCMDLIBindexOfStr ( localcommand, DIFFCMD ) && ( 0<delimiterpostion) )
	{
		fCMDHLIBpush ( localcommand );  //record command
		//fCMDLIBdiff ( localcommand ); //excute command
		printf ( "%s", CMDHEAD );
	}
	else if ( 0==strncmp ( VERSIONCMD, localcommand, delimiterpostion ) && ( 0<delimiterpostion) )
	{
		fCMDHLIBpush ( localcommand );  //record command
		fCMDLIBversion ( localcommand ); //excute command
		printf ( "%s", CMDHEAD );
	}	
	else if ( 0==strncmp ( HELPCMD, localcommand, delimiterpostion ) && ( 0<delimiterpostion) )
	{
		fCMDHLIBpush ( localcommand );  //record command
		fCMDLIBhelp ( localcommand ); //excute command
		printf ( "%s", CMDHEAD );
	}	
	else if ( 0==strncmp ( UNSUPPORTEDCMD, localcommand, delimiterpostion ) && ( 0<delimiterpostion) )
	{ 
		fCMDLIBerrnohandle ( ENOSYS );
		printf ( "%s", CMDHEAD );
	}
	else
	{ 
		fCMDHLIBpush ( localcommand ); //record command
		fCMDLIBerrnohandle ( ENOSYS );
		printf ( "%s", CMDHEAD );
	}

	return retcode;
}

void fClearConsoleLine ( int noOfChars )
{
	int loopcount;

	//clear old text
	for ( loopcount=noOfChars; loopcount>0; loopcount-- )
	{
		putchar ( '\b' );
		putchar ( ' ' );
		putchar ( '\b' );
	}

	putchar ( '\0' );
}
