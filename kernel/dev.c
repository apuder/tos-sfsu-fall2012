#include <kernel.h>


unsigned int num_devs = 0;
struct dev *devtab[MAX_DEVS];


int dev_receive(dev_t devno, struct pbuf *p) {
  struct dev *dev;

  if (devno < 0 || devno >= num_devs) return -ENODEV;
  dev = devtab[devno];
  if (!dev->receive) return -ENOSYS;
  dev->reads++;
  dev->input += p->tot_len;

  return dev->receive(dev->netif, p);
}
