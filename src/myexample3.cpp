/*! \file x_examples.cpp
*
* Examples main(). Calls example code.
*
*/
#include "x_examples.h"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{ 

  int rc = GALIL_EXAMPLE_OK; //return code
	char buf[1023]; //traffic buffer
        char buf1[1024];
	GCon g = 0; //var used to refer to a unique connection. A valid connection is nonzero.
        GReturn b = G_NO_ERROR;
	GSize read_bytes=0;
	ofstream myfile;
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

	GProgramDownload(g,"",0); //to erase prevoius programs
	//b=GProgramDownload(g,"i=0\r#A\rMGi\ri=i+1\rWT100\rJP#A,i<10\rEN",0);
	//b=GProgramDownload(g, "DM pos[3]\rpos[0]=_TPA\rpos[1]=_TPB\rpos[2]=_TPC\r#A\rMGpos[0]\rMGpos[1]\rMGpos[2]\rWT1000\rJP#A\rEN",0);
	 b=GProgramDownload(g, "#A\rpos=_TPA\rMGpos\rWT1000\rJP#A\rEN",0);
	GCmd(g, "XQ");
       	
	GTimeout(g,2000);//adjust timeout
	int i = 0;
        int s;
	//while ((rc = GMessage(g, buf1, sizeof(buf1))) == G_NO_ERROR)
        while(i!=1)
	
	{ 

        
	  rc = GMessage(g, buf1, sizeof(buf1));
          s= atoi (buf1);
          
          //GTimeout(g,1000);
	  /*myfile.open("testgalil.txt");
	  myfile << buf1;
	  myfile.close();*/
	  //cout << buf1 << " double is " << s << "\n";
	  printf (" s is %i\n", s);
          //GTimeout(g,10000);
	  //if (strstr(buf1, ".") != 0) //each MG has a decimal point
	      //i++; //count it
	}
	/*GTimeout(g, G_USE_INITIAL_TIMEOUT); //restore timeout
	if (i == 10)
		return GALIL_EXAMPLE_OK;
	else
	{
		cout << "Expected 10 messages\n";
		return GALIL_EXAMPLE_ERROR;
		}*/
   rc = GMessage(g, buf1, sizeof(buf1));
   cout << " now buf1 is " << buf1 << "\n";
     
}
