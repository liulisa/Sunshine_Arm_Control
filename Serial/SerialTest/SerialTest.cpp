// SerialTest.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <windows.h>

#include "Serial.h"
#include "stdafx.h"

using namespace std;

#define RX_BUFFSIZE 2

void printUsage(_TCHAR progName[]);


int _tmain(int argc, _TCHAR* argv[])
{
	//if(argc != 2)
	//{
	//	printUsage(argv[0]);

	//	cout << "press any key and enter to quit" << endl;
	//	char temp;
	//	cin >> temp;
	//
	//	//return 10;
	//}

	try
	{
		cout << "Opening com port"<< endl;
		tstring commPortName(TEXT("COM6"));
		Serial serial(commPortName);
		cout << "Port opened" << endl;
		
		while (1)
		{
			cout << "writing something to the serial port" << endl;
			serial.flush();
			char hi[] = "1";
			int bytesWritten = serial.write(hi);
			cout << bytesWritten << " bytes were written to the serial port" << endl;
			Sleep(1010);
			if(bytesWritten != sizeof(hi) - 1)
			{
				cout << "Writing to the serial port timed out" << endl;
			}

//			char buffer[] = "1";
//
//			cout << "Reading from the serial port: ";
//			while (buffer != hi)
//			{
//				int charsRead = serial.read(buffer, RX_BUFFSIZE);
//			}
//			cout << buffer << endl;
		}
	}
	catch(const char *msg)
	{
		cout << msg << endl;
	}

	cout << "press any key and enter to quit" << endl;
	char temp;
	cin >> temp;

	return 0;
}

void printUsage(_TCHAR progName[])
{
#if defined(UNICODE)
	wcout << progName << " <comm port>" << endl
		 << "e.g., " << progName << " COM1" << endl;
#else
	cout << progName << " <comm port>" << endl
		 << "e.g., " << progName << " COM1" << endl;
#endif
	
}