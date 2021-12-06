/* SPDX-License-Identifier: GPL-2.0 */
// #ifndef _LINUX_LIST_H
#define _LINUX_LIST_H

#include <linux/types.h>
#include <linux/stddef.h>
#include <linux/poison.h>
#include <linux/const.h>
#include <linux/kernel.h>

/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define my_list_for_each_entry(pos1, pos2, head, member)				\
	for (pos1 = list_first_entry(head, typeof(*pos1), member), pos2 = list_last_entry(head, typeof(*pos2), member);	\
	     !list_entry_is_head(pos1, head, member) || !list_entry_is_head(pos2, head, member);			\
	     pos1 = list_next_entry(pos1, member), pos2 = list_prev_entry(pos2, member))

// #endif