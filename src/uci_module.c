
#include <uci.h>

#include "uci_module.h"

static const char *delimiter = " ";

static ret_t uci_print_value(struct uci_option *option)
{
  ret_t ret = RET_OK;
  bool sep = false;
  struct uci_element *element;

  TRACE(">> uci_print_value option %p", option);

  do
  {
    if (!option)
    {
      TRACE("error: null param option %p\n", option);
      ret = RET_ERROR;
      break;
    }

    switch(option->type)
    {
    case UCI_TYPE_STRING:
      TRACE("%s = %s\n", option, option->v.string);
      break;
    case UCI_TYPE_LIST:
      uci_foreach_element(&option->v.list, element)
      {
        println("%s%s", (sep ? delimiter : ""), element->name);
        sep = true;
      }
      break;
    default:
      println("<unknown>\n");
      break;
    }
  }
  while(0);

  TRACE("<< uci_print_value ret %d", ret);

  return ret;
}

ret_t uci_show_value(const char *uci_path, const char *entry_name)
{
  ret_t ret = RET_OK;
  struct uci_context *uci_ctx;
  struct uci_ptr ptr;
  char uci_section_name[MAX_PATH];
  int print_size = 0;

  TRACE(">> uci_show_value uci_path %p option %p", uci_path, entry_name);

  do
  {
    if (!entry_name)
    {
      TRACE("error: null param path %p\n", entry_name);
      ret = RET_EMPTY;
      break;
    }

    print_size = snprintf(uci_section_name, sizeof(uci_section_name), "%s.%s", uci_path, entry_name);

    if (print_size < 0)
    {
      ret = RET_ERROR;
      break;
    }

    uci_ctx = uci_alloc_context();

    if(uci_ctx == NULL)
    {
      TRACE("error: cant alloc memory for uci ctx\n");
      ret = RET_EMPTY;
      break;
    }

    if (uci_lookup_ptr(uci_ctx, &ptr, uci_section_name, true) != UCI_OK)
    {
      uci_perror(uci_ctx, "get_config_entry Error");
      ret = RET_ERROR;
      break;
    }

    uci_print_value(ptr.o);
  }
  while(0);

  if (ret != RET_EMPTY)
  {
    uci_free_context(uci_ctx);
  }

  TRACE("<< uci_show_value ret %d", ret);

  return 0;
}

ret_t uci_set_value(const char *uci_path, const char *option, const char *value)
{
  ret_t ret = RET_OK;
  struct uci_context* uci_ctx;
  struct uci_ptr config;

  TRACE(">> uci_set_value option %p value %p", option, value);

  do
  {
    if (!option || !value)
    {
      TRACE("error: null param option %p value %p\n", option, value);
      ret = RET_EMPTY;
      break;
    }

    uci_ctx = uci_alloc_context();

    if (!uci_ctx)
    {
      TRACE("error: cant alloc memory for uci ctx\n");
      ret = RET_EMPTY;
      break;
    }

    if (uci_lookup_ptr(uci_ctx, &config, uci_path, true) != UCI_OK || !config.s)
    {
      uci_perror(uci_ctx, "get_config_entry Error");
      ret = RET_ERROR;
      break;
    }

    config.option = option;
    config.value = value;

    if (uci_set(uci_ctx, &config) != UCI_OK)
    {
      uci_perror(uci_ctx, "failed to set new option");
      ret = RET_ERROR;
      break;
    }

    if (uci_commit(uci_ctx, &config.p, false) != UCI_OK)
    {
      uci_perror(uci_ctx, "failed to commit changes");
      ret = RET_ERROR;
      break;
    }

  }
  while(0);

  if (ret != RET_EMPTY)
  {
    uci_free_context(uci_ctx);
  }

  TRACE("<< uci_set_value ret %d", ret);

  return 0;
}
