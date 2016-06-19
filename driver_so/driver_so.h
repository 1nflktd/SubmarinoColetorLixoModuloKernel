#ifndef _DRIVER_SO_H_
#define _DRIVER_SO_H_

#define DEVICE 50
#define DEVICE_NAME "so"
#define BUF_MSG 80

#define SUCCESS	0
#define FAILURE	-1

#define IOC_MAGIC  's'

#define IOCTL_SET_POSITION _IOR(IOC_MAGIC, 0, char *)
#define IOCTL_GET_POSITION _IOR(IOC_MAGIC, 1, char *)

#define POSITION_RIGHT 0
#define POSITION_UP 1
#define POSITION_DOWN 2
#define POSITION_LEFT 3
#define POSITION_STOP 4

#endif	
