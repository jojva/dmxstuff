#include <gtk/gtk.h>
#include "channels.h"

channelwidget::channelwidget(int channel,char *labeltext,k8062_client *client)
{
if(client !=NULL){
	active=true;
}

dmxchannel=channel;
dmxclient=client;
progressbar=gtk_progress_bar_new();
gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(progressbar),GTK_PROGRESS_BOTTOM_TO_TOP);
gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar),0.0);
if(labeltext){
	label=gtk_label_new(labeltext);
} else {
	label=NULL;
}
}

channelwidget::~channelwidget()
{
active=false;
g_object_unref((gpointer)progressbar);
}

void channelwidget::update()
{
BYTE v;
gdouble percent;
char text[4];

v=dmxclient->peek_channel((BYTE)dmxchannel);
percent=(gdouble)v/(gdouble)255;
gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar),percent);

snprintf(text,4,"%02.0f%%",percent*100.0);
gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressbar),text);
}

void channelwidget::pack(GtkWidget *container,int x,int y)
{

if(GTK_IS_TABLE(container)){
	gtk_table_attach_defaults(GTK_TABLE(container),progressbar,x,x+1,y,y+1);
	if(label)
		//gtk_table_attach_defaults(GTK_TABLE(container),label,x,x+1,y+1,y+2);
		gtk_table_attach(GTK_TABLE(container),label,x,x+1,y+1,y+2,GTK_EXPAND,GTK_SHRINK,0,0);
} else if(GTK_IS_BOX(container)){
	gtk_box_pack_start_defaults(GTK_BOX(container),progressbar);
}
}

channelcollection::channelcollection(int start_channel,int n_channel,k8062_client *client)
{
int n;
CHANNELWIDGET *w;
char labeltext[5];

channelwidgets=NULL;
free_dmx=false;
dmxclient=client;

if(start_channel>0 && n_channel>0 && start_channel+n_channel<255){
	for(n=start_channel;n<start_channel+n_channel;++n){
		w=new CHANNELWIDGET(n,NULL,client);
		channelwidgets=g_list_append(channelwidgets,(gpointer)w);
	}
}
}

channelcollection::channelcollection(int start_channel,int n_channel)
{
int n;
CHANNELWIDGET *w;
char name[12];

channelwidgets=NULL;

dmxclient=new k8062_client;
free_dmx=true;

if(start_channel>0 && n_channel>0 && start_channel+n_channel<255){
	for(n=start_channel;n<start_channel+n_channel;++n){
		snprintf(name,12,"Ch%d",n);
		w=new CHANNELWIDGET(n,name,dmxclient);
		channelwidgets=g_list_append(channelwidgets,(gpointer)w);
	}
}

}

channelcollection::~channelcollection()
{
GList *item;
CHANNELWIDGET *w;

for(item=channelwidgets;item!=NULL;item=item->next){
	w=(CHANNELWIDGET *)item->data;
	delete w;
}
channelwidgets=NULL;

if(free_dmx){
	delete dmxclient;
}
}

void channelcollection::pack_channels(GtkWidget *container,int x,int y)
{
GList *item;
CHANNELWIDGET *w;
int wx=x,wy=y;

for(item=channelwidgets;item!=NULL;item=item->next){
	w=(CHANNELWIDGET *)item->data;
	w->pack(container,wx,wy);
	++wx;
}
}

void channelcollection::update_channels()
{
GList *item;
CHANNELWIDGET *w;

for(item=channelwidgets;item!=NULL;item=item->next){
	w=(CHANNELWIDGET *)item->data;
	w->update();
}
}
