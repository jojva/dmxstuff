typedef unsigned char BYTE;

class k8062_client {
	public:
		k8062_client();
		~k8062_client();

		bool is_connected();

		bool connect();
		void shutdown();
        void set_channel(BYTE ch,BYTE n);
		//note: peek_channel only gives the value that the DMX daemon
		//is outputting, not necessarily the physical channel value
		BYTE peek_channel(BYTE ch);
	private:
		int *dmx_maxchannels;
		int *dmx_shutdown;
		int *dmx_caps;
		int *dmx_channels;
		
		int *shm;
		int shmid;
		bool connected;
};
