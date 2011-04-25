#include <tchar.h>
#include <time.h>
#include <iostream>
#include <stdio.h>
#include <string>
using namespace std;

typedef unsigned long long		QWORD;
typedef unsigned long			DWORD;
typedef unsigned short			WORD;
typedef unsigned char			BYTE;

int main( int argc, char** argv )
{
	string sInFileName;
	string sOutFileName;
	FILE* pInFile;
	FILE* pOutFile;
	string sCSV;

	// Make sure we have the right number of arguments.
	if( argc < 2 )
	{
		cout << "USAGE: evttocsv.exe infile [outfile]\n";
		return 0;
	}

	// Get the input file name.
	sInFileName = argv[1];

	if( argc > 2 )
	{
		sOutFileName = argv[2];
	}
	else
	{
		sOutFileName = sInFileName + ".csv";
	}

	cout << "In file: " << sInFileName << endl;
	cout << "Out file: " << sOutFileName << "\n\n";

	pInFile = fopen( sInFileName.c_str(), "rb" );
	if( !pInFile )
	{
		cout << "Unable to open input file " << sInFileName << "!\n";
		return 0;
	}

	pOutFile = fopen( sOutFileName.c_str(), "w" );
	if( !pOutFile )
	{
		fclose( pInFile );

		cout << "Unable to open output file " << sOutFileName << "!\n";
		return 0;
	}

	char sCrap[48];
	if( !feof( pInFile ) ) fread( (char*)&sCrap, 1, 48, pInFile );

	sCSV = "Date Created,Date Written,Event ID,Event Type,String Count,Category,Source,Computer,Description,SID,Data\n";
	fwrite( sCSV.c_str(), 1, sCSV.length(), pOutFile );

	while( !feof( pInFile ) )
	{
		DWORD dwLength;
		DWORD dwReserved;
		DWORD dwNumber;
		DWORD dwDateCreated;
		DWORD dwDateWritten;
		DWORD wEventID;
		WORD wEventType;
		WORD wStringCount;
		WORD wCategory;
		WORD wReservedFlags;
		DWORD dwClosingNumber;
		DWORD dwStringOffset;
		DWORD dwSIDLength;
		DWORD dwSIDOffset;
		DWORD dwDataLength;
		DWORD dwDataOffset;
		string sSourceName = "";
		string sComputerName = "";
		string sSID = "";
		string sData = "";
		string sStrings = "";
		string sDateCreated = "";
		string sDateWritten = "";
		TCHAR c;
		int i = 0, j = 0;
		char* csv = new char[512];

		fread( (char*)&dwLength, 1, 4, pInFile );

		if( dwLength < 92 )
		{
			delete csv;
			break;
		}

		char* pData = new char[dwLength - 4];

		fread( pData, 1, (dwLength - 4), pInFile );
		
		dwReserved = *((DWORD*)&pData[0]);
		dwNumber = *((DWORD*)&pData[4]);
		dwDateCreated = *((DWORD*)&pData[8]);
		dwDateWritten = *((DWORD*)&pData[12]);
		wEventID = *((WORD*)&pData[16]);
		wEventType = *((WORD*)&pData[20]);
		wStringCount = *((WORD*)&pData[22]);
		wCategory = *((WORD*)&pData[24]);
		wReservedFlags = *((WORD*)&pData[26]);
		dwClosingNumber = *((DWORD*)&pData[28]);
		dwStringOffset = *((DWORD*)&pData[32]);
		dwSIDLength = *((DWORD*)&pData[36]);
		dwSIDOffset = *((DWORD*)&pData[40]);
		dwDataLength = *((DWORD*)&pData[44]);
		dwDataOffset = *((DWORD*)&pData[48]);

		i = 52;
		while( (c = *((TCHAR*)&pData[i])) != '\0' )
		{
			sSourceName += (char)c;
			i += 2;
		}

		i += 2;

		while( (c = *((TCHAR*)&pData[i])) != '\0' )
		{
			sComputerName += (char)c;
			i += 2;
		}

		if( dwSIDLength )
		{
			i = dwSIDOffset - 4;

			for( j = 0; j < dwSIDLength; j++ )
			{
				sSID += (char)*((char*)&pData[i]);
				i++;
			}
		}

		if( dwDataLength )
		{
			i = dwDataOffset - 4;

			for( j = 0; j < dwDataLength; j++ )
			{
				sData += (char)*((char*)&pData[i]);
				i++;
			}
		}

		if( wStringCount > 1 )
		{
			i = dwStringOffset - 4;

			for( j = 0; j < wStringCount; j++ )
			{
				while( (c = *((TCHAR*)&pData[i])) != '\0' )
				{
					sStrings += (char)c;
					i += 2;
				}

				sStrings += " | ";

				i += 2;
			}
		}

		delete pData;

		time_t tDateCreated = (time_t)dwDateCreated;
		time_t tDateWritten = (time_t)dwDateWritten;

		sDateCreated = asctime( localtime( &tDateCreated) );
		sDateCreated = sDateCreated.substr( 0, sDateCreated.length() - 1 );

		sDateWritten = asctime( localtime( &tDateWritten) );
		sDateWritten = sDateWritten.substr( 0, sDateWritten.length() - 1 );

		sprintf( csv, "%s,%s,%d,%d,%d,%d,%s,%s,%s,,\n", sDateCreated.c_str(), sDateWritten.c_str(), wEventID, wEventType, wStringCount, wCategory, sSourceName.c_str(), sComputerName.c_str(), sStrings.c_str() );
		fwrite( csv, 1, strlen(csv), pOutFile );

		delete csv;
	}
	
	fclose( pOutFile );
	fclose( pInFile );

	return 0;
}
