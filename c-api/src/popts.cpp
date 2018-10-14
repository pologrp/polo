#include "common.hpp"

/* Parameter server options*/
extern "C" {
  void *paramserver_options() { return static_cast<void *>(new option_t); }

  void linger(void *handle, const int time) {
    static_cast<option_t *>(handle)->linger(time);
  }

  void master_timeout(void *handle, const long timeout) {
    static_cast<option_t *>(handle)->master_timeout(timeout);
  }

  void worker_timeout(void *handle, const long timeout) {
    static_cast<option_t *>(handle)->worker_timeout(timeout);
  }

  void scheduler_timeout(void *handle, const long timeout) {
    static_cast<option_t *>(handle)->scheduler_timeout(timeout);
  }

  void num_masters(void *handle, const int32_t num) {
    static_cast<option_t *>(handle)->num_masters(num);
  }

  void scheduler(void *handle, const char *address, const uint16_t pub_port,
		 const uint16_t master_port, const uint16_t worker_port) {
    static_cast<option_t *>(handle)->scheduler(address, pub_port, master_port,
					       worker_port);
  }

  void master(void *handle, const char *address, const uint16_t worker_port) {
    static_cast<option_t *>(handle)->master(address, worker_port);
  }

  void delete_paramserver_options(void *handle) {
    delete static_cast<option_t *>(handle);
  }
}
