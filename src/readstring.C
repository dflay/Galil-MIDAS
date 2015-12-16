#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <sstream>
#include "TFile.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TF1.h"
#include "TCanvas.h"
#include "cmath"
#include "TGraph.h"
using namespace std;


void readstring(){
  ifstream infile("posvstime47.txt");
  Double_t x,y;
  int n;
  string line;
 
   TCanvas *c1 = new TCanvas("c1","",200,20,1003,1003);
  TGraph *c= new TGraph();
  
 
 

    while ( getline (infile,line) )
    {    
     
      stringstream(line)>> x >> y;
     
     
      cout << x <<"\n";
      n=c->GetN();
      //cout << n << "\n";
      c->SetPoint(n,x,y);
	 
	}
  
     c->GetXaxis()->SetTitle("Time");
    c->GetYaxis()->SetTitle("Encoder Counts");
    
   
  

    //signal->SetTitle("signal");
    c->SetLineColor(4);
    c->SetLineWidth(2);
   c->Draw("AL");
   // c1->Print("Galil.pdf");
  
  

}


