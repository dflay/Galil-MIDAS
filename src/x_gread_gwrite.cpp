/*! \file x_gread_gwrite.cpp
*
* Example GRead() and GWrite() usage.
*
*/

#include "x_examples.h"

int x_gread_gwrite(GCon g)
{
	cout << "\n************************************************************************\n";
	cout << "Example GRead() and GWrite() usage\n";
	cout << "************************************************************************\n";

	char buf[1024]; //traffic buffer
	GSize read_bytes = 0; //bytes read
	GSize total_bytes = 0; //total bytes read
	
	//-------------------------------------------------------------------------
	/* 
	*  Ad hoc Command-and-Response
	*
	*  GCommand() should be used for all commands except ED, QD and DL.
	*  
	*  However, it is possible to use GWrite() and GRead() to perform a command-and-response transaction.
	*  This is useful, for example, for a firmware NRE that provides a binary response such as QR.
	*  QR polls the data record, a binary response. Note, QR is also compatible with GCommand().
	*/

	strcpy(buf, "QR\r"); //don't forget the carriage return when using GWrite() for commands.
	x_e(GWrite(g, buf, 3)); //Write the command to the controller.

	GReturn rc = G_NO_ERROR; //return code
	x_e(GTimeout(g, 100)); //adjust timeout
	/*
	*  Assuming the format of QR is unknown, this demo reads until a read times out.
	*  A faster approach would be to read for a known terminating sequence that doesn't appear in the data.
	*  For example, standard Galil commands terminate data with a \r\n:
	*/
	while (rc == G_NO_ERROR) //read until timeout
	{
		total_bytes += read_bytes;
		rc = GRead(g, buf, sizeof(buf), &read_bytes);
	}
	x_e(GTimeout(g, G_USE_INITIAL_TIMEOUT)); //restore timeout
	cout << "\nRead " << total_bytes << " QR bytes.\n"; //This includes full data record and a colon (:).
	
	return GALIL_EXAMPLE_OK;
}