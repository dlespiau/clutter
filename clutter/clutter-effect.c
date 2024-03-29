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

/**
 * SECTION:clutter-effect
 * @short_description: Base class for actor effects
 *
 * The #ClutterEffect class provides a default type and API for creating
 * effects for generic actors.
 *
 * Effects are a #ClutterActorMeta sub-class that modify the way an actor
 * is painted in a way that is not part of the actor's implementation.
 *
 * Effects should be the preferred way to affect the paint sequence of an
 * actor without sub-classing the actor itself and overriding the
 * #ClutterActor::paint virtual function.
 *
 * <refsect2 id="ClutterEffect-implementation">
 *   <title>Implementing a ClutterEffect</title>
 *   <para>
 *     Creating a sub-class of #ClutterEffect requires overriding the
 *     ‘run’ method. The implementation of the function should look
 *     something like this:
 *   </para>
 *   <programlisting>
 * void effect_run (ClutterEffect *effect, ClutterEffectRunFlags flags)
 * {
 *   /&ast; Set up initialisation of the paint such as binding a
 *      CoglOffscreen or other operations &ast;/
 *
 *   /&ast; Chain to the next item in the paint sequence. This will either call
 *      ‘run’ on the next effect or just paint the actor if this is
 *      the last effect. &ast;/
 *   ClutterActor *actor =
 *     clutter_actor_meta_get_actor (CLUTTER_ACTOR_META (effect));
 *   clutter_actor_continue_paint (actor);
 *
 *   /&ast; perform any cleanup of state, such as popping the
 *      CoglOffscreen &ast;/
 * }
 *   </programlisting>
 *   <para>
 *     The effect can optionally avoid calling
 *     clutter_actor_continue_paint() to skip any further stages of
 *     the paint sequence. This is useful for example if the effect
 *     contains a cached image of the actor. In that case it can
 *     optimise painting by avoiding the actor paint and instead
 *     painting the cached image. The %CLUTTER_EFFECT_RUN_ACTOR_DIRTY
 *     flag is useful in this case. Clutter will set this flag when a
 *     redraw has been queued on the actor since it was last
 *     painted. The effect can use this information to decide if the
 *     cached image is still valid.
 *   </para>
 *   <para>
 *     The ‘run’ virtual was added in Clutter 1.8. Prior to that there
 *     were two separate functions as follows.
 *   </para>
 *   <itemizedlist>
 *     <listitem><simpara><function>pre_paint()</function>, which is called
 *     before painting the #ClutterActor.</simpara></listitem>
 *     <listitem><simpara><function>post_paint()</function>, which is called
 *     after painting the #ClutterActor.</simpara></listitem>
 *   </itemizedlist>
 *   <para>The <function>pre_paint()</function> function was used to set
 *   up the #ClutterEffect right before the #ClutterActor's paint
 *   sequence. This function can fail, and return %FALSE; in that case, no
 *   <function>post_paint()</function> invocation will follow.</para>
 *   <para>The <function>post_paint()</function> function was called after the
 *   #ClutterActor's paint sequence.</para>
 *   <para>
 *     With these two functions it is not possible to skip the rest of
 *     the paint sequence. The default implementation of the ‘run’
 *     virtual calls pre_paint(), clutter_actor_continue_paint() and
 *     then post_paint() so that existing actors that aren't using the
 *     run virtual will continue to work. New actors using the run
 *     virtual do not need to implement pre or post paint.
 *   </para>
 *   <example id="ClutterEffect-example">
 *     <title>A simple ClutterEffect implementation</title>
 *     <para>The example below creates two rectangles: one will be
 *     painted "behind" the actor, while another will be painted "on
 *     top" of the actor.  The <function>set_actor()</function>
 *     implementation will create the two materials used for the two
 *     different rectangles; the <function>run()</function> function
 *     will paint the first material using cogl_rectangle(), before
 *     continuing and then it will paint paint the second material
 *     after.</para>
 *     <programlisting>
 *  typedef struct {
 *    ClutterEffect parent_instance;
 *
 *    CoglHandle rect_1;
 *    CoglHandle rect_2;
 *  } MyEffect;
 *
 *  typedef struct _ClutterEffectClass MyEffectClass;
 *
 *  G_DEFINE_TYPE (MyEffect, my_effect, CLUTTER_TYPE_EFFECT);
 *
 *  static void
 *  my_effect_set_actor (ClutterActorMeta *meta,
 *                       ClutterActor     *actor)
 *  {
 *    MyEffect *self = MY_EFFECT (meta);
 *
 *    /&ast; Clear the previous state &ast;/
 *    if (self-&gt;rect_1)
 *      {
 *        cogl_handle_unref (self-&gt;rect_1);
 *        self-&gt;rect_1 = NULL;
 *      }
 *
 *    if (self-&gt;rect_2)
 *      {
 *        cogl_handle_unref (self-&gt;rect_2);
 *        self-&gt;rect_2 = NULL;
 *      }
 *
 *    /&ast; Maintain a pointer to the actor &ast;
 *    self-&gt;actor = actor;
 *
 *    /&ast; If we've been detached by the actor then we should
 *     &ast; just bail out here
 *     &ast;/
 *    if (self-&gt;actor == NULL)
 *      return;
 *
 *    /&ast; Create a red material &ast;/
 *    self-&gt;rect_1 = cogl_material_new ();
 *    cogl_material_set_color4f (self-&gt;rect_1, 1.0, 0.0, 0.0, 1.0);
 *
 *    /&ast; Create a green material &ast;/
 *    self-&gt;rect_2 = cogl_material_new ();
 *    cogl_material_set_color4f (self-&gt;rect_2, 0.0, 1.0, 0.0, 1.0);
 *  }
 *
 *  static gboolean
 *  my_effect_run (ClutterEffect *effect)
 *  {
 *    MyEffect *self = MY_EFFECT (effect);
 *    gfloat width, height;
 *
 *    clutter_actor_get_size (self-&gt;actor, &amp;width, &amp;height);
 *
 *    /&ast; Paint the first rectangle in the upper left quadrant &ast;/
 *    cogl_set_source (self-&gt;rect_1);
 *    cogl_rectangle (0, 0, width / 2, height / 2);
 *
 *    /&ast; Continue to the rest of the paint sequence &ast;/
 *    clutter_actor_continue_paint (self-&gt;actor);
 *
 *    /&ast; Paint the second rectangle in the lower right quadrant &ast;/
 *    cogl_set_source (self-&gt;rect_2);
 *    cogl_rectangle (width / 2, height / 2, width, height);
 *  }
 *
 *  static void
 *  my_effect_class_init (MyEffectClass *klass)
 *  {
 *    ClutterActorMetaClas *meta_class = CLUTTER_ACTOR_META_CLASS (klass);
 *
 *    meta_class-&gt;set_actor = my_effect_set_actor;
 *
 *    klass-&gt;run = my_effect_run;
 *  }
 *     </programlisting>
 *   </example>
 * </refsect2>
 *
 * #ClutterEffect is available since Clutter 1.4
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "clutter-effect.h"

#include "clutter-actor-meta-private.h"
#include "clutter-debug.h"
#include "clutter-effect-private.h"
#include "clutter-enum-types.h"
#include "clutter-marshal.h"
#include "clutter-private.h"
#include "clutter-actor-private.h"

G_DEFINE_ABSTRACT_TYPE (ClutterEffect,
                        clutter_effect,
                        CLUTTER_TYPE_ACTOR_META);

static gboolean
clutter_effect_real_pre_paint (ClutterEffect *effect)
{
  return TRUE;
}

static void
clutter_effect_real_post_paint (ClutterEffect *effect)
{
}

static gboolean
clutter_effect_real_get_paint_volume (ClutterEffect      *effect,
                                      ClutterPaintVolume *volume)
{
  return TRUE;
}

static void
clutter_effect_real_run (ClutterEffect         *effect,
                         ClutterEffectRunFlags  flags)
{
  ClutterActorMeta *actor_meta = CLUTTER_ACTOR_META (effect);
  ClutterActor *actor;
  gboolean pre_paint_succeeded;

  /* The default implementation provides a compatibility wrapper for
     effects that haven't migrated to use the 'run' virtual yet. This
     just calls the old pre and post virtuals before chaining on */

  pre_paint_succeeded = _clutter_effect_pre_paint (effect);

  actor = clutter_actor_meta_get_actor (actor_meta);
  clutter_actor_continue_paint (actor);

  if (pre_paint_succeeded)
    _clutter_effect_post_paint (effect);
}

static void
clutter_effect_notify (GObject    *gobject,
                       GParamSpec *pspec)
{
  if (strcmp (pspec->name, "enabled") == 0)
    {
      ClutterActorMeta *meta = CLUTTER_ACTOR_META (gobject);
      ClutterActor *actor = clutter_actor_meta_get_actor (meta);

      if (actor != NULL)
        clutter_actor_queue_redraw (actor);
    }

  if (G_OBJECT_CLASS (clutter_effect_parent_class)->notify != NULL)
    G_OBJECT_CLASS (clutter_effect_parent_class)->notify (gobject, pspec);
}

static void
clutter_effect_class_init (ClutterEffectClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->notify = clutter_effect_notify;

  klass->pre_paint = clutter_effect_real_pre_paint;
  klass->post_paint = clutter_effect_real_post_paint;
  klass->get_paint_volume = clutter_effect_real_get_paint_volume;
  klass->run = clutter_effect_real_run;
}

static void
clutter_effect_init (ClutterEffect *self)
{
}

gboolean
_clutter_effect_pre_paint (ClutterEffect *effect)
{
  g_return_val_if_fail (CLUTTER_IS_EFFECT (effect), FALSE);

  return CLUTTER_EFFECT_GET_CLASS (effect)->pre_paint (effect);
}

void
_clutter_effect_post_paint (ClutterEffect *effect)
{
  g_return_if_fail (CLUTTER_IS_EFFECT (effect));

  CLUTTER_EFFECT_GET_CLASS (effect)->post_paint (effect);
}

void
_clutter_effect_run (ClutterEffect         *effect,
                     ClutterEffectRunFlags  flags)
{
  g_return_if_fail (CLUTTER_IS_EFFECT (effect));

  CLUTTER_EFFECT_GET_CLASS (effect)->run (effect, flags);
}

gboolean
_clutter_effect_get_paint_volume (ClutterEffect      *effect,
                                  ClutterPaintVolume *volume)
{
  g_return_val_if_fail (CLUTTER_IS_EFFECT (effect), FALSE);
  g_return_val_if_fail (volume != NULL, FALSE);

  return CLUTTER_EFFECT_GET_CLASS (effect)->get_paint_volume (effect, volume);
}

/**
 * clutter_effect_queue_rerun:
 * @effect: A #ClutterEffect which needs redrawing
 *
 * Queues a rerun of the effect. The effect can detect when the ‘run’
 * method is called as a result of this function because it will not
 * have the %CLUTTER_EFFECT_RUN_ACTOR_DIRTY flag set. In that case the
 * effect is free to assume that the actor has not changed its
 * appearance since the last time it was painted so it doesn't need to
 * call clutter_actor_continue_paint() if it can draw a cached
 * image. This is mostly intended for effects that are using a
 * %CoglOffscreen to redirect the actor (such as
 * %ClutterOffscreenEffect). In that case the effect can save a bit of
 * rendering time by painting the cached texture without causing the
 * entire actor to be painted.
 *
 * This function can be used by effects that have their own animatable
 * parameters. For example, an effect which adds a varying degree of a
 * red tint to an actor by redirecting it through a CoglOffscreen
 * might have a property to specify the level of tint. When this value
 * changes, the underlying actor doesn't need to be redrawn so the
 * effect can call clutter_effect_queue_rerun() to make sure the
 * effect is repainted.
 *
 * Note however that modifying the position of the parent of an actor
 * may change the appearance of the actor because its transformation
 * matrix would change. In this case a redraw wouldn't be queued on
 * the actor itself so the %CLUTTER_EFFECT_RUN_ACTOR_DIRTY would still
 * not be set. The effect can detect this case by keeping track of the
 * last modelview matrix that was used to render the actor and
 * veryifying that it remains the same in the next paint.
 *
 * Any other effects that are layered on top of the passed in effect
 * will still be passed the %CLUTTER_EFFECT_RUN_ACTOR_DIRTY flag. If
 * anything queues a redraw on the actor without specifying an effect
 * or with an effect that is lower in the chain of effects than this
 * one then that will override this call. In that case this effect
 * will instead be called with the %CLUTTER_EFFECT_RUN_ACTOR_DIRTY
 * flag set.
 *
 * Since: 1.8
 */
void
clutter_effect_queue_rerun (ClutterEffect *effect)
{
  ClutterActor *actor;

  g_return_if_fail (CLUTTER_IS_EFFECT (effect));

  actor = clutter_actor_meta_get_actor (CLUTTER_ACTOR_META (effect));

  /* If the effect has no actor then nothing needs to be done */
  if (actor != NULL)
    _clutter_actor_queue_redraw_full (actor,
                                      0, /* flags */
                                      NULL, /* clip volume */
                                      effect /* effect */);
}
