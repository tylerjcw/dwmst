#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/utsname.h>

#include "config.h"

// controls how fast dwmst will update in seconds
// change this to set default, otherwise use 'dwmst -i <seconds>'
double interval = 1;

// if true, runs once and exits. If you want to change the default behavior,
// change this here, otherwise use 'dwmst -o'
long int run_times = -1;

// used to safely return strings in a formatted manner
char *
smprintf(char *fmt, ...)
{
	va_list fmtargs;
	char *ret;
	int len;

	va_start(fmtargs, fmt);
	len = vsnprintf(NULL, 0, fmt, fmtargs);
	va_end(fmtargs);

	ret = malloc(++len);
	if(ret == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	va_start(fmtargs, fmt);
	vsnprintf(ret, len, fmt, fmtargs);
	va_end(fmtargs);

	return ret;
}

// display program usage
void
display_usage()
{
    printf("usage: dwmst [OPTION]\n");
    printf("  -h,       display this help and exit\n");
    printf("  -v,       display version information and exit\n");
    printf("  -r <int>, run int amount of times and exit, turned off by default\n");
    printf("            this argument must be an int > 0 (useful for debug)\n");
    printf("  -i <int>, set the interval (in seconds) of how often dwmst should refresh\n");
    printf("            by default there is 1 second between updates, must be an int >= 0\n");
    exit(0);
}

// display general program info & version
void
display_version()
{
    printf("dwmst 1.3 - A utility to feed info to a status bar\n");
    printf("Copyright (c) 2015, Tyler C-W\n");
    printf("License - WTFPL, Just do What the Fuck You Want\n");
    exit(0);
}

/* safely spawns a shell command and returns it's output.
   currently only used for volume (for getting mute state),
   can be used like so:
       char *curtime = SHCMD("date +'%I:%M%P'");
       char *kernel  = SHCMD("uname -r");
       char *whoami  = SHCMD("whoami");
*/
char *
SHCMD(char *cmd)
{
	FILE* process = popen(cmd, "r");
	char buffer[1024];
	fscanf(process, "%[^\n]", buffer);
	pclose(process);

	return smprintf(buffer);
}

/* Reads UPDATE_FILE to retrieve number of pacman updates.
   UPDATE_FILE is written to by a shell script executed every
   10 minutes by a cron job. the script has the following in it:
       pacman -Sy
	   pacman -Qu | wc -l > $HOME/log/updates.log
*/ 
char *
get_updates()
{
	FILE* infile = fopen(UPDATE_FILE, "r");
	int number;
	fscanf(infile, "%i", &number);
	fclose(infile);

	return smprintf(number == 0 ? PAC_NONE : PAC_UPDS, number);
}

/* reads current system volume from acpi,
   configured to work with tp_smapi for ThinkPads.
   only tested on an IBM ThinkPad T60
*/
char *
get_vol()
{
	char *mute = SHCMD("awk '/^mute/{print $2}' /proc/acpi/ibm/volume");

	FILE* infile = fopen("/proc/acpi/ibm/volume", "r");
	int number;
	fscanf(infile, "level:%i", &number);
	fclose(infile);

	int volume = round((number*100)/14);
	
	if( strncmp(mute, "off", 3) == 0 )
	{
		if (volume <= 24)
			return smprintf(VOL_25, volume);
		else if ((volume >=25) && (volume <= 49))
			return smprintf(VOL_50, volume);
		else if ((volume >= 50) && (volume <= 74))
			return smprintf(VOL_75, volume);
		else if (volume >= 75)
			return smprintf(VOL_100, volume);
	}
	else // mute == "off"
	{
		return smprintf(VOL_MUTE);
	}

	return "Unable";
}

// returns current system time in the format specified by CLK
char *
get_time()
{
	char clock[38];
	time_t current;

	time(&current);
	if(!strftime(clock, sizeof(clock) - 1, CLK, localtime(&current)))
		return NO_CLK;
	return smprintf(clock);
}


// returns kernel version number, same as 'uname -r'
char *
get_kernel()
{
	struct utsname retval;
	uname(&retval);
	
    return smprintf(KERNEL, retval.release);
}

// does the actual printing of the status
void
print_status()
{
	int times_ran = 0;

	FILE* bar = popen(STATUS_BAR, "w");

	// status loop, executed [interval] seconds
	while (true)
	{
		//print status
		fprintf(bar, KERNEL_STRING, get_kernel());
		fprintf(bar, SEPCHR_STRING);
		fprintf(bar, UPDATE_STRING, get_updates());
		fprintf(bar, SEPCHR_STRING);
		fprintf(bar, VOLUME_STRING, get_vol());
		fprintf(bar, SEPCHR_STRING);
		fprintf(bar, PCTIME_STRING, get_time());
		fprintf(bar, SEPCHR_STRING);
		fprintf(bar, CMDBTN_STRING);
		fprintf(bar, " \n");
		fflush(bar);

		/* increment the number of times the loop has ran by 1, if
		   this value is exactly equal to the specified number of times to run
		   (default is -1, or infinite times) then break the loop. If it is not, then
		   sleep for the specified interval, and run the loop again
		*/
		times_ran++;
		if (times_ran == run_times)
		{
			pclose(bar);
			break;
		}
		else
			sleep(interval);
	}

	pclose(bar);
}

// main function, all argument handling is done here
int 
main(int argc, char *argv[])
{
	int c;

	if ((c = getopt(argc, argv, "hvr:i:t")) != 4)
	{
		switch( c )
		{
			case 'h':
				display_usage();
				break;
			case 'v':
				display_version();
				break;
			case 'i':
				interval = strtod(optarg, NULL);
				print_status();
				break;
			case 'r':
				run_times = strtol(optarg, NULL, 10);
				if( run_times > 0)
					print_status();
				else
					display_usage();
				break;
			case '?':
				display_usage();
				break;
			default:
				print_status();
				break;
		}
	}

	return 0;
}
