#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/utsname.h>
#include <alsa/asoundlib.h>

#define VOL_MUTE    "^fg(#D370A3)^fn(stlarch)^fn()^fg()%d%%"
#define VOL         "^fg(#6095C5)^fn(stlarch)^fn()^fg()%d%%"
#define VOL_CH      "Console"
#define CLK         "%I:%M%P"
#define NO_CLK      "Unable"
#define UPDATE_FILE "/home/komrade/log/updates.log"

// controls how fast dwmst will update in seconds
// change this to set default, otherwise use 'dwmst -i <seconds>'
double interval = 1;

// if true, runs once and exits. If you want to change the default behavior,
// change this here, otherwise use 'dwmst -o'
bool run_once = false;

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

void
display_usage()
{
    printf("usage: dwmst [OPTION]\n");
    printf("  -h,       display this help and exit\n");
    printf("  -v,       display version information and exit\n");
    printf("  -o,       run once and exit, turned off by default (useful for debug)\n");
    printf("  -i <arg>, set the interval (in seconds) of how often dwmst should refresh\n");
    printf("            by default there is 1 second between updates.");
    exit(0);
}

void
display_version()
{
    printf("dwmst 1.0 - A status line utility for DWM & dzen\n");
    printf("Copyright (c) 2015, Tyler C-W\n");
    printf("License - WTFPL, Just do What the Fuck You Want\n");
    exit(0);
}

// Spawns a shell command and returns it's output
char *
SHCMD(char *cmd)
{
	FILE* process = popen(cmd, "r");
	char buffer[1000];
	fscanf(process, "%[^\n]", buffer);
	pclose(process);

	int sz = strlen(buffer) + 1;
	char *output = malloc(sz);
	memset(output, 0, sizeof(output));
	strncpy(output, buffer, sz);
	strstr(output, "\n");

	return output;
}

char *
get_updates()
{
	FILE* infile = fopen(UPDATE_FILE, "r");
	char updates[4];
	fscanf(infile, "%[^\n]", updates);
	fclose(infile);

	int sz = strlen(updates);
	char *output = malloc(sz);
	memset(output, 0, sizeof(output));
	strncpy(output, updates, sz);
	strstr(output, "\n");

	return output;
}

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

// returns current system time
char *
current_time()
{
	char clock[38];
	time_t current;

	time(&current);
	if(!strftime(clock, sizeof(clock) - 1, CLK, localtime(&current)))
		return NO_CLK;
	return smprintf(clock);
}


// returns kernel version number
char *
kernel_version()
{
	struct utsname retval;
	uname(&retval);
	
	int sz = strlen(retval.release) + 1;
	char *output = malloc(sz);
	memset(output, 0, sizeof(output));
	strncpy(output, retval.release, sz);
	strstr(output, "\n");

    return output;
}

void
print_status()
{
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
	if(elem == NULL)
	{
		fprintf(stderr, "dwmst: can not open device.\n");
		snd_mixer_selem_id_free(vol_info);
		snd_mixer_close(handle);
		exit(EXIT_FAILURE);
	}

	struct utsname retval;
	uname(&retval);

	for(;;sleep(interval))
	{
		char *updates = get_updates();
		char *kernel  = kernel_version();
	
		//print status
		printf("^ca(1,/home/komrade/etc/dwm/dzenSysinfo.sh)^fg(#6095C5) ^fn(stlarch)^fn()^fg()%s ^ca()", kernel);
		printf("^fg(#686868)^r(2x19)^fg()^ca(1,/home/komrade/etc/dwm/dzenPacman.sh)^fg(#6095C5) ^fn(stlarch)^fn()^fg()%s ^ca()", updates);
		printf("^fg(#686868)^r(2x19)^fg()^ca(1,pavucontrol) %s ^ca()", get_vol(handle, elem));
		printf("^fg(#686868)^r(2x19)^fg()^ca(1,/home/komrade/etc/dwm/dzenCal.sh)^fg(#6095C5) ^fn(stlarch)^fn()^fg()%s ^ca()", current_time());
		printf("^fg(#686868)^r(2x19)^fg()^fg(#6095C5)^ca(1,/home/komrade/etc/dwm/menu.sh) ^i(/home/komrade/etc/dwm/icons/dwm.xbm)^fg() ^ca()\n");
		fflush(stdout);
	
		// remember to deallocate any memory that was allocated
		free(updates);
		free(kernel);

		if (run_once)
			break;
	}

	snd_mixer_selem_id_free(vol_info);
	snd_mixer_close(handle);
}

int 
main(int argc, char *argv[])
{
	int c;

	if ((c = getopt(argc, argv, "hvoi:")) != 4)
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
			case 'o':
				run_once = true;
				print_status();
				break;
			default:
				print_status();
				break;
		}
	}

	return 0;
}
