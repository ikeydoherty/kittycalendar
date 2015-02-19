-include $(top_srcdir)/common.mk

kittycalendar_resources = $(shell glib-compile-resources --sourcedir=$(top_srcdir)/data --generate-dependencies $(top_srcdir)/data/kittycalendar.gresource.xml)

BUILT_SOURCES = \
	kitty-calendar-resources.h \
	kitty-calendar-resources.c

CLEANFILES = \
	$(BUILT_SOURCES)

# resources
kitty-calendar-resources.h: $(top_srcdir)/data/kittycalendar.gresource.xml $(kittycalendar_resources)
	glib-compile-resources --target=$@ --sourcedir=$(top_srcdir)/data --generate-header --c-name kitty_calendar $<

kitty-calendar-resources.c: $(top_srcdir)/data/kittycalendar.gresource.xml $(kittycalendar_resources)
	glib-compile-resources --target=$@ --sourcedir=$(top_srcdir)/data --generate-source --c-name kitty_calendar $<

bin_PROGRAMS = kitty-calendar

kitty_calendar_SOURCES = \
	kittycalendar.c \
	kittycalendar.h \
	kitty-calendar-resources.c \
	kitty-calendar-resources.h

kitty_calendar_CFLAGS = \
	$(GTK3_CFLAGS) \
	$(AM_CFLAGS)

kitty_calendar_LDADD = \
	$(GTK3_LIBS)
