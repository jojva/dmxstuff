#include <pthread.h>

class timer {
	public:
	timer();
	~timer();

	bool is_running();
	void set_interval(int usec);
	void run();
	void stop();

	void set_callback(void (*)(void *data),void *user_data);


};

