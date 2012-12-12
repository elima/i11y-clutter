/*
 * i11y-clutter-offscreen.h
 *
 * i11y-clutter - Set of Cogl/Clutter modules for Interactivity related apps
 * Copyright (C) 2012 Igalia S.L.
 *
 * Authors:
 *  Eduardo Lima Mitev <elima@igalia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License at http://www.gnu.org/licenses/lgpl-3.0.txt
 * for more details.
 */

#ifndef __I11Y_CLUTTER_OFFSCREEN_H__
#define __I11Y_CLUTTER_OFFSCREEN_H__

#include <clutter/clutter.h>

G_BEGIN_DECLS

#define I11Y_TYPE_CLUTTER_OFFSCREEN              (i11y_clutter_offscreen_get_type ())
#define I11Y_CLUTTER_OFFSCREEN(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), I11Y_TYPE_CLUTTER_OFFSCREEN, I11yClutterOffscreen))
#define I11Y_CLUTTER_OFFSCREEN_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), I11Y_TYPE_CLUTTER_OFFSCREEN, I11yClutterOffscreenClass))
#define I11Y_IS_CLUTTER_OFFSCREEN(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), I11Y_TYPE_CLUTTER_OFFSCREEN))
#define I11Y_IS_CLUTTER_OFFSCREEN_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), I11Y_TYPE_CLUTTER_OFFSCREEN))
#define I11Y_CLUTTER_OFFSCREEN_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), I11Y_TYPE_CLUTTER_OFFSCREEN, I11yClutterOffscreenClass))

typedef struct _I11yClutterOffscreenClass I11yClutterOffscreenClass;
typedef struct _I11yClutterOffscreen I11yClutterOffscreen;
typedef struct _I11yClutterOffscreenPrivate I11yClutterOffscreenPrivate;

struct _I11yClutterOffscreen
{
  ClutterActor parent_instance;

  I11yClutterOffscreenPrivate *priv;
};

struct _I11yClutterOffscreenClass
{
  ClutterActorClass parent_class;
};

GType                  i11y_clutter_offscreen_get_type                 (void) G_GNUC_CONST;

ClutterActor *         i11y_clutter_offscreen_new                      (guint tex_width,
                                                                        guint tex_height);

CoglTexture *          i11y_clutter_offscreen_get_texture              (I11yClutterOffscreen *self);

G_END_DECLS

#endif /* __I11Y_CLUTTER_OFFSCREEN_H__ */
