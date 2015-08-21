/* Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <odp.h>
#include <stdio.h>
#include <stdlib.h>

#include "api/ofp_utils.h"
#include "ofpi_timer.h"

#define L2_HEADER_NO_VLAN_SIZE 14

#define KASSERT(x, y)	do { if (!(x)) { printf("KASSERT %s:%d\n",__FILE__,__LINE__); \
			printf y ; printf("\n"); int *a = 0; *a = 3;}} while (0)

extern int ofp_first_log_time;
#define TICS_PER_SEC (1000000/OFP_TIMER_RESOLUTION_US)

#define OFP_LOG_TIME(a...)							\
	do {    int now = ofp_timer_ticks(0);				\
		if (ofp_first_log_time == 0) ofp_first_log_time = now; \
		int diff = now - ofp_first_log_time;			\
		printf("[%d] %3d.%02d %5d:%s:%s\n    ", odp_cpu_id(), \
		       diff/TICS_PER_SEC, diff%TICS_PER_SEC,		\
		       __LINE__, __FUNCTION__, __FILE__);		\
		printf(a); fflush(stdout); } while (0)

#define panic(x)	do {fprintf(stderr, "PANIC: %s", x); int *a = 0; *a = 3;} while (0)

static inline char *print_th_flags(uint8_t f, int or) {
	const char *t[8] = {"FIN", "SYN", "RST", "PUSH", "ACK", "URG", "ECE", "CWR"};
	static char buf[64];
	uint8_t m = 1;
	int i, n = 0;
	buf[0] = 0;
	for (i = 0; i < 8; i++) {
		if (or && (f & m)) n += sprintf(buf+n, " %s", t[i]);
		else if (!or && !(f & m)) n += sprintf(buf+n, " %s", t[i]);
		m = m << 1;
	}
	return buf;
}

static inline char *print_flags(uint32_t f, int or) {
	const char *t[] = {"ACKNOW", "DELACK", "NODEALY", "NOOPT", "SENTFIN",
			    "REQ_SCALE", "RCVD_SCALE", "REQ_TSTMP", "RCVD_TSTMP",
			    "SACK_PERMIT", "NEEDSYN", "NEEDFIN", "NOPUSH", "PREVVALID",
			   "", "",
			    "MORETOCOME", "LQ_OVERFLOW","LASTIDLE","RXWIN0SENT",
			    "FASTRECOVERY", "WASFRECOVERY", "SIGNATURE", "FORCEDATA",
			    "TSO", "TOE", "ECN_PERMIT", "ECN_SND_CWR",
			   "ECN_SND_ECE", "CONGRECOVERY", "WASCRECOVERY", ""};
	static char buf[128];
	uint32_t m = 1;
	int i, n = 0;
	buf[0] = 0;
	for (i = 0; i < 29; i++) {
		if (or && (f & m)) n += sprintf(buf+n, " %s", t[i]);
		else if (!or && !(f & m)) n += sprintf(buf+n, " %s", t[i]);
		m = m << 1;
	}
	return buf;
}

#define t_flags_or(_f, _v) do { _f |= _v;				\
		/*OFP_LOG("t_flags OR %s 0x%x\n", print_flags(_v, 1), (uint32_t)_v);*/ } while (0)
#define t_flags_and(_f, _v) do { _f &= _v;				\
		/*OFP_LOG("t_flags AND %s 0x%x\n", print_flags(_v, 0), (uint32_t)_v);*/ } while (0)

uint16_t ofp_in_cksum(register uint16_t *addr, register int len);
int ofp_cksum(const odp_packet_t pkt, unsigned int off, unsigned int len);
int ofp_getsum(const odp_packet_t pkt, unsigned int off, unsigned int len);
int ofp_in4_cksum(const odp_packet_t pkt);
void ofp_print_hex(uint8_t log_level,
	unsigned char *data, int len);
void ofp_generate_coredump(void);
int ofp_hex_to_num(char *s);
void ofp_mac_to_link_local(uint8_t *mac, uint8_t *lladdr);
void ofp_ip6_masklen_to_mask(int masklen, uint8_t *mask);
int ofp_mask_length(int masklen, uint8_t *mask);
int ofp_name_to_port_vlan(const char *dev, int *vlan);
char *ofp_port_vlan_to_ifnet_name(int port, int vlan);
int ofp_sendf(int fd, const char *fmt, ...);
int ofp_has_mac(uint8_t *mac);

void *ofp_shared_memory_alloc(const char *name, uint64_t size);
int ofp_shared_memory_free(const char *name);
void *ofp_shared_memory_lookup(const char *name);

static inline int ilog2(unsigned long long n)
{
	return 63 - __builtin_clzll(n);
}

static inline odp_bool_t ofp_ip6_is_set(uint8_t *addr)
{
	return ((*(uint64_t *)addr | *(uint64_t *)(addr + 8)) == 0 ? 0 : 1);
}
static inline odp_bool_t ofp_ip6_equal(uint8_t *addr1, uint8_t *addr2)
{
	return (((*(uint64_t *)addr1 ==  *(uint64_t *)addr2) &&
		(*(uint64_t *)(addr1 + 8) ==  *(uint64_t *)(addr2 + 8)))
	? 1 : 0);
}

#if 0
#define ofp_copy_mac(dst, src) { \
	*((uint32_t *)(dst)) = *((uint32_t *)(src));\
	*(uint16_t *)(((uint32_t *)dst) + 1) = \
		*(uint16_t *)(((uint32_t *)src) + 1); \
}
#else
#define ofp_copy_mac(dst, src) {\
	memcpy(dst, src, OFP_ETHER_ADDR_LEN); \
}
#endif

#endif