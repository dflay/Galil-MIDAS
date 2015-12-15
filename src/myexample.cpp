/*! \file x_examples.cpp
*
* Examples main(). Calls example code.
*
*/
//#include "x_examples.h"
#include "gclib.h"
#include "gclibo.h"
#include <iostream>
#include <string>


#include <iomanip>

#define GALIL_EXAMPLE_OK G_NO_ERROR //return code for correct code execution
#define GALIL_EXAMPLE_ERROR -100
using namespace std;

int main(int argc, char *argv[])
{ 

  
  GReturn b = G_NO_ERROR;
  	int rc = GALIL_EXAMPLE_OK; //return code
	char buf[30]; //traffic buffer
	GCon g = 0; //var used to refer to a unique connection. A valid connection is nonzero.

	
        //GAssign("192.168.0.13", "00:50:4c:38:19:71");
	GOpen("/dev/ttyUSB0 -t 1000 -s MG -d", &g);
	//GOpen("169.254.0.13 -s ALL -t 1000 -d",&g);
	//GOpen("00:50:4c:38:19:71 -s ALL -t 1000 -d", &g);
	GInfo(g, buf, sizeof(buf)); //grab connection string
	cout << "buf is" <<  buf << "\n";
	cout << "Example GCommand() usage\n";
        GSize read_bytes = 0;
        b=GCommand(g, "TP", buf, sizeof(buf), &read_bytes);
        cout << " b is " << b << "\n";
        if( b == 0){
        
        cout << buf << "\n";
	cout << "bytes returned" <<  read_bytes << "\n";
        cout << g << "\n";}//end if
        else {cout <<" bad command \n";}
        
        /*GCommand(g, "OF?", buf, sizeof(buf), &read_bytes);
	cout << buf << "\n";
        cout << "bytes returned" << read_bytes << "\n";
	
	GCommand(g, "PR?", buf, sizeof(buf), &read_bytes);
	cout << buf << "\n";
        cout << "bytes returned" << read_bytes << "\n";
	GCommand(g, "PR?", buf, sizeof(buf), &read_bytes);
	cout << buf << "\n";
        cout << "bytes returned" << read_bytes << "\n";*/
	

}

