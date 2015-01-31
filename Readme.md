# dwmst

dwmst - DWM Status(bar)

![Screenshot of dwmst piped in to dzen2][dwmstbar]

Introduction
------------
dwmst is a status line program for DWM (although it could really be used with any WM) inspired by Unia's [dwmst][], which in turn was inspired by TrilbyWite's [dwmStatus][]. As opposed to their statusbars, mine is formatted for dzen2 output, and displays slightly different information. Hardcoded means it is written for my machine, and will most likely not work on yours without a decent number of modifications.

[dwmst]: https://github.com/Unia/dwmst
[dwmStatus]: https://github.com/TrilbyWhite/dwmStatus

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

![Screenshot of calendar][calendar] ![Screenshot of sysinfo][sysinfo]

Usage
-----
usage: dwmst [OPTION]
  -h,       display this help and exit
  -v,       display version information and exit
  -o,       run once and exit, turned off by default (useful for debug)
  -i <arg>, set the interval (in seconds) of how often dwmst should refresh
            by default there is 1 second between updates.



[dwmstbar]: images/dwmst.png
[calendar]: images/calendar.png
[sysinfo]: images/sysinfo.png