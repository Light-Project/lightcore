#ifndef _SYS_UIO_H_
#define _SYS_UIO_H_

/* Structure for scatter/gather I/O.  */
struct iovec {
    void *iov_base;	/* Pointer to data.  */
    size_t iov_len;	/* Length of data.  */
}; 

#endif
