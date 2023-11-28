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

	  // ------- ultrasonic sensors example -----------------------------------------
	  	#define US_BAR_LEN 24 	// display bar length for US sensor
	  	#define MAX_US_DISTANCE 250.0 // max distance US

	  	// convert US value to text comment
	  	#define US_VAL(val) ((val)==KH4_US_DISABLED_SENSOR ? "Not activated" : ((val)==KH4_US_NO_OBJECT_IN_RANGE ? "No object in range" : ((val)==KH4_US_OBJECT_NEAR ? "Object at less than 25cm" : "Object in range 25..250cm")))



	  	while(!kb_kbhit())
	  	{
	  		kb_clrscr();

	  		// get and print us sensors
	  		kh4_measure_us(Buffer,dsPic);
	   		for (i=0;i<5;i++)
	   		{
	   			usvalues[i] = (short)(Buffer[i*2] | Buffer[i*2+1]<<8);


	   			if((usvalues[i] == KH4_US_DISABLED_SENSOR) || (usvalues[i] == KH4_US_NO_OBJECT_IN_RANGE))
	   			{ // out of range or disabled
	  				sprintf(bar[i],"|\33[%dC|",US_BAR_LEN);
	  			}  else
	  			{
	  				// in range or less than 25cm
	  				n=(int)(usvalues[i]*US_BAR_LEN/MAX_US_DISTANCE);

	  				if (n==0)
	  					sprintf(bar[i],"|>\33[%dC|",US_BAR_LEN-1);
	  				else
	  					if (n>=US_BAR_LEN-1)
	  						sprintf(bar[i],"|\33[%dC>|",US_BAR_LEN-1);
	  					else
	  					 sprintf(bar[i],"|\33[%dC>\33[%dC|",n,US_BAR_LEN-1-n);
	  			}
	   		}


	  		printf("\nUS sensors : distance [cm]\
	  		        \n                     50  100  150  200\
	  		        \n                0|    .    :    .    :    |%.0f\nleft 90   : %4d %s  %s\nleft 45   : %4d %s  %s\
	  		\nfront     : %4d %s  %s\nright 45  : %4d %s  %s\nright 90  : %4d %s  %s\n",MAX_US_DISTANCE,
	  			 usvalues[0],bar[0],US_VAL(usvalues[0]),usvalues[1],bar[1],US_VAL(usvalues[1]),usvalues[2],bar[2],US_VAL(usvalues[2]),usvalues[3],bar[3],US_VAL(usvalues[3]),usvalues[4],bar[4],US_VAL(usvalues[4]));
	  		printf("\nPush any key to end program\n");
	  		usleep(200000); // wait 200ms

	  	}
	    tcflush(0, TCIFLUSH);  // flush input key

	  	// good at the end of the program:
	    kh4_set_speed(0 ,0 ,dsPic); // stop robot
	    kh4_SetRGBLeds(0,0,0,0,0,0,0,0,0,dsPic); // clear rgb leds because consumes energy

	    return 0;
	  }
