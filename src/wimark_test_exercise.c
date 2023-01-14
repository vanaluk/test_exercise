/*
 ============================================================================
 Name        : wimark_test_exercise.c
 Author      : vanaluk
 Version     :
 Copyright   : Your copyright notice
 Description : Wimark test exercise
 ============================================================================
 */

#include "app_module.h"
#include "ubus_module.h"

#define UBUS_MODULE_TEST 0
#define UCI_MODULE_TEST  0

#ifdef UCI_MODULE_TEST
#include "uci_module.h"
#endif

const bool global_trace_enable = true;

enum
{
  THREAD_UBUS = 0,
  THREAD_COUNT
};

static pthread_t tid[THREAD_COUNT];

int main(int argc, char **argv)
{
  ret_t ret = RET_OK;
  int thread_error = 0;

  do
  {
    ret = app_init(UCI_CONFIG_FILE, UCI_CONFIG_SECTION);

    if (ret != RET_OK)
    {
      println("error: can't init app ret %d", ret);
      break;
    }

#ifdef UCI_MODULE_TEST
    {
      char uci_param[MAX_PATH] = { 0 };

      ret = uci_get_value(UCI_PROP_SERVER, uci_param, sizeof(uci_param));

      if (ret != RET_OK)
      {
        TRACE("warning: server config is not set");
      }

      ret = uci_get_value(UCI_PROP_TOPIC, uci_param, sizeof(uci_param));

      if (ret != RET_OK)
      {
        TRACE("warning: topic config is not set");
      }

      ret = uci_get_value(UCI_PROP_PERIOD, uci_param, sizeof(uci_param));

      if (ret != RET_OK)
      {
        TRACE("warning: period config is not set");
      }

      ret = uci_get_value(UCI_PROP_ENABLED, uci_param, sizeof(uci_param));

      if (ret != RET_OK)
      {
        TRACE("warning: enabled config is not set");
      }

      ret = uci_set_value(UCI_PROP_PERIOD, "60");

      if (ret != RET_OK)
      {
        TRACE("warning: enabled config is not set");
      }
    }
#endif

/*
#ifdef UBUS_MODULE_TEST
    ubus_module_main();
#endif
*/
    thread_error = pthread_create(&(tid[THREAD_UBUS]),
                                  NULL,
                                  &ubus_module_main, NULL);

    if (thread_error != 0)
    {
      TRACE("Thread can't be created :[%s]", strerror(thread_error));
      ret = RET_ERROR;
      break;
    }

    ret = app_main(NULL);

    if (ret != RET_OK)
    {
      TRACE("warning: app finished ret %d", ret);
    }

    pthread_cancel(tid[THREAD_UBUS]);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  }
  while(0);

  app_deinit();

  return 0;

}
