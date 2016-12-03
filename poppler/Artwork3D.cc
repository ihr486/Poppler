/* Artwork3D.cc - Intermediate buffer for 3DD binary object
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

Artwork3D::Artwork3D(Object *_streamObj, Object *_viewObj)
{
  streamObj.initNull();
  _streamObj->copy(&streamObj);
  viewObj.initNull();
  if (_viewObj->isNull()) {
    if (_streamObj->isDict()) {
      _streamObj->getDict()->lookup("DV", &viewObj);
    }
  } else {
    _viewObj->copy(&viewObj);
  }
}

Artwork3D::~Artwork3D()
{
  streamObj.free();
  viewObj.free();
}

Stream *Artwork3D::getStream()
{
  return streamObj.getStream();
}

Artwork3D *Artwork3D::copy()
{
  return new Artwork3D(&streamObj, &viewObj);
}

Artwork3D::Artwork3DKind Artwork3D::getKind()
{
  Object obj1;
  streamObj.getDict()->lookup("Subtype", &obj1);
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

Artwork3D::View3D::View3D(Object *_viewObj)
{
  viewObj.initNull();
  _viewObj->copy(&viewObj);
  projection = NULL;
  if (_viewObj->isDict()) {
    Object obj1;
    _viewObj->getDict()->lookup("P", &obj1);
    if (obj1.isDict()) {
      projection = new Projection3D(&obj1);
    }
    obj1.free();
  }
}

Artwork3D::View3D::~View3D()
{
  viewObj.free();
  delete projection;
}

Artwork3D::Projection3D::Projection3D(Object *_projectionObj)
{
  projectionObj.initNull();
  _projectionObj->copy(&projectionObj);
}

Artwork3D::Projection3D::~Projection3D()
{
  projectionObj.free();
}

Artwork3D::Projection3D *Artwork3D::View3D::getProjection()
{
  return projection;
}

static const char *getCStringProperty(Object *obj, const char *name)
{
  const char *ret = NULL;
  if (obj->isDict()) {
    Object obj1;
    obj->getDict()->lookup(name, &obj1);
    if (obj1.isString()) {
      ret = obj1.getString()->getCString();
    }
    obj1.free();
  }
  return ret;
}

static double getNumberProperty(Object *obj, const char *name)
{
  double ret = 0;
  if (obj->isDict()) {
    Object obj1;
    obj->getDict()->lookup(name, &obj1);
    if (obj1.isNum()) {
      ret = obj1.getNum();
    }
    obj1.free();
  }
  return ret;
}

const char *Artwork3D::View3D::getExternalName()
{
  return getCStringProperty(&viewObj, "XN");
}

const char *Artwork3D::View3D::getInternalName()
{
  return getCStringProperty(&viewObj, "IN");
}

const char *Artwork3D::View3D::getMatrixSelection()
{
  return getCStringProperty(&viewObj, "MS");
}

const char *Artwork3D::View3D::getViewNodePath()
{
  return getCStringProperty(&viewObj, "U3DPath");
}

bool Artwork3D::View3D::getTransform(double m[12])
{
  if (viewObj.isDict()) {
    Object obj1;
    viewObj.getDict()->lookup("C2W", &obj1);
    if (obj1.isArray()) {
      Array *arr = obj1.getArray();
      if (arr->getLength() == 12) {
        for (int i = 0; i < 12; i++) {
          Object elem;
          arr->get(i, &elem);
          if (elem.isNum()) {
            m[i] = elem.getNum();
          } else {
            elem.free();
            return false;
          }
          elem.free();
        }
        return true;
      }
    }
    obj1.free();
  }
  return false;
}

double Artwork3D::View3D::getOrbitCenter()
{
  return getNumberProperty(&viewObj, "CO");
}

const char *Artwork3D::Projection3D::getSubType()
{
  return getCStringProperty(&projectionObj, "SubType");
}

const char *Artwork3D::Projection3D::getClippingStyle()
{
  return getCStringProperty(&projectionObj, "CS");
}

double Artwork3D::Projection3D::getFarClippingDistance()
{
  return getNumberProperty(&projectionObj, "F");
}

double Artwork3D::Projection3D::getNearClippingDistance()
{
  return getNumberProperty(&projectionObj, "N");
}

double Artwork3D::Projection3D::getFieldOfView()
{
  return getNumberProperty(&projectionObj, "FOV");
}

const char *Artwork3D::Projection3D::getPerspectiveScalingType()
{
  return getCStringProperty(&projectionObj, "PS");
}

double Artwork3D::Projection3D::getPerspectiveScalingValue()
{
  return getNumberProperty(&projectionObj, "PS");
}

double Artwork3D::Projection3D::getOrthogonalScalingValue()
{
  return getNumberProperty(&projectionObj, "OS");
}

Artwork3D::View3D *Artwork3D::getView(int index)
{
  View3D *ret = NULL;
  Object viewArray;
  streamObj.getStream()->getDict()->lookup("VA", &viewArray);
  if (viewArray.isArray()) {
    if (index >= 0 && index < viewArray.getArray()->getLength()) {
      Object viewElement;
      viewArray.getArray()->get(index, &viewElement);
      ret = new View3D(&viewElement);
      viewElement.free();
    }
  }
  viewArray.free();
  return ret;
}

Artwork3D::View3D *Artwork3D::getView(const char *name)
{
  View3D *ret = NULL;
  Object viewArray;
  streamObj.getStream()->getDict()->lookup("VA", &viewArray);
  if (viewArray.isArray()) {
    for (int index = 0; index < viewArray.getArray()->getLength(); index++) {
      Object viewElement;
      viewArray.getArray()->get(index, &viewElement);
      if (viewElement.isDict()) {
        Object internalName;
        viewElement.getDict()->lookup("IN", &internalName);
        if (internalName.isString()) {
          if (!internalName.getString()->cmp(name)) {
            ret = new View3D(&viewElement);
          }
        }
        internalName.free();
      }
      viewElement.free();
    }
  }
  viewArray.free();
  return ret;
}

Artwork3D::View3D *Artwork3D::getFirstView()
{
  return getView(0);
}

Artwork3D::View3D *Artwork3D::getLastView()
{
  View3D *ret = NULL;
  Object viewArray;
  streamObj.getStream()->getDict()->lookup("VA", &viewArray);
  if (viewArray.isArray()) {
    Object viewElement;
    viewArray.getArray()->get(viewArray.getArray()->getLength() - 1, &viewElement);
    ret = new View3D(&viewElement);
    viewElement.free();
  }
  viewArray.free();
  return ret;
}

Artwork3D::View3D *Artwork3D::getDefaultView()
{
  if (streamObj.isStream()) {
    if (viewObj.isInt()) {
      return getView(viewObj.getInt());
    } else if (viewObj.isName("F")) {
      return getFirstView();
    } else if (viewObj.isName("L")) {
      return getLastView();
    } else if (viewObj.isString()) {
      return getView(viewObj.getString()->getCString());
    }
  }
  return NULL;
}
