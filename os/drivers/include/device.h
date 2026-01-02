#ifndef __DEVICE_H__
#define __DEVICE_H__

struct device {
  void *devptr;
  volatile void *data;
  void *config;
};

#endif/*__DEVICE_H__*/
