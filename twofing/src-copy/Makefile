CC = gcc
OBJECTS = twofingemu.o gestures.o easing.o
LIBS = -lm -lpthread -lXtst -lXrandr -lX11 -lXi
CFLAGS = -Wall -O2
BINDIR = $(DESTDIR)/usr/bin
NAME = twofing

twofing: $(OBJECTS)
	$(CC) -o $(NAME) $(OBJECTS) $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

install:
	mkdir -p $(BINDIR)
	mkdir -p $(DESTDIR)/etc/udev/rules.d/
	install --mode=755 $(NAME) $(BINDIR)/
	for f in rules/*.rules; do cp $$f $(DESTDIR)/etc/udev/rules.d/; done

clean:
	rm -f *.o $(NAME)

uninstall:
	rm $(BINDIR)/$(NAME)
