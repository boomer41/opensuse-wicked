/*
 * Routines for handling VLAN devices.
 *
 * Copyright (C) 2009-2010 Olaf Kirch <okir@suse.de>
 */
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <net/if_arp.h>
#include <arpa/inet.h>

#include <wicked/vlan.h>
#include "netinfo_priv.h"

/*
 * Create a new VLAN device
 */
ni_vlan_t *
__ni_vlan_new(void)
{
	ni_vlan_t *vlan;

	vlan = calloc(1, sizeof(ni_vlan_t));
	return vlan;
}

/*
 * Clone a device's VLAN configuration
 */
ni_vlan_t *
ni_vlan_clone(const ni_vlan_t *src)
{
	ni_vlan_t *dst;

	dst = __ni_vlan_new();
	if (!dst)
		return NULL;

	ni_string_dup(&dst->physdev_name, src->physdev_name);
	dst->physdev_index = src->physdev_index;
	dst->tag = src->tag;
	if (src->interface_dev)
		dst->interface_dev = ni_interface_get(src->interface_dev);

	return dst;
}

static inline void
__ni_vlan_unbind(ni_vlan_t *vlan)
{
	if (vlan->interface_dev)
		ni_interface_put(vlan->interface_dev);
	vlan->interface_dev = NULL;
}

/*
 * Given an interface index, locate the the base interface
 */
int
ni_vlan_bind_ifindex(ni_vlan_t *vlan, ni_netconfig_t *nc)
{
	ni_interface_t *real_dev;

	if (!vlan)
		return -1;

	real_dev = ni_interface_by_index(nc, vlan->physdev_index);
	if (real_dev == NULL)
		return -1;

	ni_string_dup(&vlan->physdev_name, real_dev->name);
	vlan->interface_dev = ni_interface_get(real_dev);
	return 0;
}

void
__ni_vlan_destroy(ni_vlan_t *vlan)
{
	__ni_vlan_unbind(vlan);
	ni_string_free(&vlan->physdev_name);
	vlan->physdev_index = 0;
}

void
ni_vlan_free(ni_vlan_t *vlan)
{
	__ni_vlan_destroy(vlan);
	free(vlan);
}


