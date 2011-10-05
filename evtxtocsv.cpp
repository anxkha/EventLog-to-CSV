#include <windows.h>
#include <WinEvt.h>
#include <tchar.h>
#include <time.h>
#include <stdio.h>
#include <strsafe.h>

typedef unsigned long long		QWORD;
typedef unsigned long			DWORD;
typedef unsigned short			WORD;
typedef unsigned char			BYTE;

#define ERROR_ARGUMENTS		-1
#define ERROR_BAD_INFILE	-2
#define ERROR_BAD_OUTFILE	-3

void ParseEvent( HANDLE hEvent, FILE* pFile )
{
	DWORD dwBufferSize = 0;
	DWORD dwBufferUsed = 0;
	DWORD dwPropertyCount = 0;
	LPWSTR pBuffer = NULL;

	if( !EvtRender( NULL,
					hEvent,
					EvtRenderEventXml,
					dwBufferSize,
					pBuffer,
					&dwBufferUsed,
					&dwPropertyCount ) )
	{
		if( ERROR_INSUFFICIENT_BUFFER == GetLastError() )
		{
			dwBufferSize = dwBufferUsed;
			pBuffer = (LPWSTR)malloc( dwBufferSize );

			if( pBuffer )
			{
				EvtRender( NULL,
						   hEvent,
						   EvtRenderEventXml,
						   dwBufferSize,
						   pBuffer,
						   &dwBufferUsed,
						   &dwPropertyCount );
			}
			else
			{
				_ftprintf( stderr, _T("Out of memory\n") );

				goto done;
			}
		}

		if( ERROR_SUCCESS != GetLastError() )
		{
			_ftprintf( stderr, _T("EvtRender failed with error code %d\n"), GetLastError() );
			goto done;
		}
	}

	_ftprintf( pFile, _T("%s\n"), pBuffer );

done:
	if( pBuffer ) free( pBuffer );
}

int _tmain( int argc, TCHAR** argv )
{
	EVT_HANDLE hLog;
	TCHAR sInFileName[MAX_PATH];
	TCHAR sOutFileName[MAX_PATH];
	FILE* pOutFile;
	EVT_HANDLE hEvent;
	DWORD dwToss;
	BOOL bNext = TRUE;

	// Make sure we have the right number of arguments.
	if( argc < 2 )
	{
		_ftprintf( stderr, _T("USAGE: evttocsv.exe infile [outfile]\n") );
		return ERROR_ARGUMENTS;
	}

	// Get the input file name.
	StringCchCopy( sInFileName, MAX_PATH, argv[1] );

	// If an output file is specified, use that, otherwise append ".csv" to the
	// input file's name.
	if( argc > 2 )
	{
		StringCchCopy( sOutFileName, MAX_PATH, argv[2] );
	}
	else
	{
		StringCchCopy( sOutFileName, MAX_PATH, sInFileName );
		StringCchCat( sOutFileName, MAX_PATH, _T(".csv") );
	}

	_tprintf( _T("In file: %s\n"), sInFileName );
	_tprintf( _T("Out file: %s\n\n"), sOutFileName );

	// Open the event log file specified on the command line.
	hLog = EvtQuery( NULL,
					 sInFileName,
					 NULL,
					 EvtOpenFilePath | EvtQueryReverseDirection );
	if( NULL == hLog )
	{
		_ftprintf( stderr, _T("Unable to open input file, %s\n"), sInFileName );
		return ERROR_BAD_INFILE;
	}

	// Open the output file for the conversion.
	_tfopen_s( &pOutFile, sOutFileName, _T("w") );
	if( !pOutFile )
	{
		EvtClose( hLog );

		_ftprintf( stderr, _T("Unable to open output file, %s\n"), sOutFileName );
		return ERROR_BAD_OUTFILE;
	}

	while( bNext )
	{
		// Retrieve the next log entry.
		bNext = EvtNext( hLog,
						 1,
						 &hEvent,
						 1000,
						 0,
						 &dwToss );
		if( !bNext )
		{
			switch( GetLastError() )
			{
			case ERROR_NO_MORE_ITEMS:
				_ftprintf( pOutFile, _T("\nEnd of event log reached!\n") );
				bNext = FALSE;
				break;

			case ERROR_TIMEOUT:
				_ftprintf( stderr, _T("Timeout waiting for event entry.\n") );
				continue;

			default:
				_ftprintf( stderr, _T("Unknown error code, %d\n"), GetLastError() );
				bNext = FALSE;
				break;
			}
		}

		ParseEvent( hEvent, pOutFile );

		// Close the log entry.
		EvtClose( hEvent );
	}
	
	fclose( pOutFile );
	EvtClose( hLog );

	return 0;
}
