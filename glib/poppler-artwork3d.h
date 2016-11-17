/* poppler-artwork3d.h: glib interface to Artwork3D (adapted from poppler-media.h)
 *
 * Copyright (C) 2010 Carlos Garcia Campos <carlosgc@gnome.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __POPPLER_ARTWORK3D_H__
#define __POPPLER_ARTWORK3D_H__

#include <glib-object.h>
#include "poppler.h"

G_BEGIN_DECLS

#define POPPLER_TYPE_ARTWORK3D    (poppler_artwork3d_get_type ())
#define POPPLER_ARTWORK3D(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), POPPLER_TYPE_ARTWORK3D, PopplerArtwork3D))
#define POPPLER_IS_ARTWORK3D(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), POPPLER_TYPE_ARTWORK3D))

typedef gboolean (*PopplerArtwork3DSaveFunc) (const gchar *buf,
                                              gsize count,
                                              gpointer data,
                                              GError **error);

GType    poppler_artwork3d_get_type         (void) G_GNUC_CONST;
gboolean poppler_artwork3d_save_to_file     (PopplerArtwork3D *artwork3d,
                                             const char *filename,
                                             GError **error);
gboolean poppler_artwork3d_save_to_callback (PopplerArtwork3D *artwork3d,
                                             PopplerArtwork3DSaveFunc save_func,
                                             gpointer user_data,
                                             GError **error);

G_END_DECLS

#endif