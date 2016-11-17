/* Artwork3D.cc - Intermediate buffer for 3DD binary object (adapted from Sound.cc)
 * Copyright (C) 2006-2007, Pino Toscano <pino@kde.org>
 * Copyright (C) 2016, Hiroka Ihara <ihara_h@live.jp>
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

#include "Object.h"
#include "Artwork3D.h"
#include "Stream.h"

Artwork3D::Artwork3D(Object *streamObj, Object *viewObj)
{
  this->streamObj = new Object();
  this->streamObj->initNull();
  streamObj->copy(this->streamObj);
  this->viewObj = new Object();
  this->viewObj->initNull();
  if (viewObj != NULL) {
    viewObj->copy(this->viewObj);
  }
}

Artwork3D::~Artwork3D()
{
  streamObj->free();
  delete streamObj;
  viewObj->free();
  delete viewObj;
}

Stream *Artwork3D::getStream()
{
  return streamObj->getStream();
}

Object *Artwork3D::getView()
{
  return viewObj;
}

Artwork3D *Artwork3D::copy()
{
  Artwork3D *newartwork = new Artwork3D(streamObj, viewObj);

  return newartwork;
}

Artwork3DKind Artwork3D::getKind()
{
  Object obj1;
  streamObj->getDict()->lookup("Subtype", &obj1);
  if (obj1.isName("U3D")) {
    obj1.free();
    return artwork3d_U3D;
  } else if (obj1.isName("PRC")) {
    obj1.free();
    return artwork3d_PRC;
  }
  obj1.free();
  return artwork3d_UnknownKind;
}
