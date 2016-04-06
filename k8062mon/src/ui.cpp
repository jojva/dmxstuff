#include <gtk/gtk.h>
#include "channels.h"
#include "settings.h"
#include <stdlib.h>

extern "C" {

gpointer init_ui(GtkWidget *box,SETTINGS *settings)
{
CHANNELCOLLECTION *cc;
GtkTable *table;

if(!settings){
	fputs("Internal error: ui init with no settings\n",stderr);
	abort();
}

cc=new CHANNELCOLLECTION(settings->get_start_channel(),settings->get_channels());
							//this will initialise
							//the connection to k8062
							//also
table=(GtkTable *)gtk_table_new(2,settings->get_channels(),true);
gtk_box_pack_end_defaults(GTK_BOX(box),GTK_WIDGET(table));
cc->pack_channels(GTK_WIDGET(table),0,0);
gtk_box_reorder_child(GTK_BOX(box),GTK_WIDGET(table),0);
gtk_widget_show_all(GTK_WIDGET(table));

return (gpointer)cc;
}

gboolean timer_proc(gpointer data)
{
CHANNELCOLLECTION *cc=(CHANNELCOLLECTION *)data;

//putchar('.');
//fflush(stdout);
cc->update_channels();
return TRUE;	//false disables the timer
}

int init_timer(gpointer cc,SETTINGS *settings)
{
return g_timeout_add(settings->get_refresh_time(),&timer_proc,cc);
}

}
