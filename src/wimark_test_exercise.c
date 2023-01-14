/*
 ============================================================================
 Name        : wimark_test_exercise.c
 Author      : vanaluk
 Version     :
 Copyright   : Your copyright notice
 Description : Wimark test exercise
 ============================================================================
 */

#include "global.h"
#include "uci_module.h"

static char config_file[MAX_FILENAME] = "wm2022";
static char config_name[MAX_FILENAME] = "wimark_test_exercise";

typedef struct app_context_s
{
  char uci_path[MAX_PATH];
} app_context_t;

bool global_trace_enable = true;
static app_context_t app_ctx = {0};

static ret_t app_init()
{
  ret_t ret = RET_OK;
  int print_size = 0;

  TRACE(">> app_init");

  do
  {
    print_size = snprintf(app_ctx.uci_path, sizeof(app_ctx.uci_path), "%s.%s", config_file, config_name);

    if (print_size < 0)
    {
      ret = RET_ERROR;
      break;
    }

    println("uci prop path <%s>", app_ctx.uci_path);
  }
  while(0);

  TRACE("<< app_init ret %d", ret);

  return ret;
}


int main()
{
  ret_t ret = RET_OK;

  do
  {
    ret = app_init();

    if (ret != RET_OK)
    {
      println("error: can't init app ret %d", ret);
      break;
    }

    ret = uci_show_value(app_ctx.uci_path, "period");

    if (ret != RET_OK)
    {
      println("error: can't show_config_entry %d", ret);
      break;
    }

    ret = uci_set_value(app_ctx.uci_path, "server", "my.mqtt.com");

    if (ret != RET_OK)
    {
      println("error: can't uci_set_value %d", ret);
      break;
    }

    ret = uci_set_value(app_ctx.uci_path, "topic", "/my/secret/topic");

    if (ret != RET_OK)
    {
      println("error: can't uci_set_value %d", ret);
      break;
    }

    ret = uci_set_value(app_ctx.uci_path, "enabled", "1");

    if (ret != RET_OK)
    {
      println("error: can't uci_set_value %d", ret);
      break;
    }
  }
  while(0);

  return 0;

}
