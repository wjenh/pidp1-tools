# Running PDP-1 TECO

TECO, the text editor Dan Murphy wrote for the PDP-1 between 1962 and 1965,
rewritten in am1 for the PDP-1D.

Its commands are PDP-6 TECO's of July 1965, with Murphy's PDP-1 additions.
A second command set, PDP-11 TECO, is chosen by a sense switch.
This page is enough to start it; *UsingTECO.md* is the full guide.

This is version 1.0\
Edit date 14-Nov-2026\
Initial version

## Building and starting it

This needs am1 and the pidp1-mods environment.

Just type:
```
make
```

This makes *teco.rim*.\
`make type64` makes it for the Type 64 printer instead of the Type 62.

Load *teco.rim* like any other tape using *pdp1central*, *fastload*, or the paper tape reader.
The starting address is 0100.
Starting there clears everything, the buffer, the q-registers and the display's settings,

It needs the PDP-1D's `lai` and `lia` and the multiply and divide instructions,
the BBN clock, DCS2 and all 16 banks of memory.
It uses the Type 340 for the display, the paper tape reader and punch, the Type 62 line
printer, and the Type 550 Microtape and Type 23 drum for files.

## Talking to it

Connect a telnet client to port 2030.
```
telnet <host> 2030
```
TECO types a new line, then `TECO PDP-1/PDP-6`, and waits for a command string.
ESC is ALT MODE, echoed `$`.
Two terminate a command string which then executes.
For example:
```
IHELLO$HT$$
```
inserts `HELLO` and types the buffer.\
Delete or backspace erases the last character.\
Ctrl-C stops a running command.\
`/G` (Ctrl-G) as a command ends the session, closes the connection, and leaves TECO reset
for the next caller.

While TECO waits for a command string, the Type 340 shows the text around the pointer.

## The sense switches

| Switch | On |
|---|---|
| 2 | VT100 mode, see below |
| 3 | turned on, interrupts a running command, as Ctrl-C does |
| 4 | at the greeting, PDP-11 TECO is enabled; the greeting is `TECO PDP-1/PDP-11` |
| 5 | the Type 340 is not used |
| 6 | the console is the typewriter instead of telnet, with `` ` `` for ALT MODE |

Switches 4 and 2 are read only when TECO greets a caller.
In the PDP-11 set, `EX` ends the session after saving the output, Ctrl-C at the prompt
ends without saving.

**VT100 mode.** With switch 2 on, every telnet session gets PDP-11 TECO's commands and a split
screen, the buffer in a window above the last 5 rows, which is the command window.\
The greeting is `TECO PDP-1/PDP-11 VT100 mode`.\
The terminal must be a VT100 emulation; xterm, PuTTY, MobaXterm support it, size 80 by 24 unless told
otherwise.

While switch 2 is on, the console is telnet only, the Type 340 is not used, and switches 4, 5 and
6 do nothing.

Do not press an arrow or function key in TECO, it reads one as ESC and two characters.

## Files

**Paper tape** is in ASR-33 teletype ASCII format.
*asr33tape* can be used convert a text file into a tape and back:
```
asr33tape punch notes.txt notes.ptp
asr33tape text notes.ptp notes.txt
```

**Microtape and the drum** hold files in the shared file system provided by *Am1Includes/FILESYSTEM*.
A drive is unit 1 to 8, the drum, tracks 020 to 037, is unit 9.
*samples.mtp* holds three files to try.
Mount it on drive 1 with *pdp1central* or *mtp*, then type `1EL$$` to list them and `1ERLONG$Y$$`
to read one.
*fstool* copies files between a tape image and the computer, see *Docs/UsingFstool.md*.

## Credits

- TECO was originally writtT by  Dan Murphy for the PDP-1 at MIT.
- The command set is PDP-6 TECO as MIT AI Memo 81, *TECO*, 6 July 1965, describes it.
- The second command set is TECO-11 version 40, as DEC's *Standard TECO* manual, May 1985,
  describes it.
