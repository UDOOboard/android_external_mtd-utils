/*
 * Copyright (C) 2008, 2009 Nokia Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Author: Artem Bityutskiy
 *
 * MTD library.
 */

#ifndef __LIBMTD_H__
#define __LIBMTD_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum MTD device name length */
#define MTD_NAME_MAX 127
/* Maximum MTD device type string length */
#define MTD_TYPE_MAX 64

/* MTD library descriptor */
typedef void * libmtd_t;

/**
 * @dev_count: count of MTD devices in system
 * @lowest_dev_num: lowest MTD device number
 * @highest_dev_num: highest MTD device number
 * @sysfs_supported: non-zero if sysfs is supported by MTD
 */
struct mtd_info
{
	int dev_count;
	int lowest_dev_num;
	int highest_dev_num;
	unsigned int sysfs_supported:1;
};

/**
 * struct mtd_dev_info - information about an MTD device.
 * @dev_num: MTD device number
 * @major: major number of corresponding character device
 * @minor: minor number of corresponding character device
 * @type: flash type (constants like %MTD_NANDFLASH defined in mtd-abi.h)
 * @type_str: static R/O flash type string
 * @name: device name
 * @size: device size in bytes
 * @eb_cnt: count of eraseblocks
 * @eb_size: eraseblock size
 * @min_io_size: minimum input/output unit size
 * @subpage_size: sub-page size
 * @oob_size: OOB size (zero if the device does not have OOB area)
 * @region_cnt: count of additional erase regions
 * @writable: zero if the device is read-only
 * @bb_allowed: non-zero if the MTD device may have bad eraseblocks
 */
struct mtd_dev_info
{
	int dev_num;
	int major;
	int minor;
	int type;
	const char type_str[MTD_TYPE_MAX + 1];
	const char name[MTD_NAME_MAX + 1];
	long long size;
	int eb_cnt;
	int eb_size;
	int min_io_size;
	int subpage_size;
	int oob_size;
	int region_cnt;
	unsigned int writable:1;
	unsigned int bb_allowed:1;
};

/**
 * libmtd_open - open MTD library.
 *
 * This function initializes and opens the MTD library and returns MTD library
 * descriptor in case of success and %NULL in case of failure. In case of
 * failure, errno contains zero if MTD is not present in the system, or
 * contains the error code if a real error happened.
 */
libmtd_t libmtd_open(void);

/**
 * libmtd_close - close MTD library.
 * @desc: MTD library descriptor
 */
void libmtd_close(libmtd_t desc);

/**
 * mtd_get_info - get general MTD information.
 * @desc: MTD library descriptor
 * @info: the MTD device information is returned here
 *
 * This function fills the passed @info object with general MTD information and
 * returns %0 in case of success and %-1 in case of failure. If MTD subsystem is
 * not present in the system, errno is set to @ENODEV.
 */
int mtd_get_info(libmtd_t desc, struct mtd_info *info);

/**
 * mtd_get_dev_info - get information about an MTD device.
 * @desc: MTD library descriptor
 * @node: name of the MTD device node
 * @mtd: the MTD device information is returned here
 *
 * This function gets information about MTD device defined by the @node device
 * node file and saves this information in the @mtd object. Returns %0 in case
 * of success and %-1 in case of failure. If MTD subsystem is not present in the
 * system, or the MTD device does not exist, errno is set to @ENODEV.
 */
int mtd_get_dev_info(libmtd_t desc, const char *node, struct mtd_dev_info *mtd);

/**
 * mtd_get_dev_info1 - get information about an MTD device.
 * @desc: MTD library descriptor
 * @dev_num: MTD device number to fetch information about
 * @mtd: the MTD device information is returned here
 *
 * This function is identical to 'mtd_get_dev_info()' except that it accepts
 * MTD device number, not MTD character device.
 */
int mtd_get_dev_info1(libmtd_t desc, int dev_num, struct mtd_dev_info *mtd);

/**
 * mtd_erase - erase an eraseblock.
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to erase
 *
 * This function erases eraseblock @eb of MTD device described by @fd. Returns
 * %0 in case of success and %-1 in case of failure.
 */
int mtd_erase(const struct mtd_dev_info *mtd, int fd, int eb);

/**
 * mtd_is_bad - check if eraseblock is bad.
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to check
 *
 * This function checks if eraseblock @eb is bad. Returns %0 if not, %1 if yes,
 * and %-1 in case of failure.
 */
int mtd_is_bad(const struct mtd_dev_info *mtd, int fd, int eb);

/**
 * mtd_mark_bad - marks the block as bad.
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to mark bad
 *
 * This function marks the eraseblock @eb as bad. Returns %0 if success
 * %-1 if failure
 */
int mtd_mark_bad(const struct mtd_dev_info *mtd, int fd, int eb);

/**
 * mtd_read - read data from an MTD device.
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to read from
 * @offs: offset withing the eraseblock to read from
 * @buf: buffer to read data to
 * @len: how many bytes to read
 *
 * This function reads @len bytes of data from eraseblock @eb and offset @offs
 * of the MTD device defined by @mtd and stores the read data at buffer @buf.
 * Returns %0 in case of success and %-1 in case of failure.
 */
int mtd_read(const struct mtd_dev_info *mtd, int fd, int eb, int offs,
	     void *buf, int len);

/**
 * mtd_write - write data to an MTD device.
 * @mtd: MTD device description object
 * @fd: MTD device node file descriptor
 * @eb: eraseblock to write to
 * @offs: offset withing the eraseblock to write to
 * @buf: buffer to write
 * @len: how many bytes to write
 *
 * This function writes @len bytes of data to eraseblock @eb and offset @offs
 * of the MTD device defined by @mtd. Returns %0 in case of success and %-1 in
 * case of failure.
 */
int mtd_write(const struct mtd_dev_info *mtd, int fd, int eb, int offs,
	      void *buf, int len);

/**
 * mtd_probe_node - test MTD node.
 * @desc: MTD library descriptor
 * @node: the node to test
 *
 * This function tests whether @node is an MTD device node and returns %1 if it
 * is, and %-1 if it is not (errno is ENODEV in this case) or if an error
 * occurred.
 */
int mtd_probe_node(libmtd_t desc, const char *node);

#ifdef __cplusplus
}
#endif

#endif /* __LIBMTD_H__ */
