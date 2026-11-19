# TECO for the PDP-1. "make" builds teco.rim; "make type64" builds
# teco.rim for the Type 64 printer instead of the Type 62; "make samples" makes
# samples.mtp again from Samples/; "make dist DEST=dir" copies TECO's files
# into dir. The recipe prefix is > rather than a tab.

.RECIPEPREFIX = >
AM1 = am1
AM1FLAGS =
MTFS = /opt/pidp1-mods/bin/fstool

SRCS = teco.am1 teco.ah additions.ac buffer.ac cmdread.ac control.ac display.ac dpytab.ac edit.ac \
       expr.ac interp.ac lpt.ac pageio.ac ptape.ac qreg.ac search.ac tapefile.ac \
       tapemsg.ac tapeout.ac termdata.ac termio.ac ttytab.ac greeting.ac \
       interp11.ac cmdread11.ac edit11.ac control11.ac qreg11.ac search11.ac data11.ac \
       files11.ac dpy11.ac vt11.ac

all: teco.rim

teco.rim: $(SRCS)
> $(AM1) $(AM1FLAGS) teco.am1

type64: $(SRCS)
> $(AM1) $(AM1FLAGS) -DLPT_TYPE64=1 teco.am1

samples: Samples/long.txt Samples/short.txt Samples/notes.txt
> rm -f samples.mtp
> $(MTFS) -i samples.mtp
> $(MTFS) -a -n LONG -f text -s Samples/long.txt samples.mtp
> $(MTFS) -a -n "SHORT ONE" -f text -t 1 -s Samples/short.txt samples.mtp
> $(MTFS) -a -n "NOTES TXT" -f text -r -s Samples/notes.txt samples.mtp

# the files that make up TECO, which "make dist" copies
DIST = Makefile README.md RUNNING.md UsingTECO.md samples.mtp $(SRCS)

dist:
> test -n "$(DEST)" || { echo "make dist DEST=dir"; exit 1; }
> mkdir -p $(DEST)/Samples
> cp $(DIST) $(DEST)
> cp Samples/*.txt $(DEST)/Samples

clean:
> rm -f teco.rim >/dev/null 2>&1

.PHONY: all type64 samples dist clean
