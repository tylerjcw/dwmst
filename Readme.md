# dwmst

dwmst - DWM Status(bar)

![Screenshot of dwmst piped in to dzen2][dwmstbar]

Introduction
------------
dwmst is a status line program for DWM (although it could really be used with any WM) inspired by Unia's [dwmst][], which in turn was inspired by TrilbyWite's [dwmStatus][]. As opposed to their statusbars, mine is formatted for dzen2 output, and displays slightly different information. Hardcoded means it is written for my machine, and will most likely not work on yours without a decent number of modifications.

[dwmst]: https://github.com/Unia/dwmst
[dwmStatus]: https://github.com/TrilbyWhite/dwmStatus

Installation
------------
The following dependencies are required to compile this:
- alsa-lib

To build and install this program run this:

	$ make
	$ make install clean

By default the executable is installed to $HOME/bin, you can change this in the makefile. I would recommend keeping the source code to this program after installation, as if you want to reconfigure it, this is the only way.

Information Displayed
---------------------
- Kernel release version (uname -r)
- Number of pacman updates (AUR not included)
- Current volume percentage
- Current time (AM/PM)
- dwm icon

All areas perform an action when clicked. For example - clicking the number of package updates will diplay a list of all packages needing updates, their current version, and the version they are being updated to. All this is displayed in a pop-up, static dzen that is killed when it is clicked (so these pop-ups only use CPU/MEM before they display themselves).
The clickable areas include:
- Clicking kernel release version: assorted system information
- Clicking number of package updates: list of package updates
- Clicking volume: spawns pavucontrol
- Clicking time: displays a dzen calendar with current day and holidays highlighted
- Clicking dwm icon: displays a dzen menu with a list of applications

Note that none of these scripts are included in this repo (pavucontrol is th only one that might work on your system). If you want the clickable areas to do something you will have to modify dwmst.c and replace the path of my scripts with the path of yours.

![Screenshot of calendar][calendar]

Usage
-----
	usage: dwmst [OPTION]
		-h,       display this help and exit
		-v,       display version information and exit
		-o,       run once and exit, turned off by default (useful for debug)
		-i <arg>, set the interval (in seconds) of how often dwmst should refresh
				  by default there is 1 second between updates.

dwmst only outputs formatted text on stdout, nothing more. It is up to you to decide what to do with it. I have the output formatted for dzen2, so to display my bar i have something like this in my .xinitrc:

	dwmst | dzen2 -x 656 -y 742 -fg '#dcdcdc' -bg '#303030' -h 26 -w 367 -ta r -fn '-*-bitocra-*-*-*-*-*-*-*-*-*-*-*-*' &

you could just as easily reformat the output to be more friendly with any other bar that accepts input on stdin. 

[dwmstbar]: images/dwmst.png
[calendar]: images/calendar.png