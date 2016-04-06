#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#define SETTINGSFILE ".k8062mon.rc"
#define DEBUG

void
on_mainwindow_realize                  (GtkWidget       *widget,
                                        gpointer         user_data)
{
GtkWidget *box;
GtkStatusbar *sb;
int sbid;
gpointer cc,settingsptr=NULL;
char *homedir,*settingsname;

homedir=getenv("HOME");
settingsname=(char *)malloc(strlen(homedir)+strlen(SETTINGSFILE)+2);
if(!settingsname){
	fputs("Error: not enough memory\n",stderr);
	abort();
}
snprintf(settingsname,strlen(homedir)+strlen(SETTINGSFILE)+2,"%s/%s",homedir,SETTINGSFILE);
#ifdef DEBUG
fprintf(stderr,"Loading settings from %s\n",settingsname);
#endif

settingsptr=load_settings(settingsname);
free(settingsname);

g_object_set_data((gpointer)widget,"settingsptr",settingsptr);

box=lookup_widget(widget,"vbox1");
cc=init_ui(box,settingsptr);
init_timer(cc,settingsptr);
//gtk_widget_show_all(box);
//
sb=GTK_STATUSBAR(lookup_widget(widget,"statusbar"));
sbid=gtk_statusbar_get_context_id(sb,"ui");
gtk_statusbar_pop(sb,sbid);
gtk_statusbar_push(sb,sbid,"Velleman K8062 Monitor App");
}


gboolean
on_mainwindow_delete_event             (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
gtk_exit(0);

  return FALSE;
}


gboolean
on_mainwindow_destroy_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
gtk_exit(0);
  return FALSE;
}


void
on_exit_button_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
gtk_exit(0);
}


void
on_settings_button_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
GtkWidget *mainwindow,*settingswindow;
gpointer settingsptr;

mainwindow=lookup_widget(GTK_WIDGET(toolbutton),"mainwindow");
if(!mainwindow){
	fputs("Unable to locate mainwindow ptr!\n",stderr);
	abort();
}

settingsptr=g_object_get_data((gpointer)mainwindow,"settingsptr");

settingswindow=create_settings_window();
g_object_set_data((gpointer)settingswindow,"settingsptr",settingsptr);
setup_settingswindow(settingswindow);
gtk_widget_show_all(settingswindow);
}

