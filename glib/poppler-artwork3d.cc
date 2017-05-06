/* poppler-artwork3d.cc: glib interface to Artwork3D
 *
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

#include <errno.h>
#include <glib/gstdio.h>

#include "poppler-artwork3d.h"
#include "poppler-private.h"

typedef struct _PopplerArtwork3DClass PopplerArtwork3DClass;

struct _PopplerArtwork3D
{
  GObject parent_instance;

  Artwork3D *artwork;
};

struct _PopplerByteStream
{
  GObject parent_instance;

  Stream *stream;
};

struct _PopplerArtwork3DClass
{
  GObjectClass parent_class;
};

typedef struct _PopplerView3DClass PopplerView3DClass;

struct _PopplerView3DClass
{
  GObjectClass parent_class;
};

typedef struct _PopplerByteStreamClass PopplerByteStreamClass;

struct _PopplerByteStreamClass
{
  GObjectClass parent_class;
};

G_DEFINE_TYPE (PopplerArtwork3D, poppler_artwork3d, G_TYPE_OBJECT);
G_DEFINE_TYPE (PopplerView3D, poppler_view3d, G_TYPE_OBJECT);
G_DEFINE_TYPE (PopplerByteStream, poppler_bytestream, G_TYPE_OBJECT);

static void
poppler_artwork3d_finalize (GObject *object)
{
  PopplerArtwork3D *artwork3d = POPPLER_ARTWORK3D (object);

  G_OBJECT_CLASS (poppler_artwork3d_parent_class)->finalize (object);
}

static void
poppler_artwork3d_class_init (PopplerArtwork3DClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = poppler_artwork3d_finalize;
}

static void
poppler_artwork3d_init (PopplerArtwork3D *artwork3d)
{
}

static void
poppler_view3d_finalize (GObject *object)
{
  PopplerView3D *view3d = POPPLER_VIEW3D (object);

  g_free (view3d->external_name);
  g_free (view3d->internal_name);
  g_free (view3d->view_path);

  G_OBJECT_CLASS (poppler_view3d_parent_class)->finalize (object);
}

static void
poppler_view3d_class_init (PopplerView3DClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = poppler_view3d_finalize;
}

static void
poppler_view3d_init (PopplerView3D *view3d)
{
}

static void
poppler_bytestream_finalize (GObject *object)
{
  PopplerByteStream *stream = POPPLER_BYTESTREAM (object);

  stream->stream->close();

  G_OBJECT_CLASS (poppler_bytestream_parent_class)->finalize (object);
}

static void
poppler_bytestream_class_init (PopplerByteStreamClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = poppler_bytestream_finalize;
}

static void
poppler_bytestream_init (PopplerByteStream *stream)
{
  stream->stream->reset();
}

PopplerArtwork3D *
_poppler_artwork3d_new (Artwork3D *poppler_artwork3d)
{
  PopplerArtwork3D *artwork3d;

  g_assert (poppler_artwork3d != NULL);

  artwork3d = POPPLER_ARTWORK3D (g_object_new (POPPLER_TYPE_ARTWORK3D, NULL));

  artwork3d->artwork = poppler_artwork3d;

  return artwork3d;
}

PopplerByteStream *
_poppler_bytestream_new (Stream *poppler_stream)
{
  PopplerByteStream *stream;

  g_assert (poppler_stream != NULL);

  stream = POPPLER_BYTESTREAM (g_object_new (POPPLER_TYPE_BYTESTREAM, NULL));

  stream->stream = poppler_stream;

  return stream;
}

PopplerView3D *
_poppler_view3d_new (Artwork3D::View3D *poppler_view3d)
{
  PopplerView3D *view3d;

  g_assert (poppler_view3d != NULL);

  view3d = POPPLER_VIEW3D (g_object_new (POPPLER_TYPE_VIEW3D, NULL));

  view3d->external_name = g_strdup (poppler_view3d->getExternalName());
  view3d->internal_name = g_strdup (poppler_view3d->getInternalName());
  view3d->matrix_mode = POPPLER_VIEW3D_MATRIX_MODE_U3DPATH;
  const char *MS = poppler_view3d->getMatrixSelection();
  if (MS != NULL && !strcmp (MS, "M")) {
    view3d->matrix_mode = POPPLER_VIEW3D_MATRIX_MODE_EXPLICIT;
    poppler_view3d->getTransform(view3d->transform);
  }
  view3d->view_path = g_strdup (poppler_view3d->getViewNodePath());
  view3d->orbit_center = poppler_view3d->getOrbitCenter();

  view3d->projection.subtype = POPPLER_PROJECTION3D_SUBTYPE_PERSPECTIVE;
  view3d->projection.clipping_style = POPPLER_PROJECTION3D_CLIPPING_STYLE_AUTOMATIC;
  view3d->projection.scaling_mode = POPPLER_PROJECTION3D_SCALING_MODE_VIEWPORT_WIDTH;
  Artwork3D::Projection3D *poppler_projection3d = poppler_view3d->getProjection();
  if (poppler_projection3d != NULL) {
    const char *Subtype = poppler_projection3d->getSubType();
    if (Subtype != NULL && !strcmp(Subtype, "O")) {
      view3d->projection.subtype = POPPLER_PROJECTION3D_SUBTYPE_ORTHOGONAL;
    }
    const char *CS = poppler_projection3d->getClippingStyle();
    if (CS != NULL && !strcmp(CS, "XNF")) {
      view3d->projection.clipping_style = POPPLER_PROJECTION3D_CLIPPING_STYLE_EXPLICIT;
    }
    view3d->projection.far_clipping = poppler_projection3d->getFarClippingDistance();
    view3d->projection.near_clipping = poppler_projection3d->getNearClippingDistance();
    view3d->projection.field_of_view = poppler_projection3d->getFieldOfView();
    if (view3d->projection.subtype == POPPLER_PROJECTION3D_SUBTYPE_PERSPECTIVE) {
      const char *PS = poppler_projection3d->getPerspectiveScalingType();
      if (PS != NULL && !strcmp(PS, "H")) {
        view3d->projection.scaling_mode = POPPLER_PROJECTION3D_SCALING_MODE_VIEWPORT_HEIGHT;
      } else if (PS != NULL && !strcmp(PS, "Min")) {
        view3d->projection.scaling_mode = POPPLER_PROJECTION3D_SCALING_MODE_VIEWPORT_MIN;
      } else if (PS != NULL && !strcmp(PS, "Max")) {
        view3d->projection.scaling_mode = POPPLER_PROJECTION3D_SCALING_MODE_VIEWPORT_MAX;
      } else if (PS != NULL && !strcmp(PS, "V")) {
        view3d->projection.scaling_mode = POPPLER_PROJECTION3D_SCALING_MODE_EXPLICIT;
        view3d->projection.scaling_value = poppler_projection3d->getPerspectiveScalingValue();
      }
    } else {
      view3d->projection.scaling_value = poppler_projection3d->getOrthogonalScalingValue();
    }
  }
  view3d->lighting.subtype = POPPLER_LIGHTINGSCHEME3D_SUBTYPE_ARTWORK;
  Artwork3D::LightingScheme3D *poppler_lighting3d = poppler_view3d->getLightingScheme();
  if (poppler_lighting3d != NULL) {
    const char *Subtype = poppler_lighting3d->getSubType();
    if (Subtype != NULL && !strcmp(Subtype, "None")) {
      view3d->lighting.subtype = POPPLER_LIGHTINGSCHEME3D_SUBTYPE_NONE;
    } else if (Subtype != NULL && !strcmp(Subtype, "White")) {
      view3d->lighting.subtype = POPPLER_LIGHTINGSCHEME3D_SUBTYPE_WHITE;
    } else if (Subtype != NULL && !strcmp(Subtype, "Day")) {
      view3d->lighting.subtype = POPPLER_LIGHTINGSCHEME3D_SUBTYPE_DAY;
    } else if (Subtype != NULL && !strcmp(Subtype, "Night")) {
      view3d->lighting.subtype = POPPLER_LIGHTINGSCHEME3D_SUBTYPE_NIGHT;
    } else if (Subtype != NULL && !strcmp(Subtype, "Hard")) {
      view3d->lighting.subtype = POPPLER_LIGHTINGSCHEME3D_SUBTYPE_HARD;
    } else if (Subtype != NULL && !strcmp(Subtype, "Primary")) {
      view3d->lighting.subtype = POPPLER_LIGHTINGSCHEME3D_SUBTYPE_PRIMARY;
    } else if (Subtype != NULL && !strcmp(Subtype, "Blue")) {
      view3d->lighting.subtype = POPPLER_LIGHTINGSCHEME3D_SUBTYPE_BLUE;
    } else if (Subtype != NULL && !strcmp(Subtype, "Red")) {
      view3d->lighting.subtype = POPPLER_LIGHTINGSCHEME3D_SUBTYPE_RED;
    } else if (Subtype != NULL && !strcmp(Subtype, "Cube")) {
      view3d->lighting.subtype = POPPLER_LIGHTINGSCHEME3D_SUBTYPE_CUBE;
    } else if (Subtype != NULL && !strcmp(Subtype, "CAD")) {
      view3d->lighting.subtype = POPPLER_LIGHTINGSCHEME3D_SUBTYPE_CAD;
    } else if (Subtype != NULL && !strcmp(Subtype, "Headlamp")) {
      view3d->lighting.subtype = POPPLER_LIGHTINGSCHEME3D_SUBTYPE_HEADLAMP;
    }
  }

  return view3d;
}

PopplerView3D *
poppler_artwork3d_get_default_view (PopplerArtwork3D *artwork3d)
{
  g_assert (artwork3d != NULL);

  return _poppler_view3d_new (artwork3d->artwork->getDefaultView());
}

PopplerView3D *
poppler_artwork3d_get_first_view (PopplerArtwork3D *artwork3d)
{
  g_assert (artwork3d != NULL);

  return _poppler_view3d_new (artwork3d->artwork->getFirstView());
}

PopplerView3D *
poppler_artwork3d_get_last_view (PopplerArtwork3D *artwork3d)
{
  g_assert (artwork3d != NULL);

  return _poppler_view3d_new (artwork3d->artwork->getLastView());
}

PopplerView3D *
poppler_artwork3d_get_view_by_internal_name (PopplerArtwork3D *artwork3d, const gchar *name)
{
  g_assert (artwork3d != NULL);

  return _poppler_view3d_new (artwork3d->artwork->getView(name));
}

PopplerView3D *
poppler_artwork3d_get_view_by_array_index (PopplerArtwork3D *artwork3d, int index)
{
  g_assert (artwork3d != NULL);

  return _poppler_view3d_new (artwork3d->artwork->getView(index));
}

PopplerByteStream *
poppler_artwork3d_get_stream (PopplerArtwork3D *artwork3d)
{
  g_assert (artwork3d != NULL);

  return _poppler_bytestream_new (artwork3d->artwork->getStream());
}

int
poppler_bytestream_get_char (PopplerByteStream *stream)
{
  return stream->stream->getChar ();
}

int
poppler_bytestream_get_chars (PopplerByteStream *stream, int nchars, void *buf)
{
  return stream->stream->doGetChars (nchars, (Guchar *)buf);
}

/*#define BUF_SIZE (1024)

gboolean
poppler_artwork3d_save_to_callback (PopplerArtwork3D *artwork3d,
                                    PopplerArtwork3DSaveFunc save_func,
                                    gpointer user_data,
                                    GError **error)
{
  gchar buf[BUF_SIZE];

  g_return_val_if_fail (POPPLER_IS_ARTWORK3D (artwork3d), FALSE);

  Stream *stream = artwork3d->artwork->getStream();

  g_return_val_if_fail (stream != NULL, FALSE);

  stream->reset ();

  while (1) {
    gsize i;
    for (i = 0; i < BUF_SIZE; i++) {
      int c = stream->getChar ();
      if (c == EOF) {
        stream->close ();
        return save_func (buf, i, user_data, error);
      }
      buf[i] = c;
    }
    if (!save_func (buf, BUF_SIZE, user_data, error)) {
      stream->close();
      return FALSE;
    }
  }

  stream->close ();
  return TRUE;
}

gboolean
poppler_artwork3d_save_to_file (PopplerArtwork3D *artwork3d,
                                const char *filename,
                                GError **error)
{
  gboolean result = TRUE;
  FILE *f;

  g_return_val_if_fail (POPPLER_IS_ARTWORK3D (artwork3d), FALSE);

  Stream *stream = artwork3d->artwork->getStream();

  g_return_val_if_fail (stream != NULL, FALSE);

  f = g_fopen (filename, "wb");

  if (f == NULL) {
    gchar *display_name = g_filename_display_name (filename);
    g_set_error (error,
                 G_FILE_ERROR,
                 g_file_error_from_errno (errno),
                 "Failed to open '%s' for writing: %s",
                 display_name,
                 g_strerror (errno));
    g_free (display_name);
    return FALSE;
  }

  stream->reset ();
  while(1) {
    int data = stream->getChar ();
    if (data == EOF) break;
    if (fputc (data, f) == EOF) {
      g_set_error (error,
		               G_FILE_ERROR,
		               g_file_error_from_errno (errno),
		               "Error writing to 3D artwork file: %s",
		               g_strerror (errno));
      result = FALSE;
      break;
    }
  }
  stream->close ();

  if (fclose (f) < 0)
  {
    gchar *display_name = g_filename_display_name (filename);
    g_set_error (error,
                 G_FILE_ERROR,
                 g_file_error_from_errno (errno),
                 "Failed to close '%s', all data may not have been saved: %s",
                 display_name,
                 g_strerror (errno));
    g_free (display_name);
    return FALSE;
  }

  return result;
}*/
