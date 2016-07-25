#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include "commandhistorylibrary.c"

#define CMDMAXSIZE 256
#define CMDMAXNOOFARG 10
#define EXITCMD "exit"
#define UNSUPPORTEDCMD "Command not supported\n"
#define CHMODCMD "chmod "
#define DIFFCMD "diff "
#define MORECMD "more "
#define HELPCMD "help"
#define VERSIONCMD "version"
#define LSCMD "ls "
#define GREPCMD "grep "
#define LIBVERSION "0.3.2" //Release.Milestone.Iteration

int fCMDLIBchmod ( char *command );
int fCMDLIBdiff ( char *command );
void fCMDLIBversion ( char *command );
void fCMDLIBhelp ( char *command );
int fCMDLIBindexOfStr ( char *sourcestr, char *strtofind );
char **fCMDLIBstringSplit ( char *tosplit, char *delimiter);
void fCMDLIBerrnohandle ( int errorcode );

void fCMDLIBhelp ( char *command )
{
	//display implemented commands:
	printf ("Command: %s\n", CHMODCMD );
	printf ("\tUsage: \n\t\t%sMODE FILENAME\n", CHMODCMD );	
	printf ("\t\tChanges the mode of the file indicate by FILENAME to the value defined by MODE\n");
	printf ("\t\tMODE is accepted only in octal form\n");

	printf ("Command: %s\n", VERSIONCMD );
	printf ("\tUsage: \n\t\t%s\n", VERSIONCMD );	
	printf ("\t\tDisplays library version, required includes, and Author information\n");

	printf ("Command: %s\n", HELPCMD );
	printf ("\tUsage: \n\t\t%s\n", HELPCMD );	
	printf ("\t\tDisplays implemented commands and supported arguments\n");
}

void fCMDLIBversion ( char *command )
{
	//print value of LIBVERSION
	printf ("Lib Version: %s\n", LIBVERSION );

	//print required libraryes
	printf ("Required libraryes: \n\t%s\n", "#include \"commandhistorylibrary.c\"");	
	printf ("\t%s\n", "#include <errno.h>");	
	printf ("\t%s\n", "#include <sys/stat.h>");	
	printf ("\t%s\n", "#include <ctype.h>");		

	//release notes
	printf ("Release notes:\n");
	printf ("\t%s\n", "Known Issues:");
	printf ("\t\t%s\n", "1. Left and Right arrow keys are not supported");
	printf ("\t\t%s\n", "2. malloc is used for strings but memory is only freed at programm exit. This migh result in memory issues on some systems.");
	printf ("\t\t%s\n", "3. Code contains commented code sections in functions: fGetCommand and fInterpretCommand ");
	printf ("\t%s\n", "TODO:");
	printf ("\t\t%s\n", "1. Implement support for simbolic arguments for chmod");
	printf ("\t\t%s\n", "2. For diff pseudocode and partial implementation is avaialbe. Call to function is commented.");
	printf ("\t\t%s\n", "3. Implement the \"more\" command. Parameters that need to be implemented are: -num -d -s");
	printf ("\t\t%s\n", "4. Program must also support pipes in the commands, e.g. > ls -l | grep -e 'tmp' | wc -l (it must work with any type of command!)");
	printf ("\t\t%s\n", "5. Program must also support redirection in the commands, e.g. > ls -l > out.txt (it must work with any type of command!)");
	printf ("\t\t%s\n", "6. Your program must support both pipes and redirection in the same command line, e.g. > ls -l | grep -e 'tmp' > out.txt (it must work with any type of command!)");

	//print autor
	printf ("Author: Your name goes here \n");		
}

//returns value of errno
int fCMDLIBchmod ( char *command )
{
	char *localcommand;
	mode_t rigts = 0777;
	char rigtsstr[6];
	char filename;
	char **tmpchararray;
	int argcount = 0;
	FILE * filehandle;
	int retcode;
	int loopcount;
	int itmp;
	int arepermissionsvalid = 0;

	localcommand = (char *) malloc ( strlen ( command ) );
	memcpy ( localcommand, command, strlen ( command ) );

	//split in substrings
	tmpchararray = fCMDLIBstringSplit ( localcommand, " " );

	//count arguments +1
	while ( NULL != tmpchararray [argcount] ) 
		argcount++;
	
	//check that at least 3 strings define the command
	if (  2 >= argcount ) 
	{
		retcode = EINVAL;
		fCMDLIBerrnohandle ( retcode );
	}
	else
	{
		//go back to last valid argument
		argcount--; 

		 //test if file exists and return error if it does not
		filehandle=fopen ( tmpchararray [ argcount ], "r" );
		retcode = errno;
		if ( NULL == filehandle )
		{
			fCMDLIBerrnohandle ( retcode );
		} 
		else
		{
			//close file
			if ( 0!=fclose ( filehandle ) )
			{
				retcode = errno;
				fCMDLIBerrnohandle ( retcode );
			}

			//test if octal permissions are used 
			if ( isdigit( tmpchararray [1][0] ) )
			{
				//test that octal permisions contain only 4 char
				if ( 4!=strlen( tmpchararray [1] ) )
				{
					retcode = EINVAL;
					fCMDLIBerrnohandle ( retcode );
				}
				else
				{
					//test if octal permissions are unclean (acepted only the following chars : 0,1,2,3,4,5,6,7 ) return error
					for ( loopcount=0; loopcount<4; loopcount++ )
					{
						itmp = ( tmpchararray [1][loopcount] - '0' );
						if ( ( 0> itmp) || ( 7< itmp ) )
						{
							retcode = EINVAL;
							fCMDLIBerrnohandle ( retcode );
							break;
						}
					}					
					
					if ( 4<=loopcount )
					{		
						//test if more than 1 substring is used to set octal permissions return format error
						if (	2!=argcount )
						{
							retcode = EINVAL;
							fCMDLIBerrnohandle ( retcode );
						}
						else //read octal permissions
						{
							memcpy ( rigtsstr, tmpchararray [1], 6 );
							rigts = strtol ( rigtsstr, NULL, 8 );
							arepermissionsvalid = 1;
						}
					}
				}
			}
			else //symbolic permissions are used
			{
				//until symbolic permissions will be supported
				retcode = EINVAL;
				fCMDLIBerrnohandle ( retcode );

				//if symbolic permissions are unclean (acepted only the following chars : u,o,g,+,-,=,u,g,o,r,w,x ) return error
	
				//else get list of operators indexes (max number ‌g of accepted operators is 3)
					//if more than 3 substring is used to set octal permissions return format error
	
					//else get current octal permisions
					//calculate new octal persmissions
			}

			if ( 1==arepermissionsvalid ) //set octal permisions
				if ( 0!=chmod ( tmpchararray [ argcount ], rigts ) )
				{
					retcode = errno;
					fCMDLIBerrnohandle ( retcode );
				}
		}
	}

	return retcode;
}

//TODO: 1. The "diff" command. Parameters that need to be implemented are: -a -q
//returns value of errno
int fCMDLIBdiff ( char *command )
{
	char *localcommand;
	char **tmpchararray;
	int argcount = 0;
	int optcount = 0;
	int loopcount;
	int retcode = 0; //no error
	int istextoptionactive = 0;//not active
	int isbriefoptionactive = 0;//not active
	FILE * filehandle1; 
	FILE * filehandle2 ;
	char *fopenmode;

	localcommand = (char *) malloc ( strlen ( command ) );
	memcpy ( localcommand, command, strlen ( command ) );

	//split command in to strings
	tmpchararray = fCMDLIBstringSplit ( localcommand, " " );

	//count arguments +1
	while ( NULL != tmpchararray [argcount] ) 
		argcount++;

	//count the number of sustrings that are options (check for char '-' )
	for ( loopcount=0; loopcount<argcount; loopcount++ )
		if ( '-' == tmpchararray [ loopcount ] [0] )
			optcount++;

	//test that at least 3 substrings are generated + the number of options ( command + file1 + file2 + noOptions )
	if ( 3!= (argcount-optcount) )
	{
		retcode = EINVAL;
		fCMDLIBerrnohandle ( retcode );
	}
	else
	{
		//for all options -> validate options format
			//activate option flag for each valid option
		for ( loopcount=1; loopcount<=optcount; loopcount++ )
		{
			if ( '\0'!=tmpchararray [ loopcount ] [2] )
			{
				retcode = EINVAL;
				fCMDLIBerrnohandle ( retcode );
			}
			else if ( 'a'==tmpchararray [ loopcount ] [1] )
				istextoptionactive = 1;
			else if ( 'q'==tmpchararray [ loopcount ] [1] )
				isbriefoptionactive = 1;			
			else //option not recognised
			{
				retcode = EINVAL;
				fCMDLIBerrnohandle ( retcode );
			}
		}

		if ( 0==retcode )
		{
			//set oppning mode
			if ( istextoptionactive )
				fopenmode = "r+t";
			else
				fopenmode = "r+b";

			// test if files can be opened //Open files read mode
			filehandle2=fopen ( tmpchararray[argcount-2], fopenmode );
			retcode = errno;
			if ( NULL == filehandle2 )
				fCMDLIBerrnohandle ( retcode );
			else
			{
				filehandle1=fopen ( tmpchararray[argcount-1], fopenmode );
				retcode = errno;
				if ( NULL == filehandle1 )
					fCMDLIBerrnohandle ( retcode );
				else
				{
					//while ( file1has lines and file2has lines )
						//readline file1
						//readline file2
						//compare lines
							//if lines are diferent print lines
							//if lines are equal and -q option is disabled printlines

					//if file1 lines > file2 lines
						//print remaining lines from file1

					//if file2 lines > file1 lines
						//print remaining lines from file2

					//close file
					if ( 0!=fclose ( filehandle1 ) )
					{
						retcode = errno;
						fCMDLIBerrnohandle ( retcode );
					}
				}
			
				//close file
				if ( 0!=fclose ( filehandle2 ) )
				{
					retcode = errno;
					fCMDLIBerrnohandle ( retcode );
				}
			}
		}
	}

	//return retval (last errno code)
	return retcode;	
}

//returns -1 if string to find is not present in the source string
//return position of string to find if present in the source string
int fCMDLIBindexOfStr ( char *str, char * strtofind )
{
	char *localstr;
	int retcode = -1; //strtofind not found 
	char *tmpstr;

	localstr = (char *) malloc ( strlen ( str ) );
	memcpy ( localstr, str, strlen ( str ) );

	tmpstr = strstr ( localstr, strtofind );	

	if ( NULL == tmpstr )
		return retcode;
	else
	{
		retcode = tmpstr - localstr;
	}
	
}

//returns an array of substrings
char **fCMDLIBstringSplit ( char *tosplit, char *delimiter)
{
	char *localtosplit;
	char *tmpstr = NULL;
	char **returnchar;
	int loopcount=0; 
	int loopcount2;

	localtosplit = (char *) malloc ( strlen ( tosplit ) );
	memcpy ( localtosplit, tosplit, strlen ( tosplit ) );

	//clear trailing \0
	loopcount2 = 0;
	while ( 0xa!=( localtosplit[loopcount2] ) )
		loopcount2++;
	localtosplit [loopcount2] = ' ';
	localtosplit [loopcount2] = '\0';

	tmpstr = strtok ( localtosplit, delimiter );	

	returnchar = (char**)  malloc ( CMDMAXNOOFARG );

	while ( NULL != tmpstr )
	{
		returnchar [ loopcount ] =(char*) malloc ( strlen(tmpstr) );
		memcpy ( returnchar [ loopcount ], tmpstr, strlen(tmpstr) ) ;
		loopcount++;
		returnchar [ loopcount ] = NULL;
		tmpstr = strtok ( NULL, delimiter );
	}

	return returnchar;
}

void fCMDLIBerrnohandle ( int errorcode )
{
	printf ( "ERROR: %d %s\n", errorcode, strerror(errorcode) );
};
