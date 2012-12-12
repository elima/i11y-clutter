/*
 * i11y-clutter-offscreen.c
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

#include "i11y-clutter-offscreen.h"

#define DEFAULT_TEX_WIDTH  800
#define DEFAULT_TEX_HEIGHT 600

#define I11Y_CLUTTER_OFFSCREEN_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                I11Y_TYPE_CLUTTER_OFFSCREEN, \
                                I11yClutterOffscreenPrivate)) \

struct _I11yClutterOffscreenPrivate
{
  CoglContext *context;
  CoglPipeline *pipeline;
  CoglOffscreen *offscreen;
  CoglTexture *tex;

  guint tex_width;
  guint tex_height;
};

/* properties */
enum
{
  PROP_0,
  PROP_TEXTURE_WIDTH,
  PROP_TEXTURE_HEIGHT
};

static void           i11y_clutter_offscreen_class_init     (I11yClutterOffscreenClass *class);
static void           i11y_clutter_offscreen_init           (I11yClutterOffscreen *self);
static void           i11y_clutter_offscreen_constructed    (GObject *obj);

static void           i11y_clutter_offscreen_finalize       (GObject *obj);

static void           set_property                          (GObject      *obj,
                                                             guint         prop_id,
                                                             const GValue *value,
                                                             GParamSpec   *pspec);
static void           get_property                          (GObject    *obj,
                                                             guint       prop_id,
                                                             GValue     *value,
                                                             GParamSpec *pspec);

static void           on_actor_added                        (ClutterActor *actor,
                                                             ClutterActor *child,
                                                             gpointer      user_data);
static void           on_actor_removed                      (ClutterActor *actor,
                                                             ClutterActor *child,
                                                             gpointer      user_data);

static void           paint_node                            (ClutterActor     *actor,
                                                             ClutterPaintNode *root);
static void           paint                                 (ClutterActor *actor);

G_DEFINE_TYPE (I11yClutterOffscreen, i11y_clutter_offscreen, CLUTTER_TYPE_ACTOR);

static void
i11y_clutter_offscreen_class_init (I11yClutterOffscreenClass *class)
{
  GObjectClass *obj_class = G_OBJECT_CLASS (class);
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (class);

  obj_class->finalize = i11y_clutter_offscreen_finalize;
  obj_class->constructed = i11y_clutter_offscreen_constructed;
  obj_class->get_property = get_property;
  obj_class->set_property = set_property;

  actor_class->paint = paint;
  actor_class->paint_node = paint_node;

  g_object_class_install_property (obj_class, PROP_TEXTURE_WIDTH,
                                   g_param_spec_int ("texture-width",
                                                     "Texture width",
                                                     "The width of the offscreen texture",
                                                     1,
                                                     G_MAXINT,
                                                     DEFAULT_TEX_WIDTH,
                                                     G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                                                     G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (obj_class, PROP_TEXTURE_HEIGHT,
                                   g_param_spec_int ("texture-height",
                                                     "Texture height",
                                                     "The height of the offscreen texture",
                                                     1,
                                                     G_MAXINT,
                                                     DEFAULT_TEX_HEIGHT,
                                                     G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                                                     G_PARAM_STATIC_STRINGS));

  g_type_class_add_private (class, sizeof (I11yClutterOffscreenPrivate));
}

static void
i11y_clutter_offscreen_init (I11yClutterOffscreen *self)
{
  I11yClutterOffscreenPrivate *priv;

  self->priv = priv = I11Y_CLUTTER_OFFSCREEN_GET_PRIVATE (self);

  g_signal_connect (self,
                    "actor-added",
                    G_CALLBACK (on_actor_added),
                    NULL);
  g_signal_connect (self,
                    "actor-removed",
                    G_CALLBACK (on_actor_removed),
                    NULL);
}

static void
i11y_clutter_offscreen_constructed (GObject *obj)
{
  I11yClutterOffscreen *self = I11Y_CLUTTER_OFFSCREEN (obj);

  self->priv->tex = cogl_texture_new_with_size (self->priv->tex_width,
                                                self->priv->tex_height,
                                                COGL_TEXTURE_NONE,
                                                COGL_PIXEL_FORMAT_RGBA_8888);

  self->priv->offscreen = cogl_offscreen_new_to_texture (self->priv->tex);
  cogl_push_framebuffer ((CoglFramebuffer *) self->priv->offscreen);
  cogl_translate (-1.0, 1.0, 0.0);
  cogl_scale (2.0 / self->priv->tex_width, -2.0 / self->priv->tex_height, 1.0);
  cogl_pop_framebuffer ();
}

static void
i11y_clutter_offscreen_finalize (GObject *obj)
{
  I11yClutterOffscreen *self = I11Y_CLUTTER_OFFSCREEN (obj);

  cogl_object_unref (self->priv->offscreen);
  cogl_object_unref (self->priv->pipeline);
  cogl_object_unref (self->priv->context);
  cogl_object_unref (self->priv->tex);

  G_OBJECT_CLASS (i11y_clutter_offscreen_parent_class)->finalize (obj);
}

static void
set_property (GObject      *obj,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
  I11yClutterOffscreen *self;

  self = I11Y_CLUTTER_OFFSCREEN (obj);

  switch (prop_id)
    {
    case PROP_TEXTURE_WIDTH:
      self->priv->tex_width = g_value_get_int (value);
      break;

    case PROP_TEXTURE_HEIGHT:
      self->priv->tex_height = g_value_get_int (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
      break;
    }
}

static void
get_property (GObject    *obj,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
  I11yClutterOffscreen *self;

  self = I11Y_CLUTTER_OFFSCREEN (obj);

  switch (prop_id)
    {
    case PROP_TEXTURE_WIDTH:
      g_value_set_int (value, self->priv->tex_width);
      break;

    case PROP_TEXTURE_HEIGHT:
      g_value_set_int (value, self->priv->tex_height);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
      break;
    }
}

static void
on_child_redraw (ClutterActor *actor, gpointer user_data)
{
  clutter_actor_queue_redraw (clutter_actor_get_parent (actor));
}

static void
on_actor_added (ClutterActor *actor, ClutterActor *child, gpointer user_data)
{
  g_signal_connect (child,
                    "queue-redraw",
                    G_CALLBACK (on_child_redraw),
                    actor);
}

static void
on_actor_removed (ClutterActor *actor, ClutterActor *child, gpointer user_data)
{
  g_signal_handlers_disconnect_by_func (child,
                                        on_child_redraw,
                                        actor);
}

static void
paint_node (ClutterActor *actor, ClutterPaintNode *root)
{
  /* do nothing */
}

static void
paint (ClutterActor *actor)
{
  I11yClutterOffscreen *self = I11Y_CLUTTER_OFFSCREEN (actor);
  ClutterActorBox allocation = { 0, };
  gfloat width, height;
  GList *children, *node;

  clutter_actor_get_allocation_box (actor, &allocation);
  clutter_actor_box_get_size (&allocation, &width, &height);

  cogl_push_framebuffer ((CoglFramebuffer *) self->priv->offscreen);

  /* clear background */
  cogl_framebuffer_clear4f ((CoglFramebuffer *) self->priv->offscreen,
                            COGL_BUFFER_BIT_COLOR,
                            0.0, 0.0, 0.0, 0.0);

  children = clutter_actor_get_children (CLUTTER_ACTOR (self));

  node = children;
  while (node != NULL)
    {
      ClutterActor *child;

      child = CLUTTER_ACTOR (node->data);
      clutter_actor_paint (child);

      node = node->next;
    }
  g_list_free (children);

  cogl_pop_framebuffer ();
}

/* public */

ClutterActor *
i11y_clutter_offscreen_new (guint tex_width, guint tex_height)
{
  I11yClutterOffscreen *self;

  self = g_object_new (I11Y_TYPE_CLUTTER_OFFSCREEN,
                       "texture-width", tex_width,
                       "texture-height", tex_height,
                       NULL);

  self->priv->context =
    clutter_backend_get_cogl_context (clutter_get_default_backend ());

  self->priv->pipeline = cogl_pipeline_new (self->priv->context);

  return CLUTTER_ACTOR (self);
}

/**
 * i11y_clutter_offscreen_get_texture:
 *
 * Returns: (type guint) (transfer none):
 **/
CoglTexture *
i11y_clutter_offscreen_get_texture (I11yClutterOffscreen *self)
{
  g_return_val_if_fail (I11Y_IS_CLUTTER_OFFSCREEN (self), NULL);

  return self->priv->tex;
}
