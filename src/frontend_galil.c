/* 
 * D. Flay, UMass Amherst  
 * TODO: 1. Understand how we communicate with the Galil via MIDAS
 *          - What are the basic needs for communication? 
 *          - What is the program flow? 
 *
 *       2. Clean up and optimize code
 *          - What can we get rid of/keep? 
 *          - Divide up different methods of communication (RS-232, ethernet, etc) 
 *            into their own functions/methods
 *          - Error checking and exit/shutdown procedures in case of failure 
 *          - Remove hard-coded paths, addresses, etc. (use config files?)  
 * 
 *       3. UI Improvements
 *          - What does it look like right now?  
 *          - Ease of use; what can we do "behind the scenes" 
 *            and remove from the UI?
 *          - History of motion displayed on screen? 
 *  
 */

/********************************************************************\

Name:         frontend.c
Created by:   Stefan Ritt

Contents:     Experiment specific readout code (user part) of
Midas frontend. This example simulates a "trigger
event" and a "scaler event" which are filled with
CAMAC or random data. The trigger event is filled
with two banks (ADC0 and TDC0), the scaler event
with one bank (SCLR).

$Id$

\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "midas.h"
#include "mcstd.h"
#include "experim.h"
#include "gclib.h"
#include "gclibo.h"
#include <iostream>
#include <string>
#include <iomanip>
#include "/home/matteo/midas/drivers/device/nulldev.h"
#include "/home/matteo/midas/drivers/bus/null.h"
#include "/home/matteo/midas/drivers/class/hv.h"
#include "/home/matteo/midas/drivers/bus/rs232.h"
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/timeb.h>


#define GALIL_EXAMPLE_OK G_NO_ERROR //return code for correct code execution
#define GALIL_EXAMPLE_ERROR -100
using namespace std;

/* make frontend functions callable from the C framework */
#ifdef __cplusplus
extern "C" {
#endif

   // i am defining some Galil libraries variables
   INT level1=2;
   INT axes[3];
   INT setaxes[3];
   HNDLE hDB, hkeyclient;
   char  name[32];
   int   size; //size of axes[3]
   INT size1; // size of setaxes[3]

   int i;
   int s;
   GReturn b = G_NO_ERROR;
   int rc = GALIL_EXAMPLE_OK; //return code
   char buf[1023]; //traffic buffer
   char buf1[1024];
   GCon g = 0; //var used to refer to a unique connection. A valid connection is nonzero.
   //----------------------------------------------------------


   /*-- Globals -------------------------------------------------------*/

   /* The frontend name (client name) as seen by other MIDAS clients   */
   char *frontend_name = "Sample Frontend";
   /* The frontend file name, don't change it */
   char *frontend_file_name = __FILE__;

   /* frontend_loop is called periodically if this variable is TRUE    */
   BOOL frontend_call_loop = FALSE;

   /* a frontend status page is displayed with this frequency in ms */
   INT display_period = 3000;

   /* maximum event size produced by this frontend */
   INT max_event_size = 10000;

   /* maximum event size for fragmented events (EQ_FRAGMENTED) */
   INT max_event_size_frag = 5 * 1024 * 1024;

   /* buffer size to hold events */
   INT event_buffer_size = 100 * 10000;

   /* number of channels */
#define N_ADC  4
#define N_TDC  4
#define N_SCLR 4

   /* CAMAC crate and slots */
#define CRATE      0
#define SLOT_IO   23
#define SLOT_ADC   1
#define SLOT_TDC   2
#define SLOT_SCLR  3

/*-- Function declarations -----------------------------------------*/

INT frontend_init();
INT frontend_exit();
INT begin_of_run(INT run_number, char *error);
INT end_of_run(INT run_number, char *error);
INT pause_run(INT run_number, char *error);
INT resume_run(INT run_number, char *error);
INT frontend_loop();

INT read_trigger_event(char *pevent, INT off);
INT read_scaler_event(char *pevent, INT off);
INT read_galil_event(char *pevent, INT off);    /* this looks like it was added by Matteo */  

INT poll_event(INT source, INT count, BOOL test);
INT interrupt_configure(INT cmd, INT source, POINTER_T adr);
INT db_set_value(HNDLE hDB, HNDLE hKeyRoot, const char *key_name, const void *data, INT data_size, INT num_values, DWORD type); 
INT db_find_key(HNDLE hdB, HNDLE hKey, const char *key_name, HNDLE *subhkey);
INT cm_get_experiment_database(HNDLE *hDB, HNDLE *hKeyClient);

   /* device driver list */
   /*DEVICE_DRIVER hv_driver[] = {
     {"Dummy Device", nulldev, 16, null},
     {""}
     };*/


   /*-- Equipment list ------------------------------------------------*/


   EQUIPMENT equipment[] = {

      {"Trigger",               /* equipment name */
         {1, 0,                   /* event ID, trigger mask */
            "SYSTEM",               /* event buffer */
            EQ_POLLED,              /* equipment type */
            LAM_SOURCE(0, 0xFFFFFF),        /* event source crate 0, all stations */
            "MIDAS",                /* format */
            TRUE,                   /* enabled */
            RO_RUNNING |            /* read only when running */
               RO_ODB,                 /* and update ODB */
            1000,                    /* poll for 100ms */
            0,                      /* stop run after this event limit */
            0,                      /* number of sub events */
            0,                      /* don't log history */
            "", "", "",},
         read_trigger_event,      /* readout routine */
      },

      {"Scaler",                /* equipment name */
         {2, 0,                   /* event ID, trigger mask */
            "SYSTEM",               /* event buffer */
            EQ_PERIODIC,            /* equipment type */
            0,                      /* event source */
            "MIDAS",                /* format */
            TRUE,                   /* enabled */
            RO_RUNNING | RO_TRANSITIONS |   /* read when running and on transitions */
               RO_ODB,                 /* and update ODB */
            10000,                  /* read every 10 sec */
            0,                      /* stop run after this event limit */
            0,                      /* number of sub events */
            0,                      /* log history */
            "", "", "",},
         read_scaler_event,       /* readout routine */
      },

      {"Galil",                          /* equipment name */
         {3, 0,                          /* event ID, trigger mask */
         "SYSTEM",                       /* event buffer */
         EQ_PERIODIC,                    /* equipment type */
         0,                              /* event source */
         "MIDAS",                        /* format */
         TRUE,                           /* enabled */
         RO_RUNNING | RO_TRANSITIONS |   /* read when running and on transitions */
            RO_ODB,                      /* and update ODB */
         1000,                           /* read every 10 sec */
         0,                              /* stop run after this event limit */
         0,                              /* number of sub events */
         0,                              /* log history */
         "", "", "",},
         read_galil_event,               /* readout routine */
      },



      {""}
   };


#ifdef __cplusplus
}
#endif

/********************************************************************\
  Callback routines for system transitions

  These routines are called whenever a system transition like start/
  stop of a run occurs. The routines are called on the following
  occations:
  
  frontend_init:  When the frontend program is started. This routine
  should initialize the hardware.
  
  frontend_exit:  When the frontend program is shut down. Can be used
  to releas any locked resources like memory, commu-
  nications ports etc.
  
  begin_of_run:   When a new run is started. Clear scalers, open
  rungates, etc.
  
  end_of_run:     Called on a request to stop a run. Can send
  end-of-run event and close run gates.
  
  pause_run:      When a run is paused. Should disable trigger events.
  
  resume_run:     When a run is resumed. Should enable trigger events.
\********************************************************************/

/*-- Frontend Init -------------------------------------------------*/

INT frontend_init()
{
   /* hardware initialization */
   /*INT level1=2;
     INT axes[3]={1,2,5};
     HNDLE hDB, hkeyclient;
     char  name[32];
     int   size;*/
   /*axes[0]=1;
     axes[1]=2;
     axes[2]=5;*/
   //cm_get_experiment_database(&hDB, &hkeyclient); 
   cm_get_experiment_database(&hDB, NULL);

   //size = sizeof(name);
   size1=sizeof(setaxes);
   //db_get_value(hDB, hkeyclient, "Name", name, &size, TID_STRING, TRUE);
   //printf("My name is %s\n", name);
   //cm_get_experiment_database(&hDB, &hkeyclient);
   //db_find_key(hDB,0,"/Equipment/Galil/Variables",&hkeyclient);
   db_get_value(hDB,hkeyclient,"/Equipment/Galil/Variables/Setting",&setaxes,&size1,TID_INT, FALSE);


   axes[0]=setaxes[0];
   axes[1]=setaxes[1];
   axes[2]=setaxes[2];

   //axes[0]=1;
   //axes[1]=2;
   //axes[2]=3;

   db_find_key(hDB,0,"/Equipment/Galil/Variables",&hkeyclient);
   db_set_value(hDB, 0, "/Equipment/Galil/Variables/Axes",&axes, sizeof(axes), 3, TID_INT);


   //db_set_value(hDB, 0, "/Equipment/Galil",&level1,sizeof(level1),3,TID_INT);

   // Code to communicate with Galil board



   //GOpen("/dev/ttyUSB0 -t 1000 -s MG -d", &g);      // RS-232? 
   GOpen("192.168.1.42 -s ALL -t 1000 -d",&g);        // ethernet communication: looks like an IP address 
   GOpen("00:50:4c:38:19:AA -s ALL -t 1000 -d", &g);  // ethernet communication: looks like a MAC address
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

   //-------------end code to communicate with Galil------------------

   //--------------rs232 code------------------------

   /*int fSerialPort_ptr;
     int fBufferSizeAg = 256;
     char *fWriteBufferAg;
     char *fReadBufferAg;

     int i = 0;
     char *line = "RP\r\n";
     int length = strlen(line);
     int n = write(fSerialPort_ptr, line, length);
     char *ptr;
     int nbytes;

     char devname[100];
     struct termios options;

     sprintf(devname, "/dev/ttyUSB0");
     cout << "dididsdidsdsdsddsidddsd\n";
     fWriteBufferAg = (char *)malloc(fBufferSizeAg);
     fReadBufferAg = (char *)malloc(fBufferSizeAg);
   //  printf("about to connect to serial port\n");
   fSerialPort_ptr = open(devname, O_RDWR | O_NOCTTY | O_NDELAY);

   if(fSerialPort_ptr < 0) {
   perror("opening device");
   return -1;
   }
   fcntl(fSerialPort_ptr, F_SETFL, 0); // return immediately if no data

   if(tcgetattr(fSerialPort_ptr, &options) < 0) {
   perror("tcgetattr");
   return -2;
   }

   cfsetospeed(&options, B9600);
   cfsetispeed(&options, B9600);

   /* set 7E1 */
   /*options.c_cflag |= PARENB; // no parity
     options.c_cflag &= ~PARODD; // even parity
     options.c_cflag &= ~CSTOPB; // 1 stop bits
     options.c_cflag &= ~CSIZE;
     options.c_cflag |= CS8;     // 7 bits data

     if(tcsetattr(fSerialPort_ptr, TCSANOW, &options) < 0) {
     perror("tcsetattr");
     return -3;
     }

     ioctl(fSerialPort_ptr, FIONREAD, &i);
     if (i > 0) i = read(fSerialPort_ptr, fReadBufferAg, i);


     n = write(fSerialPort_ptr, line, length);

     if(n != length) {
   //    printf("******write error\n");
   return -4;
   }

   ptr = fWriteBufferAg;
   while((nbytes = read(fSerialPort_ptr, ptr, 
   fWriteBufferAg + fBufferSizeAg - ptr - 1)) > 1) {
   ptr += nbytes;
   if(ptr[-1] == '\n' || ptr[-1] == '\r') break;
   }
    *ptr = '\0';  // NULL terminate string

    printf("PTR = %s\n",ptr);
    printf("HALLOOOOOO\n");

    close(fSerialPort_ptr);*/

   //--------------------end code rs232--------------------------









   cam_init();
   cam_crate_clear(CRATE);
   cam_crate_zinit(CRATE);

   /* enable LAM in IO unit */
   camc(CRATE, SLOT_IO, 0, 26);

   /* enable LAM in crate controller */
   cam_lam_enable(CRATE, SLOT_IO);

   /* reset external LAM Flip-Flop */
   camo(CRATE, SLOT_IO, 1, 16, 0xFF);
   camo(CRATE, SLOT_IO, 1, 16, 0);

   /* register CNAF functionality from cnaf_callback.c with debug output */
   register_cnaf_callback(1);

   /* print message and return FE_ERR_HW if frontend should not be started */

   return SUCCESS;
}

/*-- Frontend Exit -------------------------------------------------*/

INT frontend_exit()
{
   return SUCCESS;
}

/*-- Begin of Run --------------------------------------------------*/

INT begin_of_run(INT run_number, char *error)
{
   /* put here clear scalers etc. */




   return SUCCESS;
}

/*-- End of Run ----------------------------------------------------*/

INT end_of_run(INT run_number, char *error)

{

   return SUCCESS;
}

/*-- Pause Run -----------------------------------------------------*/

INT pause_run(INT run_number, char *error)
{
   return SUCCESS;
}

/*-- Resuem Run ----------------------------------------------------*/

INT resume_run(INT run_number, char *error)
{
   return SUCCESS;
}

/*-- Frontend Loop -------------------------------------------------*/

INT frontend_loop()
{
   /* if frontend_call_loop is true, this routine gets called when
      the frontend is idle or once between every event */
   return SUCCESS;
}

/*------------------------------------------------------------------*/

/********************************************************************\

  Readout routines for different events

  \********************************************************************/

/*-- Trigger event routines ----------------------------------------*/

INT poll_event(INT source, INT count, BOOL test)
   /* Polling routine for events. Returns TRUE if event
      is available. If test equals TRUE, don't return. The test
      flag is used to time the polling */
{
   int i;
   DWORD lam;

   for (i = 0; i < count; i++) {
      cam_lam_read(LAM_SOURCE_CRATE(source), &lam);

      if (lam & LAM_SOURCE_STATION(source))
         if (!test)
            return lam;
   }

   return 0;
}

/*-- Interrupt configuration ---------------------------------------*/

INT interrupt_configure(INT cmd, INT source, POINTER_T adr)
{
   switch (cmd) {
      case CMD_INTERRUPT_ENABLE:
         break;
      case CMD_INTERRUPT_DISABLE:
         break;
      case CMD_INTERRUPT_ATTACH:
         break;
      case CMD_INTERRUPT_DETACH:
         break;
   }
   return SUCCESS;
}

/*-- Event readout -------------------------------------------------*/

INT read_trigger_event(char *pevent, INT off)
{
   WORD *pdata, a;
   INT q, timeout;
   // printf("=========================jsdjsdsa====================\n");




   /*GSize read_bytes = 0;
     b=GCommand(g, "TP", buf, sizeof(buf), &read_bytes);
     cout << " b is " << b << "\n";
     if( b == 0){

     cout << buf << "\n";
     cout << "bytes returned" <<  read_bytes << "\n";
     cout << g << "\n";}//end if
     else {cout <<" bad command \n";}*/



   /* init bank structure */
   bk_init(pevent);

   /* create structured ADC0 bank */
   bk_create(pevent, "ADC0", TID_WORD, (void **)&pdata);

   /* wait for ADC conversion */
   for (timeout = 100; timeout > 0; timeout--) {
      camc_q(CRATE, SLOT_ADC, 0, 8, &q);
      if (q)
         break;
   }
   if (timeout == 0)
      ss_printf(0, 10, "No ADC gate!");

   /* use following code to read out real CAMAC ADC */
   /*
      for (a=0 ; a<N_ADC ; a++)
      cami(CRATE, SLOT_ADC, a, 0, pdata++);
    */

   /* Use following code to "simulate" data */
   for (a = 0; a < N_ADC; a++)
      *pdata++ = rand() % 1024;

   /* clear ADC */
   camc(CRATE, SLOT_ADC, 0, 9);

   bk_close(pevent, pdata);

   /* create variable length TDC bank */
   bk_create(pevent, "TDC0", TID_WORD, (void **)&pdata);

   /* use following code to read out real CAMAC TDC */
   /*
      for (a=0 ; a<N_TDC ; a++)
      cami(CRATE, SLOT_TDC, a, 0, pdata++);
    */

   /* Use following code to "simulate" data */
   for (a = 0; a < N_TDC; a++)
      *pdata++ = rand() % 1024;

   /* clear TDC */
   camc(CRATE, SLOT_TDC, 0, 9);

   bk_close(pevent, pdata);

   /* clear IO unit LAM */
   camc(CRATE, SLOT_IO, 0, 10);

   /* clear LAM in crate controller */
   cam_lam_clear(CRATE, SLOT_IO);

   /* reset external LAM Flip-Flop */
   camo(CRATE, SLOT_IO, 1, 16, 0xFF);
   camo(CRATE, SLOT_IO, 1, 16, 0);

   ss_sleep(10);

   return bk_size(pevent);
}

/*-- Scaler event --------------------------------------------------*/

INT read_scaler_event(char *pevent, INT off)
{
   DWORD *pdata, a;

   static int d=0;
   cout << "==========hello"<< ++d << "\n";




   /* init bank structure */
   bk_init(pevent);

   /* create SCLR bank */
   bk_create(pevent, "SCLR", TID_DWORD, (void **)&pdata);

   /* read scaler bank */
   for (a = 0; a < N_SCLR; a++)
      cam24i(CRATE, SLOT_SCLR, a, 0, pdata++);

   bk_close(pevent, pdata);

   return bk_size(pevent);
}

INT read_galil_event(char *pevent, INT off){
   DWORD *pdata, a;

   static int t=0;

   cout << "==========hellogalil"<< ++t << "\n";
   /* init bank structure */

   rc = GMessage(g, buf1, sizeof(buf1));
   s= atoi (buf1);
   printf (" s is %i\n", s);
   axes[0]=s;
   axes[1]=2;
   axes[2]=5;
   cm_get_experiment_database(&hDB, &hkeyclient);
   size = sizeof(name);
   db_get_value(hDB, hkeyclient, "Name", name, &size, TID_STRING, TRUE);
   printf("My name is %s\n", name);
   db_find_key(hDB,0,"/Equipment/Galil/Variables",&hkeyclient);
   db_set_value(hDB, 0, "/Equipment/Galil/Variables/Axes",&axes, sizeof(axes), 3, TID_INT);



   bk_init(pevent);

   /* create SCLR bank */
   bk_create(pevent, "SCLR", TID_DWORD, (void **)&pdata);

   return bk_size(pevent);
}

