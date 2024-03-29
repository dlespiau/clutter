/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Copyright (C) 2010  Intel Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:
 *   Emmanuele Bassi <ebassi@linux.intel.com>
 */

#if !defined(__CLUTTER_H_INSIDE__) && !defined(CLUTTER_COMPILATION)
#error "Only <clutter/clutter.h> can be included directly."
#endif

#ifndef __CLUTTER_EFFECT_H__
#define __CLUTTER_EFFECT_H__

#include <clutter/clutter-actor-meta.h>

G_BEGIN_DECLS

#define CLUTTER_TYPE_EFFECT             (clutter_effect_get_type ())
#define CLUTTER_EFFECT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_TYPE_EFFECT, ClutterEffect))
#define CLUTTER_IS_EFFECT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_TYPE_EFFECT))
#define CLUTTER_EFFECT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), CLUTTER_TYPE_EFFECT, ClutterEffectClass))
#define CLUTTER_IS_EFFECT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), CLUTTER_TYPE_EFFECT))
#define CLUTTER_EFFECT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), CLUTTER_TYPE_EFFECT, ClutterEffectClass))

typedef struct _ClutterEffectClass      ClutterEffectClass;

/**
 * ClutterEffectRunFlags:
 * @CLUTTER_EFFECT_RUN_ACTOR_DIRTY: The actor or one of its children
 * has queued a redraw before this paint. This implies that the effect
 * should call clutter_actor_continue_paint() to chain to the next
 * effect and can not cache any results from a previous paint.
 *
 * Flags passed to the ‘run’ method of #ClutterEffect.
 */
typedef enum
{
  CLUTTER_EFFECT_RUN_ACTOR_DIRTY = (1 << 0)
} ClutterEffectRunFlags;

/**
 * ClutterEffect:
 *
 * The #ClutterEffect structure contains only private data and should
 * be accessed using the provided API
 *
 * Since: 1.4
 */
struct _ClutterEffect
{
  /*< private >*/
  ClutterActorMeta parent_instance;
};

/**
 * ClutterEffectClass:
 * @pre_paint: virtual function
 * @post_paint: virtual function
 * @get_paint_volume: virtual function
 * @run: virtual function
 *
 * The #ClutterEffectClass structure contains only private data
 *
 * Since: 1.4
 */
struct _ClutterEffectClass
{
  /*< private >*/
  ClutterActorMetaClass parent_class;

  /*< public >*/
  gboolean (* pre_paint)        (ClutterEffect      *effect);
  void     (* post_paint)       (ClutterEffect      *effect);

  gboolean (* get_paint_volume) (ClutterEffect      *effect,
                                 ClutterPaintVolume *volume);

  void     (* run)              (ClutterEffect      *effect,
                                 ClutterEffectRunFlags flags);

  /*< private >*/
  void (* _clutter_effect3) (void);
  void (* _clutter_effect4) (void);
  void (* _clutter_effect5) (void);
  void (* _clutter_effect6) (void);
};

GType clutter_effect_get_type (void) G_GNUC_CONST;

void           clutter_effect_queue_rerun          (ClutterEffect *effect);

/*
 * ClutterActor API
 */

void           clutter_actor_add_effect            (ClutterActor  *self,
                                                    ClutterEffect *effect);
void           clutter_actor_add_effect_with_name  (ClutterActor  *self,
                                                    const gchar   *name,
                                                    ClutterEffect *effect);
void           clutter_actor_remove_effect         (ClutterActor  *self,
                                                    ClutterEffect *effect);
void           clutter_actor_remove_effect_by_name (ClutterActor  *self,
                                                    const gchar   *name);
GList *        clutter_actor_get_effects           (ClutterActor  *self);
ClutterEffect *clutter_actor_get_effect            (ClutterActor  *self,
                                                    const gchar   *name);
void           clutter_actor_clear_effects         (ClutterActor  *self);

G_END_DECLS

#endif /* __CLUTTER_EFFECT_H__ */
