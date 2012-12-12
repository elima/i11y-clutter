/*
 * i11y-clutter-partsys.c
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

#include <clutter/clutter.h>
#include <cogl/cogl.h>
#include <math.h>

#include "i11y-clutter-partsys.h"

#define DEFAULT_DISTRIBUTE_RANDOM FALSE
#define MAX_TEXTURES 4

#define I11Y_CLUTTER_PARTSYS_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), I11Y_TYPE_CLUTTER_PARTSYS, I11yClutterPartsysPrivate))

struct _I11yClutterPartsysPrivate
{
  CoglContext *context;
  CoglPipeline *pipeline;
  CoglPrimitive *primitive;

  CoglTexture *texs[MAX_TEXTURES];

  guint num_particles;
  CoglVerticesMode vertices_mode;

  gboolean distribute_random;
};

static void           i11y_clutter_partsys_class_init     (I11yClutterPartsysClass *class);
static void           i11y_clutter_partsys_init           (I11yClutterPartsys *self);

static void           i11y_clutter_partsys_finalize       (GObject *obj);

static void           paint                            (ClutterActor *actor);
static void           allocate                         (ClutterActor           *actor,
                                                        const ClutterActorBox  *box,
                                                        ClutterAllocationFlags  flags);

G_DEFINE_TYPE (I11yClutterPartsys,
               i11y_clutter_partsys,
               CLUTTER_TYPE_ACTOR);

static void
i11y_clutter_partsys_class_init (I11yClutterPartsysClass *class)
{
  GObjectClass *obj_class = G_OBJECT_CLASS (class);
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (class);

  obj_class->finalize = i11y_clutter_partsys_finalize;

  actor_class->paint = paint;
  actor_class->allocate = allocate;

  g_type_class_add_private (class, sizeof (I11yClutterPartsysPrivate));
}

static void
i11y_clutter_partsys_init (I11yClutterPartsys *self)
{
  I11yClutterPartsysPrivate *priv;

  self->priv = priv = I11Y_CLUTTER_PARTSYS_GET_PRIVATE (self);

  priv->distribute_random = DEFAULT_DISTRIBUTE_RANDOM;
}

static void
i11y_clutter_partsys_finalize (GObject *obj)
{
  I11yClutterPartsys *self = I11Y_CLUTTER_PARTSYS (obj);
  gint i;

  cogl_object_unref (self->priv->primitive);
  cogl_object_unref (self->priv->pipeline);
  cogl_object_unref (self->priv->context);

  for (i=0; i<MAX_TEXTURES; i++)
    if (self->priv->texs[i] != NULL)
      cogl_object_unref (self->priv->texs[i]);

  G_OBJECT_CLASS (i11y_clutter_partsys_parent_class)->finalize (obj);
}

static void
allocate (ClutterActor           *actor,
          const ClutterActorBox  *box,
          ClutterAllocationFlags  flags)
{
  I11yClutterPartsys *self = I11Y_CLUTTER_PARTSYS (actor);
  gint location;
  gfloat width, height;
  guint actor_size[2];

  clutter_actor_box_get_size (box, &width, &height);
  actor_size[0] = width;
  actor_size[1] = height;

  location = cogl_pipeline_get_uniform_location (self->priv->pipeline,
                                                 "actor_size");
  cogl_pipeline_set_uniform_int (self->priv->pipeline,
                                 location,
                                 2,
                                 1,
                                 actor_size);

  CLUTTER_ACTOR_CLASS (i11y_clutter_partsys_parent_class)->allocate (actor,
                                                                  box,
                                                                  flags);
}

static void
paint (ClutterActor *actor)
{
  I11yClutterPartsys *self = I11Y_CLUTTER_PARTSYS (actor);
  CoglFramebuffer *fb;
  ClutterActorBox allocation = { 0, };
  gfloat width, height;

  clutter_actor_get_allocation_box (actor, &allocation);
  clutter_actor_box_get_size (&allocation, &width, &height);

  fb = cogl_get_draw_framebuffer ();

  cogl_scale (width, height, 1);

  cogl_framebuffer_draw_primitive (fb,
                                   self->priv->pipeline,
                                   self->priv->primitive);
}

static CoglPrimitive *
create_particles (CoglContext      *context,
                  guint             num,
                  CoglVerticesMode  vertices_mode,
                  gboolean          distribute_random)
{
  typedef struct
  {
    gfloat x, y, z;
    gfloat s, t;
    gfloat nx, ny, nz;
  } ParticleVertex;

  CoglPrimitive *primitive;
  CoglAttributeBuffer *attr_buf;
  CoglAttribute *attr[3];
  ParticleVertex *particles;
  gint i;
  guint width = sqrt (num);
  guint c = 0;
  guint num_vertices;
  CoglIndices *indices = NULL;
  guint num_indices;

  if (distribute_random)
    {
      num_vertices = num;

      particles = g_slice_alloc (sizeof (ParticleVertex) * num_vertices);

      for (i=0; i<num; i++)
        {
          particles[i].x = g_random_double_range (0.0, 1.0);
          particles[i].y = g_random_double_range (0.0, 1.0);
          particles[i].z = g_random_double_range (-1.0, 1.0) * 10.0;

          particles[i].s = particles[i].x;
          particles[i].t = particles[i].y;

          particles[i].nx = g_random_double_range (0.0, 1.0);
          particles[i].ny = g_random_double_range (0.0, 1.0);
          particles[i].nz = g_random_double_range (0.0, 1.0);
        }
    }
  else
    {
      guint x, y;
      gfloat step = 1.0 / width;
      guint *indices_buf;

      num_vertices = num;
      num_indices = (width-1) * (width-1) * 6;

      particles = g_slice_alloc (sizeof (ParticleVertex) * num_vertices);
      indices_buf = g_slice_alloc (num_indices * sizeof (guint));

      c = 0;
      for (i=0; i<num; i++)
        {
          y = i / width;
          x = i % width;

          particles[i].x = step * x;
          particles[i].y = step * y;
          particles[i].z = 0.0;

          particles[i].s = particles[i].x;
          particles[i].t = particles[i].y;

          particles[i].nx = 0.0;
          particles[i].ny = 0.0;
          particles[i].nz = 1.0;

          if (x < width - 1 && y < width - 1)
            {
              indices_buf[c] = i;
              indices_buf[c+1] = i + width;
              indices_buf[c+2] = i + width + 1;

              indices_buf[c+3] = i;
              indices_buf[c+4] = i + width + 1;
              indices_buf[c+5] = i + 1;
              c += 6;
            }
        }

      indices = cogl_indices_new (context,
                                  COGL_INDICES_TYPE_UNSIGNED_INT,
                                  indices_buf,
                                  num_indices);

      g_slice_free1 (num_indices * sizeof (guint), indices_buf);
    }

  attr_buf = cogl_attribute_buffer_new (context,
                                        sizeof (ParticleVertex) * num_vertices,
                                        particles);
  attr[0] = cogl_attribute_new (attr_buf,
                                "cogl_position_in",
                                sizeof (ParticleVertex),
                                offsetof (ParticleVertex, x),
                                3,
                                COGL_ATTRIBUTE_TYPE_FLOAT);
  attr[1] = cogl_attribute_new (attr_buf,
                                "cogl_tex_coord0_in",
                                sizeof (ParticleVertex),
                                offsetof (ParticleVertex, s),
                                2,
                                COGL_ATTRIBUTE_TYPE_FLOAT);
  attr[2] = cogl_attribute_new (attr_buf,
                                "cogl_normal_in",
                                sizeof (ParticleVertex),
                                offsetof (ParticleVertex, nx),
                                3,
                                COGL_ATTRIBUTE_TYPE_FLOAT);
  primitive = cogl_primitive_new_with_attributes (vertices_mode,
                                                  sizeof (ParticleVertex) * num_vertices,
                                                  attr,
                                                  3);

  if (! distribute_random)
    {
      cogl_primitive_set_indices (primitive, indices, num_indices);
      cogl_object_unref (indices);
    }

  g_slice_free1 (sizeof (ParticleVertex) * num_vertices, particles);

  return primitive;
}

/* public */

ClutterActor *
i11y_clutter_partsys_new (guint num_particles, CoglVerticesMode vertices_mode)
{
  I11yClutterPartsys *self;
  gint i;
  gint location;
  gint samplers[MAX_TEXTURES];

  self = g_object_new (I11Y_TYPE_CLUTTER_PARTSYS, NULL);

  self->priv->num_particles = num_particles;
  self->priv->vertices_mode = vertices_mode;

  self->priv->context = clutter_backend_get_cogl_context (clutter_get_default_backend ());
  self->priv->pipeline = cogl_pipeline_new (self->priv->context);

  self->priv->primitive = create_particles (self->priv->context,
                                            self->priv->num_particles,
                                            self->priv->vertices_mode,
                                            self->priv->distribute_random);

  cogl_pipeline_set_color4ub (self->priv->pipeline, 0x00, 0x00, 0x00, 0xff);

  for (i=0; i<MAX_TEXTURES; i++)
    samplers[i] = i;

  location = cogl_pipeline_get_uniform_location (self->priv->pipeline, "tex");
  cogl_pipeline_set_uniform_int (self->priv->pipeline,
                                 location,
                                 1,
                                 4,
                                 &samplers);

  return CLUTTER_ACTOR (self);
}

/**
 * i11y_clutter_partsys_set_texture:
 * @texture: (type guint) (allow-none):
 **/
void
i11y_clutter_partsys_set_texture (I11yClutterPartsys *self,
                               guint             index,
                               CoglTexture      *texture)
{
  gint location;
  guint size[2 * MAX_TEXTURES] = { 0, };
  gint i;

  g_return_if_fail (I11Y_IS_CLUTTER_PARTSYS (self));
  g_return_if_fail (index < MAX_TEXTURES);

  if (self->priv->texs[index] != NULL)
    {
      cogl_object_unref (self->priv->texs[index]);
      self->priv->texs[index] = NULL;
    }

  if (texture != NULL)
    {
      self->priv->texs[index] = texture;
      cogl_object_ref (self->priv->texs[index]);

      cogl_pipeline_set_layer_texture (self->priv->pipeline,
                                       index,
                                       self->priv->texs[index]);
    }
  else
    {
      cogl_pipeline_remove_layer (self->priv->pipeline, index);
    }

  for (i=0; i<MAX_TEXTURES; i++)
    if (self->priv->texs[i])
      {
        size[i*2 + 0] = cogl_texture_get_width (self->priv->texs[i]);
        size[i*2 + 1] = cogl_texture_get_height (self->priv->texs[i]);
      }

  location = cogl_pipeline_get_uniform_location (self->priv->pipeline,
                                                 "tex_size");
  cogl_pipeline_set_uniform_int (self->priv->pipeline,
                                 location,
                                 2,
                                 MAX_TEXTURES,
                                 &size);
}

void
i11y_clutter_partsys_set_distribute_random (I11yClutterPartsys *self, gboolean distribute_random)
{
  g_return_if_fail (I11Y_IS_CLUTTER_PARTSYS (self));

  if (self->priv->distribute_random != distribute_random)
    {
      cogl_object_unref (self->priv->primitive);

      self->priv->primitive = create_particles (self->priv->context,
                                                self->priv->num_particles,
                                                self->priv->vertices_mode,
                                                distribute_random);
    }

  self->priv->distribute_random = distribute_random;
}

/**
 * i11y_clutter_partsys_add_snippet:
 * @vertex_decls: (allow-none):
 * @vertex_code:  (allow-none):
 * @fragment_decls: (allow-none):
 * @fragment_code: (allow-none):
 *
 **/
void
i11y_clutter_partsys_add_snippet (I11yClutterPartsys *self,
                               const gchar      *vertex_decls,
                               const gchar      *vertex_code,
                               const gchar      *fragment_decls,
                               const gchar      *fragment_code)
{
  CoglSnippet *snippet;

  g_return_if_fail (I11Y_IS_CLUTTER_PARTSYS (self));

  if (vertex_code != NULL)
    {
      snippet = cogl_snippet_new (COGL_SNIPPET_HOOK_VERTEX,
                                  vertex_decls != NULL ? vertex_decls : "",
                                  vertex_code);
      cogl_pipeline_add_snippet (self->priv->pipeline, snippet);
      cogl_object_unref (snippet);
    }

  if (fragment_code != NULL)
    {
      snippet = cogl_snippet_new (COGL_SNIPPET_HOOK_FRAGMENT,
                                  fragment_decls,
                                  fragment_code);
      cogl_pipeline_add_snippet (self->priv->pipeline, snippet);
      cogl_object_unref (snippet);
    }
}

/**
 * i11y_clutter_partsys_add_snippet_from_file:
 * @vertex_decls_filename: (allow-none):
 * @vertex_code_filename:  (allow-none):
 * @fragment_decls_filename: (allow-none):
 * @fragment_code_filename: (allow-none):
 *
 **/
gboolean
i11y_clutter_partsys_add_snippet_from_file (I11yClutterPartsys *self,
                                         const gchar      *vertex_decls_filename,
                                         const gchar      *vertex_code_filename,
                                         const gchar      *fragment_decls_filename,
                                         const gchar      *fragment_code_filename,
                                         GError           **error)
{
  CoglSnippet *snippet;
  gchar *vertex_decls = NULL;
  gchar *vertex_code = NULL;
  gchar *fragment_decls = NULL;
  gchar *fragment_code = NULL;
  gboolean result = FALSE;

  g_return_if_fail (I11Y_IS_CLUTTER_PARTSYS (self));

  if (vertex_decls_filename != NULL &&
      ! g_file_get_contents (vertex_decls_filename,
                             &vertex_decls,
                             NULL,
                             error))
    goto out;

  if (vertex_code_filename != NULL &&
      ! g_file_get_contents (vertex_code_filename,
                             &vertex_code,
                             NULL,
                             error))
    goto out;

  if (fragment_decls_filename != NULL &&
      ! g_file_get_contents (fragment_decls_filename,
                             &fragment_decls,
                             NULL,
                             error))
    goto out;

  if (fragment_code_filename != NULL &&
      ! g_file_get_contents (fragment_code_filename,
                             &fragment_code,
                             NULL,
                             error))
    goto out;

  i11y_clutter_partsys_add_snippet (self,
                                 vertex_decls,
                                 vertex_code,
                                 fragment_decls,
                                 fragment_code);

  result = TRUE;

 out:
  g_free (vertex_decls);
  g_free (vertex_code);
  g_free (fragment_decls);
  g_free (fragment_code);

  return result;
}

/**
 * i11y_clutter_partsys_set_uniform_float:
 * @values: (array length=count) (element-type gfloat):
 *
 **/
void
i11y_clutter_partsys_set_uniform_float (I11yClutterPartsys *self,
                                     const gchar      *name,
                                     guint             n_components,
                                     guint             count,
                                     gfloat           *values)
{
  gint location;

  g_return_if_fail (I11Y_IS_CLUTTER_PARTSYS (self));

  location = cogl_pipeline_get_uniform_location (self->priv->pipeline, name);
  cogl_pipeline_set_uniform_float (self->priv->pipeline,
                                   location,
                                   n_components,
                                   count,
                                   values);
}

/**
 * i11y_clutter_partsys_set_uniform_int:
 * @values: (array length=count) (element-type gint):
 *
 **/
void
i11y_clutter_partsys_set_uniform_int (I11yClutterPartsys *self,
                                   const gchar      *name,
                                   guint             n_components,
                                   guint             count,
                                   gint             *values)
{
  gint location;

  g_return_if_fail (I11Y_IS_CLUTTER_PARTSYS (self));

  location = cogl_pipeline_get_uniform_location (self->priv->pipeline, name);
  cogl_pipeline_set_uniform_int (self->priv->pipeline,
                                 location,
                                 n_components,
                                 count,
                                 values);
}
