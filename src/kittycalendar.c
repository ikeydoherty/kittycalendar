/*
 * kittycalendar.c
 * 
 * Copyright 2015 Ikey Doherty <ikey@evolve-os.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "kittycalendar.h"

G_DEFINE_TYPE(KittyCalendar, kitty_calendar, GTK_TYPE_WINDOW)

/* Boilerplate GObject code */
static void kitty_calendar_class_init(KittyCalendarClass *klass);
static void kitty_calendar_init(KittyCalendar *self);
static void kitty_calendar_dispose(GObject *object);

static void init_styles(KittyCalendar *self);
static void adjust_placement(KittyCalendar *self);
static void adjust_date(KittyCalendar *self);
static void click_handler(KittyCalendar *self, GtkToggleButton *button);

/* Initialisation */
static void kitty_calendar_class_init(KittyCalendarClass *klass)
{
        GObjectClass *g_object_class;

        g_object_class = G_OBJECT_CLASS(klass);
        g_object_class->dispose = &kitty_calendar_dispose;
}

/**
 * Get the ball rolling
 */
static void kitty_calendar_init(KittyCalendar *self)
{
        GtkWidget *cal = NULL, *button = NULL, *layout = NULL, *rev = NULL, *image = NULL;
        GdkVisual *visual = NULL;
        GtkStyleContext *style = NULL;
        GtkSettings *settings = NULL;
        init_styles(self);

        /* General layout etc. */
        layout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_container_add(GTK_CONTAINER(self), layout);
        g_object_set(G_OBJECT(self), "margin-end", 1, "margin-bottom", 1, NULL);

        /* Our style.. */
        style = gtk_widget_get_style_context(GTK_WIDGET(self));
        gtk_style_context_add_class(style, "osd");
        settings = gtk_widget_get_settings(GTK_WIDGET(self));
        g_object_set(G_OBJECT(settings), "gtk-application-prefer-dark-theme", TRUE, NULL);

        /* Button styling */
        button = gtk_toggle_button_new();
        gtk_widget_set_can_focus(button, FALSE);
        gtk_widget_set_hexpand(button, FALSE);
        gtk_widget_set_halign(button, GTK_ALIGN_END);
        style = gtk_widget_get_style_context(button);
        gtk_style_context_add_class(style, "cal-button");
        image = gtk_image_new_from_icon_name("calendar", GTK_ICON_SIZE_INVALID);
        gtk_image_set_pixel_size(GTK_IMAGE(image), 48);
        gtk_container_add(GTK_CONTAINER(button), image);
        gtk_box_pack_end(GTK_BOX(layout), button, FALSE, FALSE, 0);

        /* Hook up the button logic. */
        g_signal_connect_swapped(G_OBJECT(button), "clicked", G_CALLBACK(click_handler), self);

        /* Our placement */
        gtk_window_set_gravity(GTK_WINDOW(self), GDK_GRAVITY_SOUTH_WEST);
        gtk_window_set_resizable(GTK_WINDOW(self), FALSE);
        gtk_window_set_decorated(GTK_WINDOW(self), FALSE);
        visual = gdk_screen_get_rgba_visual(gtk_widget_get_screen(GTK_WIDGET(self)));
        gtk_window_set_keep_above(GTK_WINDOW(self), TRUE);
        gtk_window_set_focus_on_map(GTK_WINDOW(self), FALSE);
        gtk_window_set_skip_pager_hint(GTK_WINDOW(self), TRUE);
        gtk_window_set_skip_taskbar_hint(GTK_WINDOW(self), TRUE);
        if (visual) {
                gtk_widget_set_visual(GTK_WIDGET(self), visual);
        } else {
                g_warning("Unable to set RGBA Visual! Use a compositing WM!");
        }

        /* Calender setup.. */
        cal = gtk_calendar_new();
        self->cal = cal;
        rev = gtk_revealer_new();
        self->rev = rev;
        gtk_box_pack_start(GTK_BOX(layout), rev, FALSE, FALSE, 0);

        gtk_widget_set_margin_bottom(cal, 1);
        g_object_set(G_OBJECT(cal), "show-week-numbers", TRUE, "margin-bottom", 1, NULL);
        gtk_container_add(GTK_CONTAINER(rev), cal);

        /* Revealer voodoo */
        gtk_revealer_set_reveal_child(GTK_REVEALER(rev), FALSE);
        gtk_revealer_set_transition_type(GTK_REVEALER(rev), GTK_REVEALER_TRANSITION_TYPE_SLIDE_UP);

        g_signal_connect(G_OBJECT(self), "destroy", G_CALLBACK(gtk_main_quit), NULL);

        adjust_date(self);
        adjust_placement(self);

        gtk_widget_show_all(GTK_WIDGET(self));
}

static void kitty_calendar_dispose(GObject *object)
{
        G_OBJECT_CLASS (kitty_calendar_parent_class)->dispose (object);
}

/* Utility; return a new KittyCalendar */
KittyCalendar *kitty_calendar_new(void)
{
        KittyCalendar *self;

        self = g_object_new(KITTY_CALENDAR_TYPE, NULL);
        return self;
}

/**
 * Load our built-in CSS resource
 */
static void init_styles(KittyCalendar *self)
{
        GtkCssProvider *css_provider;
        GFile *file = NULL;
        GdkScreen *screen;

        screen = gdk_screen_get_default();

        /* Fallback */
        css_provider = gtk_css_provider_new();
        file = g_file_new_for_uri("resource://com/evolve-os/kittycalendar/style.css");
        if (gtk_css_provider_load_from_file(css_provider, file, NULL)) {
                gtk_style_context_add_provider_for_screen(screen,
                        GTK_STYLE_PROVIDER(css_provider),
                        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        }
        g_object_unref(css_provider);
        g_object_unref(file);
}

/**
 * Force placement on bottom-right-corner
 */
static void adjust_placement(KittyCalendar *self)
{
        GdkScreen *screen = NULL;
        GdkRectangle rect;
        GtkAllocation alloc;
        gint mon;
        gint x, y;

        screen = gtk_widget_get_screen(GTK_WIDGET(self));
        mon = gdk_screen_get_primary_monitor(screen);
        gdk_screen_get_monitor_geometry(screen, mon, &rect);
        gtk_widget_get_allocation(GTK_WIDGET(self), &alloc);

        x = rect.x + (rect.width - alloc.width);
        y = rect.y + (rect.height - alloc.height);

        gtk_window_move(GTK_WINDOW(self), x, y);
}

/**
 * Update the current date
 */
static void adjust_date(KittyCalendar *self)
{
        gint m, d, y;

        GDateTime *time = g_date_time_new_now_local();
        m = g_date_time_get_month(time) - 1;
        d = g_date_time_get_day_of_month(time);
        y = g_date_time_get_year(time);

        g_object_set(G_OBJECT(self->cal), "month", m, "year", y, "day", d, NULL);
        gtk_calendar_mark_day(GTK_CALENDAR(self->cal), d);
        g_date_time_unref(time);
}

/**
 * Swap visibility of the calendar
 */
static void click_handler(KittyCalendar *self, GtkToggleButton *button)
{
        gboolean active = gtk_toggle_button_get_active(button);
        GtkRevealerTransitionType type = active ?
                GTK_REVEALER_TRANSITION_TYPE_SLIDE_UP :
                GTK_REVEALER_TRANSITION_TYPE_SLIDE_DOWN;
        gtk_revealer_set_reveal_child(GTK_REVEALER(self->rev), active);
        gtk_revealer_set_transition_type(GTK_REVEALER(self->rev), type);

        if (active) {
                adjust_date(self);
        }
}

/**
 * TODO: Convert to GtkApplication, etc.
 */
int main(int argc, char **argv)
{
        KittyCalendar *cal = NULL;
        gtk_init(&argc, &argv);

        cal = kitty_calendar_new();
        gtk_main();

        return 0;
}
