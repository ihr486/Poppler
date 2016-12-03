/* poppler-artwork3d.h: glib interface to Artwork3D (adapted from poppler-media.h)
 *
 * Copyright (C) 2010 Carlos Garcia Campos <carlosgc@gnome.org>
 * Copyright (C) 2016 Hiroka Ihara <ihara_h@live.jp>
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

typedef enum _PopplerProjection3DSubType
{
  POPPLER_PROJECTION3D_SUBTYPE_PERSPECTIVE,
  POPPLER_PROJECTION3D_SUBTYPE_ORTHOGONAL
} PopplerProjection3DSubType;

typedef enum _PopplerProjection3DClippingStyle
{
  POPPLER_PROJECTION3D_CLIPPING_STYLE_AUTOMATIC,
  POPPLER_PROJECTION3D_CLIPPING_STYLE_EXPLICIT
} PopplerProjection3DClippingStyle;

typedef enum _PopplerProjection3DScalingMode
{
  POPPLER_PROJECTION3D_SCALING_MODE_EXPLICIT,
  POPPLER_PROJECTION3D_SCALING_MODE_VIEWPORT_WIDTH,
  POPPLER_PROJECTION3D_SCALING_MODE_VIEWPORT_HEIGHT,
  POPPLER_PROJECTION3D_SCALING_MODE_VIEWPORT_MIN,
  POPPLER_PROJECTION3D_SCALING_MODE_VIEWPORT_MAX
} PopplerProjection3DScalingMode;

typedef struct _PopplerProjection3D PopplerProjection3D;

struct _PopplerProjection3D
{
  PopplerProjection3DSubType subtype;
  PopplerProjection3DClippingStyle clipping_style;
  gdouble far_clipping;
  gdouble near_clipping;
  gdouble field_of_view;
  PopplerProjection3DScalingMode scaling_mode;
  gdouble scaling_value;
};

typedef enum _PopplerView3DMatrixMode
{
  POPPLER_VIEW3D_MATRIX_MODE_EXPLICIT,
  POPPLER_VIEW3D_MATRIX_MODE_U3DPATH
} PopplerView3DMatrixMode;

typedef struct _PopplerView3D PopplerView3D;

struct _PopplerView3D
{
  gchar *external_name;
  gchar *internal_name;
  PopplerView3DMatrixMode matrix_mode;
  gdouble transform[12];
  gchar *view_path;
  gdouble orbit_center;
  PopplerProjection3D projection;
};

#define POPPLER_TYPE_VIEW3D       (poppler_view3d_get_type ())
#define POPPLER_VIEW3D(obj)       (G_TYPE_CHECK_INSTANCE_CAST ((obj), POPPLER_TYPE_VIEW3D, PopplerView3D))
#define POPPLER_IS_VIEW3D         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), POPPLER_TYPE_VIEW3D))

#define POPPLER_TYPE_ARTWORK3D    (poppler_artwork3d_get_type ())
#define POPPLER_ARTWORK3D(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), POPPLER_TYPE_ARTWORK3D, PopplerArtwork3D))
#define POPPLER_IS_ARTWORK3D(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), POPPLER_TYPE_ARTWORK3D))

typedef gboolean (*PopplerArtwork3DSaveFunc) (const gchar *buf,
                                              gsize count,
                                              gpointer data,
                                              GError **error);

GType    poppler_view3d_get_type            (void) G_GNUC_CONST;

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
