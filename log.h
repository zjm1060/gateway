#ifndef __LOG_H
#define __LOG_H

#include "zlog.h"

extern zlog_category_t *zc;

#define log_start(cat) \
  do{\
    zc = zlog_get_category(cat);\
  }while(0)

#define log_info(...)  zlog_info(zc,__VA_ARGS__)
#define log_warn(...) zlog_warn(zc,__VA_ARGS__)
#define log_err(...)  zlog_error(zc,__VA_ARGS__)

#define log_dump(buf,len) hzlog_info(zc,buf,len)


#endif
