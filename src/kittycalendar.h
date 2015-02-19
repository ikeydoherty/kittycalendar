/*
 * kittycalendar.h
 * 
 * Copyright 2015 Ikey Doherty <ikey@evolve-os.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 */
#ifndef kitty_calendar_h
#define kitty_calendar_h

#include <glib-object.h>
#include <gtk/gtk.h>

typedef struct _KittyCalendar KittyCalendar;
typedef struct _KittyCalendarClass   KittyCalendarClass;

#define KITTY_CALENDAR_TYPE (kitty_calendar_get_type())
#define KITTY_CALENDAR(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KITTY_CALENDAR_TYPE, KittyCalendar))
#define IS_KITTY_CALENDAR(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KITTY_CALENDAR_TYPE))
#define KITTY_CALENDAR_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), KITTY_CALENDAR_TYPE, KittyCalendarClass))
#define IS_KITTY_CALENDAR_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), KITTY_CALENDAR_TYPE))
#define KITTY_CALENDAR_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), KITTY_CALENDAR_TYPE, KittyCalendarClass))

/* KittyCalendar object */
struct _KittyCalendar {
        GtkWindow parent;
        GtkWidget *cal;
        GtkWidget *rev;
};

/* KittyCalendar class definition */
struct _KittyCalendarClass {
        GtkWindowClass parent_class;
};

GType kitty_calendar_get_type(void);

/* KittyCalendar methods */

/**
 * Construct a new KittyCalendar
 * @return A new KittyCalendar
 */
KittyCalendar *kitty_calendar_new(void);

#endif /* kitty_calendar_h */
