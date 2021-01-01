APXS=apxs
APACHECTL=apachectl

APXS_CC=`$(APXS) -q CC`
APXS_TARGET=`$(APXS) -q TARGET`
APXS_CFLAGS=`$(APXS) -q CFLAGS`
APXS_SBINDIR=`$(APXS) -q SBINDIR`
APXS_CFLAGS_SHLIB=`$(APXS) -q CFLAGS_SHLIB`
APXS_INCLUDEDIR=`$(APXS) -q INCLUDEDIR`
APXS_LD_SHLIB=`$(APXS) -q LD_SHLIB`
APXS_LIBEXECDIR=`$(APXS) -q LIBEXECDIR`
APXS_LDFLAGS_SHLIB=`$(APXS) -q LDFLAGS_SHLIB`
APXS_SYSCONFDIR=`$(APXS) -q SYSCONFDIR`
APXS_LIBS_SHLIB=`$(APXS) -q LIBS_SHLIB`

all: mod_hsdl.so

mod_hsdl.o: mod_hsdl.cc
	g++ -c -fPIC -I$(APXS_INCLUDEDIR) -I/usr/include/apr-1/ $(APXS_CFLAGS) $(APXS_CFLAGS_SHLIB) -Wall -o $@ $< 

mod_hsdl.so: mod_hsdl.o 
	g++ -fPIC  -shared -o $@ $< $(APXS_LIBS_SHLIB)

install: all
	$(APXS)  -i -a -n 'hsdl' mod_hsdl.so

clean:
	-rm -f *.so *.o *~
