
#ifdef __cplusplus
typedef class settingsobject {
	public:
		settingsobject();
		settingsobject(int n_chans,int start_chan,int refresh_time);
		~settingsobject();

		bool load(char *filename);
		bool save();
		int get_channels();
		int get_start_channel();
		int get_refresh_time();

		void set_channels(int n);
		void set_start_channel(int n);
		void set_refresh_time(int n);
	private:
		int n_chans;
		int start_chan;
		int refresh_time;

		char *active_settings_file;
} SETTINGS;
#endif
