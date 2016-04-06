#include <gtk/gtk.h>
#include "k8062.h"

#ifdef __cplusplus
typedef class channelwidget {
	public:
		channelwidget(int dmxchannel,char *label,k8062_client *client);
		~channelwidget();

		void update();
		void pack(GtkWidget *container,int x,int y);
	private:
		GtkWidget *progressbar,*label;
		int dmxchannel;
		bool active;

		k8062_client *dmxclient;	//weak ref to master client object
} CHANNELWIDGET;

typedef class channelcollection {
	public:
		channelcollection(int start_channel,int n_channel,k8062_client *client);
		channelcollection(int start_channel,int n_channel);
		~channelcollection();
		void pack_channels(GtkWidget *container,int x,int y);
		void update_channels();

	private:
		GList *channelwidgets;
		k8062_client *dmxclient;
		bool free_dmx;
} CHANNELCOLLECTION;
#endif
