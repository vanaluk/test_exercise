/*
 * ubus_module.c
 *
 *  Created on: Jan 14, 2023
 *      Author: vanaluk
 */


#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#include <libubox/uloop.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>

#include "ubus_module.h"

enum
{
    ADD_VALUE,
    __ADD_MAX
};

static struct ubus_context *ctx;
static struct blob_buf b;

static int
ubus_status_handler(struct ubus_context *ctx, struct ubus_object *obj,
        struct ubus_request_data *req, const char *method,
        struct blob_attr *msg)
{
  int period = 0;

  TRACE(">> ubus_status_handler ctx %p", ctx);

  period = app_get_period();

  blob_buf_init(&b, 0);
  blobmsg_add_string(&b, "result", "ok");
  blobmsg_add_u16(&b, "period", period);

  ubus_send_reply(ctx, req, b.head);

  TRACE("<< ubus_status_handler");

  return 0;
}

static const struct blobmsg_policy ubus_add_policy[__ADD_MAX] = {
  [ADD_VALUE] = { .name = "period", .type = BLOBMSG_TYPE_STRING },
};

static int ubus_add_handler(struct ubus_context *ctx, struct ubus_object *obj,
        struct ubus_request_data *req, const char *method,
        struct blob_attr *msg)
{
  struct blob_attr *tb[__ADD_MAX];
  char *str_param;
  int period = 0;

  TRACE(">> ubus_add_handler ctx %p", ctx);

  blobmsg_parse(ubus_add_policy, __ADD_MAX, tb, blob_data(msg), blob_len(msg));

  if (!tb[ADD_VALUE])
  {
    TRACE("ubus_add_handler: ubus_add_policy discard parameter");
    return UBUS_STATUS_INVALID_ARGUMENT;
  }

  str_param = blobmsg_get_string(tb[ADD_VALUE]);
  TRACE("ubus_add_handler: get param value %s", str_param);

  period = atoi( str_param );

  if (period > 0)
  {
    ret_t ret = RET_OK;

    ret = app_set_period(period);

    if (ret != RET_OK)
    {
      TRACE("ubus_add_handler: can't set period");
      return UBUS_STATUS_INVALID_ARGUMENT;
    }
  }
  else
  {
    TRACE("ubus_add_handler: parameter value wrong");
    return UBUS_STATUS_INVALID_ARGUMENT;
  }

  blob_buf_init(&b, 0);
  blobmsg_add_string(&b, "result", "ok");
  blobmsg_add_u32(&b, "period", period);
  ubus_send_reply(ctx, req, b.head);

  TRACE("<< ubus_add_handler");

  return 0;
}

static const struct ubus_method methods[] = {
  { .name = "status" , .handler = ubus_status_handler } ,
  UBUS_METHOD("add", ubus_add_handler, ubus_add_policy),
};

static struct ubus_object_type wimark_object_type = UBUS_OBJECT_TYPE("wimark-ubus", methods);

static struct ubus_object wimark_object = {
  .name = "wimark-ubus",
  .type = &wimark_object_type ,
  .methods = methods,
  .n_methods = ARRAY_SIZE(methods),
};


void* ubus_module_main(void *arg)
{
  const char *ubus_socket = NULL;
  int ret = RET_OK;

  TRACE("> ubus_module_main");

  uloop_init();
  signal(SIGPIPE, SIG_IGN);

  do
  {
    ctx = ubus_connect(ubus_socket);

    if (!ctx)
    {
        TRACE("error: failed to connect to ubus\n");
        ret = RET_ERROR;
        break;
    }

    ubus_add_uloop(ctx);

    ret = ubus_add_object(ctx, &wimark_object);

    if (ret)
    {
        TRACE("error: failed to add object: %s\n", ubus_strerror(ret));
        ret = RET_ERROR;
        break;
    }

    uloop_run();

    ubus_free(ctx);
    uloop_done();
  }
  while (0);

  app_stop();

  TRACE("< ubus_module_main ret %d", ret);

  return 0;
}
