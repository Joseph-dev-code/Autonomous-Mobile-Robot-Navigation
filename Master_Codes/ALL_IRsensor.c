#include <khepera/khepera.h>
#include <signal.h>


static knet_dev_t * dsPic; // robot pic microcontroller access



static int quitReq = 0; // quit variable for loop

/--------------------------------------------------------------------/
/*!
 * Make sure the program terminate properly on a ctrl-c
 */
static void ctrlc_handler( int sig )
{
  quitReq = 1;

  kh4_set_speed(0 ,0 ,dsPic); // stop robot
  kh4_SetMode( kh4RegIdle,dsPic );

  kh4_SetRGBLeds(0,0,0,0,0,0,0,0,0,dsPic); // clear rgb leds because consumes energy

  kb_change_term_mode(0); // revert to original terminal if called

  exit(0);
}


/--------------------------------------------------------------------/
/*!
 * Main
 */
int main()
{

  double fpos,dval;
  int lpos,rpos;
  char Buffer[100],bar[12][64],revision,version;
  int i,n,val,type_of_test=0,sl,sr,pl,pr,mean;
  short index, value,sensors[12],usvalues[5];
  char c;
  long motspeed;
  char line[80],l[9];
  int kp,ki,kd;
  int pmarg,maxsp,accinc,accdiv,minspacc, minspdec; // SetSpeedProfile


  printf("\nKhepera 4 small example program\n");

  // initiate libkhepera and robot access
  if ( kh4_init(0,NULL)!=0)
  {
  	printf("\nERROR: could not initiate the libkhepera!\n\n");
  	return -1;
  }

  /* open robot socket and store the handle in its pointer */
  dsPic  = knet_open( "Khepera4:dsPic" , KNET_BUS_I2C , 0 , NULL );

	if ( dsPic==NULL)
  {
  	printf("\nERROR: could not initiate communication with Kh4 dsPic\n\n");
  	return -2;
  }

	 /* ------  initialize the motors controlers --------------------------------*/

	  /* tuned parameters */
	  pmarg=20;
	  kh4_SetPositionMargin(pmarg,dsPic ); 				// position control margin
	  kp=10;
	  ki=5;
	  kd=1;
	  kh4_ConfigurePID( kp , ki , kd,dsPic  ); 		// configure P,I,D

	  accinc=3;
	  accdiv=0;
	  minspacc=20;
	  minspdec=1;
	  maxsp=400;
	  kh4_SetSpeedProfile(accinc,accdiv,minspacc, minspdec,maxsp,dsPic ); // Acceleration increment ,  Acceleration divider, Minimum speed acc, Minimum speed dec, maximum speed

		kh4_SetMode( kh4RegIdle,dsPic );  				// Put in idle mode (no control)


	  // get revision
	  if(kh4_revision(Buffer, dsPic)==0){
	   	version=(Buffer[0]>>4) +'A';
	  	revision=Buffer[0] & 0x0F;
	    printf("\r\nVersion = %c, Revision = %u\r\n",version,revision);
	  }


	  signal( SIGINT , ctrlc_handler ); // set signal for catching ctrl-c

	  // ------- IR proximity sensors example --------------------------------------
		#define IR_BAR_LEN 15 	// display bar length for IR sensor
		while(!kb_kbhit())
		{
			kb_clrscr();

			printf("IR proximity sensors:\n");

			// get ir proximity sensor
			kh4_proximity_ir(Buffer, dsPic);

			for (i=0;i<12;i++)
			{
				sensors[i]=(Buffer[i*2] | Buffer[i*2+1]<<8);

				n=(int)(sensors[i]*IR_BAR_LEN/1024.0);

				if (n==0)
					sprintf(bar[i],"|\33[%dC>|",IR_BAR_LEN-1);
				else
					if (n>=IR_BAR_LEN-1)
						sprintf(bar[i],"|>\33[%dC|",IR_BAR_LEN-1);
					else
					 sprintf(bar[i],"|\33[%dC>\33[%dC|",IR_BAR_LEN-1-n,n);

			 }
			 printf("\n                    near               far\
			 \nback left      : %4u  %s\nleft           : %4u  %s\
			 \nfront left     : %4u  %s\nfront          : %4u  %s\
			 \nfront right    : %4u  %s\nright          : %4u  %s\
			 \nback right     : %4u  %s\nback           : %4u  %s\
			 \nground left    : %4u  %s\ngnd front left : %4u  %s\
			 \ngnd front right: %4u  %s\nground right   : %4u  %s\n",
				 sensors[0],bar[0],  sensors[1],bar[1],
				 sensors[2],bar[2],  sensors[3],bar[3],
				 sensors[4],bar[4],  sensors[5],bar[5],
				 sensors[6],bar[6],  sensors[7],bar[7],
				 sensors[8], bar[8], sensors[9], bar[9],
				 sensors[10], bar[10],sensors[11], bar[11]
				 );

			printf("\nPush any key to continue program\n");
			usleep(200000); // wait 200ms

		}
}
