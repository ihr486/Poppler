/* Artwork3D.h - Intermediate buffer for 3DD objects
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

#ifndef Artwork3D_H
#define Artwork3D_H

class Artwork3D
{
public:
  enum Artwork3DKind {
    artwork3d_UnknownKind,
    artwork3d_U3D,
    artwork3d_PRC
  };
  class Projection3D
  {
    Object projectionObj;
  public:
    Projection3D(Object *_projectionObj);
    ~Projection3D();
    const char *getSubType();
    const char *getClippingStyle();
    double getFarClippingDistance();
    double getNearClippingDistance();
    double getFieldOfView();
    const char *getPerspectiveScalingType();
    double getPerspectiveScalingValue();
    double getOrthogonalScalingValue();
  };
  class LightingScheme3D
  {
    Object lightingObj;
  public:
    LightingScheme3D(Object *_lightingObj);
    ~LightingScheme3D();
    const char *getSubType();
  };
  class View3D
  {
    Object viewObj;
    Projection3D *projection;
    LightingScheme3D *lighting;
  public:
    View3D(Object *_viewObj);
    ~View3D();
    const char *getExternalName();
    const char *getInternalName();
    const char *getMatrixSelection();
    bool getTransform(double m[12]);
    const char *getViewNodePath();
    double getOrbitCenter();
    Projection3D *getProjection();
    LightingScheme3D *getLightingScheme();
  };
  Artwork3D(Object *_streamObj, Object *_viewObj);
  ~Artwork3D();
  Stream *getStream();
  Artwork3D *copy();
  Artwork3DKind getKind();
  View3D *getDefaultView();
  View3D *getView(int index);
  View3D *getView(const char *name);
  View3D *getFirstView();
  View3D *getLastView();
private:
  Object streamObj, viewObj;
};

#endif
