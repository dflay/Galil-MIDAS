/*! \file x_examples.cpp
*
* Examples main(). Calls example code.
*
*/
#include "x_examples.h"

#include <iomanip>
#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{ 

  int rc = GALIL_EXAMPLE_OK; //return code
	char buf[1023]; //traffic buffer
        char buf1[1024];
	GCon g = 0; //var used to refer to a unique connection. A valid connection is nonzero.
        GReturn b = G_NO_ERROR;
	GSize read_bytes=0;
	ofstream myfile;
        int i=0;
	/*myfile.open("myexample3.txt");
	myfile << "Writing this to a file.\n";
        myfile.close();*/
	

	//GOpen("/dev/ttyUSB0 -t 1000 -s MG -d", &g);
	GOpen("192.168.1.42 -s ALL -t 1000 -d",&g);
	GOpen("00:50:4c:38:19:AA -s ALL -t 1000 -d", &g);
	GInfo(g, buf, sizeof(buf)); //grab connection string
	cout << "buf is" << " "<<  buf << "\n";
        if (b==0){
        cout << "Example GProgramDownload() usage\n";
        }
	else {cout << "connection failed \n";}


	while(i!=20){
	rc = GMessage(g, buf1, sizeof(buf1));
	
	i++;
           
         
	  cout << buf1;
	}
         
	rc= GMessage(g,buf1, sizeof(buf1));
	cout << buf1;


}
