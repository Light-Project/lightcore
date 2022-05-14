/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KFIFO_H_
#define _KFIFO_H_

#include <kernel.h>

struct kfifo {
    unsigned long in;
    unsigned long out;
    unsigned long mask;
    unsigned long esize;
    void *data;
};

/**
 * GENERIC_KFIFO - define a generic kfifo structure.
 * @datatype: kfifo data type.
 * @ptrtype: kfifo pointer containing data.
 * @rsize: kfifo record size.
 */
#define GENERIC_KFIFO(datatype, ptrtype, rsize)     \
    union {                                         \
        struct kfifo kfifo;                         \
        datatype *data;                             \
        const datatype *cdata;                      \
        ptrtype *ptr;                               \
        const ptrtype *cptr;                        \
        char (*rectype)[rsize];                     \
    }

/**
 * BODY_KFIFO - generate the body of normal kfifo.
 * @type: kfifo contains the type of data.
 * @ptype: kfifo pointer containing data.
 * @size: kfifo buffer size.
 * @rsize: kfifo record size.
 */
#define BODY_KFIFO(type, ptype, size, rsize) {      \
    GENERIC_KFIFO(type, ptype, rsize);              \
    type buff[((size < 2) ||                        \
        (size & (size - 1))) ? -1 : size];          \
}

/**
 * BODY_KFIFO_DYNAMIC - generate the body of dynamic kfifo.
 * @type: kfifo contains the type of data.
 * @ptype: kfifo pointer containing data.
 * @rsize: kfifo record size.
 */
#define BODY_KFIFO_DYNAMIC(type, ptype, rsize) {    \
    GENERIC_KFIFO(type, ptype, rsize);              \
    type buff[0];                                   \
}

/**
 * KFIFO_INIT - initialize normal kfifo in compound literals.
 * @name: the name of kfifo to init.
 */
#define KFIFO_INIT(name) (typeof(name)) {           \
    .kfifo = {                                      \
        .in = 0, .out = 0,                          \
        .esize = sizeof(*(name).buff),              \
        .mask = ARRAY_SIZE((name).buff) - 1,        \
        .data = (name).buff,                        \
    },                                              \
}

/**
 * KFIFO_DYNAMIC_INIT - initialize dynamic kfifo in compound literals.
 * @name: the name of kfifo to init.
 */
#define KFIFO_DYNAMIC_INIT(name) (typeof(name)) {{  \
    .kfifo = {                                      \
        .in = 0, .out = 0, .mask = 0, .data = NULL, \
        .esize = sizeof(*(name).buff),              \
    },                                              \
}}

/**
 * STRUCT_KFIFO - generate a normal kfifo structure.
 * @type: kfifo contains the type of data.
 * @size: kfifo buffer size.
 */
#define STRUCT_KFIFO(type, size) \
    struct BODY_KFIFO(type, type, size, 0)

/**
 * STRUCT_KFIFO_RECORD - generate a record kfifo structure.
 * @type: kfifo contains the type of data.
 * @size: kfifo buffer size.
 * @record: kfifo record size.
 */
#define STRUCT_KFIFO_RECORD(type, size, record) \
    struct BODY_KFIFO(type, type, size, record)

/**
 * STRUCT_KFIFO_DYNAMIC - generate a dynamic kfifo structure.
 * @type: kfifo contains the type of data.
 */
#define STRUCT_KFIFO_DYNAMIC(type) \
    struct BODY_KFIFO_DYNAMIC(type, type, 0)

/**
 * STRUCT_KFIFO_DYNAMIC_RECORD - generate a dynamic record kfifo structure.
 * @type: kfifo contains the type of data.
 * @record: kfifo record size.
 */
#define STRUCT_KFIFO_DYNAMIC_RECORD(type, record) \
    struct BODY_KFIFO_DYNAMIC(type, type, record)

/**
 * DECLARE_KFIFO - declare a normal kfifo structure.
 * @name: name of kfifo structure to declare.
 * @type: kfifo contains the type of data.
 * @size: kfifo buffer size.
 */
#define DECLARE_KFIFO(name, type, size) \
    STRUCT_KFIFO(type, size) name

/**
 * DECLARE_KFIFO_RECORD - declare a record kfifo structure.
 * @name: name of kfifo structure to declare.
 * @type: kfifo contains the type of data.
 * @size: kfifo buffer size.
 * @record: kfifo record size.
 */
#define DECLARE_KFIFO_RECORD(name, type, size, record) \
    STRUCT_KFIFO_RECORD(type, size, record) name

/**
 * DECLARE_KFIFO_DYNAMIC - declare a dynamic kfifo structure.
 * @name: name of kfifo structure to declare.
 * @type: kfifo contains the type of data.
 */
#define DECLARE_KFIFO_DYNAMIC(name, type) \
    STRUCT_KFIFO_DYNAMIC(type) name

/**
 * DECLARE_KFIFO_DYNAMIC_RECORD - declare a dynamic record kfifo structure.
 * @name: name of kfifo structure to declare.
 * @type: kfifo contains the type of data.
 * @record: kfifo record size.
 */
#define DECLARE_KFIFO_DYNAMIC_RECORD(name, type, record) \
    STRUCT_KFIFO_DYNAMIC_RECORD(type, record) name

/**
 * DEFINE_KFIFO - define a normal kfifo structure.
 * @name: name of kfifo structure to declare.
 * @type: kfifo contains the type of data.
 * @size: kfifo buffer size.
 */
#define DEFINE_KFIFO(name, type, size) \
    DECLARE_KFIFO(name, type, size) = KFIFO_INIT(name)

/**
 * DEFINE_KFIFO - define a record kfifo structure.
 * @name: name of kfifo structure to declare.
 * @type: kfifo contains the type of data.
 * @size: kfifo buffer size.
 * @record: kfifo record size.
 */
#define DEFINE_KFIFO_RECORD(name, type, size, record) \
    DECLARE_KFIFO_RECORD(name, type, size, record) = KFIFO_INIT(name)

/**
 * DEFINE_KFIFO_DYNAMIC - define a dynamic kfifo structure.
 * @name: name of kfifo structure to declare.
 * @type: kfifo contains the type of data.
 */
#define DEFINE_KFIFO_DYNAMIC(name, type) \
    DECLARE_KFIFO_DYNAMIC(name, type) = KFIFO_DYNAMIC_INIT(name)

/**
 * DEFINE_KFIFO_DYNAMIC_RECORD - declare define dynamic record kfifo structure.
 * @name: name of kfifo structure to declare.
 * @type: kfifo contains the type of data.
 * @record: kfifo record size.
 */
#define DEFINE_KFIFO_DYNAMIC_RECORD(name, type, record) \
    DECLARE_KFIFO_DYNAMIC_RECORD(name, type, record) = KFIFO_DYNAMIC_INIT(name)

/**
 * kfifo_initialized - check if the fifo is initialized.
 * @fifo: pointer of the fifo to check.
 */
#define kfifo_initialized(fifo) ((fifo)->kfifo.mask)

/**
 * kfifo_recsize - get the size of the record length field.
 * @fifo: pointer of the fifo to get recsize.
 */
#define kfifo_recsize(fifo)	(sizeof(*(fifo)->rectype))

/**
 * kfifo_size - get the size of the element managed by the fifo.
 * @fifo: pointer of the fifo to get size.
 */
#define kfifo_size(fifo) ((fifo)->kfifo.mask + 1)

/**
 * kfifo_esize - get the size of the fifo in elements.
 * @fifo: pointer of the fifo to get size.
 */
#define kfifo_esize(fifo) ((fifo)->kfifo.esize)

/**
 * kfifo_reset - reset fifo state.
 * @fifo: the fifo to reset.
 */
#define kfifo_reset(fifo) do {                      \
    typeof((fifo) + 1) tmp = (fifo);                \
    tmp->kfifo.out = tmp->kfifo.in = 0;             \
} while (0)

/**
 * kfifo_homing - homing unread valid data length.
 * @fifo: the fifo to homing.
 */
#define kfifo_homing(fifo) do {                     \
    typeof((fifo) + 1) tmp = (fifo);                \
    tmp->kfifo.out = tmp->kfifo.in;                 \
} while (0)

/**
 * kfifo_len - get the valid data length in kfifo.
 * @fifo: the fifo to get.
 */
#define kfifo_len(fifo) ({                          \
    typeof((fifo) + 1) tlen = (fifo);               \
    tlen->kfifo.in - tlen->kfifo.out;               \
})

/**
 * kfifo_check_empty - check whether kfifo is empty.
 * @fifo: the fifo to check.
 */
#define kfifo_check_empty(fifo) ({                  \
    typeof((fifo) + 1) ttmp = (fifo);               \
    ttmp->kfifo.in == ttmp->kfifo.out;              \
})

/**
 * kfifo_check_full - check whether kfifo is full.
 * @fifo: the fifo to check.
 */
#define kfifo_check_full(fifo) ({                   \
    typeof((fifo) + 1) ttmp = (fifo);               \
    kfifo_len(ttmp) > ttmp->kfifo.mask;             \
})

/**
 * kfifo_check_dynamic - check whether kfifo is dynamic.
 * @fifo: the fifo to check.
 */
#define kfifo_check_dynamic(fifo) (                 \
    !sizeof(fifo->buff)                             \
)

/**
 * kfifo_alloc - dynamically allocate buffer to kfifo.
 * @fifo: the fifo to allocate buffer.
 * @size: size of buffer.
 * @flags: page flags.
 */
#define kfifo_alloc(fifo, size, flags) ({           \
    typeof((fifo) + 1) tmp = (fifo);                \
    kfifo_check_dynamic(tmp) ?                      \
    kfifo_dynamic_alloc(&tmp->kfifo,                \
    sizeof(*tmp->data), size, flags) :              \
    -EINVAL;                                        \
})

/**
 * kfifo_free - dynamically free buffer to kfifo.
 * @fifo: the fifo to free buffer.
 */
#define kfifo_free(fifo) ({                         \
    typeof((fifo) + 1) tmp = (fifo);                \
    kfifo_check_dynamic(tmp) ?                      \
    kfifo_dynamic_free(&tmp->kfifo) :               \
    -EINVAL;                                        \
})

/**
 * kfifo_get - get an object from kfifo.
 * @fifo: the kfifo to get object out.
 * @value: object to get.
 */
#define kfifo_get(fifo, value) ({                   \
    typeof((fifo) + 1) tmp = (fifo);                \
    typeof(tmp->ptr) tvalue = (value);              \
    struct kfifo *kfifo = &tmp->kfifo;              \
    unsigned long recsize = sizeof(*tmp->rectype);  \
    unsigned long retval;                           \
    if (recsize)                                    \
        retval = kfifo_out_record(kfifo,            \
                tvalue, sizeof(*tvalue), recsize);  \
    else {                                          \
        retval = !kfifo_check_empty(tmp);           \
        if (retval) {                               \
            *(typeof(tmp->data)) tvalue =           \
            (kfifo_check_dynamic(tmp) ?             \
            ((typeof(tmp->data)) kfifo->data) :     \
            (tmp->buff))                            \
            [kfifo->out & tmp->kfifo.mask];         \
            smp_wmb();                              \
            ++kfifo->out;                           \
        }                                           \
    }                                               \
    retval;                                         \
})

/**
 * kfifo_put - put an object into kfifo.
 * @fifo: the kfifo to put object in.
 * @value: object to put.
 */
#define kfifo_put(fifo, value) ({                   \
    typeof((fifo) + 1) tmp = (fifo);                \
    typeof(*tmp->cdata) tvalue = (value);           \
    struct kfifo *kfifo = &tmp->kfifo;              \
    unsigned long recsize = sizeof(*tmp->rectype);  \
    unsigned long retval;                           \
    if (recsize)                                    \
        retval = kfifo_in_record(kfifo,             \
                &tvalue, sizeof(tvalue), recsize);  \
    else {                                          \
        retval = !kfifo_check_full(tmp);            \
        if (retval) {                               \
            (kfifo_check_dynamic(tmp) ?             \
            ((typeof(tmp->data)) kfifo->data) :     \
            (tmp->buff))                            \
            [kfifo->in & tmp->kfifo.mask] =         \
            *(typeof(tmp->data)) &tvalue;           \
            ++kfifo->in;                            \
        }                                           \
    }                                               \
    retval;                                         \
})

/**
 * kfifo_out - copy continuous data from kfifo.
 * @fifo: the kfifo to copy data out.
 * @buff: the buffer to copy data in.
 * @len: number of continuously copied objects.
 */
#define kfifo_out(fifo, buff, len) ({               \
    typeof((fifo) + 1) tmp = (fifo);                \
    typeof(tmp->ptr) tbuff = (buff);                \
    struct kfifo *kfifo = &tmp->kfifo;              \
    unsigned long tlen = (len);                     \
    unsigned long recsize = sizeof(*tmp->rectype);  \
    (recsize) ?                                     \
    kfifo_out_record(kfifo, tbuff, tlen, recsize) : \
    kfifo_out_flat(kfifo, tbuff, tlen);             \
})

/**
 * kfifo_in - copy continuous data into kfifo.
 * @fifo: the kfifo to copy data in.
 * @buff: the buffer to copy data out.
 * @len: number of continuously copied objects.
 */
#define kfifo_in(fifo, buff, len) ({                \
    typeof((fifo) + 1) tmp = (fifo);                \
    typeof(tmp->cptr) tbuff = (buff);               \
    struct kfifo *kfifo = &tmp->kfifo;              \
    unsigned long tlen = (len);                     \
    unsigned long recsize = sizeof(*tmp->rectype);  \
    (recsize) ?                                     \
    kfifo_in_record(kfifo, tbuff, tlen, recsize) :  \
    kfifo_in_flat(kfifo, tbuff, tlen);              \
})

extern unsigned long kfifo_peek_flat(struct kfifo *kfifo, void *buff, unsigned long len);
extern unsigned long kfifo_out_flat(struct kfifo *kfifo, void *buff, unsigned long len);
extern unsigned long kfifo_in_flat(struct kfifo *kfifo, const void *buff, unsigned long len);
extern unsigned long kfifo_peek_record(struct kfifo *kfifo, void *buff, unsigned long len, unsigned long record);
extern unsigned long kfifo_out_record(struct kfifo *kfifo, void *buff, unsigned long len, unsigned long record);
extern unsigned long kfifo_in_record(struct kfifo *kfifo, const void *buff, unsigned long len, unsigned long record);
extern state kfifo_dynamic_alloc(struct kfifo *kfifo, size_t esize, size_t size, gfp_t flags);
extern void kfifo_dynamic_free(struct kfifo *kfifo);

#endif  /* _KFIFO_H_ */
