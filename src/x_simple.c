/*! \file x_simple.cpp
*
* A very simple example for using gclib. See x_examples.cpp for more in-depth examples.
*
*/
#include "gclibo.h" //by including the open-source header, all other headers are pulled in.

int main(int argc, char * argv[])
{
	GReturn rc;
	char buf[1024]; //traffic buffer
	
	rc = GVersion(buf, sizeof(buf));
	printf("rc: %d\n", (int) rc);
	printf("version: %s\n", buf); //Print the library version

	GCon g; //var used to refer to a unique connection
	//Gopen was "10.1.3.17 -d" before change
	
	rc = GOpen("/dev/ttyUSB0 -t 1000 -s MG -d", &g); //Open a connection to Galil, store the identifier in g.
	printf("rc: %d\n", (int) rc);

	rc = GInfo(g, buf, sizeof(buf));
	printf("rc: %d\n", (int) rc);
	printf("info: %s\n", buf); //Print the connection info
	
	rc = GCommand(g, "MG TIME", buf, sizeof(buf), 0); //Send MG TIME. Because response is ASCII, don't care about bytes read.
	printf("rc: %d\n", (int) rc);
	
	printf("response: %s\n", buf); //Print the response
	
	rc = GClose(g); //Don't forget to close!
	
	return rc;
}
