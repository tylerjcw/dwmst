PROG       = dwmst
CC         = gcc
PREFIX    ?= ${HOME}
BINPREFIX  = ${PREFIX}/bin

LIBS     = -lm
CFLAGS   += -Wall -Wextra

${PROG}: ${PROG}.c
	@${CC} ${CFLAGS} ${LIBS} ${PROG}.c -o ${PROG}

install:
	install -Dm755 ${PROG} ${BINPREFIX}/

uninstall:
	rm -f ${BINPREFIX}/${PROG}

clean:
	rm -f ${PROG}

.PHONY: debug clean install uninstall
