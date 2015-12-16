/********************************************************************\

  Name:         temp.c
  Created by:   Joe Nash

  Contents:     looks for an INPT bank and reads values into another test 
		bank, TEMP

  $Id: temp.c

\********************************************************************/

/*-- Include files -------------------------------------------------*/
#include <stdio.h>
#include <time.h>
#include <fstream>

/* midas includes */
#include "midas.h"
#include "rmidas.h"
#include "experim.h"
#include "analyzer.h"

/* root includes */
#include <TH1D.h>
#include <TH2D.h>
#include <TTree.h>
#include <TH1F.h>
#include <TBranch.h>
#include "TFile.h"
#include <iostream>

/* Joe's includes */



extern EXP_PARAM exp_param;
extern RUNINFO runinfo;


using namespace std;

/*-- Module declaration --------------------------------------------*/

INT galil_calib(EVENT_HEADER *, void *);
INT galil_init(void);
INT galil_eor(INT run_number);
INT galil_bor(INT run_number);



ANA_MODULE movement_module = {
   "movement",       		/* module name           */
   "Matteo Bartolini",               /* author                */
   galil_calib,                /* event routine         */
   galil_bor,                	/* BOR routine           */
   galil_eor,                  /* EOR routine           */
   galil_init,                        /* init routine          */
   NULL,                        /* exit routine          */
   NULL,                        /* parameter structure   */
   0,                           /* structure size        */
   NULL,                        /* initial parameters    */
};

static TH1F *hPosHists[3];
static TH1F *hSPIDHists[3];
static TGraph *posgraph;
//static TFile *fTreeFile=NULL;
//static TTree *fEventTree=NULL;
//static TBranch *fEventBranch=NULL;
ofstream myfile;



/*-- init routine --------------------------------------------------*/

INT galil_init(void)
{

  char name[256]; 
  char name1[256];
  int i;
 
   
  /*fTreeFile= TFile::Open("/home/galil/experiment/galil.root","recreate");

  fEventTree = new TTree("movement","");
  fEventTree->SetAutoSave(300000000); // autosave when 300 Mbyte written.
  fEventTree->SetMaxVirtualSize(300000000); // 300 Mbyte
  fEventTree->Branch("x",&x,"x/I");
  fEventTree->Branch("y",&y,"y/I");
  fEventTree->Branch("z",&z,"z/I");
  fEventTree->Branch("vx",&vx,"vx/I");
  fEventTree->Branch("vy",&vy,"vy/I");
  fEventTree->Branch("vz",&vz,"vz/I");*/
  
  
 
 
   /* book histos */ 

  for (i=0; i < 3; i++){
    char title[256];
    char title1[256];
    sprintf(name, "POSN%02d", i);
    sprintf(title, " pos %d", i);
    sprintf(name1,"SPID%02d",i);
    sprintf(title1," spid %d",i);
 
  hPosHists[i] = h1_book<TH1F>(name, title, 1000, 0, 500000);
  hSPIDHists[i]= h1_book<TH1F>(name1,title1,100,-700,7000);

  }
  
  posgraph= new TGraph();
   return SUCCESS;
}


/*-----exit routine-------------*/

INT galil_bor(INT run_number)
{

   return SUCCESS;
}

/*-- EOR routine ---------------------------------------------------*/

INT galil_eor(INT run_number)
{
   return SUCCESS;
}

/*-- event routine -------------------------------------------------*/

INT galil_calib(EVENT_HEADER * pheader, void *pevent)
{
  INT i;
  char buffer[500];
  HNDLE hDB;
  float *pdata;
  float *pspeed;
  int n;
  INT runnumber;
  INT size= sizeof(runnumber);
   cm_get_experiment_database(&hDB, NULL);
   db_get_value(hDB,0,"/Runinfo/Run number",&runnumber,&size,TID_INT, 0);
  

   /* look for temp bank */
  if (!bk_locate(pevent,"AXES",&pdata))
    return 1;

   if (!bk_locate(pevent,"SPID", &pspeed))
     return 1;

   /* create  bank */
   //bk_create(pevent, "POSN", TID_FLOAT, (void**)&pdata);


   /* copy partial bank*/
   for ( i = 0; i < 3; i++) {
     //printf ("hello\n");
     hPosHists[i] ->Fill(pdata[i],1);
     hSPIDHists[i]->Fill(pspeed[i],1);
    
     }
  
   n = posgraph->GetN();
   //printf("run number is %d\n",runnumber);
   posgraph->SetPoint(n,pheader->time_stamp,pdata[0]);
   
   sprintf(buffer,"posvstime%d.txt",runnumber);

   myfile.open(buffer,std::ios_base::app);
   myfile << pheader->time_stamp <<" "<< pdata[0]<<" " << n <<"\n";
   myfile.close();

   /* close bank */
   bk_close(pevent,pdata);
     bk_close(pevent,pspeed);
   
     /*x = pdata[0];
   y= pdata[1];
   z= pdata[2];
   vx= pspeed[0];
   vy= pspeed[1];
   vz= pspeed[2];
   
   fEventTree->Fill();*/

  

  

   return SUCCESS;
}
