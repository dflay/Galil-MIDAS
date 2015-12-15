// basic file operations
#include <iostream>
#include <fstream>
#include <stdio.h>
using namespace std;

int main () {
  ofstream myfile;
  char buffer [200];
  int a=26;
  int b=34;
  int c=41;
  int n;
  n=sprintf (buffer,"PAA=%d\nPAB=%d\nPAC=%d\n",a,b,c);
  myfile.open ("open.txt");
  myfile << buffer;
  myfile.close();
  myfile.open("open.txt",std::ios_base::app);
  myfile << "#A\nTPA\nhello\nEN\n";
  return 0;
}
