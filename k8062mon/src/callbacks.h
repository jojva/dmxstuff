#include <gtk/gtk.h>


void
on_mainwindow_realize                  (GtkWidget       *widget,
                                        gpointer         user_data);

gboolean
on_mainwindow_delete_event             (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_mainwindow_destroy_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_exit_button_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_settings_button_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_settings_ok_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_settings_cancel_clicked             (GtkButton       *button,
                                        gpointer         user_data);
