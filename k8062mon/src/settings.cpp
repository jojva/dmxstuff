#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "settings.h"
extern "C" {
#include "support.h"
};

#define BUFSIZE 512
#define DEBUG

//Compiled-in defaults
#define N_CHANNELS 32
#define START_CHANNEL 1
#define UI_REFRESH	100	//time in milliseconds

extern "C" {
gpointer load_settings(char *settingsfile)
{
if(!settingsfile){
	fputs("Program error: No settings file given\n",stderr);
	return NULL;
}
SETTINGS *so=new SETTINGS(N_CHANNELS,START_CHANNEL,UI_REFRESH);
so->load(settingsfile);
//if(so->load(settingsfile)) return (gpointer)so; else return NULL;
return so;
}

void setup_settingswindow(GtkWidget *settingswindow)
{
SETTINGS *so=(SETTINGS *)g_object_get_data(G_OBJECT(settingswindow),"settingsptr");
GtkSpinButton *spin;

if(!so){
	fputs("Program error: Unable to locate settings object\n",stderr);
	abort();
}

spin=GTK_SPIN_BUTTON(lookup_widget(settingswindow,"n_channels_spin"));
gtk_spin_button_set_value(spin,(gdouble)so->get_channels());
spin=GTK_SPIN_BUTTON(lookup_widget(settingswindow,"start_channel_spin"));
gtk_spin_button_set_value(spin,(gdouble)so->get_start_channel());
spin=GTK_SPIN_BUTTON(lookup_widget(settingswindow,"refresh_time_spin"));
gtk_spin_button_set_value(spin,(gdouble)so->get_refresh_time());

}

void
on_settings_ok_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
gdouble n;
GtkWidget *settingswindow;
settingswindow=lookup_widget(GTK_WIDGET(button),"settings_window");
SETTINGS *so=(SETTINGS *)g_object_get_data(G_OBJECT(settingswindow),"settingsptr");
GtkSpinButton *spin;

if(!so){
	fputs("Program error: Unable to locate settings object\n",stderr);
	abort();
}

spin=GTK_SPIN_BUTTON(lookup_widget(settingswindow,"n_channels_spin"));
so->set_channels((int)gtk_spin_button_get_value(spin));
spin=GTK_SPIN_BUTTON(lookup_widget(settingswindow,"start_channel_spin"));
so->set_start_channel((int)gtk_spin_button_get_value(spin));
spin=GTK_SPIN_BUTTON(lookup_widget(settingswindow,"refresh_time_spin"));
so->set_refresh_time((int)gtk_spin_button_get_value(spin));
so->save();

gtk_widget_destroy(settingswindow);
}


void
on_settings_cancel_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
GtkWidget *settingswindow;
settingswindow=lookup_widget(GTK_WIDGET(button),"settings_window");

gtk_widget_destroy(settingswindow);
}

}; //extern "C"

settingsobject::settingsobject()
{
n_chans=0;
start_chan=0;
refresh_time=0;
active_settings_file=NULL;
}

settingsobject::settingsobject(int a,int b,int c)
{
n_chans=a;
start_chan=b;
refresh_time=c;
active_settings_file=NULL;
}

settingsobject::~settingsobject()
{
if(active_settings_file){
	free(active_settings_file);
	active_settings_file=NULL;
}
}

int settingsobject::get_channels()
{
return n_chans;
}

int settingsobject::get_start_channel()
{
return start_chan;
}

int settingsobject::get_refresh_time()
{
return refresh_time;
}

void settingsobject::set_channels(int n)
{
n_chans=n;
}

void settingsobject::set_start_channel(int n)
{
start_chan=n;
}

void settingsobject::set_refresh_time(int n)
{
refresh_time=n;
}

bool settingsobject::load(char *filename)
{
FILE *fp;
int n;
char buf[BUFSIZE];

if(!filename) return false;

if(active_settings_file){
	free(active_settings_file);
}
active_settings_file=strdup(filename);

fp=fopen(filename,"r");
if(!fp) return false;

while(!feof(fp)){
	fgets(buf,BUFSIZE,fp);
	
	if(sscanf(buf,"dmx_channels=%d",&n)){
		n_chans=n;
		#ifdef DEBUG
		printf("Got dmx channels %d\n",n_chans);
		#endif
	} else if(sscanf(buf,"dmx_start=%d",&n)){
		start_chan=n;
		#ifdef DEBUG
		printf("Got start channel %d\n",start_chan);
		#endif
	} else if(sscanf(buf,"ui_refresh=%d",&n)){
		refresh_time=n;
		#ifdef DEBUG
		printf("Got refresh time %d\n",refresh_time);
		#endif
	}
}
fclose(fp);
return true;
}

bool settingsobject::save()
{
FILE *fp;
if(!active_settings_file) return false;

fp=fopen(active_settings_file,"w");
if(!fp){
	fprintf(stderr,"Error: Unable to open settings file '%s' for writing\n",active_settings_file);
	return false;
}

fprintf(fp,"dmx_channels=%d\ndmx_start=%d\nui_refresh=%d\n\n",n_chans,start_chan,refresh_time);
fclose(fp);
}


