#ifndef _LIGHTCORE_PCI_H_
#define _LIGHTCORE_PCI_H_

/*
 *	7:3 = slot
 *	2:0 = function
 */
#define PCI_DEVFN(dev, func)    ((((dev) & 0x1f) << 3) | ((func) & 0x07))
#define PCI_SLOT(devfn)         (((devfn) >> 3) & 0x1f)
#define PCI_FUNC(devfn)         ((devfn) & 0x07)


#endif
