# ex/vi 3.7 for the PDP-1. "make" builds vi.rim; "make type64" builds vi.rim
# for the Type 64 printer instead of the Type 62; "make samples" makes
# samples.mtp again from Samples/; "make dist DEST=dir" copies vi's files into
# dir. The recipe prefix is > rather than a tab.

.RECIPEPREFIX = >
AM1 = am1
AM1FLAGS =
MTFS = /opt/pidp1-mods/bin/fstool

SRCS = vi.am1 vi.ah exaddr.ac excmd.ac exops.ac files.ac fslib.ac greeting.ac heap.ac \
       keys.ac lines.ac lpt.ac messages.ac options.ac ptape.ac regex.ac screen.ac \
       subst.ac termio.ac vaddr.ac vchange.ac vindent.ac vinsert.ac vmacro.ac vmain.ac \
       vmotion.ac vops.ac vtab.ac

SAMPLES = Samples/notes.txt Samples/long.txt Samples/exrc.txt Samples/tags.txt \
          Samples/prog.c

all: vi.rim

vi.rim: $(SRCS)
> $(AM1) $(AM1FLAGS) vi.am1

type64: $(SRCS)
> $(AM1) $(AM1FLAGS) -DLPT_TYPE64=1 vi.am1

samples: $(SAMPLES)
> rm -f samples.mtp
> $(MTFS) -i samples.mtp
> $(MTFS) -a -n "NOTES TXT" -f text -s Samples/notes.txt samples.mtp
> $(MTFS) -a -n "LONG TXT" -f text -s Samples/long.txt samples.mtp
> $(MTFS) -a -n "EXRC" -f text -s Samples/exrc.txt samples.mtp
> $(MTFS) -a -n "TAGS" -f text -s Samples/tags.txt samples.mtp
> $(MTFS) -a -n "PROG C" -f text -s Samples/prog.c samples.mtp

# the files that make up vi, which "make dist" copies
DIST = Makefile RUNNING.md samples.mtp $(SRCS)

dist:
> test -n "$(DEST)" || { echo "make dist DEST=dir"; exit 1; }
> mkdir -p $(DEST)/Samples
> cp $(DIST) $(DEST)
> cp $(SAMPLES) $(DEST)/Samples

clean:
> rm -f vi.rim >/dev/null 2>&1

.PHONY: all type64 samples dist clean
