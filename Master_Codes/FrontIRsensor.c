#include <khepera/khepera.h>
#include <signal.h>

static knet_dev_t *dsPic; // robot pic microcontroller access
static int quitReq = 0;   // quit variable for loop

// Ctrl+C handler
static void ctrlc_handler(int sig)
{
  quitReq = 1;
  kh4_set_speed(0, 0, dsPic); // stop robot
  kh4_SetMode(kh4RegIdle, dsPic);
  kb_change_term_mode(0);    // revert to original terminal if called
  exit(0);
}

double convertToCm(int sensor_value)
{
	double m = -0.03;
	double b = 20;
    return m * sensor_value + b;
}

int main()
{
  char Buffer[100];
  int sensors[12];

  printf("\nKhepera 4 small example program\n");

  if (kh4_init(0, NULL) != 0)
  {
    printf("\nERROR: could not initiate the libkhepera!\n\n");
    return -1;
  }

  dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);

  if (dsPic == NULL)
  {
    printf("\nERROR: could not initiate communication with Kh4 dsPic\n\n");
    return -2;
  }

  signal(SIGINT, ctrlc_handler);

  // IR proximity sensors example
  int i;
  while (!kb_kbhit())
  {
    // get ir proximity sensor (12 sensors)
    kh4_proximity_ir(Buffer, dsPic);
    for (i = 0; i < 12; i++)
    {
      sensors[i] = (Buffer[i * 2] | Buffer[i * 2 + 1] << 8);
      printf("\nfront: %4u\n", sensors[3]);
	  double distance_cm = convertToCm(sensors[3]);
      printf("\nSensor: %4u, Distance: %.2f cm", sensors[3], distance_cm);
	  //printf("\nResult:", result);
      //usleep(100000); // wait 200ms
    }
    usleep(100000);
  }

  return 0;
}

