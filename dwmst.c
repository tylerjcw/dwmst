#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/utsname.h>
#include <alsa/asoundlib.h>

#define CLK         "%I:%M%P" 									 // Format string for time
#define NO_CLK      "Unable"  									 // Format string when unable to get time (should rarely/never see)
#define KERNEL      "^fg(#6095C5)^fn(stlarch)^fn()^fg()%s"      // Kernel version format string
#define PAC_UPDS    "^fg(#D370A3)^fn(stlarch)^fn()^fg()%i"      // Format string when there ARE updates
#define PAC_NONE    "^fg(#6095C5)^fn(stlarch)^fn()^fg()%i"      // Format string for NO updates
#define VOL_MUTE    "^fg(#D370A3)^fn(stlarch)^fn()^fg()%d%%"    // Format string for MUTED volume
#define VOL         "^fg(#6095C5)^fn(stlarch)^fn()^fg()%d%%"    // Format string for NON_MUTED volume
#define VOL_CH      "Console"
#define UPDATE_FILE "/home/komrade/log/updates.log"

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
    printf("  -r <int>, run once and exit, turned off by default (useful for debug)\n");
    printf("            this argument must be an int > 0");
    printf("  -i <int>, set the interval (in seconds) of how often dwmst should refresh\n");
    printf("            by default there is 1 second between updates, must be an int >= 0\n");
    exit(0);
}

// display general program info & version
void
display_version()
{
    printf("dwmst 1.0 - A status line utility for DWM & dzen\n");
    printf("Copyright (c) 2015, Tyler C-W\n");
    printf("License - WTFPL, Just do What the Fuck You Want\n");
    exit(0);
}

/* safely spawns a shell command and returns it's output.
   currently not used for anything, could be used like so:
       char *curtime = SHCMD("date +'%I:%M%P'");
       char *kernel  = SHCMD("uname -r");
       char *whoami  = SHCMD("whoami");
*/
char *
SHCMD(char *cmd)
{
	FILE* process = popen(cmd, "r");
	char buffer[1000];
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

/* polls the mixer specified by VOL_CH to retrieve current
   system volume, requires alsa-lib
*/
char *
get_vol(snd_mixer_t *handle, snd_mixer_elem_t *elem) {
	int mute = 0;
	long vol, max, min;

	snd_mixer_handle_events(handle);
	snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
	snd_mixer_selem_get_playback_volume(elem, 0, &vol);
	snd_mixer_selem_get_playback_switch(elem, 0, &mute);

	return smprintf(mute == 0 ? VOL_MUTE : VOL, (vol * 100) / max);
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

	// set up the alsa mixer
	snd_mixer_t* handle;
	snd_mixer_elem_t* elem;
	snd_mixer_selem_id_t* vol_info;

	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, "default");
	snd_mixer_selem_register(handle, NULL, NULL);
	snd_mixer_load(handle);
	snd_mixer_selem_id_malloc(&vol_info);
	snd_mixer_selem_id_set_name(vol_info, VOL_CH);
	elem = snd_mixer_find_selem(handle, vol_info);
	// if there was an error opening the mixer, report it
	if(elem == NULL)
	{
		fprintf(stderr, "dwmst: can not open device.\n");
		snd_mixer_selem_id_free(vol_info);
		snd_mixer_close(handle);
		exit(EXIT_FAILURE);
	}

	// status loop, executed [interval] seconds
	while (true)
	{
		//print status
		printf("^ca(1,/home/komrade/etc/dwm/dzenSysinfo.sh)%s ^ca()", get_kernel());
		printf("^fg(#686868)^r(2x19)^fg()^ca(1,/home/komrade/etc/dwm/dzenPacman.sh) %s ^ca()", get_updates());
		printf("^fg(#686868)^r(2x19)^fg()^ca(1,pavucontrol) %s ^ca()", get_vol(handle, elem));
		printf("^fg(#686868)^r(2x19)^fg()^ca(1,/home/komrade/etc/dwm/dzenCal.sh) ^fg(#6095C5)^fn(stlarch)^fn()^fg()%s ^ca()", get_time());
		printf("^fg(#686868)^r(2x19)^fg()^fg(#6095C5)^ca(1,/home/komrade/etc/dwm/menu.sh) ^i(/home/komrade/etc/dwm/icons/dwm.xbm)^fg() ^ca()\n");
		fflush(stdout);

		/* increment the number of times the loop has ran by 1, if
		   this value is exactly equal to the specified number of times to run
		   (default is -1, or infinite times) then break the loop. If it is not, then
		   sleep for the specified interval, and run the loop again
		*/
		times_ran++;
		if (times_ran == run_times)
			break;
		else
			sleep(interval);
	}

	// clean up the alsa mixer
	snd_mixer_selem_id_free(vol_info);
	snd_mixer_close(handle);
}

// main function, all argument handling is done here
int 
main(int argc, char *argv[])
{
	int c;

	if ((c = getopt(argc, argv, "hvr:i:")) != 4)
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
				switch( run_times )
				{
					case 0:
						display_usage();
						break;
					default:
						print_status();
						break;
				}
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
