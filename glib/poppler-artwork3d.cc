/* poppler-artwork3d.cc: glib interface to Artwork3D
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

#include "poppler-artwork3d.h"
#include "poppler-private.h"

typedef struct _PopplerArtwork3DClass PopplerArtwork3DClass;

struct _PopplerArtwork3D
{
  GObject parent_instance;
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
_poppler_artwork3d_new (Artwork3D *artwork3d)
{
  PopplerArtwork3D *poppler_artwork3d;

  g_assert (artwork3d != NULL);

  poppler_artwork3d = POPPLER_ARTWORK3D (g_object_new (POPPLER_TYPE_ARTWORK3D, NULL));

  return poppler_artwork3d;
}
