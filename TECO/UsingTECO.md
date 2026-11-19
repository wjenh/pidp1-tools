# Using PDP-1 TECO

TECO is the text editor Dan Murphy wrote for the PDP-1 between 1962 and 1965.
Unfortunately, there is only fragmentary documentation surviving, and no sources.
This is a rewrite of it in am1 for the PDP-1D.
Its commands are those of PDP-6 TECO as MIT AI Memo 81, *TECO*, 6 July 1965, describes them,
here called "the memo", with Murphy's PDP-1 additional commands.

A second command set, PDP-11 TECO, can be chosen, as can the VT100 split screen mode.

## 1. What it needs, and starting it

TECO needs a pidp1-mods PDP-1 with:
- muldiv and lailia enabled, the default;
- all 16 banks of memory;
- the BBN timesharing clock for `/@` and the timing of RUBOUT and the display;
- DCS2 for the console over telnet, and/or the Soroban typewriter;
- the paper tape reader and punch, the Type 62 line printer (or
  the Type 64 if chosen at build time), the Type 340 display, Type 550 Microtape, and the Type 23 drum.

To build it, type `make`.

Load `teco.rim` like any other tape, with *pdp1central*, *fastload*, or the reader.
It can be restarted at address 0100.
Starting there clears everything.

## 2. Talking to TECO

**Over telnet**, connect to port 2030 on the machine running pidp1:
```
telnet <host> 2030
```
If your typing does not echo until RETURN, change your telnet client setting to character mode.\
TECO types a new line and `TECO PDP-1/PDP-6`, then waits for a command string.

**On the Soroban typewriter**, turn sense switch 6 on.
The telnet port is disabled, and TECO prints the greeting on the Soroban.
Be sure the Soroban is started before TECO or you won't see the greeting.
If VT100 split-screen mode is enabled, the Soroban can't be selected.

**Typing a command string**:
- Every character is echoed.
  The ESC key is ALT MODE and echoes `$`.
  A control character with no visible effect echoes as `^` and its letter, `^A`.
- RETURN is treated as CR/LF.
- RUBOUT, a delete or Backspace, erases the last character and echoes it.
  RepeatBd RUBOUTs erase the whole string, then TECO types CR/LF.
  RUBOUT on an empty string also types CR/LF.
- Two ALT MODEs in a row end the command string.
  TECO echoes `$$`, CR/LF, and executes it.
  A single ALT MODE inside a string ends a command and does not execute, multiple commands can be given this way.
- A string holds 2,046 characters.
  Over that TECO rings the bell and takes no more, but will accept RUBOUT and ALT MODE.
- Lower-case command letters act as upper case in commands.
  When entered as text they are kept as typed.
- In the memo, `/` before a letter is that control character, so `/G` is Ctrl-G and `/U` Ctrl-U.
  An actual typed `/` is division.

**Errors**.
An illegal or meaningless command types `?` and the rest of the string is dropped.
Typing `?` as the first key after one shows the string through the bad command.
A search that fails without `:` is an error also.
A push-down list overflow, a macro that calls itself without end or more than 102 open parentheses, types one bell.

**Stopping a command.** Ctrl-C typed while a command string runs or when sense switch 3 turned on stops
it as an error, `?`.
A Ctrl-C typed after more than 128 characters have been typed ahead is seen
only when TECO reads it, sense switch 3 always stops immediately.
Ctrl-C typed into a string is text.

**Typing ahead** is safe.
Keys typed while a command runs wait for the next read and nothing pasted over telnet is lost
regardless of how much arrives at once.

**Leaving.** `/G` (Ctrl-G) as a command ends the session.
Over telnet TECO waits for its output to be written, closes the connection and waits for a new one.
The next connect gets a reinitialized TECO session.
On the Soroban the program halts, CONTINUE resumes with everything kept, there is no automatic close.

**The Soroban** lacks some of TECO's keys.
These stand in and TECO prints them the same way, a printed listing can be typed back in:
| Key | Is |
|---|---|
| `` ` `` (the overbar) | ALT MODE, printed `` ` `` |
| `\|` | `;` |
| `~` | `:` |
| `#` | `%` |
| `@` and a letter | that control character, so `@G` is `/G` and `@I` is TAB |
| `@0` | `/@` |
| `@` and one of `` ` \| ~ # @ `` | that character itself |
| `@(`, `@)` | `{`, `}` |
| `@&` | the dollar sign |

On the Soroban the bell is silent.
Note that the Soroban loses keys typed while it prints, so wait until TECO has finished any output.

## 3. What TECO keeps

- **The buffer**, the text being edited, holds 49,152 characters of 7-bit ASCII.
  A page ends at a FORM character, which does not enter the buffer.
  The **pointer** sits between two characters.
  Its value, `.`, is the number of characters to its left, 0 to `Z`, the end of the buffer.
- **36 q-registers**, `0` to `9` and `A` to `Z`, each either undefined, a number, or a text string.
  Their texts share 14,332 characters, all of which one register can take.
- **The command string**, kept after it runs, for `?`.

## 4. Values

Values are 36-bit two's complement, -34,359,738,368 to 34,359,738,367; a longer number wraps.
Arithmetic runs left to right with no precedence, `2+3*4=` types 20, parentheses group.
Division truncates toward 0, and `n/0` is n.

| Command | Value |
|---|---|
| `0`-`9` | a decimal number |
| `B` | 0 |
| `Z` | the number of characters in the buffer |
| `.` | the pointer |
| `H` | `B,Z`, the whole buffer |
| `Qi` | the number in q-register i (0 if never set) |
| `%i` | adds 1 to q-register i; the new value |
| `nA` | the code of the character n-1 after the pointer, so `1A` is the one right of it and `0A` the one left, 0 past the end |
| `/@` | how long TECO has run, in 60ths of a second |
| `m+n m-n m*n m/n m&n` | sum, difference, product, quotient, bitwise and |
| `( )` | grouping |
| `m,n` | two arguments |
| `n=` | types n in decimal |
| `nUi` | puts n in q-register i |

A minus with nothing after it is the default negated, so `-D` is `-1D` and `-C` is `-1C`.
Values written side by side add.

## 5. Editing

| Command | Does |
|---|---|
| `nJ`, `J` | the pointer to n, or to 0 |
| `nC`, `nR` | the pointer n right, n left |
| `nL`, `L` | n lines right (past n LFs); 0 or negative, to the start of the line, or n lines back |
| `nT`, `T`, `i,jT` | types n lines from the pointer, back if negative, the line, or characters i+1 to j; `HT` the buffer |
| `nD` | deletes n characters right of the pointer, or left if negative |
| `nK`, `m,nK` | deletes what `nL` passes over, or characters m+1 to n |
| `Itext$` | inserts the text up to the ALT MODE |
| `^Ijtextj` | (up arrow and I) inserts the text between two j's |
| `nI` | inserts the character whose code is n (0 to 127) |
| `m,nXi`, `nXi` | copies characters m+1 to n, or n lines, into q-register i |
| `Gi` | inserts q-register i's text or its number's digits at the pointer |

A position outside 0 to `Z` is an error and a failed command changes nothing.

## 6. Pages in and out

Input is paper tape at the start, output the punch.

| Command | Does |
|---|---|
| `Y` | empties the buffer and reads the next page, through its FORM; the pointer at 0 |
| `A` | appends the next page; the pointer stays where it is |
| `PW` | outputs the buffer and a FORM |
| `P`, `nP` | `PW` then `Y`, n times |
| `i,jP` | outputs characters i+1 to j, no FORM |
| `nF` | punches n blank frames |
| `/S` | input from paper tape |
| `/C` | clears what the reader has read ahead |
| `/Q` | input from the tape file opened by `ER` again |
| `/R`, `/T` | the punch on, off |
| `/E`, `/B` | the line printer on, off |
| `/W`, `/V` | Microtape output on, off |

A page larger than the buffer is read as far as its last whole line that fits, the next
`Y` or `A` continues from there.
`PW` always outputs a FORM after the buffer.

**Paper tape** is in ASR-33 teletype ASCII format.
Every character punched with its eighth hole also punched.
`asr33tape` converts a text file into a tape and back:
```
asr33tape punch notes.txt notes.ptp
asr33tape text notes.ptp notes.txt
```
A FORM in the text ends a page.

## 7. Searches

`Stext$` finds the text right of the pointer and leaves the pointer after it.
`N` does a `P` and searches on when the buffer has no match, `_` (left arrow) does a `Y`.
A count before the letter finds the nth match.
Modifiers come first.
`:` gives the search a value, -1 for found and 0 not.
`^` (up arrow) takes the text between two copies of a delimiter, `^S/text/`.
A search that fails leaves the pointer at the end of the buffer.
Case matters.

## 8. Macros, iterations and conditionals

| Command | Does |
|---|---|
| `Mi` | executes q-register i's text as commands, it takes its caller's argument and leaves its value |
| `<` ... `>` | an iteration; `n<` runs it n times, 0 not at all, negative without end |
| `n;` | leaves the iteration if n is 0 or more |
| `:` | (alone) leaves the iteration if the last search failed |
| `!tag!`, `Otag$` | a tag and a jump to it in the same macro or command string |
| `n"G`, `n"L`, `n"N`, `n"E` | runs to the matching `'` only if n>0, n<0, n not 0, n=0 |
| `n"C` | runs to `'` only if n is an upper-case letter, a digit, `.`, `$` or `%` |

The memo's examples run as they are described; for instance, to count the line feeds in the buffer:
```
J0UN<:S
$;%N>QN=$$
```

## 9. The display, if enabled

While TECO waits for a command string the Type 340 shows 20 lines each side of the
pointer which is a flashing vertical bar.

| Command | Does |
|---|---|
| `n/N` | lines shown each side of the pointer |
| `n/D` | the size, 0 to 3 (1 at the start), for 144, 72, 36 or 18 columns |
| `nV`, `V`, `i,jV` | shows n lines, the line, or characters i+1 to j, until the next string, only as the string's last command |
| `nVW`, `VW`, `i,jVW` | as V, and waits for a key whose code is its value |
| `u/U` | the directory of drive u, or for the drum, 9, only as the last command |

`/U` shows, at size 1 whatever the size, a map of the volume's blocks, a column a block,
each holding its file's character (1 to 9, then A to V), `*` for the directory's, blank when free.
Below it is a line for each file with its number, mode character, names and length.
While the Microtape moves the screen shows WAIT in its center.

With sense switch 5 on, TECO does not use the 340.
Nothing is shown.
`V`, `VW` and `/U` still work, showing nothing.

Characters with no glyph, including `@` and BELL which the 340 lack, are shown as a down arrow
with their code in octal below the line.

## 10. Files on Microtape and the drum

A file has one or two names of up to six characters separated by a blank.
`u` is a drive, 1 to 8, or 9 for the drum.
An optional unit, written o here, is the last one named.

| Command | Does |
|---|---|
| `oERnm$` | opens file nm for reading; `Y`, `A`, `N`, `P` reads it |
| `oEI` | starts output to the volume |
| `oEFnm$` | names the output file as nm and writes the directory entry |
| `oEDnm$` | deletes nm from the directory in memory |
| `oENnm$qp$`, `i,uENnm$qp$` | renames nm to qp and sets its mode to i, in memory |
| `uEC` | writes the directory in memory to the volume |
| `u,vEC` | writes v's directory onto u; `u,0EC` an empty directory |
| `uET`, `uEK` | reads the directory into memory or forgets it |
| `oEL` | lists the directory, free entries and blocks, then each file's mode character, names and blocks |

`ED` and `EN` change only the directory copy in memory.
`EL` shows the change, `EC` or `EF` writes it, `EK` forgets it
A line ends on the tape in CR alone; TECO writes CR/LF as CR and reads CR as CR/LF.
Output goes to a scratch file, not listed, until `EF` names it.

**The drum** is unit 9 of the same tape commands, as in `9EL`, `9ERnm$`, `9EI` ... `EFnm$`.
Its files are on tracks 020 to 037, 256 blocks with 253 for files.
Tracks 0 to 017 are not used or modified.
Before its first use, `9,0EC` makes an empty directory.
Tt refuses a drum that holds another program's data.
A copy of a directory between the drum and a tape (`1,9EC`) is `?`.

The messages typed in place of the `?` are `FILE NOT FOUND`, `FILE EXISTS`, `FILE READ
ONLY`, `BAD DIRECTORY`, `DIRECTORY FULL`, `TAPE FULL`, `FOREIGN TAPE`, `UNIT UNABLE`, `UNIT
LOCKED` and `DRUM FAILED`.

`samples.mtp` holds three files to try, `LONG`, 500 numbered lines, `SHORT ONE`, two
lines, mode 1, and `NOTES TXT`, two lines, read-only.
Mount it on drive 1 with *pdp1central* or *mtp*, then `1EL$$` and `1ERLONG$Y$$`.

## 11. The PDP-1 additions

| Command | Does |
|---|---|
| `/X` | the test word switches, 18 bits unsigned |
| `n/X` | sense switch n, 1 to 6, -1 on and 0 off |
| `1/P`, `0/P` | the check character after each FORM punched, on and off (off at the start) |

The check character is the exclusive or of the page's characters punched without the
eighth hole.
Reading always checks it.
A page whose check character differs types `PARITY ERROR` and is read all the same.
A tape without check characters reads normally.

Sense switch 2 on is VT100 mode (see section 13), 3 turned on stops a running command,
4 at the greeting selects the second command set, 5 on turns the display off,
6 on puts the console on the Soroban.
With 2 on, 4, 5 and 6 are ignored but can still be read.

## 12. Differences from PDP-6 TECO

The memo is the specification.
PDP-6 TECO's surviving binary, later than the memo, differs from it in places, this TECO follows the memo:
- Errors are the bare `?`, not a three-letter code and `?`; `?` typed back shows the
  string through the bad command.
- RUBOUTs with no pause erase the whole string; the binary erases one at a time.
- A failed search leaves the pointer at the end of the buffer; the binary puts it at 0.
- `:` alone leaves an iteration after a failed search.
- A push-down list overflow types one bell and no `?`.
- `W` alone is no command, `VW` is, and `?` with no error pending does nothing.

Where this TECO chose differently from both, for the PDP-1:
- ALT MODE is ESC, the 1965 Teletype's 0175 and 0176 are text.
- Lower-case command letters act as upper case, the binary takes them as text.
- An ALT MODE between commands ends an argument and the commands after it run.
- `/@` counts 60ths of a second from the BBN clock.
- `Q` of a text register is `?`, as is `nI` outside 0 to 127, a position outside the buffer,
  and `]`.
- Ctrl-C, or sense switch 3, stops a running command; `/G` ends a session rather than
  returning to MACDMP which doesn't exist.
- `n<` takes a count as the binary does, the memo's `<` takes none.
- Q-register text is 14,332 characters, so `HXA` of a full buffer does not fit.
- the push-down list holds 102 open parentheses.
- Files are on Type 550 Microtape and the drum with the file system library's directory,
  not MAC DECtape; their messages are added to the library's messages.
- The 340 has no `@` and no bell symbol, so both appear as a down arrow and the octal code.
- Murphy's drum fields, `n/F`, `n/O`, are not built.
  The drum is a volume of files instead.

## 13. The second command set, PDP-11 TECO

Sense switch 4 on when TECO connects selects TECO-11 version 40's commands, as described in
DEC's *Standard TECO* manual, May 1985.
The greeting says `TECO PDP-1/PDP-11`, and the prompt is `*`.
The switch is read only at the greeting, never while a while a runs.
The next connection, or the Soroban when switch 6 enables it, gets this command set at that time.
Sense switch 2 on selects this set also, regardless of the switch 4 setting, VT100 mode, below.

The manual is the reference for every command below\
This section lists what this TECO adds, leaves out, or does differently.

**Starting.** Nothing is open at the greeting.
`Y` is `?NFI` until `ER`, and `P` `?NFO` until `EW`.
The flags start as the manual states, `ED` 0, `EH` 0, `ES` 0, `EU` -1, `EV` 0,
`^X` 0 (searches ignore case), the radix decimal, `ET` 1028 (bit 4, and bit 1024,
the refresh scope, which cannot be cleared), `EO` 40, `-1EJ` 7 (a PDP-11) and `0EJ` 0.

**Typing**.
- RUBOUT (Delete) erases one character and echoes it.
- Ctrl-U erases the line.
- Ctrl-G Ctrl-G erases the whole string.
- Ctrl-G space retypes the line and Ctrl-G `*` the string.
- Ctrl-C erases the string.
- Backspace is RUBOUT, except as the first key, where it is `-LT`.
- LF or ESC as the first key is `LT`.
- Also as the first key, `?` types the last string through its error.
- `*q` saves the last string in q-register q.
- Ctrl-W redraws the display.

A caret and a letter is that control character, `^A` is Ctrl-A, wherever a command takes a control character.
On the Soroban the stand-ins of section 2 hold (`#` is `%`, `|` is `;`), a `@`
before one of them is the character itself, so `@#` is or and `@|` the else of a conditional.

**Leaving.**
Ctrl-C at the prompt, `^C^C`, or `EX` ends the session as the 1965 set's `/G` does.\
Ctrl-C at the prompt leaves without saving the output, `EX` saving it first.\
Ctrl-C or sense switch 3 on while a command runs stops execution.

**Errors** are TECO-11's.
Each is `?` and a three-letter code, then the message as `EH` says, `EH` 0 and 2 the code and message,\
1 the code alone, and with 4 added the string through the error.\
A Microtape or drum failure comes under the nearest code with the
file system library's message, as in `?FNF   FILE NOT FOUND`.
`?` as the first key types the string again.\
Warnings start with `%`, as `%Superseding existing file` and `%Search fail in iter` do.

**Values** are 36 bits, -34,359,738,368 to 34,359,738,367, as in the 1965 set, not
TECO-11's 16, so `-1==` types 777777777777 and `-1===` FFFFFFFFF.
The radix is decimal, octal after `^O`, and 8, 10 or 16 with `n^R`; in radix 16 the digits take A to F.
`\` reads a number from the buffer, `n\` writes one to the buffer; `==` and `===` type in octal and hexadecimal;
`^_` is not, `#` or, `^^x` the character code of x.

**Q-registers** hold a number and a text at the same time.
`Uq` sets the number and leaves the text, `^Uq` the sets the text and leaves the number.
Each macro level has local registers `.A` to `.Z` and `.0` to `.9` created when used.
Only four levels below the prompt level can hold them at once, a fifth gives `?MEM`.
`[q` and `]q` push and pop a register, both number and text, up to 32 deep.
Q-register text is 14,332 characters across all registers.
`Xq` needs space for both the old text and the new at the same time.

**Control** is `"` with every test the manual lists, `|` for the else, `F> F<
F' F|`, `nO` with a list of tags, `$$` and `n$$` to leave a macro, `;` and `:;`, and `?` to trace.
`n<` with n of 0 or less does not run.
`"C` takes the RT-11 symbol characters, letters, digits, `.` and `$`.

**Searches** are `S N _ E_` with a count, backward with `-nS`, bounded
with `m,nS`, `FB FC FS FN F_ FR`, `:S` and `::S`, the match constructs (`^X ^S ^N`, `^E`
and its letters, `^Q ^R ^V ^W ^EQq ^EUq`), `G_`, `ES`.
A failed search moves the pointer to 0, unless `ED&16`.
With `^X` 0, a letter matches either case and `` ` { | } ~ `` match `@ [ \ ] ^`.
A search string holds up to 100 characters.

**Files** are the RT-11 specifications, `dev:name.ext`.
`DT1:` to `DT8:` are Microtape, `DR:` the drum with its file system volume on tracks 020 to 037,
`PC:` paper tape, `LP:` the printer.
`name.ext` is the 1965 set's two names, so `DT1:SHORT.ONE` is `SHORT ONE`.
A specification with no device uses the last one named.
The commands are `ER EW EB EC EF EK EX EI EN`, `G*` and `:G*`, `Y :Y EY A :A n:A P :P PW`,
and two of TECO-11's operating-system calls.
`:EGDIR dev:$` lists a unit's directory and `:EGDEL file$` deletes a file.
`EB` keeps the old file as `name.BAK` with the name cut to five characters.
A file `EW` names that is already there is replaced only when the output is closed, so
`EK` leaves it unchanged.
`Y` and `_` are refused with`?YCA` while output is open and the buffer holds text,
unless `ED&2` is given.
`EY` and `E_` yank regardless.
`P` writes a FORM after the page only when the page read ended at one, `^E`; `PW` always does.
One input and one output are open at a time.

**The display** is TECO-11's refresh scope, drawn before every prompt.
`nW` shows n lines each side of the pointer's, `0W` off until `W`, `nW` or the next caller, `W` on and
drawn immediately.
`1:W` sets the columns of the size, `m,1:W` the size nearest m columns, 144, 72, 36 or 18,
`2:W` the rows, `0:W` -1.
`nEV` types lines before each prompt, as `ES` does after a search.
The 1965 set's `/U` map of a tape has no TECO-11 form.

**A VT100 window**.
A telnet session terminal is a VT100 or VT102, can use split screen mode.\
MobaXterm, xterm, PuTTY and others can emulate one if the terminal type is set properly.

`5,7:W` keeps the bottom 5 rows for the command window and shows the buffer in the rows above it,
updated before every prompt.
the character after the pointer is in reverse video or a reversed space at a line's end.
`0,7:W` reverts to normal full-screen mode.
While it is split the 340 is unused, as after `0W`, and it is re-enabled when the split ends.
However, if disabled via the sense switch, it remains off.

TECO cannot ask the terminal its size, so tell it if you want other than 80 x 24.
The screen is 80 x 24 by default, until `m,1:W` sets the columns, 20 to 132,
`m,2:W` sets the rows, 12 to 66 , set them before the split.
`5,7:W` declares a VT100 if no terminal is declared; `4,0:W` for a VT100 or `8,0:W` for a VT102
declares one without splitting, and `-1,0:W` reverts it.
While one is declared, `W` and `n:W` have the manual's video terminal meanings, not the 340's above.
`nW` puts the pointer's line on row n of the window, line 16 initially.
`3:W` -1 shows every character (`^M`, `^J`, `^I`),
`5:W` holds the window stationary, -1 while the pointer stays in it, n until it comes within n
rows of an edge, `-1W` redraws it, and Ctrl-W as the first key redraws it.
Every new session starts with no type declared.

A paint takes about 0.7 s on a full window of long lines.
Leaving, `EX` or Ctrl-C, reverts to full-screen mode.

**VT100 mode.**
With sense switch 2 on, every telnet session automatically gets window mode without typing `5,7:W`.
The screen is cleared and split as `5,7:W` splits it, a VT100 declared, and the
greeting `TECO PDP-1/PDP-11 VT100 mode` is typed in the command rows.
The switch is read only at each greeting, as switch 4 is.
While it is on, the console is telnet only, the 340 is unused the whole session,
and switches 4, 5 and 6 are ignored.
Switch 3 and Ctrl-C still break a running command.
Full screen mode can still be exited, `0,7:W`, and the terminal type changed, e.g.`-1,0:W`.
Regardless, the 340 remains unused.

Turned on during a session, switch 2 takes effect at the next session, but switch 6 is
immediately ignored.

Do *not* press an arrow key or a function key.
TECO-11 reads the sequences they send only with `ED&32`, which is not implemented,
so an arrow key is ESC and two characters.
As the first key, ESC types the current line and the rest are typed into the next string.

**The PDP-1 additions.**
`^F` is the test word from the front panel switches, `n^F` sense switch n, -1 on, 0 of),
and `EWPC:name/P` punches the check character after each FORM.

**Left out** are TECO-11's second input and output streams, `EP`, `EA`, video terminals
other than the VT100 and VT102, `ED&32` and `ET&2`, reverse paging, the log file, and the
commands of TECO-10 and of particular operating systems except for the two stated above, `EZ`, `EQ`).
Text is 7-bit ASCII, so `nI` and `n^Uq` take n modulo 128.

**Where this TECO and TECO-11 differ**, aside from the 36-bit values, is in `ET`, `EO`, and
`EJ` above; a fifth set of local registers give the error `?MEM`.
`;` with no argument uses the last search's result.
`m,nS` is bounded as the manual's Appendix C states.

**The manual's examples** in its section 5.20 run as printed, with `DT1:` for
its files and their search strings filled in.
They are the macros that keeps the caller's radix, splitting, merging and rearranging files,
and sort with `O`.

One of them needs a correction.\
The structured sort sets `0UZ` before the outer
`<`, so it never ends once a pass has switched two lines, move it inside:\
`<0UZ J 0AUA L ...`.\
Tags, comments included, pass a value through as the manual says, so the radix macro's argument reaches `+0U0`.
