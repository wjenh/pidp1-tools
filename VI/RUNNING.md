# Running ex/vi on the PDP-1

vi, Bill Joy's display editor, with its line editor ex, rewritten in am1 for the PDP-1D.
Its commands are those of ex/vi 3.7 as 4.3BSD shipped it; *An Introduction to Display
Editing with Vi* and the *Ex Reference Manual, Version 3.7* (4.3BSD's USD:15 and USD:16)
are its manuals.
This page is enough to start it, and lists where the PDP-1 differs from those manuals.

This is version 1.0\
Edit date 16-Nov-2026\
Initial version

## Building and starting it

This needs am1 and the pidp1-mods environment.

Just type:
```
make
```

This makes *vi.rim*.
`make type64` makes it for the Type 64 printer instead of the Type 62.
`make samples` makes *samples.mtp* again from *Samples/*, with *fstool*.
`make dist DEST=dir` copies the files that make up vi into dir.

Load *vi.rim* like any other tape using *pdp1central*, *fastload*, or the paper tape reader.
The starting address is 0100.
Starting there clears everything, and so does each caller's leaving: the next caller finds
an empty buffer, no file names, empty registers, no marks, no maps or abbreviations, and
every option at its default.

It needs the PDP-1D's `lai` and `lia`, the multiply and divide instructions (`muldiv=on`), the
BBN clock, DCS2 and all 16 banks of memory.
It uses the Type 23 drum and the Type 550 Microtape for files, the paper tape reader and punch,
and the Type 62 line printer.

## Talking to it

Connect a telnet client to port 2030 of the machine running vi:
```
telnet <host> 2030
```
vi types a new line, then `ex/vi 3.7 PDP-1`, and ex's prompt, `:`.
The buffer is empty and has no file name.
One caller is served at a time.

The client must send each key as it is typed. If your typing does not echo until RETURN,
press Ctrl-], type `mode character`, and press RETURN.

**The terminal.** vi cannot ask the terminal what it is, so you tell it:
```
:set term=vt100
```
(or `vt102`; `dumb`, the default, is any terminal). Then `:vi` starts visual mode, and `Q`
goes back to ex. The screen is 80 by 24 unless you say otherwise with `:set lines=n`
(12 to 66) and `:set columns=n` (20 to 132). On a `vt102` vi also uses the terminal's own
insert and delete line, which makes some changes faster. xterm, PuTTY and most terminal
programs emulate a VT102.

**The keys.** Return ends a command line. ESC ends an insert. Delete, or Ctrl-C, stops a
running command (`Interrupt`). The arrow keys move in command mode; in an insert an arrow
ends the insert and then moves, which 3.7 itself does not do. vi tells an arrow from a typed
ESC by the time between the keys, so over a slow link type ESC and the next key a moment
apart.

**Leaving.** `:q`, `:wq`, `:x` and `ZZ` end the session, close the connection, and leave vi
empty for the next caller. `:q` refuses while the buffer has changes not written; `:q!`
leaves anyway, and the changes are lost.

**A lost connection.** If the connection drops while the buffer has changes not written,
vi saves the buffer on the drum, replacing any buffer saved before. The next caller types
`:recover` (or `:rec`) to get it back.

## The sense switches

| Switch | On |
|---|---|
| 2 | each caller starts in visual mode, with `term` set to `vt100`; the greeting ends ` visual mode` |
| 3 | turned on, stops a running command, as Delete does |

Switch 2 is read when vi greets a caller. Switch 4 is kept for a second command set; 5 and 6
are not used.

## Files

A file's name is `dev:name.ext`, for example `DT1:NOTES.TXT`, letters taken as upper case;
`name.ext` is at most 9 characters, the dot counting as one. A name without a device is on
the current file's device, or on the drum before there is one. `%` and `#` stand for the
current and the alternate file's full names, as in 3.7.

| Device | What it is |
|---|---|
| `DT1:` to `DT8:` | a Microtape on drive 1 to 8 |
| `DR:` | the drum, tracks 020 to 037: the same volume as PDP-1 TECO's, so the two editors share files |
| `PC:` | paper tape: `:r PC:` reads the reader's tape, `:w PC:` punches the buffer, in FOCAL's ASCII form |
| `LP:` | the line printer: `:w LP:` prints the buffer; it cannot be read |

Microtape and the drum hold files in the shared file system of *Am1Includes/FILESYSTEM*,
a line ending in a carriage return.
*fstool* copies files between a tape image and the host, see *Docs/UsingFstool.md*.
*asr33tape*, installed with the emulator, turns a text file into a paper tape and back.

**Formatting.** `:format DT1:` gives the tape on drive 1 an empty directory, and
`:format DR:` the drum's volume. A volume that holds files, or data vi does not recognize,
is refused until you type `:format!`. A blank tape needs no `!`.

**Starting files.** At each greeting vi runs the commands in `DR:EXRC`, if there is one, as
`:source DR:EXRC` would. `:ta` and Ctrl-] look tags up in `DR:TAGS` (the `tags` option); a
tags file is made on a host and copied in, with each file's name in the `dev:name.ext` form.

**The samples.** *samples.mtp* holds five files to try: `NOTES.TXT`, a short text;
`LONG.TXT`, 200 numbered lines to scroll through; `EXRC`, a starting file; `PROG.C` and
`TAGS`, a small program and its tags. Mount it on drive 1 with *pdp1central*, then
```
:e DT1:NOTES.TXT
```
To use the starting file and the tags, copy them to the drum:
```
:e DT1:EXRC
:w DR:EXRC
:e DT1:PROG.C
:w DR:PROG.C
:e DT1:TAGS
:w DR:TAGS
```

## Where it differs from 3.7

- No shell: `:!`, `:sh`, `!` and the filters that run a host program are not there; nor is
  `:stop` or Ctrl-Z.
- No open mode: visual mode needs a declared VT100 or VT102.
- No LISP mode: the `lisp` option and `=` are not there.
- Options that served the host (`directory`, `shell`, `mesg`, `modeline`, `optimize`,
  `redraw`, `open`, `w300`, `w1200`, `w9600`) are not there; `lines` and `columns` are new.
- `:recover` reads the drum's saved buffer, as above, rather than a host's preserved file;
  `preserve` is not there.
- The starting file is `DR:EXRC` only, for everyone; there is no `EXINIT`.
- `:format` is new.
- A line holds at most 1,023 characters, and the buffer at most 4,095 lines. The text lives
  in five banks of memory, about 36,000 characters of typical source; a change keeps the old
  lines for `u` while it makes the new ones, so a file of about half that leaves room for
  any change. A file larger than the memory is read as far as it fits, with `File too
  large`, and `:w` back to it is refused unless `:w!`. A change with no room for its undo is
  refused with `Out of memory saving lines for undo`.
- `:version` types `Version 3.7 PDP-1, 6/7/85`.

## Credits

- ex and vi are Bill Joy's, written at Berkeley; 3.7 is the version of 6/7/85, with Mark
  Horton's changes, as 4.3BSD shipped it.
- The documents are Joy and Horton's *An Introduction to Display Editing with Vi* and *Ex
  Reference Manual, Version 3.7*.
