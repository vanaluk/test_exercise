/*
 * global_module.c
 *
 *  Created on: Jan 14, 2023
 *      Author: vanaluk
 */

#include "app_module.h"
#include "uci_module.h"

app_context_t app_ctx = {0};

void app_global_lock(void)
{
  pthread_mutex_lock(&app_ctx.lock);
}

void app_global_unlock(void)
{
  pthread_mutex_unlock(&app_ctx.lock);
}

ret_t app_init(char *uci_config_file, char *uci_config_section)
{
  ret_t ret = RET_OK;
  int print_size = 0;

  TRACE(">> app_init");

  bzero(&app_ctx, sizeof(app_ctx));

  do
  {
    if (pthread_mutex_init(&app_ctx.lock, NULL) != 0)
    {
        TRACE("error: mutex init has failed");
        ret = RET_ERROR;
        break;
    }

    print_size = snprintf(app_ctx.uci_config_path, sizeof(app_ctx.uci_config_path), "%s.%s", uci_config_file, uci_config_section);

    if (print_size < 0)
    {
      ret = RET_ERROR;
      break;
    }

    println("info: app uci path <%s>", app_ctx.uci_config_path);

    {
      char uci_param[MAX_PATH] = {0};

      ret = uci_get_value(UCI_PROP_SERVER, app_ctx.server, sizeof(app_ctx.server));

      if (ret != RET_OK)
      {
        TRACE("warning: server config is not set");
      }

      ret = uci_get_value(UCI_PROP_TOPIC, app_ctx.topic, sizeof(app_ctx.topic));

      if (ret != RET_OK)
      {
        TRACE("warning: topic config is not set");
      }

      ret = uci_get_value(UCI_PROP_PERIOD, uci_param, sizeof(uci_param));

      if (strlen(uci_param) > 0 && ret == RET_OK)
      {
        app_ctx.period = atoi(uci_param);
      }
      else
      {
        TRACE("warning: period config is not set");
        app_ctx.period = 0;
      }
      app_ctx.last_period = app_ctx.period;

      ret = uci_get_value(UCI_PROP_ENABLED, uci_param, sizeof(uci_param));

      if (strlen(uci_param) > 0 && ret == RET_OK)
      {
        app_ctx.enabled = (atoi(uci_param) > 0) ? true : false;
      }
      else
      {
        TRACE("warning: enabled config is set to false by default");
        app_ctx.enabled = false;
      }
    }
  }
  while(0);

  TRACE("<< app_init ret %d", ret);

  return ret;
}

void app_deinit(void)
{
  pthread_mutex_destroy(&app_ctx.lock);
  bzero(&app_ctx, sizeof(app_ctx));
}

ret_t app_set_period(int period)
{
  ret_t ret = RET_OK;

  app_global_lock();

  app_ctx.period = period;

  app_global_unlock();

  return ret;
}

int app_get_period(void)
{
  int period;

  app_global_lock();

  period = app_ctx.period;

  app_global_unlock();

  return period;
}

ret_t app_main(void* arg)
{
  ret_t ret = RET_OK;

  TRACE("> app_main");

  app_ctx.app_thread_running = true;

  while (app_is_running())
  {
    int safe_period = app_get_period();

    if (app_ctx.last_period != safe_period)
    {
      char str_period[256];
      int result = 0;

      result = sprintf(str_period,"%d", safe_period);

      if (result < 0)
      {
        ret = RET_ERROR;
        app_ctx.app_thread_running = false;
        TRACE("error: sprintf fail");
        break;
      }

      ret = uci_set_value(UCI_PROP_PERIOD, str_period);

      if (ret != RET_OK)
      {
        ret = RET_ERROR;
        app_ctx.app_thread_running = false;
        TRACE("error: uci set new period fail");
        break;
      }

      app_ctx.last_period = safe_period;

      // todo: restart mqqt
    }

    // todo: mqtt work
  }

  TRACE("< app_main ret %d", ret);

  return ret;
}

bool app_is_running(void)
{
  bool running = false;

  app_global_lock();

  running = app_ctx.app_thread_running;

  app_global_unlock();

  return running;
}

void app_stop(void)
{
  app_global_lock();

  app_ctx.app_thread_running = false;

  app_global_unlock();
}
