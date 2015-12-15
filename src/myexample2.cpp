/*! \file x_examples.cpp
*
* Examples main(). Calls example code.
*
*/
#include "x_examples.h"

#include <iomanip>

int main(int argc, char *argv[])
{ 

  	int rc = GALIL_EXAMPLE_OK; //return code
	char buf[1023]; //traffic buffer
	GCon g = 0; //var used to refer to a unique connection. A valid connection is nonzero.
        GReturn b = G_NO_ERROR;
	GSize read_bytes=0;
	

	//b=GOpen("/dev/ttyUSB0 -t 1000 -s MG -d", &g);
	b=GOpen("192.168.15.1 -s ALL -t 1000 -d",&g);
	b=GOpen("00:50:4c:38:19:71 -s ALL -t 1000 -d", &g);
	GInfo(g, buf, sizeof(buf)); //grab connection string
	cout << "buf is" << " "<<  buf << "\n";
        if (b==0){
        cout << "Example GProgramDownload() usage\n";
        }
	else {cout << "connection failed \n";}

	GProgramDownload(g,"",0); //to erase prevoius programs
	b=GProgramDownloadFile(g,"galil4.dmc",0);
	if (b==0){
	  //GCmd(g,"XQ"); //execute the code
          GCommand(g, "SPA=?", buf, sizeof(buf),&read_bytes);
	  cout << "speed is" <<buf << "\n";
	}
	else { cout << "download failed \n";}


}
