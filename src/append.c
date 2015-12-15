// basic file operations
#include <iostream>
#include <fstream>
using namespace std;

int main () {
  ofstream myfile;
  myfile.open ("open.txt");
  myfile << "Writing this to a file\n another line\n line3\n";
  myfile.close();
  return 0;
}
