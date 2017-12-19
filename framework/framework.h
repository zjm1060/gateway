/*
 * framework.h
 *
 *  Created on: 2014Äê5ÔÂ19ÈÕ
 *      Author: zjm
 */

#ifndef FRAMEWORK_H_
#define FRAMEWORK_H_

enum{
	ERR_OK = 0,
	ERR_FIAL = -1,
	ERR_BUSY = -2,
	ERR_MEMORY = -3,
	ERR_CLOSED = -4,
	ERR_IO = -5,
	ERR_TIMEOUT = -6,
	ERR_FORWARD = -7,
	ERR_NODATA = -8,
};

#define lambda(return_type, function_body) \
({ \
      return_type $this function_body \
	  	  $this; \
})


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "includes.h"

#include "sig.h"
#include "Req.h"
#include "Thread.h"
#include "Timer.h"
#include "list.h"
#include "menu/gpio.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#endif /* FRAMEWORK_H_ */
