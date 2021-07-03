#ifndef _DIV64_H_
#define _DIV64_H_

# define do_div(n,base) ({              \
    uint32_t __base = (base);           \
    uint32_t __rem;                     \
    __rem = ((uint64_t)(n)) % __base;   \
    (n) = ((uint64_t)(n)) / __base;     \
    __rem;                              \
 })

#endif /* _DIV64_H_ */
