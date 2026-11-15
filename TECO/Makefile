# TECO for the PDP-1. "make" builds teco.rim; "make type64" builds
# teco.rim for the Type 64 printer instead of the Type 62.

AM1 = am1
AM1FLAGS =

SRCS = teco.am1 teco.ah additions.ac buffer.ac cmdread.ac control.ac display.ac dpytab.ac edit.ac \
       expr.ac interp.ac lpt.ac pageio.ac ptape.ac qreg.ac search.ac tapefile.ac \
       tapemsg.ac tapeout.ac termdata.ac termio.ac ttytab.ac greeting.ac \
       interp11.ac cmdread11.ac edit11.ac control11.ac qreg11.ac search11.ac data11.ac \
       files11.ac dpy11.ac vt11.ac

all: teco.rim

teco.rim: $(SRCS)
	$(AM1) $(AM1FLAGS) teco.am1

type64: $(SRCS)
	$(AM1) $(AM1FLAGS) -DLPT_TYPE64=1 teco.am1

clean:
	rm -f teco.rim >/dev/null 2>&1
