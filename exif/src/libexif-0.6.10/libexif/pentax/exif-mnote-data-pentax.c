/* exif-mnote-data-pentax.c
 *
 * Copyright � 2002, 2003 Lutz Mueller <lutz@users.sourceforge.net>
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

#include "config.h"
#include "exif-mnote-data-pentax.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <libexif/exif-byte-order.h>
#include <libexif/exif-utils.h>

/* #define DEBUG */

static void
exif_mnote_data_pentax_clear (ExifMnoteDataPentax *n)
{
	unsigned int i;

	if (!n) return;

	if (n->entries) {
		for (i = 0; i < n->count; i++)
			if (n->entries[i].data) {
				free (n->entries[i].data);
				n->entries[i].data = NULL;
			}
		free (n->entries);
		n->entries = NULL;
		n->count = 0;
	}
}

static void
exif_mnote_data_pentax_free (ExifMnoteData *n)
{
	if (!n) return;

	exif_mnote_data_pentax_clear ((ExifMnoteDataPentax *) n);
}

static char *
exif_mnote_data_pentax_get_value (ExifMnoteData *d, unsigned int i, char *val, unsigned int maxlen)
{
	ExifMnoteDataPentax *n = (ExifMnoteDataPentax *) d;

	if (!n) return NULL;
	if (n->count <= i) return NULL;
	return mnote_pentax_entry_get_value (&n->entries[i], val, maxlen);
}

static void
exif_mnote_data_pentax_load (ExifMnoteData *en,
		const unsigned char *buf, unsigned int buf_size)
{
	ExifMnoteDataPentax *n = (ExifMnoteDataPentax *) en;
	unsigned int i, o, s;
	ExifShort c;

	/* Number of entries */
	if (buf_size < 2) return;
	c = exif_get_short (buf + 6 + n->offset, n->order);
	n->entries = malloc (sizeof (MnotePentaxEntry) * c);
	if (!n->entries) return;
	memset (n->entries, 0, sizeof (MnotePentaxEntry) * c);

	for (i = 0; i < c; i++) {
	    o = 6 + 2 + n->offset + 12 * i;
	    if (o + 8 > buf_size) return;

	    n->count = i + 1;
	    n->entries[i].tag        = exif_get_short (buf + o + 0, n->order);
	    n->entries[i].format     = exif_get_short (buf + o + 2, n->order);
	    n->entries[i].components = exif_get_long  (buf + o + 4, n->order);
	    n->entries[i].order      = n->order;

            /*
             * Size? If bigger than 4 bytes, the actual data is not
             * in the entry but somewhere else (offset).
             */
            s = exif_format_get_size (n->entries[i].format) *
                                      n->entries[i].components;
            if (!s) return;
            o += 8;
            if (s > 4) o = exif_get_long (buf + o, n->order) + 6;
            if (o + s > buf_size) return;
                                                                                
            /* Sanity check */
            n->entries[i].data = malloc (sizeof (char) * s);
            if (!n->entries[i].data) return;
            memset (n->entries[i].data, 0, sizeof (char) * s);
            n->entries[i].size = s;
            memcpy (n->entries[i].data, buf + o, s);
        }
}

static unsigned int
exif_mnote_data_pentax_count (ExifMnoteData *n)
{
	return n ? ((ExifMnoteDataPentax *) n)->count : 0;
}

static unsigned int
exif_mnote_data_pentax_get_id (ExifMnoteData *d, unsigned int n)
{
	ExifMnoteDataPentax *note = (ExifMnoteDataPentax *) d;

	if (!note) return 0;
	if (note->count <= n) return 0;
	return note->entries[n].tag;
}

static const char *
exif_mnote_data_pentax_get_name (ExifMnoteData *d, unsigned int n)
{
	ExifMnoteDataPentax *note = (ExifMnoteDataPentax *) d;

	if (!note) return NULL;
	if (note->count <= n) return NULL;
	return mnote_pentax_tag_get_name (note->entries[n].tag);
}

static const char *
exif_mnote_data_pentax_get_title (ExifMnoteData *d, unsigned int n)
{
	ExifMnoteDataPentax *note = (ExifMnoteDataPentax *) d;

	if (!note) return NULL;
	if (note->count <= n) return NULL;
	return mnote_pentax_tag_get_title (note->entries[n].tag);
}

static const char *
exif_mnote_data_pentax_get_description (ExifMnoteData *d, unsigned int n)
{
	ExifMnoteDataPentax *note = (ExifMnoteDataPentax *) d;
	
	if (!note) return NULL;
	if (note->count <= n) return NULL;
	return mnote_pentax_tag_get_description (note->entries[n].tag);
}

static void
exif_mnote_data_pentax_set_offset (ExifMnoteData *d, unsigned int o)
{
	if (d) ((ExifMnoteDataPentax *) d)->offset = o;
}

static void
exif_mnote_data_pentax_set_byte_order (ExifMnoteData *d, ExifByteOrder o)
{
	ExifByteOrder o_orig;
        ExifMnoteDataPentax *n = (ExifMnoteDataPentax *) d;
        unsigned int i, fs;
        ExifShort s;
        ExifLong l;
        ExifSLong sl;
        ExifRational r;
        ExifSRational sr;
                                                                                
        if (!n) return;
                                                                                
        o_orig = n->order;
        n->order = o;
        for (i = 0; i < n->count; i++) {
                n->entries[i].order = o;
                fs = exif_format_get_size (n->entries[i].format);
                switch (n->entries[i].format) {
                case EXIF_FORMAT_SHORT:
                        for (i = 0; i < n->entries[i].components; i++) {
                                s = exif_get_short (n->entries[i].data + (i*fs),                                                    o_orig);
                                exif_set_short (n->entries[i].data + (i * fs),
                                                o, s);
                        }
                        break;
                case EXIF_FORMAT_LONG:
                        for (i = 0; i < n->entries[i].components; i++) {
                                l = exif_get_long (n->entries[i].data + (i*fs),
                                                   o_orig);
                                exif_set_long (n->entries[i].data + (i * fs),
                                               o, l);
                        }
                        break;
                case EXIF_FORMAT_RATIONAL:
                        for (i = 0; i < n->entries[i].components; i++) {
                                r = exif_get_rational (n->entries[i].data +
                                                       (i * fs), o_orig);
                                exif_set_rational (n->entries[i].data +
                                        (i * fs), o, r);
                        }
                        break;
                case EXIF_FORMAT_SLONG:
                        for (i = 0; i < n->entries[i].components; i++) {
                                sl = exif_get_slong (n->entries[i].data +
                                                (i * fs), o_orig);
                                exif_set_slong (n->entries[i].data +
                                                (i * fs), o, sl);
                        }
                        break;
                case EXIF_FORMAT_SRATIONAL:
                        for (i = 0; i < n->entries[i].components; i++) {
                                sr = exif_get_srational (n->entries[i].data +
                                                (i * fs), o_orig);
                                exif_set_srational (n->entries[i].data +
                                                (i * fs), o, sr);
                        }
                        break;
                case EXIF_FORMAT_UNDEFINED:
                case EXIF_FORMAT_BYTE:
                case EXIF_FORMAT_ASCII:
                default:
                        /* Nothing here. */
                        break;
                }
        }
}

ExifMnoteData *
exif_mnote_data_pentax_new (void)
{
	ExifMnoteData *d;

	d = calloc (1, sizeof (ExifMnoteDataPentax));
	if (!d) return NULL;

	exif_mnote_data_construct (d);

	/* Set up function pointers */
	d->methods.free            = exif_mnote_data_pentax_free;
	d->methods.set_byte_order  = exif_mnote_data_pentax_set_byte_order;
	d->methods.set_offset      = exif_mnote_data_pentax_set_offset;
	d->methods.load            = exif_mnote_data_pentax_load;
	d->methods.count           = exif_mnote_data_pentax_count;
	d->methods.get_id          = exif_mnote_data_pentax_get_id;
	d->methods.get_name        = exif_mnote_data_pentax_get_name;
	d->methods.get_title       = exif_mnote_data_pentax_get_title;
	d->methods.get_description = exif_mnote_data_pentax_get_description;
	d->methods.get_value       = exif_mnote_data_pentax_get_value;

	return d;
}
