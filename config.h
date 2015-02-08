#define STATUS_BAR      "dzen2 -p -xs 1 -x 656 -y 754 -fg '#dcdcdc' -bg '#303030' -h 14 -w 367 -ta r -fn '-*-bitocra-*-*-*-*-*-*-*-*-*-*-*-*'"

#define SEPCHR_STRING   " ^fg(#686868)^r(2x10)^fg() "
#define KERNEL_STRING   "^ca(1,/home/komrade/etc/dwm/dzenSysinfo.sh)%s^ca()"
#define UPDATE_STRING   "^ca(1,/home/komrade/etc/dwm/dzenPacman.sh)%s^ca()"
#define VOLUME_STRING   "^ca(1,pavucontrol)%s^ca()"
#define PCTIME_STRING   "^ca(1,/home/komrade/etc/dwm/dzenCal.sh)^fg(#6095C5)^fn(stlarch)^fn()^fg()%s^ca()"
#define CMDBTN_STRING   "^fg(#6095C5)^ca(1,tpl -t)^i(/home/komrade/etc/dwm/icons/dwm.xbm)^fg()^ca()"

#define CLK         "%I:%M%P" 									  // Format string for time
#define NO_CLK      "Unable"  								      // Format string when unable to get time (should rarely/never see)
#define KERNEL      "^fg(#6095C5)^fn(stlarch)^fn()^fg()%s"       // Kernel version format string
#define PAC_UPDS    "^fg(#EFBD8B)^fn(stlarch)^fn()^fg()%02i"      // Format string when there ARE updates
#define PAC_NONE    "^fg(#6095C5)^fn(stlarch)^fn()^fg()%02i"      // Format string for NO updates
#define VOL_MUTE    "^fg(#D370A3)^fn(stlarch)^fn()^fg()mute"      // Format string for mute
#define VOL_25      "^fg(#D370A3)^fn(stlarch)^fn()^fg()%03i%%"    // Format string for volume < 25%
#define VOL_50      "^fg(#EFBD8B)^fn(stlarch)^fn()^fg()%03i%%"    // Format string for volume < 50%
#define VOL_75      "^fg(#6D9E3F)^fn(stlarch)^fn()^fg()%03i%%"    // Format string for volume < 75%
#define VOL_100     "^fg(#6095C5)^fn(stlarch)^fn()^fg()%03i%%"    // Format string for volume < 100%
#define VOL_CH      "Console"
#define UPDATE_FILE "/home/komrade/log/updates.log"