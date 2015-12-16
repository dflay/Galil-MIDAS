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

void fillhisto(){

 ifstream infile("histo.txt");
 Double_t x;
  string line;

  TH1F *f1= new TH1F ("","", 10,3915,3925);
  TCanvas *c1 = new TCanvas("c1","",200,20,1003,1003);

  while( getline (infile,line)){

    stringstream(line) >> x;
    cout << x << endl;
    f1->Fill(x);
  }

  f1->Draw("HIST");

}

counts file
3919  84
3919  141
3917  205
3918  272
3917  447
3918  429
3917  494

