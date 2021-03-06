/* exif-entry.h
 *
 * Copyright � 2001 Lutz M�ller <lutz@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details. 
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __EXIF_ENTRY_H__
#define __EXIF_ENTRY_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _ExifEntry        ExifEntry;
typedef struct _ExifEntryPrivate ExifEntryPrivate;

#include <libexif/exif-content.h>
#include <libexif/exif-format.h>

struct _ExifEntry {
        ExifTag tag;
        ExifFormat format;
        unsigned long components;

        unsigned char *data;
        unsigned int size;

	/* Content containing this entry */
	ExifContent *parent;

	ExifEntryPrivate *priv;
};

/* Lifecycle */
ExifEntry  *exif_entry_new   (void);
void        exif_entry_ref   (ExifEntry *entry);
void        exif_entry_unref (ExifEntry *entry);
void        exif_entry_free  (ExifEntry *entry);

void        exif_entry_initialize (ExifEntry *entry, ExifTag tag);

/* For your convenience */
const char *exif_entry_get_value (ExifEntry *entry, char *val,
				  unsigned int maxlen);

void        exif_entry_dump      (ExifEntry *entry, unsigned int indent);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EXIF_ENTRY_H__ */
