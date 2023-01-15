/*
 * stat_module.c
 *
 *  Created on: Jan 15, 2023
 *      Author: vanaluk
 */


#include "stat_module.h"

struct cpustat {
    unsigned long t_user;
    unsigned long t_nice;
    unsigned long t_system;
    unsigned long t_idle;
    unsigned long t_iowait;
    unsigned long t_irq;
    unsigned long t_softirq;
};

static void skip_lines(FILE *fp, int numlines)
{
  int cnt = 0;
  char ch;
  while((cnt < numlines) && ((ch = getc(fp)) != EOF))
  {
      if (ch == '\n')
          cnt++;
  }
  return;
}

static ret_t get_stats(struct cpustat *st, int cpunum)
{
  ret_t ret = RET_OK;
  FILE *fp = fopen("/proc/stat", "r");
  char cpun[255];
  int lskip;

  do
  {
    if (!fp)
    {
      ret = RET_ERROR;
      break;
    }

    lskip = cpunum+1;
    skip_lines(fp, lskip);

    fscanf(fp, "%s %ld %ld %ld %ld %ld %ld %ld", cpun, &(st->t_user), &(st->t_nice),
        &(st->t_system), &(st->t_idle), &(st->t_iowait), &(st->t_irq),
        &(st->t_softirq));
    fclose(fp);
  }
  while(0);

  return ret;
}

static double calculate_load(struct cpustat *prev, struct cpustat *cur)
{
  int idle_prev = (prev->t_idle) + (prev->t_iowait);
  int idle_cur = (cur->t_idle) + (cur->t_iowait);

  int nidle_prev = (prev->t_user) + (prev->t_nice) + (prev->t_system) + (prev->t_irq) + (prev->t_softirq);
  int nidle_cur = (cur->t_user) + (cur->t_nice) + (cur->t_system) + (cur->t_irq) + (cur->t_softirq);

  int total_prev = idle_prev + nidle_prev;
  int total_cur = idle_cur + nidle_cur;

  double totald = (double) total_cur - (double) total_prev;
  double idled = (double) idle_cur - (double) idle_prev;

  double cpu_perc = (1000 * (totald - idled) / totald + 1) / 10;

  return cpu_perc;
}

ret_t stat_get_cpu_load(char *out_str, int leng)
{
  ret_t ret = RET_OK;
  struct cpustat st0_0, st0_1;
  int copied = 0;

  do
  {
    ret = get_stats(&st0_0, -1);

    if (ret != RET_OK)
    {
      TRACE("error: cant get_stats st0_0");
      break;
    }

    sleep(1);

    ret = get_stats(&st0_1, -1);

    if (ret != RET_OK)
    {
      TRACE("error: cant get_stats st0_1");
      break;
    }

    copied = snprintf(out_str, leng, "CPU: %lf%%", calculate_load(&st0_0, &st0_1));

    if (copied < 0)
    {
      TRACE("error: cant get cpu load");
      ret = RET_ERROR;
      break;
    }
  }
  while (0);

  return ret;
}
