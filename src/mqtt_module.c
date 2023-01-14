/*
 * mqtt_client.c
 *
 *  Created on: Jan 14, 2023
 *      Author: vanaluk
 */

#include <mosquitto.h>
#include <sys/wait.h>
#include <errno.h>

#include "mqtt_module.h"

#define MQTT_MEASUREMENTS_QOS 0
static char client_id[] = "qimark-mqtt-test";

#define MQTT_MSG_MAX_LEN (4096)
#define MQTT_TOPIC_MAX_LEN 128

#define MQTT_IPC_WAIT_CONTROL (1)

typedef struct mqtt_client_msg_s
{
  char                topic_name[MQTT_TOPIC_MAX_LEN];
  char                msg[MQTT_MSG_MAX_LEN];
  unsigned int        len;
  int                 qos;
} mqtt_client_msg_t;

typedef struct mqtt_client_cfg_s
{
  char  broker_ip[MQTT_MSG_MAX_LEN];
  int   broker_port;
  int   keeapalive;
} mqtt_client_cfg_t;

typedef struct mqtt_client_module_context_s
{
  mqtt_client_cfg_t mqtt_config;
  bool              mqtt_pub_in_progress;
  bool              mqtt_disconnected;
  //bool              mqtt_reinitialised;
} mqtt_client_module_ctx_t;

static struct mosquitto *mosq;
static mqtt_client_module_ctx_t mqtt_client_ctx;

static ret_t mqtt_client_send_msg_to_mosquitto(void)
{
  ret_t ret = RET_OK;
  int   mqtt_ret = 0;
  char  topic_name[MQTT_TOPIC_MAX_LEN] = { 0 };

  TRACE(">mqtt_client_send_msg_to_mosquitto");

  do
  {
    mqtt_client_msg_t msg = {0};

    if (mqtt_client_ctx.mqtt_pub_in_progress)
    {
      TRACE("publish is in progress");
      break;
    }

    mqtt_client_ctx.mqtt_pub_in_progress = true;

    app_get_topic(topic_name, sizeof(topic_name));
    strcpy(msg.topic_name, topic_name);
    strcpy(msg.msg, "cpu=100");
    msg.len = strlen(msg.msg);
    msg.qos = MQTT_MEASUREMENTS_QOS;

    TRACE("---- mosquitto_publish");
    TRACE("---- msg->topic_name = %s", msg.topic_name);
    TRACE("---- msg->len = %d", msg.len);
    TRACE("---- msg->msg= %s", msg.msg);

    mqtt_ret = mosquitto_publish(mosq, NULL, msg.topic_name, msg.len, msg.msg, msg.qos, false);

    if (mqtt_ret == MOSQ_ERR_NO_CONN)
    {
      TRACE("Client has been disconnected from Mosquitto.");
      ret = RET_ERROR;
      break;
    }
    else if (ret != MOSQ_ERR_SUCCESS)
    {
      TRACE("Mosquitto pubish error occured, ret = %d", ret);
      ret = RET_ERROR;
      break;
    }
    ret = RET_OK;
  }
  while (0);

  TRACE("<mqtt_client_send_msg_to_mosquitto, ret = %d", ret);

  return ret;
}

static void mqtt_connect_callback(struct mosquitto *mosq, void *obj, int result)
{
  UNUSED(obj);
  TRACE("mosquitto connect callback, rc=%d", result);

  mqtt_client_ctx.mqtt_disconnected = false;
  mqtt_client_ctx.mqtt_pub_in_progress = false;
  //mqtt_client_ctx.mqtt_reinitialised = false;

  //if reconnect, we want to send accumulated messages
  //mqtt_client_send_msg_to_mosquitto();
}


static void mqtt_disconnect_callback(struct mosquitto *mosq, void *obj, int rc)
{
  UNUSED(mosq);
  UNUSED(obj);
  mqtt_client_ctx.mqtt_pub_in_progress = false;
  mqtt_client_ctx.mqtt_disconnected = true;

  if (rc != 0)
  {
    TRACE("mosquitto unexpected disconnect occured, rc = %d", rc);
  }

  TRACE("mosquitto try reconnect");
}

static void mqtt_message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{


  UNUSED(mosq);
  UNUSED(obj);
  TRACE("mosquitto message callback");
  //TRACE("recieve msg from broker: topic: %s , len %d, msg: %s", message->topic, message->payloadlen, message->payload);
}

static void mqtt_subscribe_callback(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos)
{
  UNUSED(mosq);
  UNUSED(obj);
  UNUSED(mid);
  UNUSED(qos_count);
  UNUSED(granted_qos);
  TRACE("mosquitto subscribe callback");
}

static void mqtt_unsubscribe_callback(struct mosquitto *mosq, void *obj, int mid)
{
  UNUSED(mosq);
  UNUSED(obj);
  UNUSED(mid);
  TRACE("mosquitto unsubscribe callback");
}


static void mqtt_publish_callback(struct mosquitto *mosq, void *obj, int mid)
{
  TRACE(">mosquitto publish callback");
  UNUSED(mosq);
  UNUSED(obj);
  UNUSED(mid);

  if (!mqtt_client_ctx.mqtt_pub_in_progress)
  {
    TRACE("mqtt_publish_callback called when mqtt_pub_in_progress is not set!");
  }

  mqtt_client_ctx.mqtt_pub_in_progress = false;

  TRACE("<mosquitto publish callback");
}


static void mqtt_log_callback(struct mosquitto *mosq, void *obj, int level, const char *str)
{
  UNUSED(mosq);
  UNUSED(obj);

  switch(level)
  {
    case MOSQ_LOG_DEBUG:
      TRACE("MOSQ_LOG_DEBUG: %s", str);
    break;

    case MOSQ_LOG_INFO:
      TRACE("MOSQ_LOG_INFO: %s", str);
    break;

    case MOSQ_LOG_NOTICE:
      TRACE("MOSQ_LOG_NOTICE: %s", str);
    break;

    case MOSQ_LOG_WARNING:
      TRACE("MOSQ_LOG_WARNING: %s", str);
    break;

    case MOSQ_LOG_ERR:
      TRACE("MOSQ_LOG_ERR: %s", str);
    break;

    default:
      TRACE("Unknown MOSQ loglevel!");
  }
}

static void mqtt_init_config(mqtt_client_cfg_t *cfg)
{
  app_get_server(cfg->broker_ip, sizeof(cfg->broker_ip));
  cfg->broker_port = 1883;
  cfg->keeapalive  = 60;
}

static int mqtt_client_mosquitto_connect(struct mosquitto **mosq_ctx)
{
  ret_t ret;

  TRACE(">mqtt_client_mosquitto_connect");

  if (!(*mosq_ctx))
  {
    *mosq_ctx = mosquitto_new(client_id, true, 0);
    ret = MOSQ_ERR_SUCCESS;
  }
  else
  {
    ret = mosquitto_reinitialise(*mosq_ctx, client_id, true, 0);
  }

  if (!(*mosq_ctx) || ret != MOSQ_ERR_SUCCESS)
  {
      TRACE("failed to init mosquitto object");
      ret = RET_ERROR;
      return ret;
  }

  mosquitto_message_callback_set(*mosq_ctx, mqtt_message_callback);
  mosquitto_connect_callback_set(*mosq_ctx, mqtt_connect_callback);
  mosquitto_disconnect_callback_set(*mosq_ctx, mqtt_disconnect_callback);
  mosquitto_subscribe_callback_set(*mosq_ctx, mqtt_subscribe_callback);
  mosquitto_unsubscribe_callback_set(*mosq_ctx, mqtt_unsubscribe_callback);
  mosquitto_publish_callback_set(*mosq_ctx, mqtt_publish_callback);
  mosquitto_log_callback_set(*mosq_ctx, mqtt_log_callback);

  ret = mosquitto_connect(*mosq_ctx, mqtt_client_ctx.mqtt_config.broker_ip, mqtt_client_ctx.mqtt_config.broker_port, mqtt_client_ctx.mqtt_config.keeapalive);

  if (ret != MOSQ_ERR_SUCCESS)
  {
    TRACE("failed to mosquitto connect %d", ret);
    ret = RET_ERROR;
  }

  TRACE("<mqtt_client_mosquitto_connect %d", ret);

  return ret;
}

static ret_t mqtt_client_init()
{
  ret_t ret;

  TRACE(">mqtt_client_init");

  bzero(&mqtt_client_ctx, sizeof(mqtt_client_ctx));

  do
  {
    /* Init mosquitto client */
    mqtt_client_ctx.mqtt_pub_in_progress = false;
    mqtt_client_ctx.mqtt_disconnected = true;
    //mqtt_client_ctx.mqtt_reinitialised = false;

    mqtt_init_config(&mqtt_client_ctx.mqtt_config);

    mosquitto_lib_init();
    mosq = NULL;

    ret = mqtt_client_mosquitto_connect(&mosq);
  }
  while (0);

  TRACE("<mqtt_client_init %d", ret);

  return ret;
}

static void mqtt_client_deinit(void)
{
  if (mosq)
  {
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
  }

  mosquitto_lib_cleanup();
  bzero(&mqtt_client_ctx, sizeof(mqtt_client_ctx));
}

ret_t mqtt_client_reinit(void)
{
  ret_t ret;

  TRACE(">mqtt_client_reinit");

  mqtt_client_deinit();
  ret = mqtt_client_init();

  TRACE("<mqtt_client_reinit ret %d", ret);

  return ret;
}

static void mqtt_client_loop()
{
  ret_t         ret;

  TRACE(">mqtt_client_loop");

  while(mqtt_is_running())
  {
    /* Check connection to mqtt broker */
    if (mqtt_client_ctx.mqtt_disconnected)
    {
      TRACE("Mosquitto try reconnect");

      ret = mosquitto_reconnect(mosq);

      if (ret == MOSQ_ERR_SUCCESS)
      {
        TRACE("Mosquitto reconnect successful");
        mqtt_client_ctx.mqtt_disconnected = false;
      }
      else
      {
        TRACE("Mosquitto reconnect failed %d", ret);
        ret = RET_ERROR;
        break;
#if 0
        if (!mqtt_client_ctx.mqtt_reinitialised)
        {
          TRACE("reinitialise the connection");
          /* Try to reinitialise connection */
          ret = mqtt_client_mosquitto_connect(&mosq);
          if (ret != RET_OK)
          {
            TRACE("still failed to connect, wait...");
          }
          mqtt_client_ctx.mqtt_reinitialised = true;
        }
#endif
      }
    }

    if (app_get_mqtt_reinit())
    {
      app_set_mqtt_reinit(false);
      ret = mqtt_client_reinit();

      if (ret != RET_OK)
      {
        TRACE("Mosquitto reinit failed");
        break;
      }
    }

    //usleep(1000); // better use cond_wait or IPC
    mqtt_client_send_msg_to_mosquitto();
    sleep(10);
  }

  TRACE("<mqtt_client_loop");
}


void* mqtt_module_main(void *arg)
{
  TRACE("> mqtt_module_main");

  mqtt_set_running();

  do
  {
    if (mqtt_client_init() != RET_OK)
    {
      TRACE("sgw mqtt_client init failed");
      break;
    }

    mqtt_client_loop();
  }
  while(0);

  mqtt_client_deinit();

  app_stop();

  TRACE("< mqtt_module_main");

  return 0;
}


