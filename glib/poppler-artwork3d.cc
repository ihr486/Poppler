/* poppler-artwork3d.cc: glib interface to Artwork3D (adapted from poppler-media.cc)
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

struct _PopplerArtwork3DClass
{
  GObjectClass parent_class;
};

G_DEFINE_TYPE (PopplerArtwork3D, poppler_artwork3d, G_TYPE_OBJECT);

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

PopplerArtwork3D *
_poppler_artwork3d_new (Artwork3D *poppler_artwork3d)
{
  PopplerArtwork3D *artwork3d;

  g_assert (poppler_artwork3d != NULL);

  artwork3d = POPPLER_ARTWORK3D (g_object_new (POPPLER_TYPE_ARTWORK3D, NULL));

  artwork3d->artwork = poppler_artwork3d;

  return artwork3d;
}

#define BUF_SIZE (1024)

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
}
