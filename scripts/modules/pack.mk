# SPDX-License-Identifier: GPL-2.0-or-later
# ==========================================================================
# pack tools
# ==========================================================================

# Cpio
# ---------------------------------------------------------------------------

quiet_cmd_cpio = $(ECHO_CPIO)  $@
      cmd_cpio = find $(real-prereqs) | $(CPIO) -o -H newc --quiet > $@

# Gzip
# ---------------------------------------------------------------------------

quiet_cmd_gzip = $(ECHO_GZIP)  $@
      cmd_gzip = cat $(real-prereqs) | $(KGZIP) -n -f -9 > $@

# Bzip2
# ---------------------------------------------------------------------------

# Bzip2 and LZMA do not include size in file... so we have to fake that;
# append the size as a 32-bit littleendian number as gzip does.
size_append = printf $(shell                                            \
    dec_size=0;                                                         \
    for F in $(real-prereqs); do                                        \
        fsize=$$($(CONFIG_SHELL) $(srctree)/scripts/file-size.sh $$F);  \
        dec_size=$$(expr $$dec_size + $$fsize);                         \
    done;                                                               \
    printf "%08x\n" $$dec_size |                                        \
    sed 's/\(..\)/\1 /g' | {                                            \
        read ch0 ch1 ch2 ch3;                                           \
        for ch in $$ch3 $$ch2 $$ch1 $$ch0; do                           \
            printf '%s%03o' '\\' $$((0x$$ch));                          \
        done;                                                           \
    }                                                                   \
)

quiet_cmd_bzip2 = $(ECHO_BZIP2)  $@
      cmd_bzip2 = { cat $(real-prereqs) | $(KBZIP2) -9; $(size_append); } > $@

# Lzma
# ---------------------------------------------------------------------------

quiet_cmd_lzma = $(ECHO_LZMA)  $@
      cmd_lzma = { cat $(real-prereqs) | $(LZMA) -9; $(size_append); } > $@

quiet_cmd_lzo = $(ECHO_LZO)  $@
      cmd_lzo = { cat $(real-prereqs) | $(KLZOP) -9; $(size_append); } > $@

quiet_cmd_lz4 = $(ECHO_LZ4)  $@
      cmd_lz4 = { cat $(real-prereqs) | $(LZ4) -l -c1 stdin stdout; $(size_append); } > $@

# XZ
# ---------------------------------------------------------------------------
# Use xzkern to compress the kernel image and xzmisc to compress other things.
#
# xzkern uses a big LZMA2 dictionary since it doesn't increase memory usage
# of the kernel decompressor. A BCJ filter is used if it is available for
# the target architecture. xzkern also appends uncompressed size of the data
# using size_append. The .xz format has the size information available at
# the end of the file too, but it's in more complex format and it's good to
# avoid changing the part of the boot code that reads the uncompressed size.
# Note that the bytes added by size_append will make the xz tool think that
# the file is corrupt. This is expected.
#
# xzmisc doesn't use size_append, so it can be used to create normal .xz
# files. xzmisc uses smaller LZMA2 dictionary than xzkern, because a very
# big dictionary would increase the memory usage too much in the multi-call
# decompression mode. A BCJ filter isn't used either.
quiet_cmd_xzkern = $(ECHO_XZKERN)  $@
      cmd_xzkern = { cat $(real-prereqs) | sh $(srctree)/scripts/xz_wrap.sh; $(size_append); } > $@

quiet_cmd_xzmisc = $(ECHO_XZMISC)  $@
      cmd_xzmisc = cat $(real-prereqs) | $(XZ) --check=crc32 --lzma2=dict=1MiB > $@

# ZSTD
# ---------------------------------------------------------------------------
# Appends the uncompressed size of the data using size_append. The .zst
# format has the size information available at the beginning of the file too,
# but it's in a more complex format and it's good to avoid changing the part
# of the boot code that reads the uncompressed size.
#
# Note that the bytes added by size_append will make the zstd tool think that
# the file is corrupt. This is expected.
#
# zstd uses a maximum window size of 8 MB. zstd22 uses a maximum window size of
# 128 MB. zstd22 is used for kernel compression because it is decompressed in a
# single pass, so zstd doesn't need to allocate a window buffer. When streaming
# decompression is used, like initramfs decompression, zstd22 should likely not
# be used because it would require zstd to allocate a 128 MB buffer.

quiet_cmd_zstd = $(ECHO_ZSTD)  $@
      cmd_zstd = { cat $(real-prereqs) | $(ZSTD) -19; $(size_append); } > $@

quiet_cmd_zstd22 = $(ECHO_ZSTD22)  $@
      cmd_zstd22 = { cat $(real-prereqs) | $(ZSTD) -22 --ultra; $(size_append); } > $@
