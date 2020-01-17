# svkbd - simple virtual keyboard
# See LICENSE file for copyright and license details.

include config.mk

SRC = svkbd.c

all: options svkbd-${LAYOUT}

options:
	@echo svkbd build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

config.h: config.mk
	@echo creating $@ from config.def.h
	@cp config.def.h $@

svkbd-%: layout.%.h config.h ${SRC}
	@echo creating layout.h from $<
	@cp $< layout.h
	@echo CC -o $@
	@${CC} -o $@ ${SRC} ${LDFLAGS} ${CFLAGS}

clean:
	@echo cleaning
	@for i in svkbd-*; \
	do \
		if [ -x $$i ]; \
		then \
			rm -f $$i 2> /dev/null; \
		fi \
	done; true
	@rm -f ${OBJ} svkbd-${VERSION}.tar.gz 2> /dev/null; true

dist: clean
	@echo creating dist tarball
	@mkdir -p svkbd-${VERSION}
	@cp LICENSE Makefile README config.def.h config.mk \
		${SRC} svkbd-${VERSION}
	@for i in layout.*.h; \
	do \
		cp $$i svkbd-${VERSION}; \
	done
	@tar -cf svkbd-${VERSION}.tar svkbd-${VERSION}
	@gzip svkbd-${VERSION}.tar
	@rm -rf svkbd-${VERSION}

install: all
	@echo installing executable files to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@for i in svkbd-*; \
	do \
		if [ -x $$i ]; \
		then \
			echo CP $$i; \
			cp $$i ${DESTDIR}${PREFIX}/bin; \
			chmod 755 ${DESTDIR}${PREFIX}/bin/$$i; \
		fi \
	done
#	@echo installing manual page to ${DESTDIR}${MANPREFIX}/man1
#	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
#	@sed "s/VERSION/${VERSION}/g" < svkbd.1 > ${DESTDIR}${MANPREFIX}/man1/svkbd.1
#	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/svkbd.1

uninstall:
	@echo removing executable files from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/svkbd-*
#	@echo removing manual page from ${DESTDIR}${MANPREFIX}/man1
#	@rm -f ${DESTDIR}${MANPREFIX}/man1/svkbd.1

.PHONY: all options clean dist install uninstall
