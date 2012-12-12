/*
 * i11y-clutter-partsys.h
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

#ifndef __I11Y_CLUTTER_PARTSYS_H__
#define __I11Y_CLUTTER_PARTSYS_H__

#include <clutter/clutter.h>

G_BEGIN_DECLS

#define I11Y_TYPE_CLUTTER_PARTSYS              (i11y_clutter_partsys_get_type ())
#define I11Y_CLUTTER_PARTSYS(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), I11Y_TYPE_CLUTTER_PARTSYS, I11yClutterPartsys))
#define I11Y_CLUTTER_PARTSYS_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), I11Y_TYPE_CLUTTER_PARTSYS, I11yClutterPartsysClass))
#define I11Y_IS_CLUTTER_PARTSYS(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), I11Y_TYPE_CLUTTER_PARTSYS))
#define I11Y_IS_CLUTTER_PARTSYS_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), I11Y_TYPE_CLUTTER_PARTSYS))
#define I11Y_CLUTTER_PARTSYS_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), I11Y_TYPE_CLUTTER_PARTSYS, I11yClutterPartsysClass))

typedef struct _I11yClutterPartsysClass I11yClutterPartsysClass;
typedef struct _I11yClutterPartsys I11yClutterPartsys;
typedef struct _I11yClutterPartsysPrivate I11yClutterPartsysPrivate;

struct _I11yClutterPartsys
{
  ClutterActor parent_instance;

  I11yClutterPartsysPrivate *priv;
};

struct _I11yClutterPartsysClass
{
  ClutterActorClass parent_class;
};

GType                  i11y_clutter_partsys_get_type                 (void) G_GNUC_CONST;

ClutterActor *         i11y_clutter_partsys_new                      (guint            num_particles,
                                                                   CoglVerticesMode vertices_mode);

void                   i11y_clutter_partsys_set_texture              (I11yClutterPartsys *self,
                                                                   guint             index,
                                                                   CoglTexture      *texture);

void                   i11y_clutter_partsys_set_distribute_random    (I11yClutterPartsys *self,
                                                                   gboolean          distribute_random);

void                   i11y_clutter_partsys_add_snippet              (I11yClutterPartsys *self,
                                                                   const gchar      *vertex_decls,
                                                                   const gchar      *vertex_code,
                                                                   const gchar      *fragment_decls,
                                                                   const gchar      *fragment_code);
gboolean               i11y_clutter_partsys_add_snippet_from_file   (I11yClutterPartsys *self,
                                                                  const gchar      *vertex_decls_filename,
                                                                  const gchar      *vertex_code_filename,
                                                                  const gchar      *fragment_decls_filename,
                                                                  const gchar      *fragment_code_filename,
                                                                  GError           **error);

void                   i11y_clutter_partsys_set_uniform_float        (I11yClutterPartsys *self,
                                                                   const gchar      *name,
                                                                   guint             n_components,
                                                                   guint             count,
                                                                   gfloat           *values);
void                   i11y_clutter_partsys_set_uniform_int          (I11yClutterPartsys *self,
                                                                   const gchar      *name,
                                                                   guint             n_components,
                                                                   guint             count,
                                                                   gint             *values);

G_END_DECLS

#endif /* __I11Y_CLUTTER_PARTSYS_H__ */
