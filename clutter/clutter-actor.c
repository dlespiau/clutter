/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Authored By Matthew Allum  <mallum@openedhand.com>
 *
 * Copyright (C) 2006, 2007, 2008 OpenedHand Ltd
 * Copyright (C) 2009, 2010 Intel Corp
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
 */

/**
 * SECTION:clutter-actor
 * @short_description: Base abstract class for all visual stage actors.
 *
 * #ClutterActor is a base abstract class for all visual elements on the
 * stage. Every object that must appear on the main #ClutterStage must also
 * be a #ClutterActor, either by using one of the classes provided by
 * Clutter, or by implementing a new #ClutterActor subclass.
 *
 * Every actor is a 2D surface positioned and optionally transformed
 * in 3D space. The actor is positioned relative to top left corner of
 * it parent with the childs origin being its anchor point (also top
 * left by default).
 *
 * <refsect2 id="ClutterActor-box">
 *   <title>Actor bounding box and transformations</title>
 *   <para>Any actor's 2D surface is contained inside its bounding box,
 *   as described by the #ClutterActorBox structure:</para>
 *   <figure id="actor-box">
 *     <title>Bounding box of an Actor</title>
 *     <graphic fileref="actor-box.png" format="PNG"/>
 *   </figure>
 *   <para>The actor box represents the untransformed area occupied by an
 *   actor. Each visible actor that has been put on a #ClutterStage also
 *   has a transformed area, depending on the actual transformations
 *   applied to it by the developer (scale, rotation). Tranforms will
 *   also be applied to any child actors. Also applied to all actors by
 *   the #ClutterStage is a perspective transformation. API is provided
 *   for both tranformed and untransformed actor geometry information.</para>
 *   <para>The GL 'modelview' transform matrix for the actor is constructed
 *   from the actor settings by the following order of operations:</para>
 *   <orderedlist>
 *     <listitem><para>Translation by actor x, y coords,</para></listitem>
 *     <listitem><para>Translation by actor depth (z),</para></listitem>
 *     <listitem><para>Scaling by scale_x, scale_y,</para></listitem>
 *     <listitem><para>Rotation around z axis,</para></listitem>
 *     <listitem><para>Rotation around y axis,</para></listitem>
 *     <listitem><para>Rotation around x axis,</para></listitem>
 *     <listitem><para>Negative translation by anchor point x,
 *     y,</para></listitem>
 *     <listitem><para>Rectangular Clip is applied (this is not an operation
 *     on the matrix as such, but it is done as part of the transform set
 *     up).</para></listitem>
 *   </orderedlist>
 *   <para>An actor can either be explicitly sized and positioned, using the
 *   various size and position accessors, like clutter_actor_set_x() or
 *   clutter_actor_set_width(); or it can have a preferred width and
 *   height, which then allows a layout manager to implicitly size and
 *   position it by "allocating" an area for an actor. This allows for
 *   actors to be manipulated in both a fixed (or static) parent container
 *   (i.e. children of #ClutterGroup) and a more automatic (or dynamic)
 *   layout based parent container.</para>
 *   <para>When accessing the position and size of an actor, the simple
 *   accessors like clutter_actor_get_width() and clutter_actor_get_x()
 *   will return a value depending on whether the actor has been explicitly
 *   sized and positioned by the developer or implicitly by the layout
 *   manager.</para>
 *   <para>Depending on whether you are querying an actor or implementing a
 *   layout manager, you should either use the simple accessors or use the
 *   size negotiation API.</para>
 * </refsect2>
 *
 * <refsect2 id="ClutterActor-event-handling">
 *   <title>Event Handling</title>
 *   <para>Clutter actors are also able to receive input events and react to
 *   them. Events are handled in the following ways:</para>
 *   <orderedlist>
 *     <listitem><para>Actors emit pointer events if set reactive, see
 *     clutter_actor_set_reactive()</para></listitem>
 *     <listitem><para>The stage is always reactive</para></listitem>
 *     <listitem><para>Events are handled by connecting signal handlers to
 *     the numerous event signal types.</para></listitem>
 *     <listitem><para>Event handlers must return %TRUE if they handled
 *     the event and wish to block the event emission chain, or %FALSE
 *     if the emission chain must continue</para></listitem>
 *     <listitem><para>Keyboard events are emitted if actor has focus, see
 *     clutter_stage_set_key_focus()</para></listitem>
 *     <listitem><para>Motion events (motion, enter, leave) are not emitted
 *     if clutter_set_motion_events_enabled() is called with %FALSE.
 *     See clutter_set_motion_events_enabled() documentation for more
 *     information.</para></listitem>
 *     <listitem><para>Once emitted, an event emission chain has two
 *     phases: capture and bubble. An emitted event starts in the capture
 *     phase (see ClutterActor::captured-event) beginning at the stage and
 *     traversing every child actor until the event source actor is reached.
 *     The emission then enters the bubble phase, traversing back up the
 *     chain via parents until it reaches the stage. Any event handler can
 *     abort this chain by returning %TRUE (meaning "event handled").
 *     </para></listitem>
 *     <listitem><para>Pointer events will 'pass through' non reactive
 *     overlapping actors.</para></listitem>
 *   </orderedlist>
 *   <figure id="event-flow">
 *     <title>Event flow in Clutter</title>
 *     <graphic fileref="event-flow.png" format="PNG"/>
 *   </figure>
 *   <para>Every '?' box in the diagram above is an entry point for
 *   application code.</para>
 * </refsect2>
 *
 * <refsect2 id="ClutterActor-subclassing">
 *   <title>Implementing a ClutterActor</title>
 *   <para>For implementing a new custom actor class, please read <link
 *   linkend="clutter-subclassing-ClutterActor">the corresponding
 *   section</link> of the API reference.</para>
 * </refsect2>
 *
 * <refsect2 id="ClutterActor-script">
 *   <title>ClutterActor custom properties for #ClutterScript</title>
 *   <para>#ClutterActor defines a custom "rotation" property which
 *   allows a short-hand description of the rotations to be applied
 *   to an actor.</para>
 *   <para>The syntax of the "rotation" property is the following:</para>
 *   <informalexample>
 *     <programlisting>
 * "rotation" : [
 *   { "&lt;axis&gt;" : [ &lt;angle&gt;, [ &lt;center&gt; ] ] }
 * ]
 *     </programlisting>
 *   </informalexample>
 *   <para>where the <emphasis>axis</emphasis> is the name of an enumeration
 *   value of type #ClutterRotateAxis and <emphasis>angle</emphasis> is a
 *   floating point value representing the rotation angle on the given axis,
 *   in degrees.</para>
 *   <para>The <emphasis>center</emphasis> array is optional, and if present
 *   it must contain the center of rotation as described by two coordinates:
 *   Y and Z for "x-axis"; X and Z for "y-axis"; and X and Y for
 *   "z-axis".</para>
 *   <para>#ClutterActor will also parse every positional and dimensional
 *   property defined as a string through clutter_units_from_string(); you
 *   should read the documentation for the #ClutterUnits parser format for
 *   the valid units and syntax.</para>
 * </refsect2>
 *
 * <refsect2 id="ClutterActor-animating">
 *   <title>Custom animatable properties</title>
 *   <para>#ClutterActor allows accessing properties of #ClutterAction
 *   and #ClutterConstraint instances associated to an actor instance
 *   for animation purposes.</para>
 *   <para>In order to access a specific #ClutterAction or a #ClutterConstraint
 *   property it is necessary to set the #ClutterActorMeta:name property on the
 *   given action or constraint.</para>
 *   <para>The property can be accessed using the following syntax:</para>
 *   <informalexample>
 *     <programlisting>
 * @&lt;section&gt;.&lt;meta-name&gt;.&lt;property-name&gt;
 *     </programlisting>
 *   </informalexample>
 *   <para>The initial <emphasis>@</emphasis> is mandatory.</para>
 *   <para>The <emphasis>section</emphasis> fragment can be one between
 *   "actions", "constraints" and "effects".</para>
 *   <para>The <emphasis>meta-name</emphasis> fragment is the name of the
 *   action or constraint, as specified by the #ClutterActorMeta:name
 *   property.</para>
 *   <para>The <emphasis>property-name</emphasis> fragment is the name of the
 *   action or constraint property to be animated.</para>
 *   <example id="example-ClutterActor-animating-meta">
 *     <title>Animating a constraint property</title>
 *     <para>The example below animates a #ClutterBindConstraint applied to an
 *     actor using clutter_actor_animate(). The <emphasis>rect</emphasis> has
 *     a binding constraint for the <emphasis>origin</emphasis> actor, and in
 *     its initial state is fully transparent and overlapping the actor to
 *     which is bound to. </para>
 *     <programlisting>
 * constraint = clutter_bind_constraint_new (origin, CLUTTER_BIND_X, 0.0);
 * clutter_actor_meta_set_name (CLUTTER_ACTOR_META (constraint), "bind-x");
 * clutter_actor_add_constraint (rect, constraint);
 *
 * constraint = clutter_bind_constraint_new (origin, CLUTTER_BIND_Y, 0.0);
 * clutter_actor_meta_set_name (CLUTTER_ACTOR_META (constraint), "bind-y");
 * clutter_actor_add_constraint (rect, constraint);
 *
 * clutter_actor_set_reactive (rect, TRUE);
 * clutter_actor_set_opacity (rect, 0);
 *
 * g_signal_connect (rect, "button-press-event",
 *                   G_CALLBACK (on_button_press),
 *                   NULL);
 *     </programlisting>
 *     <para>On button press, the rectangle "slides" from behind the actor to
 *     which is bound to, using the #ClutterBindConstraint:offset property and
 *     the #ClutterActor:opacity property.</para>
 *     <programlisting>
 * float new_offset = clutter_actor_get_width (origin) + h_padding;
 *
 * clutter_actor_animate (rect, CLUTTER_EASE_OUT_CUBIC, 500,
 *                        "opacity", 255,
 *                        "@constraints.bind-x.offset", new_offset,
 *                        NULL);
 *     </programlisting>
 *   </example>
 * </refsect2>
 */

/**
 * CLUTTER_ACTOR_IS_MAPPED:
 * @a: a #ClutterActor
 *
 * Evaluates to %TRUE if the %CLUTTER_ACTOR_MAPPED flag is set.
 *
 * Means "the actor will be painted if the stage is mapped."
 *
 * %TRUE if the actor is visible; and all parents with possible exception
 * of the stage are visible; and an ancestor of the actor is a toplevel.
 *
 * Clutter auto-maintains the mapped flag whenever actors are
 * reparented or shown/hidden.
 *
 * Since: 0.2
 */

/**
 * CLUTTER_ACTOR_IS_REALIZED:
 * @a: a #ClutterActor
 *
 * Evaluates to %TRUE if the %CLUTTER_ACTOR_REALIZED flag is set.
 *
 * The realized state has an actor-dependant interpretation. If an
 * actor wants to delay allocating resources until it is attached to a
 * stage, it may use the realize state to do so. However it is
 * perfectly acceptable for an actor to allocate Cogl resources before
 * being realized because there is only one GL context used by Clutter
 * so any resources will work on any stage.  If an actor is mapped it
 * must also be realized, but an actor can be realized and unmapped
 * (this is so hiding an actor temporarily doesn't do an expensive
 * unrealize/realize).
 *
 * To be realized an actor must be inside a stage, and all its parents
 * must be realized.
 *
 * Since: 0.2
 */

/**
 * CLUTTER_ACTOR_IS_VISIBLE:
 * @a: a #ClutterActor
 *
 * Evaluates to %TRUE if the actor has been shown, %FALSE if it's hidden.
 * Equivalent to the ClutterActor::visible object property.
 *
 * Note that an actor is only painted onscreen if it's mapped, which
 * means it's visible, and all its parents are visible, and one of the
 * parents is a toplevel stage.
 *
 * Since: 0.2
 */

/**
 * CLUTTER_ACTOR_IS_REACTIVE:
 * @a: a #ClutterActor
 *
 * Evaluates to %TRUE if the %CLUTTER_ACTOR_REACTIVE flag is set.
 *
 * Only reactive actors will receive event-related signals.
 *
 * Since: 0.6
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "cogl/cogl.h"

#include "clutter-actor-private.h"

#include "clutter-action.h"
#include "clutter-actor-meta-private.h"
#include "clutter-animatable.h"
#include "clutter-behaviour.h"
#include "clutter-constraint.h"
#include "clutter-container.h"
#include "clutter-debug.h"
#include "clutter-effect-private.h"
#include "clutter-enum-types.h"
#include "clutter-main.h"
#include "clutter-marshal.h"
#include "clutter-flatten-effect.h"
#include "clutter-paint-volume-private.h"
#include "clutter-private.h"
#include "clutter-profile.h"
#include "clutter-scriptable.h"
#include "clutter-script-private.h"
#include "clutter-shader.h"
#include "clutter-stage-private.h"
#include "clutter-units.h"

typedef struct _ShaderData ShaderData;
typedef struct _AnchorCoord AnchorCoord;

#define CLUTTER_ACTOR_GET_PRIVATE(obj) \
(G_TYPE_INSTANCE_GET_PRIVATE ((obj), CLUTTER_TYPE_ACTOR, ClutterActorPrivate))

/* Internal helper struct to represent a point that can be stored in
   either direct pixel coordinates or as a fraction of the actor's
   size. It is used for the anchor point, scale center and rotation
   centers. */
struct _AnchorCoord
{
  gboolean is_fractional;

  union
  {
    /* Used when is_fractional == TRUE */
    struct
    {
      gdouble x;
      gdouble y;
    } fraction;

    /* Use when is_fractional == FALSE */
    ClutterVertex units;
  } v;
};

/* 3 entries should be a good compromise, few layout managers
 * will ask for 3 different preferred size in each allocation cycle */
#define N_CACHED_SIZE_REQUESTS 3
typedef struct _SizeRequest SizeRequest;
struct _SizeRequest
{
  guint  age;
  gfloat for_size;
  gfloat min_size;
  gfloat natural_size;
};

/* Internal enum used to control mapped state update.  This is a hint
 * which indicates when to do something other than just enforce
 * invariants.
 */
typedef enum {
  MAP_STATE_CHECK,           /* just enforce invariants. */
  MAP_STATE_MAKE_UNREALIZED, /* force unrealize, ignoring invariants,
                              * used when about to unparent.
                              */
  MAP_STATE_MAKE_MAPPED,     /* set mapped, error if invariants not met;
                              * used to set mapped on toplevels.
                              */
  MAP_STATE_MAKE_UNMAPPED    /* set unmapped, even if parent is mapped,
                              * used just before unmapping parent.
                              */
} MapStateChange;

struct _ClutterActorPrivate
{
  /* fixed_x, fixed_y, and the allocation box are all in parent
   * coordinates.
   */
  gfloat fixed_x;
  gfloat fixed_y;

  /* request mode */
  ClutterRequestMode request_mode;

  /* our cached size requests for different width / height */
  SizeRequest width_requests[N_CACHED_SIZE_REQUESTS];
  SizeRequest height_requests[N_CACHED_SIZE_REQUESTS];

  /* An age of 0 means the entry is not set */
  guint cached_height_age;
  guint cached_width_age;

  gfloat request_min_width;
  gfloat request_min_height;
  gfloat request_natural_width;
  gfloat request_natural_height;

  ClutterActorBox allocation;
  ClutterAllocationFlags allocation_flags;

  guint position_set                : 1;
  guint min_width_set               : 1;
  guint min_height_set              : 1;
  guint natural_width_set           : 1;
  guint natural_height_set          : 1;
  /* cached request is invalid (implies allocation is too) */
  guint needs_width_request         : 1;
  /* cached request is invalid (implies allocation is too) */
  guint needs_height_request        : 1;
  /* cached allocation is invalid (request has changed, probably) */
  guint needs_allocation            : 1;
  guint show_on_set_parent          : 1;
  guint has_clip                    : 1;
  guint clip_to_allocation          : 1;
  guint enable_model_view_transform : 1;
  guint enable_paint_unmapped       : 1;
  guint has_pointer                 : 1;
  guint propagated_one_redraw       : 1;
  guint paint_volume_valid          : 1;
  guint last_paint_volume_valid     : 1;
  guint in_clone_paint              : 1;
  guint transform_valid             : 1;

  gfloat clip[4];

  /* Rotation angles */
  gdouble rxang;
  gdouble ryang;
  gdouble rzang;

  /* Rotation center: X axis */
  AnchorCoord rx_center;

  /* Rotation center: Y axis */
  AnchorCoord ry_center;

  /* Rotation center: Z axis */
  AnchorCoord rz_center;

  /* Anchor point coordinates */
  AnchorCoord anchor;

  /* depth */
  gfloat z;

  CoglMatrix transform;

  guint8 opacity;
  gint   opacity_override;

  ClutterOffscreenRedirect offscreen_redirect;

  /* This is an internal effect used to implement the
     offscreen-redirect property */
  ClutterEffect  *flatten_effect;

  ClutterActor   *parent_actor;
  GList          *children;
  gint            n_children;

  gchar          *name;
  guint32         id; /* Unique ID */

  gint32 pick_id;

  gdouble         scale_x;
  gdouble         scale_y;
  AnchorCoord     scale_center;

  PangoContext   *pango_context;

  ClutterTextDirection text_direction;

  gint internal_child;

  /* XXX: This is a workaround for not being able to break the ABI
   * of the QUEUE_REDRAW signal. It's an out-of-band argument.
   * See clutter_actor_queue_clipped_redraw() for details.
   */
  ClutterPaintVolume *oob_queue_redraw_clip;

  ClutterMetaGroup *actions;
  ClutterMetaGroup *constraints;
  ClutterMetaGroup *effects;

  /* used when painting, to update the paint volume */
  ClutterEffect *current_effect;

  /* This is used to store an effect which needs to be redrawn. A
     redraw can be queued to start from a particular effect. This is
     used by parametrised effects that can cache an image of the
     actor. If a parameter of the effect changes then it only needs to
     redraw the cached image, not the actual actor */
  ClutterEffect *effect_to_redraw;

  ClutterPaintVolume paint_volume;

  /* This is used when painting effects to implement the
     clutter_actor_continue_paint() function. It points to the node in
     the list of effects that is next in the chain */
  const GList *next_effect_to_paint;

  /* NB: This volume isn't relative to this actor, it is in eye
   * coordinates so that it can remain valid after the actor changes.
   */
  ClutterPaintVolume last_paint_volume;

  ClutterStageQueueRedrawEntry *queue_redraw_entry;
};

enum
{
  PROP_0,

  PROP_NAME,

  /* X, Y, WIDTH, HEIGHT are "do what I mean" properties;
   * when set they force a size request, when gotten they
   * get the allocation if the allocation is valid, and the
   * request otherwise
   */
  PROP_X,
  PROP_Y,
  PROP_WIDTH,
  PROP_HEIGHT,

  /* Then the rest of these size-related properties are the "actual"
   * underlying properties set or gotten by X, Y, WIDTH, HEIGHT
   */
  PROP_FIXED_X,
  PROP_FIXED_Y,

  PROP_FIXED_POSITION_SET,

  PROP_MIN_WIDTH,
  PROP_MIN_WIDTH_SET,

  PROP_MIN_HEIGHT,
  PROP_MIN_HEIGHT_SET,

  PROP_NATURAL_WIDTH,
  PROP_NATURAL_WIDTH_SET,

  PROP_NATURAL_HEIGHT,
  PROP_NATURAL_HEIGHT_SET,

  PROP_REQUEST_MODE,

  /* Allocation properties are read-only */
  PROP_ALLOCATION,

  PROP_DEPTH,

  PROP_CLIP,
  PROP_HAS_CLIP,
  PROP_CLIP_TO_ALLOCATION,

  PROP_OPACITY,

  PROP_OFFSCREEN_REDIRECT,

  PROP_VISIBLE,
  PROP_MAPPED,
  PROP_REALIZED,
  PROP_REACTIVE,

  PROP_SCALE_X,
  PROP_SCALE_Y,
  PROP_SCALE_CENTER_X,
  PROP_SCALE_CENTER_Y,
  PROP_SCALE_GRAVITY,

  PROP_ROTATION_ANGLE_X,
  PROP_ROTATION_ANGLE_Y,
  PROP_ROTATION_ANGLE_Z,
  PROP_ROTATION_CENTER_X,
  PROP_ROTATION_CENTER_Y,
  PROP_ROTATION_CENTER_Z,
  /* This property only makes sense for the z rotation because the
     others would depend on the actor having a size along the
     z-axis */
  PROP_ROTATION_CENTER_Z_GRAVITY,

  PROP_ANCHOR_X,
  PROP_ANCHOR_Y,
  PROP_ANCHOR_GRAVITY,

  PROP_SHOW_ON_SET_PARENT,

  PROP_TEXT_DIRECTION,
  PROP_HAS_POINTER,

  PROP_ACTIONS,
  PROP_CONSTRAINTS,
  PROP_EFFECT,

  PROP_LAST
};

static GParamSpec *obj_props[PROP_LAST];

enum
{
  SHOW,
  HIDE,
  DESTROY,
  PARENT_SET,
  KEY_FOCUS_IN,
  KEY_FOCUS_OUT,
  PAINT,
  PICK,
  REALIZE,
  UNREALIZE,
  QUEUE_REDRAW,
  QUEUE_RELAYOUT,
  EVENT,
  CAPTURED_EVENT,
  BUTTON_PRESS_EVENT,
  BUTTON_RELEASE_EVENT,
  SCROLL_EVENT,
  KEY_PRESS_EVENT,
  KEY_RELEASE_EVENT,
  MOTION_EVENT,
  ENTER_EVENT,
  LEAVE_EVENT,
  ALLOCATION_CHANGED,

  LAST_SIGNAL
};

static guint actor_signals[LAST_SIGNAL] = { 0, };

static void clutter_scriptable_iface_init (ClutterScriptableIface *iface);
static void clutter_animatable_iface_init (ClutterAnimatableIface *iface);
static void atk_implementor_iface_init    (AtkImplementorIface    *iface);

static void clutter_actor_shader_pre_paint  (ClutterActor *actor,
                                             gboolean      repeat);
static void clutter_actor_shader_post_paint (ClutterActor *actor);

/* These setters are all static for now, maybe they should be in the
 * public API, but they are perhaps obscure enough to leave only as
 * properties
 */
static void clutter_actor_set_min_width          (ClutterActor *self,
                                                  gfloat        min_width);
static void clutter_actor_set_min_height         (ClutterActor *self,
                                                  gfloat        min_height);
static void clutter_actor_set_natural_width      (ClutterActor *self,
                                                  gfloat        natural_width);
static void clutter_actor_set_natural_height     (ClutterActor *self,
                                                  gfloat        natural_height);
static void clutter_actor_set_min_width_set      (ClutterActor *self,
                                                  gboolean      use_min_width);
static void clutter_actor_set_min_height_set     (ClutterActor *self,
                                                  gboolean      use_min_height);
static void clutter_actor_set_natural_width_set  (ClutterActor *self,
                                                  gboolean  use_natural_width);
static void clutter_actor_set_natural_height_set (ClutterActor *self,
                                                  gboolean  use_natural_height);
static void clutter_actor_update_map_state       (ClutterActor  *self,
                                                  MapStateChange change);
static void clutter_actor_unrealize_not_hiding   (ClutterActor *self);

/* Helper routines for managing anchor coords */
static void clutter_anchor_coord_get_units (ClutterActor      *self,
                                            const AnchorCoord *coord,
                                            gfloat            *x,
                                            gfloat            *y,
                                            gfloat            *z);
static void clutter_anchor_coord_set_units (AnchorCoord       *coord,
                                            gfloat             x,
                                            gfloat             y,
                                            gfloat             z);

static ClutterGravity clutter_anchor_coord_get_gravity (AnchorCoord    *coord);
static void           clutter_anchor_coord_set_gravity (AnchorCoord    *coord,
                                                        ClutterGravity  gravity);

static gboolean clutter_anchor_coord_is_zero (const AnchorCoord *coord);

static void _clutter_actor_queue_only_relayout (ClutterActor *self);

static void _clutter_actor_get_relative_modelview (ClutterActor *self,
                                                   ClutterActor *ancestor,
                                                   CoglMatrix *matrix);

static ClutterPaintVolume *_clutter_actor_get_paint_volume_mutable (ClutterActor *self);

/* Helper macro which translates by the anchor coord, applies the
   given transformation and then translates back */
#define TRANSFORM_ABOUT_ANCHOR_COORD(a,m,c,_transform)  G_STMT_START { \
  gfloat _tx, _ty, _tz;                                                \
  clutter_anchor_coord_get_units ((a), (c), &_tx, &_ty, &_tz);         \
  cogl_matrix_translate ((m), _tx, _ty, _tz);                          \
  { _transform; }                                                      \
  cogl_matrix_translate ((m), -_tx, -_ty, -_tz);        } G_STMT_END

static GQuark quark_shader_data = 0;

G_DEFINE_ABSTRACT_TYPE_WITH_CODE (ClutterActor,
                                  clutter_actor,
                                  G_TYPE_INITIALLY_UNOWNED,
                                  G_IMPLEMENT_INTERFACE (CLUTTER_TYPE_SCRIPTABLE,
                                                         clutter_scriptable_iface_init)
                                  G_IMPLEMENT_INTERFACE (CLUTTER_TYPE_ANIMATABLE,
                                                         clutter_animatable_iface_init)
                                  G_IMPLEMENT_INTERFACE (ATK_TYPE_IMPLEMENTOR,
                                                         atk_implementor_iface_init));

G_CONST_RETURN gchar *
_clutter_actor_get_debug_name (ClutterActor *actor)
{
  return actor->priv->name != NULL ? actor->priv->name
                                   : G_OBJECT_TYPE_NAME (actor);
}

#ifdef CLUTTER_ENABLE_DEBUG
/* XXX - this is for debugging only, remove once working (or leave
 * in only in some debug mode). Should leave it for a little while
 * until we're confident in the new map/realize/visible handling.
 */
static inline void
clutter_actor_verify_map_state (ClutterActor *self)
{
  ClutterActorPrivate *priv = self->priv;

  if (CLUTTER_ACTOR_IS_REALIZED (self))
    {
      /* all bets are off during reparent when we're potentially realized,
       * but should not be according to invariants
       */
      if (!CLUTTER_ACTOR_IN_REPARENT (self))
        {
          if (priv->parent_actor == NULL)
            {
              if (CLUTTER_ACTOR_IS_TOPLEVEL (self))
                {
                }
              else
                g_warning ("Realized non-toplevel actor '%s' should "
                           "have a parent",
                           _clutter_actor_get_debug_name (self));
            }
          else if (!CLUTTER_ACTOR_IS_REALIZED (priv->parent_actor))
            {
              g_warning ("Realized actor %s has an unrealized parent %s",
                         _clutter_actor_get_debug_name (self),
                         _clutter_actor_get_debug_name (priv->parent_actor));
            }
        }
    }

  if (CLUTTER_ACTOR_IS_MAPPED (self))
    {
      if (!CLUTTER_ACTOR_IS_REALIZED (self))
        g_warning ("Actor '%s' is mapped but not realized",
                   _clutter_actor_get_debug_name (self));

      /* remaining bets are off during reparent when we're potentially
       * mapped, but should not be according to invariants
       */
      if (!CLUTTER_ACTOR_IN_REPARENT (self))
        {
          if (priv->parent_actor == NULL)
            {
              if (CLUTTER_ACTOR_IS_TOPLEVEL (self))
                {
                  if (!CLUTTER_ACTOR_IS_VISIBLE (self) &&
                      !CLUTTER_ACTOR_IN_DESTRUCTION (self))
                    {
                      g_warning ("Toplevel actor '%s' is mapped "
                                 "but not visible",
                                 _clutter_actor_get_debug_name (self));
                    }
                }
              else
                {
                  g_warning ("Mapped actor '%s' should have a parent",
                             _clutter_actor_get_debug_name (self));
                }
            }
          else
            {
              ClutterActor *iter = self;

              /* check for the enable_paint_unmapped flag on the actor
               * and parents; if the flag is enabled at any point of this
               * branch of the scene graph then all the later checks
               * become pointless
               */
              while (iter != NULL)
                {
                  if (iter->priv->enable_paint_unmapped)
                    return;

                  iter = iter->priv->parent_actor;
                }

              if (!CLUTTER_ACTOR_IS_VISIBLE (priv->parent_actor))
                {
                  g_warning ("Actor '%s' should not be mapped if parent '%s'"
                             "is not visible",
                             _clutter_actor_get_debug_name (self),
                             _clutter_actor_get_debug_name (priv->parent_actor));
                }

              if (!CLUTTER_ACTOR_IS_REALIZED (priv->parent_actor))
                {
                  g_warning ("Actor '%s' should not be mapped if parent '%s'"
                             "is not realized",
                             _clutter_actor_get_debug_name (self),
                             _clutter_actor_get_debug_name (priv->parent_actor));
                }

              if (!CLUTTER_ACTOR_IS_TOPLEVEL (priv->parent_actor))
                {
                  if (!CLUTTER_ACTOR_IS_MAPPED (priv->parent_actor))
                    g_warning ("Actor '%s' is mapped but its non-toplevel "
                               "parent '%s' is not mapped",
                               _clutter_actor_get_debug_name (self),
                               _clutter_actor_get_debug_name (priv->parent_actor));
                }
            }
        }
    }
}

#endif /* CLUTTER_ENABLE_DEBUG */

static void
clutter_actor_set_mapped (ClutterActor *self,
                          gboolean      mapped)
{
  if (CLUTTER_ACTOR_IS_MAPPED (self) == mapped)
    return;

  if (mapped)
    {
      CLUTTER_ACTOR_GET_CLASS (self)->map (self);
      g_assert (CLUTTER_ACTOR_IS_MAPPED (self));
    }
  else
    {
      CLUTTER_ACTOR_GET_CLASS (self)->unmap (self);
      g_assert (!CLUTTER_ACTOR_IS_MAPPED (self));
    }
}

/* this function updates the mapped and realized states according to
 * invariants, in the appropriate order.
 */
static void
clutter_actor_update_map_state (ClutterActor  *self,
                                MapStateChange change)
{
  gboolean was_mapped;

  was_mapped = CLUTTER_ACTOR_IS_MAPPED (self);

  if (CLUTTER_ACTOR_IS_TOPLEVEL (self))
    {
      /* the mapped flag on top-level actors must be set by the
       * per-backend implementation because it might be asynchronous.
       *
       * That is, the MAPPED flag on toplevels currently tracks the X
       * server mapped-ness of the window, while the expected behavior
       * (if used to GTK) may be to track WM_STATE!=WithdrawnState.
       * This creates some weird complexity by breaking the invariant
       * that if we're visible and all ancestors shown then we are
       * also mapped - instead, we are mapped if all ancestors
       * _possibly excepting_ the stage are mapped. The stage
       * will map/unmap for example when it is minimized or
       * moved to another workspace.
       *
       * So, the only invariant on the stage is that if visible it
       * should be realized, and that it has to be visible to be
       * mapped.
       */
      if (CLUTTER_ACTOR_IS_VISIBLE (self))
        clutter_actor_realize (self);

      switch (change)
        {
        case MAP_STATE_CHECK:
          break;

        case MAP_STATE_MAKE_MAPPED:
          g_assert (!was_mapped);
          clutter_actor_set_mapped (self, TRUE);
          break;

        case MAP_STATE_MAKE_UNMAPPED:
          g_assert (was_mapped);
          clutter_actor_set_mapped (self, FALSE);
          break;

        case MAP_STATE_MAKE_UNREALIZED:
          /* we only use MAKE_UNREALIZED in unparent,
           * and unparenting a stage isn't possible.
           * If someone wants to just unrealize a stage
           * then clutter_actor_unrealize() doesn't
           * go through this codepath.
           */
          g_warning ("Trying to force unrealize stage is not allowed");
          break;
        }

      if (CLUTTER_ACTOR_IS_MAPPED (self) &&
          !CLUTTER_ACTOR_IS_VISIBLE (self) &&
          !CLUTTER_ACTOR_IN_DESTRUCTION (self))
        {
          g_warning ("Clutter toplevel of type '%s' is not visible, but "
                     "it is somehow still mapped",
                     _clutter_actor_get_debug_name (self));
        }
    }
  else
    {
      ClutterActorPrivate *priv = self->priv;
      ClutterActor *parent = priv->parent_actor;
      gboolean should_be_mapped;
      gboolean may_be_realized;
      gboolean must_be_realized;

      should_be_mapped = FALSE;
      may_be_realized = TRUE;
      must_be_realized = FALSE;

      if (parent == NULL || change == MAP_STATE_MAKE_UNREALIZED)
        {
          may_be_realized = FALSE;
        }
      else
        {
          /* Maintain invariant that if parent is mapped, and we are
           * visible, then we are mapped ...  unless parent is a
           * stage, in which case we map regardless of parent's map
           * state but do require stage to be visible and realized.
           *
           * If parent is realized, that does not force us to be
           * realized; but if parent is unrealized, that does force
           * us to be unrealized.
           *
           * The reason we don't force children to realize with
           * parents is _clutter_actor_rerealize(); if we require that
           * a realized parent means children are realized, then to
           * unrealize an actor we would have to unrealize its
           * parents, which would end up meaning unrealizing and
           * hiding the entire stage. So we allow unrealizing a
           * child (as long as that child is not mapped) while that
           * child still has a realized parent.
           *
           * Also, if we unrealize from leaf nodes to root, and
           * realize from root to leaf, the invariants are never
           * violated if we allow children to be unrealized
           * while parents are realized.
           *
           * When unmapping, MAP_STATE_MAKE_UNMAPPED is specified
           * to force us to unmap, even though parent is still
           * mapped. This is because we're unmapping from leaf nodes
           * up to root nodes.
           */
          if (CLUTTER_ACTOR_IS_VISIBLE (self) &&
              change != MAP_STATE_MAKE_UNMAPPED)
            {
              gboolean parent_is_visible_realized_toplevel;

              parent_is_visible_realized_toplevel =
                (CLUTTER_ACTOR_IS_TOPLEVEL (parent) &&
                 CLUTTER_ACTOR_IS_VISIBLE (parent) &&
                 CLUTTER_ACTOR_IS_REALIZED (parent));

              if (CLUTTER_ACTOR_IS_MAPPED (parent) ||
                  parent_is_visible_realized_toplevel)
                {
                  must_be_realized = TRUE;
                  should_be_mapped = TRUE;
                }
            }

          /* if the actor has been set to be painted even if unmapped
           * then we should map it and check for realization as well;
           * this is an override for the branch of the scene graph
           * which begins with this node
           */
          if (priv->enable_paint_unmapped)
            {
              if (priv->parent_actor == NULL)
                g_warning ("Attempting to map an unparented actor '%s'",
                           _clutter_actor_get_debug_name (self));

              should_be_mapped = TRUE;
              must_be_realized = TRUE;
            }

          if (!CLUTTER_ACTOR_IS_REALIZED (parent))
            may_be_realized = FALSE;
        }

      if (change == MAP_STATE_MAKE_MAPPED && !should_be_mapped)
        {
          if (parent == NULL)
            g_warning ("Attempting to map a child that does not "
                       "meet the necessary invariants: the actor '%s' "
                       "has no parent",
                       _clutter_actor_get_debug_name (self));
          else
            g_warning ("Attempting to map a child that does not "
                       "meet the necessary invariants: the actor '%s' "
                       "is parented to an unmapped actor '%s'",
                       _clutter_actor_get_debug_name (self),
                       _clutter_actor_get_debug_name (priv->parent_actor));
        }

      /* If in reparent, we temporarily suspend unmap and unrealize.
       *
       * We want to go in the order "realize, map" and "unmap, unrealize"
       */

      /* Unmap */
      if (!should_be_mapped && !CLUTTER_ACTOR_IN_REPARENT (self))
        clutter_actor_set_mapped (self, FALSE);

      /* Realize */
      if (must_be_realized)
        clutter_actor_realize (self);

      /* if we must be realized then we may be, presumably */
      g_assert (!(must_be_realized && !may_be_realized));

      /* Unrealize */
      if (!may_be_realized && !CLUTTER_ACTOR_IN_REPARENT (self))
        clutter_actor_unrealize_not_hiding (self);

      /* Map */
      if (should_be_mapped)
        {
          if (!must_be_realized)
            g_warning ("Somehow we think actor '%s' should be mapped but "
                       "not realized, which isn't allowed",
                       _clutter_actor_get_debug_name (self));

          /* realization is allowed to fail (though I don't know what
           * an app is supposed to do about that - shouldn't it just
           * be a g_error? anyway, we have to avoid mapping if this
           * happens)
           */
          if (CLUTTER_ACTOR_IS_REALIZED (self))
            clutter_actor_set_mapped (self, TRUE);
        }
    }

#ifdef CLUTTER_ENABLE_DEBUG
  /* check all invariants were kept */
  clutter_actor_verify_map_state (self);
#endif
}

static void
clutter_actor_real_map (ClutterActor *self)
{
  ClutterActorPrivate *priv = self->priv;
  ClutterActor *stage;
  GList *c;

  g_assert (!CLUTTER_ACTOR_IS_MAPPED (self));

  CLUTTER_NOTE (ACTOR, "Mapping actor '%s'",
                _clutter_actor_get_debug_name (self));

  CLUTTER_ACTOR_SET_FLAGS (self, CLUTTER_ACTOR_MAPPED);

  stage = _clutter_actor_get_stage_internal (self);
  priv->pick_id = _clutter_stage_acquire_pick_id (CLUTTER_STAGE (stage), self);
  CLUTTER_NOTE (ACTOR, "Pick id '%d' for actor '%s'",
                priv->pick_id,
                _clutter_actor_get_debug_name (self));

  /* notify on parent mapped before potentially mapping
   * children, so apps see a top-down notification.
   */
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_MAPPED]);

  for (c = self->priv->children; c; c = c->next)
    {
      ClutterActor *child = c->data;
      clutter_actor_map (child);
    }
}

/**
 * clutter_actor_map:
 * @self: A #ClutterActor
 *
 * Sets the %CLUTTER_ACTOR_MAPPED flag on the actor and possibly maps
 * and realizes its children if they are visible. Does nothing if the
 * actor is not visible.
 *
 * Calling this is allowed in only one case: you are implementing the
 * #ClutterActor <function>map()</function> virtual function in an actor
 * and you need to map the children of that actor. It is not necessary
 * to call this if you implement #ClutterContainer because the default
 * implementation will automatically map children of containers.
 *
 * When overriding map, it is mandatory to chain up to the parent
 * implementation.
 *
 * Since: 1.0
 */
void
clutter_actor_map (ClutterActor *self)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  if (CLUTTER_ACTOR_IS_MAPPED (self))
    return;

  if (!CLUTTER_ACTOR_IS_VISIBLE (self))
    return;

  clutter_actor_update_map_state (self, MAP_STATE_MAKE_MAPPED);
}

static void
clutter_actor_real_unmap (ClutterActor *self)
{
  ClutterActorPrivate *priv = self->priv;
  GList *c;

  g_assert (CLUTTER_ACTOR_IS_MAPPED (self));

  CLUTTER_NOTE (ACTOR, "Unmapping actor '%s'",
                _clutter_actor_get_debug_name (self));

  for (c = priv->children; c; c = c->next)
    {
      ClutterActor *child = c->data;
      clutter_actor_unmap (child);
    }

  CLUTTER_ACTOR_UNSET_FLAGS (self, CLUTTER_ACTOR_MAPPED);

  /* clear the contents of the last paint volume, so that hiding + moving +
   * showing will not result in the wrong area being repainted
   */
  _clutter_paint_volume_init_static (&priv->last_paint_volume, NULL);
  priv->last_paint_volume_valid = TRUE;

  /* notify on parent mapped after potentially unmapping
   * children, so apps see a bottom-up notification.
   */
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_MAPPED]);

  /* relinquish keyboard focus if we were unmapped while owning it */
  if (!CLUTTER_ACTOR_IS_TOPLEVEL (self))
    {
      ClutterStage *stage;

      stage = CLUTTER_STAGE (_clutter_actor_get_stage_internal (self));

      if (stage != NULL)
        _clutter_stage_release_pick_id (stage, priv->pick_id);

      priv->pick_id = -1;

      if (stage != NULL &&
          clutter_stage_get_key_focus (stage) == self)
        {
          clutter_stage_set_key_focus (stage, NULL);
        }
    }
}

/**
 * clutter_actor_unmap:
 * @self: A #ClutterActor
 *
 * Unsets the %CLUTTER_ACTOR_MAPPED flag on the actor and possibly
 * unmaps its children if they were mapped.
 *
 * Calling this is allowed in only one case: you are implementing the
 * #ClutterActor <function>unmap()</function> virtual function in an actor
 * and you need to unmap the children of that actor. It is not necessary
 * to call this if you implement #ClutterContainer because the default
 * implementation will automatically unmap children of containers.
 *
 * When overriding unmap, it is mandatory to chain up to the parent
 * implementation.
 *
 * Since: 1.0
 */
void
clutter_actor_unmap (ClutterActor *self)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  if (!CLUTTER_ACTOR_IS_MAPPED (self))
    return;

  clutter_actor_update_map_state (self, MAP_STATE_MAKE_UNMAPPED);
}

static void
clutter_actor_real_show (ClutterActor *self)
{
  if (!CLUTTER_ACTOR_IS_VISIBLE (self))
    {
      ClutterActorPrivate *priv = self->priv;

      CLUTTER_ACTOR_SET_FLAGS (self, CLUTTER_ACTOR_VISIBLE);

      /* we notify on the "visible" flag in the clutter_actor_show()
       * wrapper so the entire show signal emission completes first
       * (?)
       */
      clutter_actor_update_map_state (self, MAP_STATE_CHECK);

      /* we queue a relayout unless the actor is inside a
       * container that explicitly told us not to
       */
      if (priv->parent_actor &&
          (!(priv->parent_actor->flags & CLUTTER_ACTOR_NO_LAYOUT)))
        {
          /* While an actor is hidden the parent may not have
           * allocated/requested so we need to start from scratch
           * and avoid the short-circuiting in
           * clutter_actor_queue_relayout().
           */
          priv->needs_width_request  = FALSE;
          priv->needs_height_request = FALSE;
          priv->needs_allocation     = FALSE;
          clutter_actor_queue_relayout (self);
        }
    }
}

static inline void
set_show_on_set_parent (ClutterActor *self,
                        gboolean      set_show)
{
  ClutterActorPrivate *priv = self->priv;

  set_show = !!set_show;

  if (priv->show_on_set_parent == set_show)
    return;

  if (priv->parent_actor == NULL)
    {
      priv->show_on_set_parent = set_show;
      g_object_notify_by_pspec (G_OBJECT (self),
                                obj_props[PROP_SHOW_ON_SET_PARENT]);
    }
}

/**
 * clutter_actor_show:
 * @self: A #ClutterActor
 *
 * Flags an actor to be displayed. An actor that isn't shown will not
 * be rendered on the stage.
 *
 * Actors are visible by default.
 *
 * If this function is called on an actor without a parent, the
 * #ClutterActor:show-on-set-parent will be set to %TRUE as a side
 * effect.
 */
void
clutter_actor_show (ClutterActor *self)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  /* simple optimization */
  if (CLUTTER_ACTOR_IS_VISIBLE (self))
    {
      /* we still need to set the :show-on-set-parent property, in
       * case show() is called on an unparented actor
       */
      set_show_on_set_parent (self, TRUE);
      return;
    }

#ifdef CLUTTER_ENABLE_DEBUG
  clutter_actor_verify_map_state (self);
#endif

  priv = self->priv;

  g_object_freeze_notify (G_OBJECT (self));

  set_show_on_set_parent (self, TRUE);

  g_signal_emit (self, actor_signals[SHOW], 0);
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_VISIBLE]);

  if (priv->parent_actor)
    clutter_actor_queue_redraw (priv->parent_actor);

  g_object_thaw_notify (G_OBJECT (self));
}

/**
 * clutter_actor_show_all:
 * @self: a #ClutterActor
 *
 * Calls clutter_actor_show() on all children of an actor (if any).
 *
 * Since: 0.2
 */
void
clutter_actor_show_all (ClutterActor *self)
{
  ClutterActorClass *klass;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  klass = CLUTTER_ACTOR_GET_CLASS (self);
  if (klass->show_all)
    klass->show_all (self);
}

void
clutter_actor_real_hide (ClutterActor *self)
{
  if (CLUTTER_ACTOR_IS_VISIBLE (self))
    {
      ClutterActorPrivate *priv = self->priv;

      CLUTTER_ACTOR_UNSET_FLAGS (self, CLUTTER_ACTOR_VISIBLE);

      /* we notify on the "visible" flag in the clutter_actor_hide()
       * wrapper so the entire hide signal emission completes first
       * (?)
       */
      clutter_actor_update_map_state (self, MAP_STATE_CHECK);

      /* we queue a relayout unless the actor is inside a
       * container that explicitly told us not to
       */
      if (priv->parent_actor &&
          (!(priv->parent_actor->flags & CLUTTER_ACTOR_NO_LAYOUT)))
        clutter_actor_queue_relayout (priv->parent_actor);
    }
}

/**
 * clutter_actor_hide:
 * @self: A #ClutterActor
 *
 * Flags an actor to be hidden. A hidden actor will not be
 * rendered on the stage.
 *
 * Actors are visible by default.
 *
 * If this function is called on an actor without a parent, the
 * #ClutterActor:show-on-set-parent property will be set to %FALSE
 * as a side-effect.
 */
void
clutter_actor_hide (ClutterActor *self)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  /* simple optimization */
  if (!CLUTTER_ACTOR_IS_VISIBLE (self))
    {
      /* we still need to set the :show-on-set-parent property, in
       * case hide() is called on an unparented actor
       */
      set_show_on_set_parent (self, FALSE);
      return;
    }

#ifdef CLUTTER_ENABLE_DEBUG
  clutter_actor_verify_map_state (self);
#endif

  priv = self->priv;

  g_object_freeze_notify (G_OBJECT (self));

  set_show_on_set_parent (self, FALSE);

  g_signal_emit (self, actor_signals[HIDE], 0);
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_VISIBLE]);

  if (priv->parent_actor)
    clutter_actor_queue_redraw (priv->parent_actor);

  g_object_thaw_notify (G_OBJECT (self));
}

/**
 * clutter_actor_hide_all:
 * @self: a #ClutterActor
 *
 * Calls clutter_actor_hide() on all child actors (if any).
 *
 * Since: 0.2
 */
void
clutter_actor_hide_all (ClutterActor *self)
{
  ClutterActorClass *klass;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  klass = CLUTTER_ACTOR_GET_CLASS (self);
  if (klass->hide_all)
    klass->hide_all (self);
}

/**
 * clutter_actor_realize:
 * @self: A #ClutterActor
 *
 * Realization informs the actor that it is attached to a stage. It
 * can use this to allocate resources if it wanted to delay allocation
 * until it would be rendered. However it is perfectly acceptable for
 * an actor to create resources before being realized because Clutter
 * only ever has a single rendering context so that actor is free to
 * be moved from one stage to another.
 *
 * This function does nothing if the actor is already realized.
 *
 * Because a realized actor must have realized parent actors, calling
 * clutter_actor_realize() will also realize all parents of the actor.
 *
 * This function does not realize child actors, except in the special
 * case that realizing the stage, when the stage is visible, will
 * suddenly map (and thus realize) the children of the stage.
 **/
void
clutter_actor_realize (ClutterActor *self)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

#ifdef CLUTTER_ENABLE_DEBUG
  clutter_actor_verify_map_state (self);
#endif

  if (CLUTTER_ACTOR_IS_REALIZED (self))
    return;

  /* To be realized, our parent actors must be realized first.
   * This will only succeed if we're inside a toplevel.
   */
  if (priv->parent_actor != NULL)
    clutter_actor_realize (priv->parent_actor);

  if (CLUTTER_ACTOR_IS_TOPLEVEL (self))
    {
      /* toplevels can be realized at any time */
    }
  else
    {
      /* "Fail" the realization if parent is missing or unrealized;
       * this should really be a g_warning() not some kind of runtime
       * failure; how can an app possibly recover? Instead it's a bug
       * in the app and the app should get an explanatory warning so
       * someone can fix it. But for now it's too hard to fix this
       * because e.g. ClutterTexture needs reworking.
       */
      if (priv->parent_actor == NULL ||
          !CLUTTER_ACTOR_IS_REALIZED (priv->parent_actor))
        return;
    }

  CLUTTER_NOTE (ACTOR, "Realizing actor '%s'", _clutter_actor_get_debug_name (self));

  CLUTTER_ACTOR_SET_FLAGS (self, CLUTTER_ACTOR_REALIZED);
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_REALIZED]);

  g_signal_emit (self, actor_signals[REALIZE], 0);

  /* Stage actor is allowed to unset the realized flag again in its
   * default signal handler, though that is a pathological situation.
   */

  /* If realization "failed" we'll have to update child state. */
  clutter_actor_update_map_state (self, MAP_STATE_CHECK);
}

void
clutter_actor_real_unrealize (ClutterActor *self)
{
  /* we must be unmapped (implying our children are also unmapped) */
  g_assert (!CLUTTER_ACTOR_IS_MAPPED (self));
}

/**
 * clutter_actor_unrealize:
 * @self: A #ClutterActor
 *
 * Unrealization informs the actor that it may be being destroyed or
 * moved to another stage. The actor may want to destroy any
 * underlying graphics resources at this point. However it is
 * perfectly acceptable for it to retain the resources until the actor
 * is destroyed because Clutter only ever uses a single rendering
 * context and all of the graphics resources are valid on any stage.
 *
 * Because mapped actors must be realized, actors may not be
 * unrealized if they are mapped. This function hides the actor to be
 * sure it isn't mapped, an application-visible side effect that you
 * may not be expecting.
 *
 * This function should not be called by application code.
 */
void
clutter_actor_unrealize (ClutterActor *self)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (!CLUTTER_ACTOR_IS_MAPPED (self));

/* This function should not really be in the public API, because
 * there isn't a good reason to call it. ClutterActor will already
 * unrealize things for you when it's important to do so.
 *
 * If you were using clutter_actor_unrealize() in a dispose
 * implementation, then don't, just chain up to ClutterActor's
 * dispose.
 *
 * If you were using clutter_actor_unrealize() to implement
 * unrealizing children of your container, then don't, ClutterActor
 * will already take care of that.
 *
 * If you were using clutter_actor_unrealize() to re-realize to
 * create your resources in a different way, then use
 * _clutter_actor_rerealize() (inside Clutter) or just call your
 * code that recreates your resources directly (outside Clutter).
 */

#ifdef CLUTTER_ENABLE_DEBUG
  clutter_actor_verify_map_state (self);
#endif

  clutter_actor_hide (self);

  clutter_actor_unrealize_not_hiding (self);
}

static ClutterActorTraverseVisitFlags
unrealize_actor_before_children_cb (ClutterActor *self,
                                    int depth,
                                    void *user_data)
{
  /* If an actor is already unrealized we know its children have also
   * already been unrealized... */
  if (!CLUTTER_ACTOR_IS_REALIZED (self))
    return CLUTTER_ACTOR_TRAVERSE_VISIT_SKIP_CHILDREN;

  g_signal_emit (self, actor_signals[UNREALIZE], 0);

  return CLUTTER_ACTOR_TRAVERSE_VISIT_CONTINUE;
}

static ClutterActorTraverseVisitFlags
unrealize_actor_after_children_cb (ClutterActor *self,
                                   int depth,
                                   void *user_data)
{
  /* We want to unset the realized flag only _after_
   * child actors are unrealized, to maintain invariants.
   */
  CLUTTER_ACTOR_UNSET_FLAGS (self, CLUTTER_ACTOR_REALIZED);
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_REALIZED]);
  return CLUTTER_ACTOR_TRAVERSE_VISIT_CONTINUE;
}

/*
 * clutter_actor_unrealize_not_hiding:
 * @self: A #ClutterActor
 *
 * Unrealization informs the actor that it may be being destroyed or
 * moved to another stage. The actor may want to destroy any
 * underlying graphics resources at this point. However it is
 * perfectly acceptable for it to retain the resources until the actor
 * is destroyed because Clutter only ever uses a single rendering
 * context and all of the graphics resources are valid on any stage.
 *
 * Because mapped actors must be realized, actors may not be
 * unrealized if they are mapped. You must hide the actor or one of
 * its parents before attempting to unrealize.
 *
 * This function is separate from clutter_actor_unrealize() because it
 * does not automatically hide the actor.
 * Actors need not be hidden to be unrealized, they just need to
 * be unmapped. In fact we don't want to mess up the application's
 * setting of the "visible" flag, so hiding is very undesirable.
 *
 * clutter_actor_unrealize() does a clutter_actor_hide() just for
 * backward compatibility.
 */
static void
clutter_actor_unrealize_not_hiding (ClutterActor *self)
{
  _clutter_actor_traverse (self,
                           CLUTTER_ACTOR_TRAVERSE_DEPTH_FIRST,
                           unrealize_actor_before_children_cb,
                           unrealize_actor_after_children_cb,
                           NULL);
}

/*
 * _clutter_actor_rerealize:
 * @self: A #ClutterActor
 * @callback: Function to call while unrealized
 * @data: data for callback
 *
 * If an actor is already unrealized, this just calls the callback.
 *
 * If it is realized, it unrealizes temporarily, calls the callback,
 * and then re-realizes the actor.
 *
 * As a side effect, leaves all children of the actor unrealized if
 * the actor was realized but not showing.  This is because when we
 * unrealize the actor temporarily we must unrealize its children
 * (e.g. children of a stage can't be realized if stage window is
 * gone). And we aren't clever enough to save the realization state of
 * all children. In most cases this should not matter, because
 * the children will automatically realize when they next become mapped.
 */
void
_clutter_actor_rerealize (ClutterActor    *self,
                          ClutterCallback  callback,
                          void            *data)
{
  gboolean was_mapped;
  gboolean was_showing;
  gboolean was_realized;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

#ifdef CLUTTER_ENABLE_DEBUG
  clutter_actor_verify_map_state (self);
#endif

  was_realized = CLUTTER_ACTOR_IS_REALIZED (self);
  was_mapped = CLUTTER_ACTOR_IS_MAPPED (self);
  was_showing = CLUTTER_ACTOR_IS_VISIBLE (self);

  /* Must be unmapped to unrealize. Note we only have to hide this
   * actor if it was mapped (if all parents were showing).  If actor
   * is merely visible (but not mapped), then that's fine, we can
   * leave it visible.
   */
  if (was_mapped)
    clutter_actor_hide (self);

  g_assert (!CLUTTER_ACTOR_IS_MAPPED (self));

  /* unrealize self and all children */
  clutter_actor_unrealize_not_hiding (self);

  if (callback != NULL)
    {
      (* callback) (self, data);
    }

  if (was_showing)
    clutter_actor_show (self); /* will realize only if mapping implies it */
  else if (was_realized)
    clutter_actor_realize (self); /* realize self and all parents */
}

static void
clutter_actor_real_pick (ClutterActor       *self,
			 const ClutterColor *color)
{
  /* the default implementation is just to paint a rectangle
   * with the same size of the actor using the passed color
   */
  if (clutter_actor_should_pick_paint (self))
    {
      ClutterActorBox box = { 0, };
      float width, height;

      clutter_actor_get_allocation_box (self, &box);

      width = box.x2 - box.x1;
      height = box.y2 - box.y1;

      cogl_set_source_color4ub (color->red,
                                color->green,
                                color->blue,
                                color->alpha);

      cogl_rectangle (0, 0, width, height);
    }
}

/**
 * clutter_actor_should_pick_paint:
 * @self: A #ClutterActor
 *
 * Should be called inside the implementation of the
 * #ClutterActor::pick virtual function in order to check whether
 * the actor should paint itself in pick mode or not.
 *
 * This function should never be called directly by applications.
 *
 * Return value: %TRUE if the actor should paint its silhouette,
 *   %FALSE otherwise
 */
gboolean
clutter_actor_should_pick_paint (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), FALSE);

  if (CLUTTER_ACTOR_IS_MAPPED (self) &&
      (_clutter_context_get_pick_mode () == CLUTTER_PICK_ALL ||
       CLUTTER_ACTOR_IS_REACTIVE (self)))
    return TRUE;

  return FALSE;
}

static void
clutter_actor_real_get_preferred_width (ClutterActor *self,
                                        gfloat        for_height,
                                        gfloat       *min_width_p,
                                        gfloat       *natural_width_p)
{
  /* Default implementation is always 0x0, usually an actor
   * using this default is relying on someone to set the
   * request manually
   */
  CLUTTER_NOTE (LAYOUT, "Default preferred width: 0, 0");

  if (min_width_p)
    *min_width_p = 0;

  if (natural_width_p)
    *natural_width_p = 0;
}

static void
clutter_actor_real_get_preferred_height (ClutterActor *self,
                                         gfloat        for_width,
                                         gfloat       *min_height_p,
                                         gfloat       *natural_height_p)
{
  /* Default implementation is always 0x0, usually an actor
   * using this default is relying on someone to set the
   * request manually
   */
  CLUTTER_NOTE (LAYOUT, "Default preferred height: 0, 0");

  if (min_height_p)
    *min_height_p = 0;

  if (natural_height_p)
    *natural_height_p = 0;
}

static void
clutter_actor_store_old_geometry (ClutterActor    *self,
                                  ClutterActorBox *box)
{
  *box = self->priv->allocation;
}

static inline void
clutter_actor_notify_if_geometry_changed (ClutterActor          *self,
                                          const ClutterActorBox *old)
{
  ClutterActorPrivate *priv = self->priv;
  GObject *obj = G_OBJECT (self);

  g_object_freeze_notify (obj);

  /* to avoid excessive requisition or allocation cycles we
   * use the cached values.
   *
   * - if we don't have an allocation we assume that we need
   *   to notify anyway
   * - if we don't have a width or a height request we notify
   *   width and height
   * - if we have a valid allocation then we check the old
   *   bounding box with the current allocation and we notify
   *   the changes
   */
  if (priv->needs_allocation)
    {
      g_object_notify_by_pspec (obj, obj_props[PROP_X]);
      g_object_notify_by_pspec (obj, obj_props[PROP_Y]);
      g_object_notify_by_pspec (obj, obj_props[PROP_WIDTH]);
      g_object_notify_by_pspec (obj, obj_props[PROP_HEIGHT]);
    }
  else if (priv->needs_width_request || priv->needs_height_request)
    {
      g_object_notify_by_pspec (obj, obj_props[PROP_WIDTH]);
      g_object_notify_by_pspec (obj, obj_props[PROP_HEIGHT]);
    }
  else
    {
      gfloat xu, yu;
      gfloat widthu, heightu;

      xu = priv->allocation.x1;
      yu = priv->allocation.y1;
      widthu = priv->allocation.x2 - priv->allocation.x1;
      heightu = priv->allocation.y2 - priv->allocation.y1;

      if (xu != old->x1)
        g_object_notify_by_pspec (obj, obj_props[PROP_X]);

      if (yu != old->y1)
        g_object_notify_by_pspec (obj, obj_props[PROP_Y]);

      if (widthu != (old->x2 - old->x1))
        g_object_notify_by_pspec (obj, obj_props[PROP_WIDTH]);

      if (heightu != (old->y2 - old->y1))
        g_object_notify_by_pspec (obj, obj_props[PROP_HEIGHT]);
    }

  g_object_thaw_notify (obj);
}

static void
clutter_actor_real_allocate (ClutterActor           *self,
                             const ClutterActorBox  *box,
                             ClutterAllocationFlags  flags)
{
  ClutterActorPrivate *priv = self->priv;
  gboolean x1_changed, y1_changed, x2_changed, y2_changed;
  gboolean flags_changed;
  ClutterActorBox old_alloc = { 0, };

  clutter_actor_store_old_geometry (self, &old_alloc);

  x1_changed = priv->allocation.x1 != box->x1;
  y1_changed = priv->allocation.y1 != box->y1;
  x2_changed = priv->allocation.x2 != box->x2;
  y2_changed = priv->allocation.y2 != box->y2;

  flags_changed = priv->allocation_flags != flags;

  priv->allocation = *box;
  priv->allocation_flags = flags;
  priv->needs_allocation = FALSE;

  g_object_freeze_notify (G_OBJECT (self));

  if (x1_changed || y1_changed || x2_changed || y2_changed || flags_changed)
    {
      CLUTTER_NOTE (LAYOUT, "Allocation for '%s' changed",
                    _clutter_actor_get_debug_name (self));

      priv->transform_valid = FALSE;

      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ALLOCATION]);

      /* we also emit the ::allocation-changed signal for people
       * that wish to track the allocation flags
       */
      g_signal_emit (self, actor_signals[ALLOCATION_CHANGED], 0,
                     &priv->allocation,
                     flags);
    }

  clutter_actor_notify_if_geometry_changed (self, &old_alloc);

  g_object_thaw_notify (G_OBJECT (self));
}

static void
_clutter_actor_signal_queue_redraw (ClutterActor *self,
                                    ClutterActor *origin)
{
  /* no point in queuing a redraw on a destroyed actor */
  if (CLUTTER_ACTOR_IN_DESTRUCTION (self))
    return;

  /* NB: We can't bail out early here if the actor is hidden in case
   * the actor bas been cloned. In this case the clone will need to
   * receive the signal so it can queue its own redraw.
   */

  /* calls klass->queue_redraw in default handler */
  g_signal_emit (self, actor_signals[QUEUE_REDRAW], 0, origin);
}

static void
clutter_actor_real_queue_redraw (ClutterActor *self,
                                 ClutterActor *origin)
{
  ClutterActor *parent;

  CLUTTER_NOTE (PAINT, "Redraw queued on '%s' (from: '%s')",
                _clutter_actor_get_debug_name (self),
                origin != NULL ? _clutter_actor_get_debug_name (origin)
                               : "same actor");

  /* no point in queuing a redraw on a destroyed actor */
  if (CLUTTER_ACTOR_IN_DESTRUCTION (self))
    return;

  /* If the actor isn't visible, we still had to emit the signal
   * to allow for a ClutterClone, but the appearance of the parent
   * won't change so we don't have to propagate up the hierarchy.
   */
  if (!CLUTTER_ACTOR_IS_VISIBLE (self))
    return;

  /* Although we could determine here that a full stage redraw
   * has already been queued and immediately bail out, we actually
   * guarantee that we will propagate a queue-redraw signal to our
   * parent at least once so that it's possible to implement a
   * container that tracks which of its children have queued a
   * redraw.
   */
  if (self->priv->propagated_one_redraw)
    {
      ClutterActor *stage = _clutter_actor_get_stage_internal (self);
      if (stage != NULL &&
          _clutter_stage_has_full_redraw_queued (CLUTTER_STAGE (stage)))
        return;
    }

  self->priv->propagated_one_redraw = TRUE;

  /* If the queue redraw is coming from a child actor then we'll
     assume the queued effect is no longer valid. If this actor has
     had a redraw queued then that will mean it will instead redraw
     the whole actor. If it hasn't had a redraw queued then it will
     stay that way */
  if (self != origin)
    self->priv->effect_to_redraw = NULL;

  /* notify parents, if they are all visible eventually we'll
   * queue redraw on the stage, which queues the redraw idle.
   */
  parent = clutter_actor_get_parent (self);
  if (parent != NULL)
    {
      /* this will go up recursively */
      _clutter_actor_signal_queue_redraw (parent, origin);
    }
}

void
clutter_actor_real_queue_relayout (ClutterActor *self)
{
  ClutterActorPrivate *priv = self->priv;

  /* no point in queueing a redraw on a destroyed actor */
  if (CLUTTER_ACTOR_IN_DESTRUCTION (self))
    return;

  priv->needs_width_request  = TRUE;
  priv->needs_height_request = TRUE;
  priv->needs_allocation     = TRUE;

  /* reset the cached size requests */
  memset (priv->width_requests, 0,
          N_CACHED_SIZE_REQUESTS * sizeof (SizeRequest));
  memset (priv->height_requests, 0,
          N_CACHED_SIZE_REQUESTS * sizeof (SizeRequest));

  /* We need to go all the way up the hierarchy */
  if (priv->parent_actor != NULL)
    _clutter_actor_queue_only_relayout (priv->parent_actor);
}

/**
 * clutter_actor_apply_relative_transform_to_point:
 * @self: A #ClutterActor
 * @ancestor: (allow-none): A #ClutterActor ancestor, or %NULL to use the
 *   default #ClutterStage
 * @point: A point as #ClutterVertex
 * @vertex: The translated #ClutterVertex
 *
 * Transforms @point in coordinates relative to the actor into
 * ancestor-relative coordinates using the relevant transform
 * stack (i.e. scale, rotation, etc).
 *
 * If @ancestor is %NULL the ancestor will be the #ClutterStage. In
 * this case, the coordinates returned will be the coordinates on
 * the stage before the projection is applied. This is different from
 * the behaviour of clutter_actor_apply_transform_to_point().
 *
 * Since: 0.6
 */
void
clutter_actor_apply_relative_transform_to_point (ClutterActor        *self,
						 ClutterActor        *ancestor,
						 const ClutterVertex *point,
						 ClutterVertex       *vertex)
{
  gfloat w;
  CoglMatrix matrix;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (ancestor == NULL || CLUTTER_IS_ACTOR (ancestor));
  g_return_if_fail (point != NULL);
  g_return_if_fail (vertex != NULL);

  *vertex = *point;
  w = 1.0;

  if (ancestor == NULL)
    ancestor = _clutter_actor_get_stage_internal (self);

  if (ancestor == NULL)
    {
      *vertex = *point;
      return;
    }

  _clutter_actor_get_relative_modelview (self, ancestor, &matrix);
  cogl_matrix_transform_point (&matrix, &vertex->x, &vertex->y, &vertex->z, &w);
}

static gboolean
_clutter_actor_fully_transform_vertices (ClutterActor *self,
                                         const ClutterVertex *vertices_in,
                                         ClutterVertex *vertices_out,
                                         int n_vertices)
{
  ClutterActor *stage;
  CoglMatrix modelview;
  CoglMatrix projection;
  float viewport[4];

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), FALSE);

  /* NB: _clutter_actor_apply_modelview_transform_recursive will never
   * include the transformation between stage coordinates and OpenGL
   * eye coordinates, we have to explicitly use the
   * stage->apply_transform to get that... */
  stage = _clutter_actor_get_stage_internal (self);

  /* We really can't do anything meaningful in this case so don't try
   * to do any transform */
  if (stage == NULL)
    return FALSE;

  /* Setup the modelview */
  cogl_matrix_init_identity (&modelview);
  _clutter_actor_apply_modelview_transform (stage, &modelview);
  _clutter_actor_apply_modelview_transform_recursive (self, stage, &modelview);

  /* Fetch the projection and viewport */
  _clutter_stage_get_projection_matrix (CLUTTER_STAGE (stage), &projection);
  _clutter_stage_get_viewport (CLUTTER_STAGE (stage),
                               &viewport[0],
                               &viewport[1],
                               &viewport[2],
                               &viewport[3]);

  _clutter_util_fully_transform_vertices (&modelview,
                                          &projection,
                                          viewport,
                                          vertices_in,
                                          vertices_out,
                                          n_vertices);

  return TRUE;
}

/**
 * clutter_actor_apply_transform_to_point:
 * @self: A #ClutterActor
 * @point: A point as #ClutterVertex
 * @vertex: The translated #ClutterVertex
 *
 * Transforms @point in coordinates relative to the actor
 * into screen-relative coordinates with the current actor
 * transformation (i.e. scale, rotation, etc)
 *
 * Since: 0.4
 **/
void
clutter_actor_apply_transform_to_point (ClutterActor        *self,
                                        const ClutterVertex *point,
                                        ClutterVertex       *vertex)
{
  g_return_if_fail (point != NULL);
  g_return_if_fail (vertex != NULL);
  _clutter_actor_fully_transform_vertices (self, point, vertex, 1);
}

/* _clutter_actor_get_relative_modelview:
 *
 * Retrieves the modelview transformation relative to some ancestor
 * actor, or the stage if NULL is given for the ancestor.
 *
 * Note: This will never include the transformations from
 * stage::apply_transform since that would give you a modelview
 * transform relative to the OpenGL window coordinate space that the
 * stage lies within.
 *
 * If you need to do a full modelview + projective transform and get
 * to window coordinates then you should explicitly apply the stage
 * transform to an identity matrix and use
 * _clutter_actor_apply_modelview_transform like:
 *
 *   cogl_matrix_init_identity (&mtx);
 *   stage = _clutter_actor_get_stage_internal (self);
 *   _clutter_actor_apply_modelview_transform (stage, &mtx);
 */
/* FIXME: We should be caching the stage relative modelview along with the
 * actor itself */
static void
_clutter_actor_get_relative_modelview (ClutterActor *self,
                                       ClutterActor *ancestor,
                                       CoglMatrix *matrix)
{
  g_return_if_fail (ancestor != NULL);

  cogl_matrix_init_identity (matrix);

  _clutter_actor_apply_modelview_transform_recursive (self, ancestor, matrix);
}

/* Project the given @box into stage window coordinates, writing the
 * transformed vertices to @verts[]. */
static gboolean
_clutter_actor_transform_and_project_box (ClutterActor          *self,
					  const ClutterActorBox *box,
					  ClutterVertex          verts[])
{
  ClutterVertex box_vertices[4];

  box_vertices[0].x = box->x1;
  box_vertices[0].y = box->y1;
  box_vertices[0].z = 0;
  box_vertices[1].x = box->x2;
  box_vertices[1].y = box->y1;
  box_vertices[1].z = 0;
  box_vertices[2].x = box->x1;
  box_vertices[2].y = box->y2;
  box_vertices[2].z = 0;
  box_vertices[3].x = box->x2;
  box_vertices[3].y = box->y2;
  box_vertices[3].z = 0;

  return
    _clutter_actor_fully_transform_vertices (self, box_vertices, verts, 4);
}

/**
 * clutter_actor_get_allocation_vertices:
 * @self: A #ClutterActor
 * @ancestor: (allow-none): A #ClutterActor to calculate the vertices
 *   against, or %NULL to use the #ClutterStage
 * @verts: (out) (array fixed-size=4) (element-type Clutter.Vertex): return
 *   location for an array of 4 #ClutterVertex in which to store the result
 *
 * Calculates the transformed coordinates of the four corners of the
 * actor in the plane of @ancestor. The returned vertices relate to
 * the #ClutterActorBox coordinates as follows:
 * <itemizedlist>
 *   <listitem><para>@verts[0] contains (x1, y1)</para></listitem>
 *   <listitem><para>@verts[1] contains (x2, y1)</para></listitem>
 *   <listitem><para>@verts[2] contains (x1, y2)</para></listitem>
 *   <listitem><para>@verts[3] contains (x2, y2)</para></listitem>
 * </itemizedlist>
 *
 * If @ancestor is %NULL the ancestor will be the #ClutterStage. In
 * this case, the coordinates returned will be the coordinates on
 * the stage before the projection is applied. This is different from
 * the behaviour of clutter_actor_get_abs_allocation_vertices().
 *
 * Since: 0.6
 */
void
clutter_actor_get_allocation_vertices (ClutterActor  *self,
                                       ClutterActor  *ancestor,
                                       ClutterVertex  verts[])
{
  ClutterActorPrivate *priv;
  ClutterActorBox box;
  ClutterVertex vertices[4];
  CoglMatrix modelview;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (ancestor == NULL || CLUTTER_IS_ACTOR (ancestor));

  if (ancestor == NULL)
    ancestor = _clutter_actor_get_stage_internal (self);

  /* Fallback to a NOP transform if the actor isn't parented under a
   * stage. */
  if (ancestor == NULL)
    ancestor = self;

  priv = self->priv;

  /* if the actor needs to be allocated we force a relayout, so that
   * we will have valid values to use in the transformations */
  if (priv->needs_allocation)
    {
      ClutterActor *stage = _clutter_actor_get_stage_internal (self);
      if (stage)
        _clutter_stage_maybe_relayout (stage);
      else
        {
          box.x1 = box.y1 = 0;
          /* The result isn't really meaningful in this case but at
           * least try to do something *vaguely* reasonable... */
          clutter_actor_get_size (self, &box.x2, &box.y2);
        }
    }

  clutter_actor_get_allocation_box (self, &box);

  vertices[0].x = box.x1;
  vertices[0].y = box.y1;
  vertices[0].z = 0;
  vertices[1].x = box.x2;
  vertices[1].y = box.y1;
  vertices[1].z = 0;
  vertices[2].x = box.x1;
  vertices[2].y = box.y2;
  vertices[2].z = 0;
  vertices[3].x = box.x2;
  vertices[3].y = box.y2;
  vertices[3].z = 0;

  _clutter_actor_get_relative_modelview (self, ancestor, &modelview);

  cogl_matrix_transform_points (&modelview,
                                3,
                                sizeof (ClutterVertex),
                                vertices,
                                sizeof (ClutterVertex),
                                vertices,
                                4);
}

/**
 * clutter_actor_get_abs_allocation_vertices:
 * @self: A #ClutterActor
 * @verts: (out) (array fixed-size=4): Pointer to a location of an array
 *   of 4 #ClutterVertex where to store the result.
 *
 * Calculates the transformed screen coordinates of the four corners of
 * the actor; the returned vertices relate to the #ClutterActorBox
 * coordinates  as follows:
 * <itemizedlist>
 *   <listitem><para>v[0] contains (x1, y1)</para></listitem>
 *   <listitem><para>v[1] contains (x2, y1)</para></listitem>
 *   <listitem><para>v[2] contains (x1, y2)</para></listitem>
 *   <listitem><para>v[3] contains (x2, y2)</para></listitem>
 * </itemizedlist>
 *
 * Since: 0.4
 */
void
clutter_actor_get_abs_allocation_vertices (ClutterActor  *self,
                                           ClutterVertex  verts[])
{
  ClutterActorPrivate *priv;
  ClutterActorBox actor_space_allocation;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  /* if the actor needs to be allocated we force a relayout, so that
   * the actor allocation box will be valid for
   * _clutter_actor_transform_and_project_box()
   */
  if (priv->needs_allocation)
    {
      ClutterActor *stage = _clutter_actor_get_stage_internal (self);
      /* There's nothing meaningful we can do now */
      if (!stage)
        return;

      _clutter_stage_maybe_relayout (stage);
    }

  /* NB: _clutter_actor_transform_and_project_box expects a box in the actor's
   * own coordinate space... */
  actor_space_allocation.x1 = 0;
  actor_space_allocation.y1 = 0;
  actor_space_allocation.x2 = priv->allocation.x2 - priv->allocation.x1;
  actor_space_allocation.y2 = priv->allocation.y2 - priv->allocation.y1;
  _clutter_actor_transform_and_project_box (self,
					    &actor_space_allocation,
					    verts);
}

static void
clutter_actor_real_apply_transform (ClutterActor *self,
                                    CoglMatrix   *matrix)
{
  ClutterActorPrivate *priv = self->priv;

  if (!priv->transform_valid)
    {
      CoglMatrix *transform = &priv->transform;

      cogl_matrix_init_identity (transform);

      cogl_matrix_translate (transform,
                             priv->allocation.x1,
                             priv->allocation.y1,
                             0.0);

      if (priv->z)
        cogl_matrix_translate (transform, 0, 0, priv->z);

      /*
       * because the rotation involves translations, we must scale
       * before applying the rotations (if we apply the scale after
       * the rotations, the translations included in the rotation are
       * not scaled and so the entire object will move on the screen
       * as a result of rotating it).
       */
      if (priv->scale_x != 1.0 || priv->scale_y != 1.0)
        {
          TRANSFORM_ABOUT_ANCHOR_COORD (self, transform,
                                        &priv->scale_center,
                                        cogl_matrix_scale (transform,
                                                           priv->scale_x,
                                                           priv->scale_y,
                                                           1.0));
        }

      if (priv->rzang)
        TRANSFORM_ABOUT_ANCHOR_COORD (self, transform,
                                      &priv->rz_center,
                                      cogl_matrix_rotate (transform,
                                                          priv->rzang,
                                                          0, 0, 1.0));

      if (priv->ryang)
        TRANSFORM_ABOUT_ANCHOR_COORD (self, transform,
                                      &priv->ry_center,
                                      cogl_matrix_rotate (transform,
                                                          priv->ryang,
                                                          0, 1.0, 0));

      if (priv->rxang)
        TRANSFORM_ABOUT_ANCHOR_COORD (self, transform,
                                      &priv->rx_center,
                                      cogl_matrix_rotate (transform,
                                                          priv->rxang,
                                                          1.0, 0, 0));

      if (!clutter_anchor_coord_is_zero (&priv->anchor))
        {
          gfloat x, y, z;

          clutter_anchor_coord_get_units (self, &priv->anchor, &x, &y, &z);
          cogl_matrix_translate (transform, -x, -y, -z);
        }

      priv->transform_valid = TRUE;
    }

  cogl_matrix_multiply (matrix, matrix, &priv->transform);
}

/* Applies the transforms associated with this actor to the given
 * matrix. */
void
_clutter_actor_apply_modelview_transform (ClutterActor *self,
                                          CoglMatrix *matrix)
{
  CLUTTER_ACTOR_GET_CLASS (self)->apply_transform (self, matrix);
}

/* Recursively applies the transforms associated with this actor and
 * its ancestors to the given matrix. Use NULL if you want this
 * to go all the way down to the stage.
 */
void
_clutter_actor_apply_modelview_transform_recursive (ClutterActor *self,
						    ClutterActor *ancestor,
                                                    CoglMatrix *matrix)
{
  ClutterActor *parent;

  /* Note we terminate before ever calling stage->apply_transform()
   * since that would conceptually be relative to the underlying
   * window OpenGL coordinates so we'd need a special @ancestor
   * value to represent the fake parent of the stage. */
  if (self == ancestor)
    return;

  parent = clutter_actor_get_parent (self);

  if (parent != NULL)
    _clutter_actor_apply_modelview_transform_recursive (parent, ancestor,
                                                        matrix);

  _clutter_actor_apply_modelview_transform (self, matrix);
}

static void
_clutter_actor_draw_paint_volume_full (ClutterActor *self,
                                       ClutterPaintVolume *pv,
                                       const char *label,
                                       const CoglColor *color)
{
  static CoglMaterial *outline = NULL;
  CoglHandle vbo;
  ClutterVertex line_ends[12 * 2];
  int n_vertices;

  if (outline == NULL)
    outline = cogl_material_new ();

  _clutter_paint_volume_complete (pv);

  n_vertices = pv->is_2d ? 4 * 2 : 12 * 2;

  /* Front face */
  line_ends[0] = pv->vertices[0]; line_ends[1] = pv->vertices[1];
  line_ends[2] = pv->vertices[1]; line_ends[3] = pv->vertices[2];
  line_ends[4] = pv->vertices[2]; line_ends[5] = pv->vertices[3];
  line_ends[6] = pv->vertices[3]; line_ends[7] = pv->vertices[0];

  if (!pv->is_2d)
    {
      /* Back face */
      line_ends[8] = pv->vertices[4]; line_ends[9] = pv->vertices[5];
      line_ends[10] = pv->vertices[5]; line_ends[11] = pv->vertices[6];
      line_ends[12] = pv->vertices[6]; line_ends[13] = pv->vertices[7];
      line_ends[14] = pv->vertices[7]; line_ends[15] = pv->vertices[4];

      /* Lines connecting front face to back face */
      line_ends[16] = pv->vertices[0]; line_ends[17] = pv->vertices[4];
      line_ends[18] = pv->vertices[1]; line_ends[19] = pv->vertices[5];
      line_ends[20] = pv->vertices[2]; line_ends[21] = pv->vertices[6];
      line_ends[22] = pv->vertices[3]; line_ends[23] = pv->vertices[7];
    }

  vbo = cogl_vertex_buffer_new (n_vertices);
  cogl_vertex_buffer_add (vbo,
                          "gl_Vertex",
                          3, /* n_components */
                          COGL_ATTRIBUTE_TYPE_FLOAT,
                          FALSE, /* normalized */
                          0, /* stride */
                          line_ends);

  cogl_material_set_color (outline, color);
  cogl_set_source (outline);
  cogl_vertex_buffer_draw (vbo, COGL_VERTICES_MODE_LINES,
                           0 , n_vertices);
  cogl_object_unref (vbo);

  if (label)
    {
      PangoLayout *layout;
      layout = pango_layout_new (clutter_actor_get_pango_context (self));
      pango_layout_set_text (layout, label, -1);
      cogl_pango_render_layout (layout,
                                pv->vertices[0].x,
                                pv->vertices[0].y,
                                color,
                                0);
      g_object_unref (layout);
    }
}

static void
_clutter_actor_draw_paint_volume (ClutterActor *self)
{
  ClutterPaintVolume *pv;
  CoglColor color;

  pv = _clutter_actor_get_paint_volume_mutable (self);
  if (!pv)
    {
      gfloat width, height;
      ClutterPaintVolume fake_pv;

      ClutterActor *stage = _clutter_actor_get_stage_internal (self);
      _clutter_paint_volume_init_static (&fake_pv, stage);

      clutter_actor_get_size (self, &width, &height);
      clutter_paint_volume_set_width (&fake_pv, width);
      clutter_paint_volume_set_height (&fake_pv, height);

      cogl_color_init_from_4f (&color, 0, 0, 1, 1);
      _clutter_actor_draw_paint_volume_full (self, &fake_pv,
                                             _clutter_actor_get_debug_name (self),
                                             &color);

      clutter_paint_volume_free (&fake_pv);
    }
  else
    {
      cogl_color_init_from_4f (&color, 0, 1, 0, 1);
      _clutter_actor_draw_paint_volume_full (self, pv,
                                             _clutter_actor_get_debug_name (self),
                                             &color);
    }
}

static void
_clutter_actor_paint_cull_result (ClutterActor *self,
                                  gboolean success,
                                  ClutterCullResult result)
{
  ClutterPaintVolume *pv;
  CoglColor color;

  if (success)
    {
      if (result == CLUTTER_CULL_RESULT_IN)
        cogl_color_init_from_4f (&color, 0, 1, 0, 1);
      else if (result == CLUTTER_CULL_RESULT_OUT)
        cogl_color_init_from_4f (&color, 0, 0, 1, 1);
      else
        cogl_color_init_from_4f (&color, 0, 1, 1, 1);
    }
  else
    cogl_color_init_from_4f (&color, 1, 1, 1, 1);

  if (success && (pv = _clutter_actor_get_paint_volume_mutable (self)))
    _clutter_actor_draw_paint_volume_full (self, pv,
                                           _clutter_actor_get_debug_name (self),
                                           &color);
  else
    {
      PangoLayout *layout;
      char *label =
        g_strdup_printf ("CULL FAILURE: %s", _clutter_actor_get_debug_name (self));
      cogl_color_init_from_4f (&color, 1, 1, 1, 1);
      cogl_set_source_color (&color);

      layout = pango_layout_new (clutter_actor_get_pango_context (self));
      pango_layout_set_text (layout, label, -1);
      cogl_pango_render_layout (layout,
                                0,
                                0,
                                &color,
                                0);
      g_free (label);
      g_object_unref (layout);
    }
}

static int clone_paint_level = 0;

void
_clutter_actor_push_clone_paint (void)
{
  clone_paint_level++;
}

void
_clutter_actor_pop_clone_paint (void)
{
  clone_paint_level--;
}

static gboolean
in_clone_paint (void)
{
  return clone_paint_level > 0;
}

/* Returns TRUE if the actor can be ignored */
/* FIXME: we should return a ClutterCullResult, and
 * clutter_actor_paint should understand that a CLUTTER_CULL_RESULT_IN
 * means there's no point in trying to cull descendants of the current
 * node. */
static gboolean
cull_actor (ClutterActor *self, ClutterCullResult *result_out)
{
  ClutterActorPrivate *priv = self->priv;
  ClutterActor *stage;
  const ClutterPlane *stage_clip;

  if (!priv->last_paint_volume_valid)
    {
      CLUTTER_NOTE (CLIPPING, "Bail from cull_actor without culling (%s): "
                    "->last_paint_volume_valid == FALSE",
                    _clutter_actor_get_debug_name (self));
      return FALSE;
    }

  if (G_UNLIKELY (clutter_paint_debug_flags & CLUTTER_DEBUG_DISABLE_CULLING))
    return FALSE;

  stage = _clutter_actor_get_stage_internal (self);
  stage_clip = _clutter_stage_get_clip (CLUTTER_STAGE (stage));
  if (G_UNLIKELY (!stage_clip))
    {
      CLUTTER_NOTE (CLIPPING, "Bail from cull_actor without culling (%s): "
                    "No stage clip set",
                    _clutter_actor_get_debug_name (self));
      return FALSE;
    }

  if (cogl_get_draw_framebuffer () !=
      _clutter_stage_get_active_framebuffer (CLUTTER_STAGE (stage)))
    {
      CLUTTER_NOTE (CLIPPING, "Bail from cull_actor without culling (%s): "
                    "Current framebuffer doesn't correspond to stage",
                    _clutter_actor_get_debug_name (self));
      return FALSE;
    }

  *result_out =
    _clutter_paint_volume_cull (&priv->last_paint_volume, stage_clip);
  return TRUE;
}

static void
_clutter_actor_update_last_paint_volume (ClutterActor *self)
{
  ClutterActorPrivate *priv = self->priv;
  const ClutterPaintVolume *pv;

  if (priv->last_paint_volume_valid)
    {
      clutter_paint_volume_free (&priv->last_paint_volume);
      priv->last_paint_volume_valid = FALSE;
    }

  pv = clutter_actor_get_paint_volume (self);
  if (!pv)
    {
      CLUTTER_NOTE (CLIPPING, "Bail from update_last_paint_volume (%s): "
                    "Actor failed to report a paint volume",
                    _clutter_actor_get_debug_name (self));
      return;
    }

  _clutter_paint_volume_copy_static (pv, &priv->last_paint_volume);

  _clutter_paint_volume_transform_relative (&priv->last_paint_volume,
                                            NULL); /* eye coordinates */

  priv->last_paint_volume_valid = TRUE;
}

static inline gboolean
actor_has_shader_data (ClutterActor *self)
{
  return g_object_get_qdata (G_OBJECT (self), quark_shader_data) != NULL;
}

guint32
_clutter_actor_get_pick_id (ClutterActor *self)
{
  if (self->priv->pick_id < 0)
    return 0;

  return self->priv->pick_id;
}

/* This is the same as clutter_actor_add_effect except that it doesn't
   queue a redraw and it doesn't notify on the effect property */
static void
_clutter_actor_add_effect_internal (ClutterActor  *self,
                                    ClutterEffect *effect)
{
  ClutterActorPrivate *priv = self->priv;

  if (priv->effects == NULL)
    {
      priv->effects = g_object_new (CLUTTER_TYPE_META_GROUP, NULL);
      priv->effects->actor = self;
    }

  _clutter_meta_group_add_meta (priv->effects, CLUTTER_ACTOR_META (effect));
}

/* This is the same as clutter_actor_remove_effect except that it doesn't
   queue a redraw and it doesn't notify on the effect property */
static void
_clutter_actor_remove_effect_internal (ClutterActor  *self,
                                       ClutterEffect *effect)
{
  ClutterActorPrivate *priv = self->priv;

  if (priv->effects == NULL)
    return;

  _clutter_meta_group_remove_meta (priv->effects, CLUTTER_ACTOR_META (effect));
}

static gboolean
needs_flatten_effect (ClutterActor *self)
{
  ClutterActorPrivate *priv = self->priv;

  switch (priv->offscreen_redirect)
    {
    case CLUTTER_OFFSCREEN_REDIRECT_AUTOMATIC_FOR_OPACITY:
      if (!clutter_actor_has_overlaps (self))
        return FALSE;
      /* flow through */
    case CLUTTER_OFFSCREEN_REDIRECT_ALWAYS_FOR_OPACITY:
      return clutter_actor_get_paint_opacity (self) < 255;

    case CLUTTER_OFFSCREEN_REDIRECT_ALWAYS:
      return TRUE;
    }

  g_assert_not_reached ();
}

static void
add_or_remove_flatten_effect (ClutterActor *self)
{
  ClutterActorPrivate *priv = self->priv;

  /* Add or remove the flatten effect depending on the
     offscreen-redirect property. */
  if (needs_flatten_effect (self))
    {
      if (priv->flatten_effect == NULL)
        {
          ClutterActorMeta *actor_meta;
          gint priority;

          priv->flatten_effect = _clutter_flatten_effect_new ();
          /* Keep a reference to the effect so that we can queue
             redraws from it */
          g_object_ref_sink (priv->flatten_effect);

          /* Set the priority of the effect to high so that it will
             always be applied to the actor first. It uses an internal
             priority so that it won't be visible to applications */
          actor_meta = CLUTTER_ACTOR_META (priv->flatten_effect);
          priority = CLUTTER_ACTOR_META_PRIORITY_INTERNAL_HIGH;
          _clutter_actor_meta_set_priority (actor_meta, priority);

          /* This will add the effect without queueing a redraw */
          _clutter_actor_add_effect_internal (self, priv->flatten_effect);
        }
    }
  else
    {
      if (priv->flatten_effect != NULL)
        {
          /* Destroy the effect so that it will lose its fbo cache of
             the actor */
          _clutter_actor_remove_effect_internal (self, priv->flatten_effect);
          g_object_unref (priv->flatten_effect);
          priv->flatten_effect = NULL;
        }
    }
}

/**
 * clutter_actor_paint:
 * @self: A #ClutterActor
 *
 * Renders the actor to display.
 *
 * This function should not be called directly by applications.
 * Call clutter_actor_queue_redraw() to queue paints, instead.
 *
 * This function is context-aware, and will either cause a
 * regular paint or a pick paint.
 *
 * This function will emit the #ClutterActor::paint signal or
 * the #ClutterActor::pick signal, depending on the context.
 *
 * This function does not paint the actor if the actor is set to 0,
 * unless it is performing a pick paint.
 */
void
clutter_actor_paint (ClutterActor *self)
{
  ClutterActorPrivate *priv;
  ClutterPickMode pick_mode;
  gboolean clip_set = FALSE;
  CLUTTER_STATIC_COUNTER (actor_paint_counter,
                          "Actor real-paint counter",
                          "Increments each time any actor is painted",
                          0 /* no application private data */);
  CLUTTER_STATIC_COUNTER (actor_pick_counter,
                          "Actor pick-paint counter",
                          "Increments each time any actor is painted "
                          "for picking",
                          0 /* no application private data */);

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  if (CLUTTER_ACTOR_IN_DESTRUCTION (self))
    return;

  priv = self->priv;

  pick_mode = _clutter_context_get_pick_mode ();

  /* It's an important optimization that we consider painting of
   * actors with 0 opacity to be a NOP... */
  if (pick_mode == CLUTTER_PICK_NONE &&
      /* ignore top-levels, since they might be transparent */
      !CLUTTER_ACTOR_IS_TOPLEVEL (self) &&
      /* Use the override opacity if its been set */
      ((priv->opacity_override >= 0) ?
       priv->opacity_override : priv->opacity) == 0)
    {
      priv->propagated_one_redraw = FALSE;
      return;
    }

  /* if we aren't paintable (not in a toplevel with all
   * parents paintable) then do nothing.
   */
  if (!CLUTTER_ACTOR_IS_MAPPED (self))
    return;

  /* mark that we are in the paint process */
  CLUTTER_SET_PRIVATE_FLAGS (self, CLUTTER_IN_PAINT);

  cogl_push_matrix();

  if (priv->enable_model_view_transform)
    {
      CoglMatrix matrix;
      /* XXX: It could be better to cache the modelview with the actor
       * instead of progressively building up the transformations on
       * the matrix stack every time we paint. */
      cogl_get_modelview_matrix (&matrix);
      _clutter_actor_apply_modelview_transform (self, &matrix);
      cogl_set_modelview_matrix (&matrix);
    }

  if (priv->has_clip)
    {
      cogl_clip_push_rectangle (priv->clip[0],
                                priv->clip[1],
                                priv->clip[0] + priv->clip[2],
                                priv->clip[1] + priv->clip[3]);
      clip_set = TRUE;
    }
  else if (priv->clip_to_allocation)
    {
      gfloat width, height;

      width  = priv->allocation.x2 - priv->allocation.x1;
      height = priv->allocation.y2 - priv->allocation.y1;

      cogl_clip_push_rectangle (0, 0, width, height);
      clip_set = TRUE;
    }

  if (pick_mode == CLUTTER_PICK_NONE)
    {
      CLUTTER_COUNTER_INC (_clutter_uprof_context, actor_paint_counter);

      /* We check whether we need to add the flatten effect before
         each paint so that we can avoid having a mechanism for
         applications to notify when the value of the
         has_overlaps virtual changes. */
      add_or_remove_flatten_effect (self);

      /* We save the current paint volume so that the next time the
       * actor queues a redraw we can constrain the redraw to just
       * cover the union of the new bounding box and the old.
       *
       * We also fetch the current paint volume to perform culling so
       * we can avoid painting actors outside the current clip region.
       *
       * If we are painting inside a clone, we should neither update
       * the paint volume or use it to cull painting, since the paint
       * box represents the location of the source actor on the
       * screen.
       *
       * XXX: We are starting to do a lot of vertex transforms on
       * the CPU in a typical paint, so at some point we should
       * audit these and consider caching some things.
       */
      if (!in_clone_paint ())
        {
          gboolean success;
          /* annoyingly gcc warns if uninitialized even though
           * the initialization is redundant :-( */
          ClutterCullResult result = CLUTTER_CULL_RESULT_IN;

          if (G_LIKELY ((clutter_paint_debug_flags &
                         (CLUTTER_DEBUG_DISABLE_CULLING |
                          CLUTTER_DEBUG_DISABLE_CLIPPED_REDRAWS)) !=
                        (CLUTTER_DEBUG_DISABLE_CULLING |
                         CLUTTER_DEBUG_DISABLE_CLIPPED_REDRAWS)))
            _clutter_actor_update_last_paint_volume (self);

          success = cull_actor (self, &result);

          if (G_UNLIKELY (clutter_paint_debug_flags & CLUTTER_DEBUG_REDRAWS))
            _clutter_actor_paint_cull_result (self, success, result);
          else if (result == CLUTTER_CULL_RESULT_OUT && success)
            goto done;
        }

      if (priv->effects == NULL)
        {
          if (actor_has_shader_data (self))
            clutter_actor_shader_pre_paint (self, FALSE);
          priv->next_effect_to_paint = NULL;
        }
      else
        priv->next_effect_to_paint =
          _clutter_meta_group_peek_metas (priv->effects);

      clutter_actor_continue_paint (self);

      if (priv->effects == NULL &&
          actor_has_shader_data (self))
        clutter_actor_shader_post_paint (self);

      if (G_UNLIKELY (clutter_paint_debug_flags & CLUTTER_DEBUG_PAINT_VOLUMES))
        _clutter_actor_draw_paint_volume (self);
    }
  else
    {
      ClutterColor col = { 0, };

      CLUTTER_COUNTER_INC (_clutter_uprof_context, actor_pick_counter);

      _clutter_id_to_color (_clutter_actor_get_pick_id (self), &col);

      /* Actor will then paint silhouette of itself in supplied
       * color.  See clutter_stage_get_actor_at_pos() for where
       * picking is enabled.
       */
      g_signal_emit (self, actor_signals[PICK], 0, &col);
    }

done:
  if (clip_set)
    cogl_clip_pop();

  cogl_pop_matrix();

  /* paint sequence complete */
  CLUTTER_UNSET_PRIVATE_FLAGS (self, CLUTTER_IN_PAINT);
}

/**
 * clutter_actor_continue_paint:
 * @self: A #ClutterActor
 *
 * Run the next stage of the paint sequence. This function should only
 * be called within the implementation of the ‘run’ virtual of a
 * #ClutterEffect. It will cause the run method of the next effect to
 * be applied, or it will paint the actual actor if the current effect
 * is the last effect in the chain.
 *
 * Since: 1.8
 */
void
clutter_actor_continue_paint (ClutterActor *self)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  /* This should only be called from with in the ‘run’ implementation
     of a ClutterEffect */
  g_return_if_fail (CLUTTER_ACTOR_IN_PAINT (self));

  priv = self->priv;

  /* Skip any effects that are disabled */
  while (priv->next_effect_to_paint &&
         !clutter_actor_meta_get_enabled (priv->next_effect_to_paint->data))
    priv->next_effect_to_paint = priv->next_effect_to_paint->next;

  /* If this has come from the last effect then we'll just paint the
     actual actor */
  if (priv->next_effect_to_paint == NULL)
    {
      priv->propagated_one_redraw = FALSE;

      g_signal_emit (self, actor_signals[PAINT], 0);
    }
  else
    {
      ClutterEffect *old_current_effect;
      ClutterEffectRunFlags run_flags = 0;

      /* Cache the current effect so that we can put it back before
         returning */
      old_current_effect = priv->current_effect;

      priv->current_effect = priv->next_effect_to_paint->data;
      priv->next_effect_to_paint = priv->next_effect_to_paint->next;

      if (priv->propagated_one_redraw)
        {
          /* If there's an effect queued with this redraw then all
             effects up to that one will be considered dirty. It is
             expected the queued effect will paint the cached image
             and not call clutter_actor_continue_paint again (although
             it should work ok if it does) */
          if (priv->effect_to_redraw == NULL ||
              priv->current_effect != priv->effect_to_redraw)
            run_flags |= CLUTTER_EFFECT_RUN_ACTOR_DIRTY;
        }

      _clutter_effect_run (priv->current_effect, run_flags);

      priv->current_effect = old_current_effect;
    }
}

/* internal helper function set the rotation angle without affecting
   the center point
 */
static void
clutter_actor_set_rotation_internal (ClutterActor      *self,
                                     ClutterRotateAxis  axis,
                                     gdouble            angle)
{
  ClutterActorPrivate *priv = self->priv;

  g_object_freeze_notify (G_OBJECT (self));

  priv->transform_valid = FALSE;

  switch (axis)
    {
    case CLUTTER_X_AXIS:
      priv->rxang = angle;
      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ROTATION_ANGLE_X]);
      break;

    case CLUTTER_Y_AXIS:
      priv->ryang = angle;
      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ROTATION_ANGLE_Y]);
      break;

    case CLUTTER_Z_AXIS:
      priv->rzang = angle;
      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ROTATION_ANGLE_Z]);
      break;
    }

  g_object_thaw_notify (G_OBJECT (self));

  clutter_actor_queue_redraw (self);
}

static void
clutter_actor_set_property (GObject      *object,
			    guint         prop_id,
			    const GValue *value,
			    GParamSpec   *pspec)
{
  ClutterActor *actor = CLUTTER_ACTOR (object);
  ClutterActorPrivate *priv = actor->priv;

  switch (prop_id)
    {
    case PROP_X:
      clutter_actor_set_x (actor, g_value_get_float (value));
      break;

    case PROP_Y:
      clutter_actor_set_y (actor, g_value_get_float (value));
      break;

    case PROP_WIDTH:
      clutter_actor_set_width (actor, g_value_get_float (value));
      break;

    case PROP_HEIGHT:
      clutter_actor_set_height (actor, g_value_get_float (value));
      break;

    case PROP_FIXED_X:
      clutter_actor_set_x (actor, g_value_get_float (value));
      break;

    case PROP_FIXED_Y:
      clutter_actor_set_y (actor, g_value_get_float (value));
      break;

    case PROP_FIXED_POSITION_SET:
      clutter_actor_set_fixed_position_set (actor, g_value_get_boolean (value));
      break;

    case PROP_MIN_WIDTH:
      clutter_actor_set_min_width (actor, g_value_get_float (value));
      break;

    case PROP_MIN_HEIGHT:
      clutter_actor_set_min_height (actor, g_value_get_float (value));
      break;

    case PROP_NATURAL_WIDTH:
      clutter_actor_set_natural_width (actor, g_value_get_float (value));
      break;

    case PROP_NATURAL_HEIGHT:
      clutter_actor_set_natural_height (actor, g_value_get_float (value));
      break;

    case PROP_MIN_WIDTH_SET:
      clutter_actor_set_min_width_set (actor, g_value_get_boolean (value));
      break;

    case PROP_MIN_HEIGHT_SET:
      clutter_actor_set_min_height_set (actor, g_value_get_boolean (value));
      break;

    case PROP_NATURAL_WIDTH_SET:
      clutter_actor_set_natural_width_set (actor, g_value_get_boolean (value));
      break;

    case PROP_NATURAL_HEIGHT_SET:
      clutter_actor_set_natural_height_set (actor, g_value_get_boolean (value));
      break;

    case PROP_REQUEST_MODE:
      clutter_actor_set_request_mode (actor, g_value_get_enum (value));
      break;

    case PROP_DEPTH:
      clutter_actor_set_depth (actor, g_value_get_float (value));
      break;

    case PROP_OPACITY:
      clutter_actor_set_opacity (actor, g_value_get_uint (value));
      break;

    case PROP_OFFSCREEN_REDIRECT:
      clutter_actor_set_offscreen_redirect (actor, g_value_get_enum (value));
      break;

    case PROP_NAME:
      clutter_actor_set_name (actor, g_value_get_string (value));
      break;

    case PROP_VISIBLE:
      if (g_value_get_boolean (value) == TRUE)
	clutter_actor_show (actor);
      else
	clutter_actor_hide (actor);
      break;

    case PROP_SCALE_X:
      clutter_actor_set_scale (actor,
                               g_value_get_double (value),
                               priv->scale_y);
      break;

    case PROP_SCALE_Y:
      clutter_actor_set_scale (actor,
                               priv->scale_x,
                               g_value_get_double (value));
      break;

    case PROP_SCALE_CENTER_X:
      {
	gfloat center_x = g_value_get_float (value);
        gfloat center_y;

        clutter_anchor_coord_get_units (actor, &priv->scale_center,
                                        NULL,
                                        &center_y,
                                        NULL);
	clutter_actor_set_scale_full (actor,
                                      priv->scale_x,
                                      priv->scale_y,
                                      center_x,
                                      center_y);
      }
      break;

    case PROP_SCALE_CENTER_Y:
      {
        gfloat center_y = g_value_get_float (value);
	gfloat center_x;

        clutter_anchor_coord_get_units (actor, &priv->scale_center,
                                        &center_x,
                                        NULL,
                                        NULL);
	clutter_actor_set_scale_full (actor,
                                      priv->scale_x,
                                      priv->scale_y,
                                      center_x,
                                      center_y);
      }
      break;

    case PROP_SCALE_GRAVITY:
      clutter_actor_set_scale_with_gravity (actor,
                                            priv->scale_x,
                                            priv->scale_y,
                                            g_value_get_enum (value));
      break;

    case PROP_CLIP:
      {
        const ClutterGeometry *geom = g_value_get_boxed (value);

	clutter_actor_set_clip (actor,
				geom->x, geom->y,
				geom->width, geom->height);
      }
      break;

    case PROP_CLIP_TO_ALLOCATION:
      clutter_actor_set_clip_to_allocation (actor, g_value_get_boolean (value));
      break;

    case PROP_REACTIVE:
      clutter_actor_set_reactive (actor, g_value_get_boolean (value));
      break;

    case PROP_ROTATION_ANGLE_X:
      clutter_actor_set_rotation_internal (actor,
                                           CLUTTER_X_AXIS,
                                           g_value_get_double (value));
      break;

    case PROP_ROTATION_ANGLE_Y:
      clutter_actor_set_rotation_internal (actor,
                                           CLUTTER_Y_AXIS,
                                           g_value_get_double (value));
      break;

    case PROP_ROTATION_ANGLE_Z:
      clutter_actor_set_rotation_internal (actor,
                                           CLUTTER_Z_AXIS,
                                           g_value_get_double (value));
      break;

    case PROP_ROTATION_CENTER_X:
      {
        const ClutterVertex *center;

        if ((center = g_value_get_boxed (value)))
          clutter_actor_set_rotation (actor,
                                      CLUTTER_X_AXIS,
                                      priv->rxang,
                                      center->x,
                                      center->y,
                                      center->z);
      }
      break;

    case PROP_ROTATION_CENTER_Y:
      {
        const ClutterVertex *center;

        if ((center = g_value_get_boxed (value)))
          clutter_actor_set_rotation (actor,
                                      CLUTTER_Y_AXIS,
                                      priv->ryang,
                                      center->x,
                                      center->y,
                                      center->z);
      }
      break;

    case PROP_ROTATION_CENTER_Z:
      {
        const ClutterVertex *center;

        if ((center = g_value_get_boxed (value)))
          clutter_actor_set_rotation (actor,
                                      CLUTTER_Z_AXIS,
                                      priv->rzang,
                                      center->x,
                                      center->y,
                                      center->z);
      }
      break;

    case PROP_ROTATION_CENTER_Z_GRAVITY:
      clutter_actor_set_z_rotation_from_gravity (actor, priv->rzang,
                                                 g_value_get_enum (value));
      break;

    case PROP_ANCHOR_X:
      {
        gfloat anchor_x = g_value_get_float (value);
        gfloat anchor_y;

        clutter_anchor_coord_get_units (actor, &priv->anchor,
                                        NULL,
                                        &anchor_y,
                                        NULL);
	clutter_actor_set_anchor_point (actor, anchor_x, anchor_y);
      }
      break;

    case PROP_ANCHOR_Y:
      {
        gfloat anchor_y = g_value_get_float (value);
        gfloat anchor_x;

        clutter_anchor_coord_get_units (actor, &priv->anchor,
                                        &anchor_x,
                                        NULL,
                                        NULL);
	clutter_actor_set_anchor_point (actor, anchor_x, anchor_y);
      }
      break;

    case PROP_ANCHOR_GRAVITY:
      clutter_actor_set_anchor_point_from_gravity (actor,
                                                   g_value_get_enum (value));
      break;

    case PROP_SHOW_ON_SET_PARENT:
      priv->show_on_set_parent = g_value_get_boolean (value);
      break;

    case PROP_TEXT_DIRECTION:
      clutter_actor_set_text_direction (actor, g_value_get_enum (value));
      break;

    case PROP_ACTIONS:
      clutter_actor_add_action (actor, g_value_get_object (value));
      break;

    case PROP_CONSTRAINTS:
      clutter_actor_add_constraint (actor, g_value_get_object (value));
      break;

    case PROP_EFFECT:
      clutter_actor_add_effect (actor, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
clutter_actor_get_property (GObject    *object,
			    guint       prop_id,
			    GValue     *value,
			    GParamSpec *pspec)
{
  ClutterActor *actor = CLUTTER_ACTOR (object);
  ClutterActorPrivate *priv = actor->priv;

  switch (prop_id)
    {
    case PROP_X:
      g_value_set_float (value, clutter_actor_get_x (actor));
      break;

    case PROP_Y:
      g_value_set_float (value, clutter_actor_get_y (actor));
      break;

    case PROP_WIDTH:
      g_value_set_float (value, clutter_actor_get_width (actor));
      break;

    case PROP_HEIGHT:
      g_value_set_float (value, clutter_actor_get_height (actor));
      break;

    case PROP_FIXED_X:
      g_value_set_float (value, priv->fixed_x);
      break;

    case PROP_FIXED_Y:
      g_value_set_float (value, priv->fixed_y);
      break;

    case PROP_FIXED_POSITION_SET:
      g_value_set_boolean (value, priv->position_set);
      break;

    case PROP_MIN_WIDTH:
      g_value_set_float (value, priv->request_min_width);
      break;

    case PROP_MIN_HEIGHT:
      g_value_set_float (value, priv->request_min_height);
      break;

    case PROP_NATURAL_WIDTH:
      g_value_set_float (value, priv->request_natural_width);
      break;

    case PROP_NATURAL_HEIGHT:
      g_value_set_float (value, priv->request_natural_height);
      break;

    case PROP_MIN_WIDTH_SET:
      g_value_set_boolean (value, priv->min_width_set);
      break;

    case PROP_MIN_HEIGHT_SET:
      g_value_set_boolean (value, priv->min_height_set);
      break;

    case PROP_NATURAL_WIDTH_SET:
      g_value_set_boolean (value, priv->natural_width_set);
      break;

    case PROP_NATURAL_HEIGHT_SET:
      g_value_set_boolean (value, priv->natural_height_set);
      break;

    case PROP_REQUEST_MODE:
      g_value_set_enum (value, priv->request_mode);
      break;

    case PROP_ALLOCATION:
      g_value_set_boxed (value, &priv->allocation);
      break;

    case PROP_DEPTH:
      g_value_set_float (value, clutter_actor_get_depth (actor));
      break;

    case PROP_OPACITY:
      g_value_set_uint (value, priv->opacity);
      break;

    case PROP_OFFSCREEN_REDIRECT:
      g_value_set_enum (value, priv->offscreen_redirect);
      break;

    case PROP_NAME:
      g_value_set_string (value, priv->name);
      break;

    case PROP_VISIBLE:
      g_value_set_boolean (value, CLUTTER_ACTOR_IS_VISIBLE (actor));
      break;

    case PROP_MAPPED:
      g_value_set_boolean (value, CLUTTER_ACTOR_IS_MAPPED (actor));
      break;

    case PROP_REALIZED:
      g_value_set_boolean (value, CLUTTER_ACTOR_IS_REALIZED (actor));
      break;

    case PROP_HAS_CLIP:
      g_value_set_boolean (value, priv->has_clip);
      break;

    case PROP_CLIP:
      {
        ClutterGeometry clip = { 0, };

        clip.x      = priv->clip[0];
        clip.y      = priv->clip[1];
        clip.width  = priv->clip[2];
        clip.height = priv->clip[3];

        g_value_set_boxed (value, &clip);
      }
      break;

    case PROP_CLIP_TO_ALLOCATION:
      g_value_set_boolean (value, priv->clip_to_allocation);
      break;

    case PROP_SCALE_X:
      g_value_set_double (value, priv->scale_x);
      break;

    case PROP_SCALE_Y:
      g_value_set_double (value, priv->scale_y);
      break;

    case PROP_SCALE_CENTER_X:
      {
        gfloat center;

        clutter_actor_get_scale_center (actor, &center, NULL);

        g_value_set_float (value, center);
      }
      break;

    case PROP_SCALE_CENTER_Y:
      {
        gfloat center;

        clutter_actor_get_scale_center (actor, NULL, &center);

        g_value_set_float (value, center);
      }
      break;

    case PROP_SCALE_GRAVITY:
      g_value_set_enum (value, clutter_actor_get_scale_gravity (actor));
      break;

    case PROP_REACTIVE:
      g_value_set_boolean (value, clutter_actor_get_reactive (actor));
      break;

    case PROP_ROTATION_ANGLE_X:
      g_value_set_double (value, priv->rxang);
      break;

    case PROP_ROTATION_ANGLE_Y:
      g_value_set_double (value, priv->ryang);
      break;

    case PROP_ROTATION_ANGLE_Z:
      g_value_set_double (value, priv->rzang);
      break;

    case PROP_ROTATION_CENTER_X:
      {
        ClutterVertex center;

        clutter_actor_get_rotation (actor, CLUTTER_X_AXIS,
                                    &center.x,
                                    &center.y,
                                    &center.z);

        g_value_set_boxed (value, &center);
      }
      break;

    case PROP_ROTATION_CENTER_Y:
      {
        ClutterVertex center;

        clutter_actor_get_rotation (actor, CLUTTER_Y_AXIS,
                                    &center.x,
                                    &center.y,
                                    &center.z);

        g_value_set_boxed (value, &center);
      }
      break;

    case PROP_ROTATION_CENTER_Z:
      {
        ClutterVertex center;

        clutter_actor_get_rotation (actor, CLUTTER_Z_AXIS,
                                    &center.x,
                                    &center.y,
                                    &center.z);

        g_value_set_boxed (value, &center);
      }
      break;

    case PROP_ROTATION_CENTER_Z_GRAVITY:
      g_value_set_enum (value, clutter_actor_get_z_rotation_gravity (actor));
      break;

    case PROP_ANCHOR_X:
      {
        gfloat anchor_x;

        clutter_anchor_coord_get_units (actor, &priv->anchor,
                                        &anchor_x,
                                        NULL,
                                        NULL);
        g_value_set_float (value, anchor_x);
      }
      break;

    case PROP_ANCHOR_Y:
      {
        gfloat anchor_y;

        clutter_anchor_coord_get_units (actor, &priv->anchor,
                                        NULL,
                                        &anchor_y,
                                        NULL);
        g_value_set_float (value, anchor_y);
      }
      break;

    case PROP_ANCHOR_GRAVITY:
      g_value_set_enum (value, clutter_actor_get_anchor_point_gravity (actor));
      break;

    case PROP_SHOW_ON_SET_PARENT:
      g_value_set_boolean (value, priv->show_on_set_parent);
      break;

    case PROP_TEXT_DIRECTION:
      g_value_set_enum (value, priv->text_direction);
      break;

    case PROP_HAS_POINTER:
      g_value_set_boolean (value, priv->has_pointer);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
clutter_actor_dispose (GObject *object)
{
  ClutterActor *self = CLUTTER_ACTOR (object);
  ClutterActorPrivate *priv = self->priv;

  CLUTTER_NOTE (MISC, "Disposing of object (id=%d) of type '%s' (ref_count:%d)",
		priv->id,
		g_type_name (G_OBJECT_TYPE (self)),
                object->ref_count);

  /* avoid recursing when called from clutter_actor_destroy() */
  if (priv->parent_actor != NULL)
    {
      ClutterActor *parent = priv->parent_actor;

      /* go through the Container implementation unless this
       * is an internal child and has been marked as such
       */
      if (CLUTTER_IS_CONTAINER (parent) &&
          !CLUTTER_ACTOR_IS_INTERNAL_CHILD (self))
        {
          clutter_container_remove_actor (CLUTTER_CONTAINER (parent), self);
        }
      else
        clutter_actor_unparent (self);
    }

  /* parent should be gone */
  g_assert (priv->parent_actor == NULL);

  if (!CLUTTER_ACTOR_IS_TOPLEVEL (self))
    {
      /* can't be mapped or realized with no parent */
      g_assert (!CLUTTER_ACTOR_IS_MAPPED (self));
      g_assert (!CLUTTER_ACTOR_IS_REALIZED (self));
    }

  if (priv->pango_context)
    {
      g_object_unref (priv->pango_context);
      priv->pango_context = NULL;
    }

  if (priv->actions != NULL)
    {
      g_object_unref (priv->actions);
      priv->actions = NULL;
    }

  if (priv->constraints != NULL)
    {
      g_object_unref (priv->constraints);
      priv->constraints = NULL;
    }

  if (priv->effects != NULL)
    {
      g_object_unref (priv->effects);
      priv->effects = NULL;
    }

  if (priv->flatten_effect != NULL)
    {
      g_object_unref (priv->flatten_effect);
      priv->flatten_effect = NULL;
    }

  g_signal_emit (self, actor_signals[DESTROY], 0);

  G_OBJECT_CLASS (clutter_actor_parent_class)->dispose (object);
}

static void
clutter_actor_finalize (GObject *object)
{
  ClutterActorPrivate *priv = CLUTTER_ACTOR (object)->priv;

  CLUTTER_NOTE (MISC, "Finalize actor (name='%s', id=%d) of type '%s'",
                priv->name != NULL ? priv->name : "<none>",
		priv->id,
		g_type_name (G_OBJECT_TYPE (object)));

  _clutter_context_release_id (priv->id);

  g_free (priv->name);

  G_OBJECT_CLASS (clutter_actor_parent_class)->finalize (object);
}


/**
 * clutter_actor_get_accessible:
 * @self: a #ClutterActor
 *
 * Returns the accessible object that describes the actor to an
 * assistive technology.
 *
 * If no class-specific #AtkObject implementation is available for the
 * actor instance in question, it will inherit an #AtkObject
 * implementation from the first ancestor class for which such an
 * implementation is defined.
 *
 * The documentation of the <ulink
 * url="http://developer.gnome.org/doc/API/2.0/atk/index.html">ATK</ulink>
 * library contains more information about accessible objects and
 * their uses.
 *
 * Returns: (transfer none): the #AtkObject associated with @actor
 */
AtkObject *
clutter_actor_get_accessible (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), NULL);

  return CLUTTER_ACTOR_GET_CLASS (self)->get_accessible (self);
}

static AtkObject *
clutter_actor_real_get_accessible (ClutterActor *actor)
{
  return atk_gobject_accessible_for_object (G_OBJECT (actor));
}

static AtkObject *
_clutter_actor_ref_accessible (AtkImplementor *implementor)
{
  AtkObject *accessible;

  accessible = clutter_actor_get_accessible (CLUTTER_ACTOR (implementor));
  if (accessible != NULL)
    g_object_ref (accessible);

  return accessible;
}

static void
atk_implementor_iface_init (AtkImplementorIface *iface)
{
  iface->ref_accessible = _clutter_actor_ref_accessible;
}

static gboolean
clutter_actor_real_get_paint_volume (ClutterActor       *self,
                                     ClutterPaintVolume *volume)
{
  return FALSE;
}

static gboolean
clutter_actor_real_has_overlaps (ClutterActor *self)
{
  /* By default we'll assume that all actors need an offscreen
     redirect to get the correct opacity. This effectively favours
     accuracy over efficiency. Actors such as ClutterTexture that
     would never need an offscreen redirect can override this to
     return FALSE. */
  return TRUE;
}

static void
clutter_actor_class_init (ClutterActorClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *pspec;

  quark_shader_data = g_quark_from_static_string ("-clutter-actor-shader-data");

  object_class->set_property = clutter_actor_set_property;
  object_class->get_property = clutter_actor_get_property;
  object_class->dispose      = clutter_actor_dispose;
  object_class->finalize     = clutter_actor_finalize;

  g_type_class_add_private (klass, sizeof (ClutterActorPrivate));

  /**
   * ClutterActor:x:
   *
   * X coordinate of the actor in pixels. If written, forces a fixed
   * position for the actor. If read, returns the fixed position if any,
   * otherwise the allocation if available, otherwise 0.
   */
  pspec = g_param_spec_float ("x",
                              P_("X coordinate"),
                              P_("X coordinate of the actor"),
                              -G_MAXFLOAT, G_MAXFLOAT,
                              0.0,
                              CLUTTER_PARAM_READWRITE);
  obj_props[PROP_X] = pspec;
  g_object_class_install_property (object_class, PROP_X, pspec);

  /**
   * ClutterActor:y:
   *
   * Y coordinate of the actor in pixels. If written, forces a fixed
   * position for the actor.  If read, returns the fixed position if
   * any, otherwise the allocation if available, otherwise 0.
   */
  pspec = g_param_spec_float ("y",
                              P_("Y coordinate"),
                              P_("Y coordinate of the actor"),
                              -G_MAXFLOAT, G_MAXFLOAT,
                              0.0,
                              CLUTTER_PARAM_READWRITE);
  obj_props[PROP_Y] = pspec;
  g_object_class_install_property (object_class, PROP_Y, pspec);

  /**
   * ClutterActor:width:
   *
   * Width of the actor (in pixels). If written, forces the minimum and
   * natural size request of the actor to the given width. If read, returns
   * the allocated width if available, otherwise the width request.
   */
  pspec = g_param_spec_float ("width",
                              P_("Width"),
                              P_("Width of the actor"),
                              0.0, G_MAXFLOAT,
                              0.0,
                              CLUTTER_PARAM_READWRITE);
  obj_props[PROP_WIDTH] = pspec;
  g_object_class_install_property (object_class, PROP_WIDTH, pspec);
  /**
   * ClutterActor:height:
   *
   * Height of the actor (in pixels).  If written, forces the minimum and
   * natural size request of the actor to the given height. If read, returns
   * the allocated height if available, otherwise the height request.
   */
  pspec = g_param_spec_float ("height",
                              P_("Height"),
                              P_("Height of the actor"),
                              0.0, G_MAXFLOAT,
                              0.0,
                              CLUTTER_PARAM_READWRITE);
  obj_props[PROP_HEIGHT] = pspec;
  g_object_class_install_property (object_class, PROP_HEIGHT, pspec);

  /**
   * ClutterActor:fixed-x:
   *
   * The fixed X position of the actor in pixels.
   *
   * Writing this property sets #ClutterActor:fixed-position-set
   * property as well, as a side effect
   *
   * Since: 0.8
   */
  pspec = g_param_spec_float ("fixed-x",
                              P_("Fixed X"),
                              P_("Forced X position of the actor"),
                              -G_MAXFLOAT, G_MAXFLOAT,
                              0.0,
                              CLUTTER_PARAM_READWRITE);
  obj_props[PROP_FIXED_X] = pspec;
  g_object_class_install_property (object_class, PROP_FIXED_X, pspec);

  /**
   * ClutterActor:fixed-y:
   *
   * The fixed Y position of the actor in pixels.
   *
   * Writing this property sets the #ClutterActor:fixed-position-set
   * property as well, as a side effect
   *
   * Since: 0.8
   */
  pspec = g_param_spec_float ("fixed-y",
                              P_("Fixed Y"),
                              P_("Forced Y position of the actor"),
                              -G_MAXFLOAT, G_MAXFLOAT,
                              0,
                              CLUTTER_PARAM_READWRITE);
  obj_props[PROP_FIXED_Y] = pspec;
  g_object_class_install_property (object_class, PROP_FIXED_Y, pspec);

  /**
   * ClutterActor:fixed-position-set:
   *
   * This flag controls whether the #ClutterActor:fixed-x and
   * #ClutterActor:fixed-y properties are used
   *
   * Since: 0.8
   */
  pspec = g_param_spec_boolean ("fixed-position-set",
                                P_("Fixed position set"),
                                P_("Whether to use fixed positioning "
                                   "for the actor"),
                                FALSE,
                                CLUTTER_PARAM_READWRITE);
  obj_props[PROP_FIXED_POSITION_SET] = pspec;
  g_object_class_install_property (object_class,
                                   PROP_FIXED_POSITION_SET,
                                   pspec);

  /**
   * ClutterActor:min-width:
   *
   * A forced minimum width request for the actor, in pixels
   *
   * Writing this property sets the #ClutterActor:min-width-set property
   * as well, as a side effect.
   *
   *This property overrides the usual width request of the actor.
   *
   * Since: 0.8
   */
  pspec = g_param_spec_float ("min-width",
                              P_("Min Width"),
                              P_("Forced minimum width request for the actor"),
                              0.0, G_MAXFLOAT,
                              0.0,
                              CLUTTER_PARAM_READWRITE);
  obj_props[PROP_MIN_WIDTH] = pspec;
  g_object_class_install_property (object_class, PROP_MIN_WIDTH, pspec);

  /**
   * ClutterActor:min-height:
   *
   * A forced minimum height request for the actor, in pixels
   *
   * Writing this property sets the #ClutterActor:min-height-set property
   * as well, as a side effect. This property overrides the usual height
   * request of the actor.
   *
   * Since: 0.8
   */
  pspec = g_param_spec_float ("min-height",
                              P_("Min Height"),
                              P_("Forced minimum height request for the actor"),
                              0.0, G_MAXFLOAT,
                              0.0,
                              CLUTTER_PARAM_READWRITE);
  obj_props[PROP_MIN_HEIGHT] = pspec;
  g_object_class_install_property (object_class, PROP_MIN_HEIGHT, pspec);

  /**
   * ClutterActor:natural-width:
   *
   * A forced natural width request for the actor, in pixels
   *
   * Writing this property sets the #ClutterActor:natural-width-set
   * property as well, as a side effect. This property overrides the
   * usual width request of the actor
   *
   * Since: 0.8
   */
  pspec = g_param_spec_float ("natural-width",
                              P_("Natural Width"),
                              P_("Forced natural width request for the actor"),
                              0.0, G_MAXFLOAT,
                              0.0,
                              CLUTTER_PARAM_READWRITE);
  obj_props[PROP_NATURAL_WIDTH] = pspec;
  g_object_class_install_property (object_class, PROP_NATURAL_WIDTH, pspec);

  /**
   * ClutterActor:natural-height:
   *
   * A forced natural height request for the actor, in pixels
   *
   * Writing this property sets the #ClutterActor:natural-height-set
   * property as well, as a side effect. This property overrides the
   * usual height request of the actor
   *
   * Since: 0.8
   */
  pspec = g_param_spec_float ("natural-height",
                              P_("Natural Height"),
                              P_("Forced natural height request for the actor"),
                              0.0, G_MAXFLOAT,
                              0.0,
                              CLUTTER_PARAM_READWRITE);
  obj_props[PROP_NATURAL_HEIGHT] = pspec;
  g_object_class_install_property (object_class, PROP_NATURAL_HEIGHT, pspec);

  /**
   * ClutterActor:min-width-set:
   *
   * This flag controls whether the #ClutterActor:min-width property
   * is used
   *
   * Since: 0.8
   */
  pspec = g_param_spec_boolean ("min-width-set",
                                P_("Minimum width set"),
                                P_("Whether to use the min-width property"),
                                FALSE,
                                CLUTTER_PARAM_READWRITE);
  obj_props[PROP_MIN_WIDTH_SET] = pspec;
  g_object_class_install_property (object_class, PROP_MIN_WIDTH_SET, pspec);

  /**
   * ClutterActor:min-height-set:
   *
   * This flag controls whether the #ClutterActor:min-height property
   * is used
   *
   * Since: 0.8
   */
  pspec = g_param_spec_boolean ("min-height-set",
                                P_("Minimum height set"),
                                P_("Whether to use the min-height property"),
                                FALSE,
                                CLUTTER_PARAM_READWRITE);
  obj_props[PROP_MIN_HEIGHT_SET] = pspec;
  g_object_class_install_property (object_class, PROP_MIN_HEIGHT_SET, pspec);

  /**
   * ClutterActor:natural-width-set:
   *
   * This flag controls whether the #ClutterActor:natural-width property
   * is used
   *
   * Since: 0.8
   */
  pspec = g_param_spec_boolean ("natural-width-set",
                                P_("Natural width set"),
                                P_("Whether to use the natural-width property"),
                                FALSE,
                                CLUTTER_PARAM_READWRITE);
  obj_props[PROP_NATURAL_WIDTH_SET] = pspec;
  g_object_class_install_property (object_class,
                                   PROP_NATURAL_WIDTH_SET,
                                   pspec);

  /**
   * ClutterActor:natural-height-set:
   *
   * This flag controls whether the #ClutterActor:natural-height property
   * is used
   *
   * Since: 0.8
   */
  pspec = g_param_spec_boolean ("natural-height-set",
                                P_("Natural height set"),
                                P_("Whether to use the natural-height property"),
                                FALSE,
                                CLUTTER_PARAM_READWRITE);
  obj_props[PROP_NATURAL_HEIGHT_SET] = pspec;
  g_object_class_install_property (object_class,
                                   PROP_NATURAL_HEIGHT_SET,
                                   pspec);

  /**
   * ClutterActor:allocation:
   *
   * The allocation for the actor, in pixels
   *
   * This is property is read-only, but you might monitor it to know when an
   * actor moves or resizes
   *
   * Since: 0.8
   */
  pspec = g_param_spec_boxed ("allocation",
                              P_("Allocation"),
                              P_("The actor's allocation"),
                              CLUTTER_TYPE_ACTOR_BOX,
                              CLUTTER_PARAM_READABLE);
  obj_props[PROP_ALLOCATION] = pspec;
  g_object_class_install_property (object_class, PROP_ALLOCATION, pspec);

  /**
   * ClutterActor:request-mode:
   *
   * Request mode for the #ClutterActor. The request mode determines the
   * type of geometry management used by the actor, either height for width
   * (the default) or width for height.
   *
   * For actors implementing height for width, the parent container should get
   * the preferred width first, and then the preferred height for that width.
   *
   * For actors implementing width for height, the parent container should get
   * the preferred height first, and then the preferred width for that height.
   *
   * For instance:
   *
   * |[
   *   ClutterRequestMode mode;
   *   gfloat natural_width, min_width;
   *   gfloat natural_height, min_height;
   *
   *   mode = clutter_actor_get_request_mode (child);
   *   if (mode == CLUTTER_REQUEST_HEIGHT_FOR_WIDTH)
   *     {
   *       clutter_actor_get_preferred_width (child, -1,
   *                                          &amp;min_width,
   *                                          &amp;natural_width);
   *       clutter_actor_get_preferred_height (child, natural_width,
   *                                           &amp;min_height,
   *                                           &amp;natural_height);
   *     }
   *   else
   *     {
   *       clutter_actor_get_preferred_height (child, -1,
   *                                           &amp;min_height,
   *                                           &amp;natural_height);
   *       clutter_actor_get_preferred_width (child, natural_height,
   *                                          &amp;min_width,
   *                                          &amp;natural_width);
   *     }
   * ]|
   *
   * will retrieve the minimum and natural width and height depending on the
   * preferred request mode of the #ClutterActor "child".
   *
   * The clutter_actor_get_preferred_size() function will implement this
   * check for you.
   *
   * Since: 0.8
   */
  pspec = g_param_spec_enum ("request-mode",
                             P_("Request Mode"),
                             P_("The actor's request mode"),
                             CLUTTER_TYPE_REQUEST_MODE,
                             CLUTTER_REQUEST_HEIGHT_FOR_WIDTH,
                             CLUTTER_PARAM_READWRITE);
  obj_props[PROP_REQUEST_MODE] = pspec;
  g_object_class_install_property (object_class, PROP_REQUEST_MODE, pspec);

  /**
   * ClutterActor:depth:
   *
   * The position of the actor on the Z axis
   *
   * Since: 0.6
   */
  pspec = g_param_spec_float ("depth",
                              P_("Depth"),
                              P_("Position on the Z axis"),
                              -G_MAXFLOAT, G_MAXFLOAT,
                              0.0,
                              CLUTTER_PARAM_READWRITE);
  obj_props[PROP_DEPTH] = pspec;
  g_object_class_install_property (object_class, PROP_DEPTH, pspec);

  /**
   * ClutterActor:opacity:
   *
   * Opacity of an actor, between 0 (fully transparent) and
   * 255 (fully opaque)
   */
  pspec = g_param_spec_uint ("opacity",
                             P_("Opacity"),
                             P_("Opacity of an actor"),
                             0, 255,
                             255,
                             CLUTTER_PARAM_READWRITE);
  obj_props[PROP_OPACITY] = pspec;
  g_object_class_install_property (object_class, PROP_OPACITY, pspec);

  /**
   * ClutterActor:offscreen-redirect:
   *
   * Whether to flatten the actor into a single image. See
   * clutter_actor_set_offscreen_redirect() for details.
   *
   * Since: 1.8
   */
  pspec = g_param_spec_enum ("offscreen-redirect",
                             P_("Offscreen redirect"),
                             P_("Whether to flatten the actor into a "
                                "single image"),
                             CLUTTER_TYPE_OFFSCREEN_REDIRECT,
                             CLUTTER_OFFSCREEN_REDIRECT_AUTOMATIC_FOR_OPACITY,
                             CLUTTER_PARAM_READWRITE);
  obj_props[PROP_OFFSCREEN_REDIRECT] = pspec;
  g_object_class_install_property (object_class,
                                   PROP_OFFSCREEN_REDIRECT,
                                   pspec);

  /**
   * ClutterActor:visible:
   *
   * Whether the actor is set to be visible or not
   *
   * See also #ClutterActor:mapped
   */
  pspec = g_param_spec_boolean ("visible",
                                P_("Visible"),
                                P_("Whether the actor is visible or not"),
                                FALSE,
                                CLUTTER_PARAM_READWRITE);
  obj_props[PROP_VISIBLE] = pspec;
  g_object_class_install_property (object_class, PROP_VISIBLE, pspec);

  /**
   * ClutterActor:mapped:
   *
   * Whether the actor is mapped (will be painted when the stage
   * to which it belongs is mapped)
   *
   * Since: 1.0
   */
  pspec = g_param_spec_boolean ("mapped",
                                P_("Mapped"),
                                P_("Whether the actor will be painted"),
                                FALSE,
                                CLUTTER_PARAM_READABLE);
  obj_props[PROP_MAPPED] = pspec;
  g_object_class_install_property (object_class, PROP_MAPPED, pspec);

  /**
   * ClutterActor:realized:
   *
   * Whether the actor has been realized
   *
   * Since: 1.0
   */
  pspec = g_param_spec_boolean ("realized",
                                P_("Realized"),
                                P_("Whether the actor has been realized"),
                                FALSE,
                                CLUTTER_PARAM_READABLE);
  obj_props[PROP_REALIZED] = pspec;
  g_object_class_install_property (object_class, PROP_REALIZED, pspec);

  /**
   * ClutterActor:reactive:
   *
   * Whether the actor is reactive to events or not
   *
   * Only reactive actors will emit event-related signals
   *
   * Since: 0.6
   */
  pspec = g_param_spec_boolean ("reactive",
                                P_("Reactive"),
                                P_("Whether the actor is reactive to events"),
                                FALSE,
                                CLUTTER_PARAM_READWRITE);
  obj_props[PROP_REACTIVE] = pspec;
  g_object_class_install_property (object_class, PROP_REACTIVE, pspec);

  /**
   * ClutterActor:has-clip:
   *
   * Whether the actor has the #ClutterActor:clip property set or not
   */
  pspec = g_param_spec_boolean ("has-clip",
                                P_("Has Clip"),
                                P_("Whether the actor has a clip set"),
                                FALSE,
                                CLUTTER_PARAM_READABLE);
  obj_props[PROP_HAS_CLIP] = pspec;
  g_object_class_install_property (object_class, PROP_HAS_CLIP, pspec);

  /**
   * ClutterActor:clip:
   *
   * The clip region for the actor, in actor-relative coordinates
   *
   * Every part of the actor outside the clip region will not be
   * painted
   */
  pspec = g_param_spec_boxed ("clip",
                              P_("Clip"),
                              P_("The clip region for the actor"),
                              CLUTTER_TYPE_GEOMETRY,
                              CLUTTER_PARAM_READWRITE);
  obj_props[PROP_CLIP] = pspec;
  g_object_class_install_property (object_class, PROP_CLIP, pspec);

  /**
   * ClutterActor:name:
   *
   * The name of the actor
   *
   * Since: 0.2
   */
  pspec = g_param_spec_string ("name",
                               P_("Name"),
                               P_("Name of the actor"),
                               NULL,
                               CLUTTER_PARAM_READWRITE);
  obj_props[PROP_NAME] = pspec;
  g_object_class_install_property (object_class, PROP_NAME, pspec);

  /**
   * ClutterActor:scale-x:
   *
   * The horizontal scale of the actor
   *
   * Since: 0.6
   */
  pspec = g_param_spec_double ("scale-x",
                               P_("Scale X"),
                               P_("Scale factor on the X axis"),
                               0.0, G_MAXDOUBLE,
                               1.0,
                               CLUTTER_PARAM_READWRITE);
  obj_props[PROP_SCALE_X] = pspec;
  g_object_class_install_property (object_class, PROP_SCALE_X, pspec);

  /**
   * ClutterActor:scale-y:
   *
   * The vertical scale of the actor
   *
   * Since: 0.6
   */
  pspec = g_param_spec_double ("scale-y",
                               P_("Scale Y"),
                               P_("Scale factor on the Y axis"),
                               0.0, G_MAXDOUBLE,
                               1.0,
                               CLUTTER_PARAM_READWRITE);
  obj_props[PROP_SCALE_Y] = pspec;
  g_object_class_install_property (object_class, PROP_SCALE_Y, pspec);

  /**
   * ClutterActor:scale-center-x:
   *
   * The horizontal center point for scaling
   *
   * Since: 1.0
   */
  pspec = g_param_spec_float ("scale-center-x",
                              P_("Scale Center X"),
                              P_("Horizontal scale center"),
                              -G_MAXFLOAT, G_MAXFLOAT,
                              0.0,
                              CLUTTER_PARAM_READWRITE);
  obj_props[PROP_SCALE_CENTER_X] = pspec;
  g_object_class_install_property (object_class, PROP_SCALE_CENTER_X, pspec);

  /**
   * ClutterActor:scale-center-y:
   *
   * The vertical center point for scaling
   *
   * Since: 1.0
   */
  pspec = g_param_spec_float ("scale-center-y",
                              P_("Scale Center Y"),
                              P_("Vertical scale center"),
                              -G_MAXFLOAT, G_MAXFLOAT,
                              0.0,
                              CLUTTER_PARAM_READWRITE);
  obj_props[PROP_SCALE_CENTER_Y] = pspec;
  g_object_class_install_property (object_class, PROP_SCALE_CENTER_Y, pspec);

  /**
   * ClutterActor:scale-gravity:
   *
   * The center point for scaling expressed as a #ClutterGravity
   *
   * Since: 1.0
   */
  pspec = g_param_spec_enum ("scale-gravity",
                             P_("Scale Gravity"),
                             P_("The center of scaling"),
                             CLUTTER_TYPE_GRAVITY,
                             CLUTTER_GRAVITY_NONE,
                             CLUTTER_PARAM_READWRITE);
  obj_props[PROP_SCALE_GRAVITY] = pspec;
  g_object_class_install_property (object_class,
                                   PROP_SCALE_GRAVITY,
                                   pspec);

  /**
   * ClutterActor:rotation-angle-x:
   *
   * The rotation angle on the X axis
   *
   * Since: 0.6
   */
  pspec = g_param_spec_double ("rotation-angle-x",
                               P_("Rotation Angle X"),
                               P_("The rotation angle on the X axis"),
                               -G_MAXDOUBLE, G_MAXDOUBLE,
                               0.0,
                               CLUTTER_PARAM_READWRITE);
  obj_props[PROP_ROTATION_ANGLE_X] = pspec;
  g_object_class_install_property (object_class, PROP_ROTATION_ANGLE_X, pspec);

  /**
   * ClutterActor:rotation-angle-y:
   *
   * The rotation angle on the Y axis
   *
   * Since: 0.6
   */
  pspec = g_param_spec_double ("rotation-angle-y",
                               P_("Rotation Angle Y"),
                               P_("The rotation angle on the Y axis"),
                               -G_MAXDOUBLE, G_MAXDOUBLE,
                               0.0,
                               CLUTTER_PARAM_READWRITE);
  obj_props[PROP_ROTATION_ANGLE_Y] = pspec;
  g_object_class_install_property (object_class, PROP_ROTATION_ANGLE_Y, pspec);

  /**
   * ClutterActor:rotation-angle-z:
   *
   * The rotation angle on the Z axis
   *
   * Since: 0.6
   */
  pspec = g_param_spec_double ("rotation-angle-z",
                               P_("Rotation Angle Z"),
                               P_("The rotation angle on the Z axis"),
                               -G_MAXDOUBLE, G_MAXDOUBLE,
                               0.0,
                               CLUTTER_PARAM_READWRITE);
  obj_props[PROP_ROTATION_ANGLE_Z] = pspec;
  g_object_class_install_property (object_class, PROP_ROTATION_ANGLE_Z, pspec);

  /**
   * ClutterActor:rotation-center-x:
   *
   * The rotation center on the X axis.
   *
   * Since: 0.6
   */
  pspec = g_param_spec_boxed ("rotation-center-x",
                              P_("Rotation Center X"),
                              P_("The rotation center on the X axis"),
                              CLUTTER_TYPE_VERTEX,
                              CLUTTER_PARAM_READWRITE);
  obj_props[PROP_ROTATION_CENTER_X] = pspec;
  g_object_class_install_property (object_class,
                                   PROP_ROTATION_CENTER_X,
                                   pspec);

  /**
   * ClutterActor:rotation-center-y:
   *
   * The rotation center on the Y axis.
   *
   * Since: 0.6
   */
  pspec = g_param_spec_boxed ("rotation-center-y",
                              P_("Rotation Center Y"),
                              P_("The rotation center on the Y axis"),
                              CLUTTER_TYPE_VERTEX,
                              CLUTTER_PARAM_READWRITE);
  obj_props[PROP_ROTATION_CENTER_Y] = pspec;
  g_object_class_install_property (object_class,
                                   PROP_ROTATION_CENTER_Y,
                                   pspec);

  /**
   * ClutterActor:rotation-center-z:
   *
   * The rotation center on the Z axis.
   *
   * Since: 0.6
   */
  pspec = g_param_spec_boxed ("rotation-center-z",
                              P_("Rotation Center Z"),
                              P_("The rotation center on the Z axis"),
                              CLUTTER_TYPE_VERTEX,
                              CLUTTER_PARAM_READWRITE);
  obj_props[PROP_ROTATION_CENTER_Z] = pspec;
  g_object_class_install_property (object_class,
                                   PROP_ROTATION_CENTER_Z,
                                   pspec);

  /**
   * ClutterActor:rotation-center-z-gravity:
   *
   * The rotation center on the Z axis expressed as a #ClutterGravity.
   *
   * Since: 1.0
   */
  pspec = g_param_spec_enum ("rotation-center-z-gravity",
                             P_("Rotation Center Z Gravity"),
                             P_("Center point for rotation around the Z axis"),
                             CLUTTER_TYPE_GRAVITY,
                             CLUTTER_GRAVITY_NONE,
                             CLUTTER_PARAM_READWRITE);
  obj_props[PROP_ROTATION_CENTER_Z_GRAVITY] = pspec;
  g_object_class_install_property (object_class,
                                   PROP_ROTATION_CENTER_Z_GRAVITY,
                                   pspec);

  /**
   * ClutterActor:anchor-x:
   *
   * The X coordinate of an actor's anchor point, relative to
   * the actor coordinate space, in pixels
   *
   * Since: 0.8
   */
  pspec = g_param_spec_float ("anchor-x",
                              P_("Anchor X"),
                              P_("X coordinate of the anchor point"),
                              -G_MAXFLOAT, G_MAXFLOAT,
                              0,
                              CLUTTER_PARAM_READWRITE);
  obj_props[PROP_ANCHOR_X] = pspec;
  g_object_class_install_property (object_class, PROP_ANCHOR_X, pspec);

  /**
   * ClutterActor:anchor-y:
   *
   * The Y coordinate of an actor's anchor point, relative to
   * the actor coordinate space, in pixels
   *
   * Since: 0.8
   */
  pspec = g_param_spec_float ("anchor-y",
                              P_("Anchor Y"),
                              P_("Y coordinate of the anchor point"),
                              -G_MAXFLOAT, G_MAXFLOAT,
                              0,
                              CLUTTER_PARAM_READWRITE);
  obj_props[PROP_ANCHOR_Y] = pspec;
  g_object_class_install_property (object_class, PROP_ANCHOR_Y, pspec);

  /**
   * ClutterActor:anchor-gravity:
   *
   * The anchor point expressed as a #ClutterGravity
   *
   * Since: 1.0
   */
  pspec = g_param_spec_enum ("anchor-gravity",
                             P_("Anchor Gravity"),
                             P_("The anchor point as a ClutterGravity"),
                             CLUTTER_TYPE_GRAVITY,
                             CLUTTER_GRAVITY_NONE,
                             CLUTTER_PARAM_READWRITE);
  obj_props[PROP_ANCHOR_GRAVITY] = pspec;
  g_object_class_install_property (object_class,
                                   PROP_ANCHOR_GRAVITY, pspec);

  /**
   * ClutterActor:show-on-set-parent:
   *
   * If %TRUE, the actor is automatically shown when parented.
   *
   * Calling clutter_actor_hide() on an actor which has not been
   * parented will set this property to %FALSE as a side effect.
   *
   * Since: 0.8
   */
  pspec = g_param_spec_boolean ("show-on-set-parent",
                                P_("Show on set parent"),
                                P_("Whether the actor is shown when parented"),
                                TRUE,
                                CLUTTER_PARAM_READWRITE);
  obj_props[PROP_SHOW_ON_SET_PARENT] = pspec;
  g_object_class_install_property (object_class,
                                   PROP_SHOW_ON_SET_PARENT,
                                   pspec);

  /**
   * ClutterActor:clip-to-allocation:
   *
   * Whether the clip region should track the allocated area
   * of the actor.
   *
   * This property is ignored if a clip area has been explicitly
   * set using clutter_actor_set_clip().
   *
   * Since: 1.0
   */
  pspec = g_param_spec_boolean ("clip-to-allocation",
                                P_("Clip to Allocation"),
                                P_("Sets the clip region to track the "
                                   "actor's allocation"),
                                FALSE,
                                CLUTTER_PARAM_READWRITE);
  obj_props[PROP_CLIP_TO_ALLOCATION] = pspec;
  g_object_class_install_property (object_class,
                                   PROP_CLIP_TO_ALLOCATION,
                                   pspec);

  pspec = g_param_spec_enum ("text-direction",
                             P_("Text Direction"),
                             P_("Direction of the text"),
                             CLUTTER_TYPE_TEXT_DIRECTION,
                             CLUTTER_TEXT_DIRECTION_LTR,
                             CLUTTER_PARAM_READWRITE);
  obj_props[PROP_TEXT_DIRECTION] = pspec;
  g_object_class_install_property (object_class,
                                   PROP_TEXT_DIRECTION,
                                   pspec);

  /**
   * ClutterActor:has-pointer:
   *
   * Whether the actor contains the pointer of a #ClutterInputDevice
   * or not.
   *
   * Since: 1.2
   */
  pspec = g_param_spec_boolean ("has-pointer",
                                P_("Has Pointer"),
                                P_("Whether the actor contains the pointer "
                                   "of an input device"),
                                FALSE,
                                CLUTTER_PARAM_READABLE);
  obj_props[PROP_HAS_POINTER] = pspec;
  g_object_class_install_property (object_class,
                                   PROP_HAS_POINTER,
                                   pspec);

  /**
   * ClutterActor:actions:
   *
   * Adds a #ClutterAction to the actor
   *
   * Since: 1.4
   */
  pspec = g_param_spec_object ("actions",
                               P_("Actions"),
                               P_("Adds an action to the actor"),
                               CLUTTER_TYPE_ACTION,
                               CLUTTER_PARAM_WRITABLE);
  obj_props[PROP_ACTIONS] = pspec;
  g_object_class_install_property (object_class, PROP_ACTIONS, pspec);

  /**
   * ClutterActor:constraints:
   *
   * Adds a #ClutterConstraint to the actor
   *
   * Since: 1.4
   */
  pspec = g_param_spec_object ("constraints",
                               P_("Constraints"),
                               P_("Adds a constraint to the actor"),
                               CLUTTER_TYPE_CONSTRAINT,
                               CLUTTER_PARAM_WRITABLE);
  obj_props[PROP_CONSTRAINTS] = pspec;
  g_object_class_install_property (object_class, PROP_CONSTRAINTS, pspec);

  /**
   * ClutterActor:effect:
   *
   * Adds #ClutterEffect to the list of effects be applied on a #ClutterActor
   *
   * Since: 1.4
   */
  pspec = g_param_spec_object ("effect",
                               "Effect",
                               "Add an effect to be applied on the actor",
                               CLUTTER_TYPE_EFFECT,
                               CLUTTER_PARAM_WRITABLE);
  obj_props[PROP_EFFECT] = pspec;
  g_object_class_install_property (object_class, PROP_EFFECT, pspec);

  /**
   * ClutterActor::destroy:
   * @actor: the #ClutterActor which emitted the signal
   *
   * The ::destroy signal notifies that all references held on the
   * actor which emitted it should be released.
   *
   * The ::destroy signal should be used by all holders of a reference
   * on @actor.
   *
   * This signal might result in the finalization of the #ClutterActor
   * if all references are released.
   *
   * Composite actors and actors implementing the #ClutterContainer
   * interface should override the default implementation of the
   * class handler of this signal and call clutter_actor_destroy() on
   * their children. When overriding the default class handler, it is
   * required to chain up to the parent's implementation.
   *
   * Since: 0.2
   */
  actor_signals[DESTROY] =
    g_signal_new (I_("destroy"),
		  G_TYPE_FROM_CLASS (object_class),
                  G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
		  G_STRUCT_OFFSET (ClutterActorClass, destroy),
		  NULL, NULL,
		  _clutter_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);
  /**
   * ClutterActor::show:
   * @actor: the object which received the signal
   *
   * The ::show signal is emitted when an actor is visible and
   * rendered on the stage.
   *
   * Since: 0.2
   */
  actor_signals[SHOW] =
    g_signal_new (I_("show"),
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_FIRST,
		  G_STRUCT_OFFSET (ClutterActorClass, show),
		  NULL, NULL,
		  _clutter_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);
  /**
   * ClutterActor::hide:
   * @actor: the object which received the signal
   *
   * The ::hide signal is emitted when an actor is no longer rendered
   * on the stage.
   *
   * Since: 0.2
   */
  actor_signals[HIDE] =
    g_signal_new (I_("hide"),
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_FIRST,
		  G_STRUCT_OFFSET (ClutterActorClass, hide),
		  NULL, NULL,
		  _clutter_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);
  /**
   * ClutterActor::parent-set:
   * @actor: the object which received the signal
   * @old_parent: the previous parent of the actor, or %NULL
   *
   * This signal is emitted when the parent of the actor changes.
   *
   * Since: 0.2
   */
  actor_signals[PARENT_SET] =
    g_signal_new (I_("parent-set"),
                  G_TYPE_FROM_CLASS (object_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ClutterActorClass, parent_set),
                  NULL, NULL,
                  _clutter_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1,
                  CLUTTER_TYPE_ACTOR);

  /**
   * ClutterActor::queue-redraw:
   * @actor: the actor we're bubbling the redraw request through
   * @origin: the actor which initiated the redraw request
   *
   * The ::queue_redraw signal is emitted when clutter_actor_queue_redraw()
   * is called on @origin.
   *
   * The default implementation for #ClutterActor chains up to the
   * parent actor and queues a redraw on the parent, thus "bubbling"
   * the redraw queue up through the actor graph. The default
   * implementation for #ClutterStage queues a clutter_redraw() in a
   * main loop idle handler.
   *
   * Note that the @origin actor may be the stage, or a container; it
   * does not have to be a leaf node in the actor graph.
   *
   * Toolkits embedding a #ClutterStage which require a redraw and
   * relayout cycle can stop the emission of this signal using the
   * GSignal API, redraw the UI and then call clutter_redraw()
   * themselves, like:
   *
   * |[
   *   static void
   *   on_redraw_complete (void)
   *   {
   *     /&ast; execute the Clutter drawing pipeline &ast;/
   *     clutter_redraw ();
   *   }
   *
   *   static void
   *   on_stage_queue_redraw (ClutterStage *stage)
   *   {
   *     /&ast; this prevents the default handler to run &ast;/
   *     g_signal_stop_emission_by_name (stage, "queue-redraw");
   *
   *     /&ast; queue a redraw with the host toolkit and call
   *      &ast; a function when the redraw has been completed
   *      &ast;/
   *     queue_a_redraw (G_CALLBACK (on_redraw_complete));
   *   }
   * ]|
   *
   * <note><para>This signal is emitted before the Clutter paint
   * pipeline is executed. If you want to know when the pipeline has
   * been completed you should connect to the ::paint signal on the
   * Stage with g_signal_connect_after().</para></note>
   *
   * Since: 1.0
   */
  actor_signals[QUEUE_REDRAW] =
    g_signal_new (I_("queue-redraw"),
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (ClutterActorClass, queue_redraw),
		  NULL, NULL,
		  _clutter_marshal_VOID__OBJECT,
		  G_TYPE_NONE, 1,
                  CLUTTER_TYPE_ACTOR);

  /**
   * ClutterActor::queue-relayout
   * @actor: the actor being queued for relayout
   *
   * The ::queue_layout signal is emitted when clutter_actor_queue_relayout()
   * is called on an actor.
   *
   * The default implementation for #ClutterActor chains up to the
   * parent actor and queues a relayout on the parent, thus "bubbling"
   * the relayout queue up through the actor graph.
   *
   * The main purpose of this signal is to allow relayout to be propagated
   * properly in the procense of #ClutterClone actors. Applications will
   * not normally need to connect to this signal.
   *
   * Since: 1.2
   */
  actor_signals[QUEUE_RELAYOUT] =
    g_signal_new (I_("queue-relayout"),
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (ClutterActorClass, queue_relayout),
		  NULL, NULL,
		  _clutter_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);

  /**
   * ClutterActor::event:
   * @actor: the actor which received the event
   * @event: a #ClutterEvent
   *
   * The ::event signal is emitted each time an event is received
   * by the @actor. This signal will be emitted on every actor,
   * following the hierarchy chain, until it reaches the top-level
   * container (the #ClutterStage).
   *
   * Return value: %TRUE if the event has been handled by the actor,
   *   or %FALSE to continue the emission.
   *
   * Since: 0.6
   */
  actor_signals[EVENT] =
    g_signal_new (I_("event"),
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (ClutterActorClass, event),
		  _clutter_boolean_handled_accumulator, NULL,
		  _clutter_marshal_BOOLEAN__BOXED,
		  G_TYPE_BOOLEAN, 1,
		  CLUTTER_TYPE_EVENT | G_SIGNAL_TYPE_STATIC_SCOPE);
  /**
   * ClutterActor::button-press-event:
   * @actor: the actor which received the event
   * @event: a #ClutterButtonEvent
   *
   * The ::button-press-event signal is emitted each time a mouse button
   * is pressed on @actor.
   *
   * Return value: %TRUE if the event has been handled by the actor,
   *   or %FALSE to continue the emission.
   *
   * Since: 0.6
   */
  actor_signals[BUTTON_PRESS_EVENT] =
    g_signal_new (I_("button-press-event"),
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (ClutterActorClass, button_press_event),
		  _clutter_boolean_handled_accumulator, NULL,
		  _clutter_marshal_BOOLEAN__BOXED,
		  G_TYPE_BOOLEAN, 1,
		  CLUTTER_TYPE_EVENT | G_SIGNAL_TYPE_STATIC_SCOPE);
  /**
   * ClutterActor::button-release-event:
   * @actor: the actor which received the event
   * @event: a #ClutterButtonEvent
   *
   * The ::button-release-event signal is emitted each time a mouse button
   * is released on @actor.
   *
   * Return value: %TRUE if the event has been handled by the actor,
   *   or %FALSE to continue the emission.
   *
   * Since: 0.6
   */
  actor_signals[BUTTON_RELEASE_EVENT] =
    g_signal_new (I_("button-release-event"),
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (ClutterActorClass, button_release_event),
		  _clutter_boolean_handled_accumulator, NULL,
		  _clutter_marshal_BOOLEAN__BOXED,
		  G_TYPE_BOOLEAN, 1,
		  CLUTTER_TYPE_EVENT | G_SIGNAL_TYPE_STATIC_SCOPE);
  /**
   * ClutterActor::scroll-event:
   * @actor: the actor which received the event
   * @event: a #ClutterScrollEvent
   *
   * The ::scroll-event signal is emitted each time the mouse is
   * scrolled on @actor
   *
   * Return value: %TRUE if the event has been handled by the actor,
   *   or %FALSE to continue the emission.
   *
   * Since: 0.6
   */
  actor_signals[SCROLL_EVENT] =
    g_signal_new (I_("scroll-event"),
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (ClutterActorClass, scroll_event),
		  _clutter_boolean_handled_accumulator, NULL,
		  _clutter_marshal_BOOLEAN__BOXED,
		  G_TYPE_BOOLEAN, 1,
		  CLUTTER_TYPE_EVENT | G_SIGNAL_TYPE_STATIC_SCOPE);
  /**
   * ClutterActor::key-press-event:
   * @actor: the actor which received the event
   * @event: a #ClutterKeyEvent
   *
   * The ::key-press-event signal is emitted each time a keyboard button
   * is pressed while @actor has key focus (see clutter_stage_set_key_focus()).
   *
   * Return value: %TRUE if the event has been handled by the actor,
   *   or %FALSE to continue the emission.
   *
   * Since: 0.6
   */
  actor_signals[KEY_PRESS_EVENT] =
    g_signal_new (I_("key-press-event"),
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (ClutterActorClass, key_press_event),
		  _clutter_boolean_handled_accumulator, NULL,
		  _clutter_marshal_BOOLEAN__BOXED,
		  G_TYPE_BOOLEAN, 1,
		  CLUTTER_TYPE_EVENT | G_SIGNAL_TYPE_STATIC_SCOPE);
  /**
   * ClutterActor::key-release-event:
   * @actor: the actor which received the event
   * @event: a #ClutterKeyEvent
   *
   * The ::key-release-event signal is emitted each time a keyboard button
   * is released while @actor has key focus (see
   * clutter_stage_set_key_focus()).
   *
   * Return value: %TRUE if the event has been handled by the actor,
   *   or %FALSE to continue the emission.
   *
   * Since: 0.6
   */
  actor_signals[KEY_RELEASE_EVENT] =
    g_signal_new (I_("key-release-event"),
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (ClutterActorClass, key_release_event),
		  _clutter_boolean_handled_accumulator, NULL,
		  _clutter_marshal_BOOLEAN__BOXED,
		  G_TYPE_BOOLEAN, 1,
		  CLUTTER_TYPE_EVENT | G_SIGNAL_TYPE_STATIC_SCOPE);
  /**
   * ClutterActor::motion-event:
   * @actor: the actor which received the event
   * @event: a #ClutterMotionEvent
   *
   * The ::motion-event signal is emitted each time the mouse pointer is
   * moved over @actor.
   *
   * Return value: %TRUE if the event has been handled by the actor,
   *   or %FALSE to continue the emission.
   *
   * Since: 0.6
   */
  actor_signals[MOTION_EVENT] =
    g_signal_new (I_("motion-event"),
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (ClutterActorClass, motion_event),
		  _clutter_boolean_handled_accumulator, NULL,
		  _clutter_marshal_BOOLEAN__BOXED,
		  G_TYPE_BOOLEAN, 1,
		  CLUTTER_TYPE_EVENT | G_SIGNAL_TYPE_STATIC_SCOPE);

  /**
   * ClutterActor::key-focus-in:
   * @actor: the actor which now has key focus
   *
   * The ::key-focus-in signal is emitted when @actor receives key focus.
   *
   * Since: 0.6
   */
  actor_signals[KEY_FOCUS_IN] =
    g_signal_new (I_("key-focus-in"),
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (ClutterActorClass, key_focus_in),
		  NULL, NULL,
		  _clutter_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);

  /**
   * ClutterActor::key-focus-out:
   * @actor: the actor which now has key focus
   *
   * The ::key-focus-out signal is emitted when @actor loses key focus.
   *
   * Since: 0.6
   */
  actor_signals[KEY_FOCUS_OUT] =
    g_signal_new (I_("key-focus-out"),
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (ClutterActorClass, key_focus_out),
		  NULL, NULL,
		  _clutter_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);

  /**
   * ClutterActor::enter-event:
   * @actor: the actor which the pointer has entered.
   * @event: a #ClutterCrossingEvent
   *
   * The ::enter-event signal is emitted when the pointer enters the @actor
   *
   * Return value: %TRUE if the event has been handled by the actor,
   *   or %FALSE to continue the emission.
   *
   * Since: 0.6
   */
  actor_signals[ENTER_EVENT] =
    g_signal_new (I_("enter-event"),
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (ClutterActorClass, enter_event),
		  _clutter_boolean_handled_accumulator, NULL,
		  _clutter_marshal_BOOLEAN__BOXED,
		  G_TYPE_BOOLEAN, 1,
		  CLUTTER_TYPE_EVENT | G_SIGNAL_TYPE_STATIC_SCOPE);

  /**
   * ClutterActor::leave-event:
   * @actor: the actor which the pointer has left
   * @event: a #ClutterCrossingEvent
   *
   * The ::leave-event signal is emitted when the pointer leaves the @actor.
   *
   * Return value: %TRUE if the event has been handled by the actor,
   *   or %FALSE to continue the emission.
   *
   * Since: 0.6
   */
  actor_signals[LEAVE_EVENT] =
    g_signal_new (I_("leave-event"),
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (ClutterActorClass, leave_event),
		  _clutter_boolean_handled_accumulator, NULL,
		  _clutter_marshal_BOOLEAN__BOXED,
		  G_TYPE_BOOLEAN, 1,
		  CLUTTER_TYPE_EVENT | G_SIGNAL_TYPE_STATIC_SCOPE);

  /**
   * ClutterActor::captured-event:
   * @actor: the actor which received the signal
   * @event: a #ClutterEvent
   *
   * The ::captured-event signal is emitted when an event is captured
   * by Clutter. This signal will be emitted starting from the top-level
   * container (the #ClutterStage) to the actor which received the event
   * going down the hierarchy. This signal can be used to intercept every
   * event before the specialized events (like
   * ClutterActor::button-press-event or ::key-released-event) are
   * emitted.
   *
   * Return value: %TRUE if the event has been handled by the actor,
   *   or %FALSE to continue the emission.
   *
   * Since: 0.6
   */
  actor_signals[CAPTURED_EVENT] =
    g_signal_new (I_("captured-event"),
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (ClutterActorClass, captured_event),
		  _clutter_boolean_handled_accumulator, NULL,
		  _clutter_marshal_BOOLEAN__BOXED,
		  G_TYPE_BOOLEAN, 1,
		  CLUTTER_TYPE_EVENT | G_SIGNAL_TYPE_STATIC_SCOPE);

  /**
   * ClutterActor::paint:
   * @actor: the #ClutterActor that received the signal
   *
   * The ::paint signal is emitted each time an actor is being painted.
   *
   * Subclasses of #ClutterActor should override the class signal handler
   * and paint themselves in that function.
   *
   * It is possible to connect a handler to the ::paint signal in order
   * to set up some custom aspect of a paint.
   *
   * Since: 0.8
   */
  actor_signals[PAINT] =
    g_signal_new (I_("paint"),
                  G_TYPE_FROM_CLASS (object_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ClutterActorClass, paint),
                  NULL, NULL,
                  _clutter_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
  /**
   * ClutterActor::realize:
   * @actor: the #ClutterActor that received the signal
   *
   * The ::realize signal is emitted each time an actor is being
   * realized.
   *
   * Since: 0.8
   */
  actor_signals[REALIZE] =
    g_signal_new (I_("realize"),
                  G_TYPE_FROM_CLASS (object_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ClutterActorClass, realize),
                  NULL, NULL,
                  _clutter_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
  /**
   * ClutterActor::unrealize:
   * @actor: the #ClutterActor that received the signal
   *
   * The ::unrealize signal is emitted each time an actor is being
   * unrealized.
   *
   * Since: 0.8
   */
  actor_signals[UNREALIZE] =
    g_signal_new (I_("unrealize"),
                  G_TYPE_FROM_CLASS (object_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ClutterActorClass, unrealize),
                  NULL, NULL,
                  _clutter_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  /**
   * ClutterActor::pick:
   * @actor: the #ClutterActor that received the signal
   * @color: the #ClutterColor to be used when picking
   *
   * The ::pick signal is emitted each time an actor is being painted
   * in "pick mode". The pick mode is used to identify the actor during
   * the event handling phase, or by clutter_stage_get_actor_at_pos().
   * The actor should paint its shape using the passed @pick_color.
   *
   * Subclasses of #ClutterActor should override the class signal handler
   * and paint themselves in that function.
   *
   * It is possible to connect a handler to the ::pick signal in order
   * to set up some custom aspect of a paint in pick mode.
   *
   * Since: 1.0
   */
  actor_signals[PICK] =
    g_signal_new (I_("pick"),
                  G_TYPE_FROM_CLASS (object_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ClutterActorClass, pick),
                  NULL, NULL,
                  _clutter_marshal_VOID__BOXED,
                  G_TYPE_NONE, 1,
                  CLUTTER_TYPE_COLOR | G_SIGNAL_TYPE_STATIC_SCOPE);

  /**
   * ClutterActor::allocation-changed:
   * @actor: the #ClutterActor that emitted the signal
   * @box: a #ClutterActorBox with the new allocation
   * @flags: #ClutterAllocationFlags for the allocation
   *
   * The ::allocation-changed signal is emitted when the
   * #ClutterActor:allocation property changes. Usually, application
   * code should just use the notifications for the :allocation property
   * but if you want to track the allocation flags as well, for instance
   * to know whether the absolute origin of @actor changed, then you might
   * want use this signal instead.
   *
   * Since: 1.0
   */
  actor_signals[ALLOCATION_CHANGED] =
    g_signal_new (I_("allocation-changed"),
                  G_TYPE_FROM_CLASS (object_class),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL,
                  _clutter_marshal_VOID__BOXED_FLAGS,
                  G_TYPE_NONE, 2,
                  CLUTTER_TYPE_ACTOR_BOX,
                  CLUTTER_TYPE_ALLOCATION_FLAGS);

  klass->show = clutter_actor_real_show;
  klass->show_all = clutter_actor_show;
  klass->hide = clutter_actor_real_hide;
  klass->hide_all = clutter_actor_hide;
  klass->map = clutter_actor_real_map;
  klass->unmap = clutter_actor_real_unmap;
  klass->unrealize = clutter_actor_real_unrealize;
  klass->pick = clutter_actor_real_pick;
  klass->get_preferred_width = clutter_actor_real_get_preferred_width;
  klass->get_preferred_height = clutter_actor_real_get_preferred_height;
  klass->allocate = clutter_actor_real_allocate;
  klass->queue_redraw = clutter_actor_real_queue_redraw;
  klass->queue_relayout = clutter_actor_real_queue_relayout;
  klass->apply_transform = clutter_actor_real_apply_transform;
  klass->get_accessible = clutter_actor_real_get_accessible;
  klass->get_paint_volume = clutter_actor_real_get_paint_volume;
  klass->has_overlaps = clutter_actor_real_has_overlaps;
}

static void
clutter_actor_init (ClutterActor *self)
{
  ClutterActorPrivate *priv;

  self->priv = priv = CLUTTER_ACTOR_GET_PRIVATE (self);

  priv->parent_actor = NULL;
  priv->has_clip = FALSE;
  priv->opacity = 0xff;
  priv->offscreen_redirect = CLUTTER_OFFSCREEN_REDIRECT_AUTOMATIC_FOR_OPACITY;
  priv->id = _clutter_context_acquire_id (self);
  priv->pick_id = -1;
  priv->scale_x = 1.0;
  priv->scale_y = 1.0;
  priv->show_on_set_parent = TRUE;

  priv->needs_width_request = TRUE;
  priv->needs_height_request = TRUE;
  priv->needs_allocation = TRUE;

  priv->cached_width_age = 1;
  priv->cached_height_age = 1;

  priv->opacity_override = -1;
  priv->enable_model_view_transform = TRUE;

  /* Initialize an empty paint volume to start with */
  _clutter_paint_volume_init_static (&priv->last_paint_volume, NULL);
  priv->last_paint_volume_valid = TRUE;

  priv->transform_valid = FALSE;

  memset (priv->clip, 0, sizeof (gfloat) * 4);
}

/**
 * clutter_actor_destroy:
 * @self: a #ClutterActor
 *
 * Destroys an actor.  When an actor is destroyed, it will break any
 * references it holds to other objects.  If the actor is inside a
 * container, the actor will be removed.
 *
 * When you destroy a container, its children will be destroyed as well.
 *
 * Note: you cannot destroy the #ClutterStage returned by
 * clutter_stage_get_default().
 */
void
clutter_actor_destroy (ClutterActor *self)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  g_object_ref (self);

  /* avoid recursion while destroying */
  if (!CLUTTER_ACTOR_IN_DESTRUCTION (self))
    {
      CLUTTER_SET_PRIVATE_FLAGS (self, CLUTTER_IN_DESTRUCTION);

      /* if we are destroying we want to unrealize ourselves
       * first before the dispose run removes the parent
       */
      if (!CLUTTER_ACTOR_IS_TOPLEVEL (self))
        clutter_actor_update_map_state (self, MAP_STATE_MAKE_UNREALIZED);

      g_object_run_dispose (G_OBJECT (self));

      CLUTTER_UNSET_PRIVATE_FLAGS (self, CLUTTER_IN_DESTRUCTION);
    }

  g_object_unref (self);
}

void
_clutter_actor_finish_queue_redraw (ClutterActor *self,
                                    ClutterPaintVolume *clip)
{
  ClutterActorPrivate *priv = self->priv;
  ClutterPaintVolume *pv;
  gboolean clipped;

  /* If we've been explicitly passed a clip volume then there's
   * nothing more to calculate, but otherwise the only thing we know
   * is that the change is constrained to the given actor.
   *
   * The idea is that if we know the paint volume for where the actor
   * was last drawn (in eye coordinates) and we also have the paint
   * volume for where it will be drawn next (in actor coordinates)
   * then if we queue a redraw for both these volumes that will cover
   * everything that needs to be redrawn to clear the old view and
   * show the latest view of the actor.
   *
   * Don't clip this redraw if we don't know what position we had for
   * the previous redraw since we don't know where to set the clip so
   * it will clear the actor as it is currently.
   */
  if (clip)
    {
      _clutter_actor_set_queue_redraw_clip (self, clip);
      clipped = TRUE;
    }
  else if (G_LIKELY (priv->last_paint_volume_valid))
    {
      pv = _clutter_actor_get_paint_volume_mutable (self);
      if (pv)
        {
          ClutterActor *stage = _clutter_actor_get_stage_internal (self);

          /* make sure we redraw the actors old position... */
          _clutter_actor_set_queue_redraw_clip (stage,
                                                &priv->last_paint_volume);
          _clutter_actor_signal_queue_redraw (stage, stage);
          _clutter_actor_set_queue_redraw_clip (stage, NULL);

          /* XXX: Ideally the redraw signal would take a clip volume
           * argument, but that would be an ABI break. Until we can
           * break the ABI we pass the argument out-of-band via an
           * actor->priv member...
           */

          /* setup the clip for the actors new position... */
          _clutter_actor_set_queue_redraw_clip (self, pv);
          clipped = TRUE;
        }
      else
        clipped = FALSE;
    }
  else
    clipped = FALSE;

  _clutter_actor_signal_queue_redraw (self, self);

  /* Just in case anyone is manually firing redraw signals without
   * using the public queue_redraw() API we are careful to ensure that
   * our out-of-band clip member is cleared before returning...
   *
   * Note: A NULL clip denotes a full-stage, un-clipped redraw
   */
  if (G_LIKELY (clipped))
    _clutter_actor_set_queue_redraw_clip (self, NULL);

  priv->queue_redraw_entry = NULL;
}

static void
_clutter_actor_get_allocation_clip (ClutterActor *self,
                                    ClutterActorBox *clip)
{
  ClutterActorBox allocation;

  /* XXX: we don't care if we get an out of date allocation here
   * because clutter_actor_queue_redraw_with_clip knows to ignore
   * the clip if the actor's allocation is invalid.
   *
   * This is noted because clutter_actor_get_allocation_box does some
   * unnecessary work to support buggy code with a comment suggesting
   * that it could be changed later which would be good for this use
   * case!
   */
  clutter_actor_get_allocation_box (self, &allocation);

  /* NB: clutter_actor_queue_redraw_with_clip expects a box in the
   * actor's own coordinate space but the allocation is in parent
   * coordinates */
  clip->x1 = 0;
  clip->y1 = 0;
  clip->x2 = allocation.x2 - allocation.x1;
  clip->y2 = allocation.y2 - allocation.y1;
}

void
_clutter_actor_queue_redraw_full (ClutterActor       *self,
                                  ClutterRedrawFlags  flags,
                                  ClutterPaintVolume *volume,
                                  ClutterEffect      *effect)
{
  ClutterPaintVolume allocation_pv;
  ClutterActorPrivate *priv;
  ClutterPaintVolume *pv;
  gboolean should_free_pv;
  ClutterActor *stage;
  gboolean was_dirty;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  /* Here's an outline of the actor queue redraw mechanism:
   *
   * The process starts in one of the following two functions which
   * are wrappers for this function:
   * clutter_actor_queue_redraw
   * _clutter_actor_queue_redraw_with_clip
   *
   * additionally, an effect can queue a redraw by wrapping this
   * function in clutter_effect_queue_rerun
   *
   * This functions queues an entry in a list associated with the
   * stage which is a list of actors that queued a redraw while
   * updating the timelines, performing layouting and processing other
   * mainloop sources before the next paint starts.
   *
   * We aim to minimize the processing done at this point because
   * there is a good chance other events will happen while updating
   * the scenegraph that would invalidate any expensive work we might
   * otherwise try to do here. For example we don't try and resolve
   * the screen space bounding box of an actor at this stage so as to
   * minimize how much of the screen redraw because it's possible
   * something else will happen which will force a full redraw anyway.
   *
   * When all updates are complete and we come to paint the stage then
   * we iterate this list and actually emit the "queue-redraw" signals
   * for each of the listed actors which will bubble up to the stage
   * for each actor and at that point we will transform the actors
   * paint volume into screen coordinates to determine the clip region
   * for what needs to be redrawn in the next paint.
   *
   * Besides minimizing redundant work another reason for this
   * deferred design is that it's more likely we will be able to
   * determine the paint volume of an actor once we've finished
   * updating the scenegraph because its allocation should be up to
   * date. NB: If we can't determine an actors paint volume then we
   * can't automatically queue a clipped redraw which can make a big
   * difference to performance.
   *
   * So the control flow goes like this:
   * One of clutter_actor_queue_redraw,
   *        _clutter_actor_queue_redraw_with_clip
   *     or clutter_effect_queue_rerun
   *
   * then control moves to:
   *   _clutter_stage_queue_actor_redraw
   *
   * later during _clutter_stage_do_update, once relayouting is done
   * and the scenegraph has been updated we will call:
   * _clutter_stage_finish_queue_redraws
   *
   * _clutter_stage_finish_queue_redraws will call
   * _clutter_actor_finish_queue_redraw for each listed actor.
   * Note: actors *are* allowed to queue further redraws during this
   * process (considering clone actors or texture_new_from_actor which
   * respond to their source queueing a redraw by queuing a redraw
   * themselves). We repeat the process until the list is empty.
   *
   * This will result in the "queue-redraw" signal being fired for
   * each actor which will pass control to the default signal handler:
   * clutter_actor_real_queue_redraw
   *
   * This will bubble up to the stages handler:
   * clutter_stage_real_queue_redraw
   *
   * clutter_stage_real_queue_redraw will transform the actors paint
   * volume into screen space and add it as a clip region for the next
   * paint.
   */

  stage = _clutter_actor_get_stage_internal (self);

  /* Ignore queuing a redraw for actors not descended from a stage */
  if (stage == NULL)
    return;

  if (flags & CLUTTER_REDRAW_CLIPPED_TO_ALLOCATION)
    {
      ClutterActorBox allocation_clip;
      ClutterVertex origin;

      /* If the actor doesn't have a valid allocation then we will
       * queue a full stage redraw. */
      if (priv->needs_allocation)
        {
          /* NB: NULL denotes an undefined clip which will result in a
           * full redraw... */
          _clutter_actor_set_queue_redraw_clip (self, NULL);
          _clutter_actor_signal_queue_redraw (self, self);
          return;
        }

      _clutter_paint_volume_init_static (&allocation_pv, self);
      pv = &allocation_pv;

      _clutter_actor_get_allocation_clip (self, &allocation_clip);

      origin.x = allocation_clip.x1;
      origin.y = allocation_clip.y1;
      origin.z = 0;
      clutter_paint_volume_set_origin (pv, &origin);
      clutter_paint_volume_set_width (pv,
                                      allocation_clip.x2 - allocation_clip.x1);
      clutter_paint_volume_set_height (pv,
                                       allocation_clip.y2 -
                                       allocation_clip.y1);
      should_free_pv = TRUE;
    }
  else
    {
      pv = volume;
      should_free_pv = FALSE;
    }

  was_dirty = priv->queue_redraw_entry != NULL;

  self->priv->queue_redraw_entry =
    _clutter_stage_queue_actor_redraw (CLUTTER_STAGE (stage),
                                       priv->queue_redraw_entry,
                                       self,
                                       pv);

  if (should_free_pv)
    clutter_paint_volume_free (pv);

  /* If this is the first redraw queued then we can directly use the
     effect parameter */
  if (!was_dirty)
    priv->effect_to_redraw = effect;
  /* Otherwise we need to merge it with the existing effect parameter */
  else if (effect)
    {
      /* If there's already an effect then we need to use whichever is
         later in the chain of actors. Otherwise a full redraw has
         already been queued on the actor so we need to ignore the
         effect parameter */
      if (priv->effect_to_redraw)
        {
          if (priv->effects == NULL)
            g_warning ("Redraw queued with an effect that is "
                       "not applied to the actor");
          else
            {
              const GList *l;

              for (l = _clutter_meta_group_peek_metas (priv->effects);
                   l != NULL;
                   l = l->next)
                {
                  if (l->data == priv->effect_to_redraw ||
                      l->data == effect)
                    priv->effect_to_redraw = l->data;
                }
            }
        }
    }
  else
    /* If no effect is specified then we need to redraw the whole
       actor */
    priv->effect_to_redraw = NULL;
}

/**
 * clutter_actor_queue_redraw:
 * @self: A #ClutterActor
 *
 * Queues up a redraw of an actor and any children. The redraw occurs
 * once the main loop becomes idle (after the current batch of events
 * has been processed, roughly).
 *
 * Applications rarely need to call this, as redraws are handled
 * automatically by modification functions.
 *
 * This function will not do anything if @self is not visible, or
 * if the actor is inside an invisible part of the scenegraph.
 *
 * Also be aware that painting is a NOP for actors with an opacity of
 * 0
 *
 * When you are implementing a custom actor you must queue a redraw
 * whenever some private state changes that will affect painting or
 * picking of your actor.
 */
void
clutter_actor_queue_redraw (ClutterActor *self)
{
  _clutter_actor_queue_redraw_full (self,
                                    0, /* flags */
                                    NULL, /* clip volume */
                                    NULL /* effect */);
}

/*
 * clutter_actor_queue_redraw_with_clip:
 * @self: A #ClutterActor
 * @flags: A mask of #ClutterRedrawFlags controlling the behaviour of
 *   this queue redraw.
 * @volume: A #ClutterPaintVolume describing the bounds of what needs to be
 *   redrawn or %NULL if you are just using a @flag to state your
 *   desired clipping.
 *
 * Queues up a clipped redraw of an actor and any children. The redraw
 * occurs once the main loop becomes idle (after the current batch of
 * events has been processed, roughly).
 *
 * If no flags are given the clip volume is defined by @volume
 * specified in actor coordinates and tells Clutter that only content
 * within this volume has been changed so Clutter can optionally
 * optimize the redraw.
 *
 * If the %CLUTTER_REDRAW_CLIPPED_TO_ALLOCATION @flag is used, @volume
 * should be %NULL and this tells Clutter to use the actor's current
 * allocation as a clip box. This flag can only be used for 2D actors,
 * because any actor with depth may be projected outside its
 * allocation.
 *
 * Applications rarely need to call this, as redraws are handled
 * automatically by modification functions.
 *
 * This function will not do anything if @self is not visible, or if
 * the actor is inside an invisible part of the scenegraph.
 *
 * Also be aware that painting is a NOP for actors with an opacity of
 * 0
 *
 * When you are implementing a custom actor you must queue a redraw
 * whenever some private state changes that will affect painting or
 * picking of your actor.
 */
void
_clutter_actor_queue_redraw_with_clip (ClutterActor       *self,
                                       ClutterRedrawFlags  flags,
                                       ClutterPaintVolume *volume)
{
  _clutter_actor_queue_redraw_full (self,
                                    flags, /* flags */
                                    volume, /* clip volume */
                                    NULL /* effect */);
}

static void
_clutter_actor_queue_only_relayout (ClutterActor *self)
{
  ClutterActorPrivate *priv = self->priv;

  if (CLUTTER_ACTOR_IN_DESTRUCTION (self))
    return;

  if (priv->needs_width_request &&
      priv->needs_height_request &&
      priv->needs_allocation)
    return; /* save some cpu cycles */

#if CLUTTER_ENABLE_DEBUG
  if (!CLUTTER_ACTOR_IS_TOPLEVEL (self) && CLUTTER_ACTOR_IN_RELAYOUT (self))
    {
      g_warning ("The actor '%s' is currently inside an allocation "
                 "cycle; calling clutter_actor_queue_relayout() is "
                 "not recommended",
                 _clutter_actor_get_debug_name (self));
    }
#endif /* CLUTTER_ENABLE_DEBUG */

  g_signal_emit (self, actor_signals[QUEUE_RELAYOUT], 0);
}

/**
 * clutter_actor_queue_relayout:
 * @self: A #ClutterActor
 *
 * Indicates that the actor's size request or other layout-affecting
 * properties may have changed. This function is used inside #ClutterActor
 * subclass implementations, not by applications directly.
 *
 * Queueing a new layout automatically queues a redraw as well.
 *
 * Since: 0.8
 */
void
clutter_actor_queue_relayout (ClutterActor *self)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  _clutter_actor_queue_only_relayout (self);
  clutter_actor_queue_redraw (self);
}

/**
 * clutter_actor_get_preferred_size:
 * @self: a #ClutterActor
 * @min_width_p: (out) (allow-none): return location for the minimum
 *   width, or %NULL
 * @min_height_p: (out) (allow-none): return location for the minimum
 *   height, or %NULL
 * @natural_width_p: (out) (allow-none): return location for the natural
 *   width, or %NULL
 * @natural_height_p: (out) (allow-none): return location for the natural
 *   height, or %NULL
 *
 * Computes the preferred minimum and natural size of an actor, taking into
 * account the actor's geometry management (either height-for-width
 * or width-for-height).
 *
 * The width and height used to compute the preferred height and preferred
 * width are the actor's natural ones.
 *
 * If you need to control the height for the preferred width, or the width for
 * the preferred height, you should use clutter_actor_get_preferred_width()
 * and clutter_actor_get_preferred_height(), and check the actor's preferred
 * geometry management using the #ClutterActor:request-mode property.
 *
 * Since: 0.8
 */
void
clutter_actor_get_preferred_size (ClutterActor *self,
                                  gfloat       *min_width_p,
                                  gfloat       *min_height_p,
                                  gfloat       *natural_width_p,
                                  gfloat       *natural_height_p)
{
  ClutterActorPrivate *priv;
  gfloat min_width, min_height;
  gfloat natural_width, natural_height;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  min_width = min_height = 0;
  natural_width = natural_height = 0;

  if (priv->request_mode == CLUTTER_REQUEST_HEIGHT_FOR_WIDTH)
    {
      CLUTTER_NOTE (LAYOUT, "Preferred size (height-for-width)");
      clutter_actor_get_preferred_width (self, -1,
                                         &min_width,
                                         &natural_width);
      clutter_actor_get_preferred_height (self, natural_width,
                                          &min_height,
                                          &natural_height);
    }
  else
    {
      CLUTTER_NOTE (LAYOUT, "Preferred size (width-for-height)");
      clutter_actor_get_preferred_height (self, -1,
                                          &min_height,
                                          &natural_height);
      clutter_actor_get_preferred_width (self, natural_height,
                                         &min_width,
                                         &natural_width);
    }

  if (min_width_p)
    *min_width_p = min_width;

  if (min_height_p)
    *min_height_p = min_height;

  if (natural_width_p)
    *natural_width_p = natural_width;

  if (natural_height_p)
    *natural_height_p = natural_height;
}

/* looks for a cached size request for this for_size. If not
 * found, returns the oldest entry so it can be overwritten */
static gboolean
_clutter_actor_get_cached_size_request (gfloat         for_size,
                                        SizeRequest   *cached_size_requests,
                                        SizeRequest  **result)
{
  guint i;

  *result = &cached_size_requests[0];

  for (i = 0; i < N_CACHED_SIZE_REQUESTS; i++)
    {
      SizeRequest *sr;

      sr = &cached_size_requests[i];

      if (sr->age > 0 &&
          sr->for_size == for_size)
        {
          CLUTTER_NOTE (LAYOUT, "Size cache hit for size: %.2f", for_size);
          *result = sr;
          return TRUE;
        }
      else if (sr->age < (*result)->age)
        {
          *result = sr;
        }
    }

  CLUTTER_NOTE (LAYOUT, "Size cache miss for size: %.2f", for_size);

  return FALSE;
}

/**
 * clutter_actor_get_preferred_width:
 * @self: A #ClutterActor
 * @for_height: available height when computing the preferred width,
 *   or a negative value to indicate that no height is defined
 * @min_width_p: (out) (allow-none): return location for minimum width,
 *   or %NULL
 * @natural_width_p: (out) (allow-none): return location for the natural
 *   width, or %NULL
 *
 * Computes the requested minimum and natural widths for an actor,
 * optionally depending on the specified height, or if they are
 * already computed, returns the cached values.
 *
 * An actor may not get its request - depending on the layout
 * manager that's in effect.
 *
 * A request should not incorporate the actor's scale or anchor point;
 * those transformations do not affect layout, only rendering.
 *
 * Since: 0.8
 */
void
clutter_actor_get_preferred_width (ClutterActor *self,
                                   gfloat        for_height,
                                   gfloat       *min_width_p,
                                   gfloat       *natural_width_p)
{
  ClutterActorClass *klass;
  ClutterActorPrivate *priv;
  gboolean found_in_cache;
  SizeRequest *cached_size_request;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  klass = CLUTTER_ACTOR_GET_CLASS (self);
  priv = self->priv;

  found_in_cache = FALSE;
  cached_size_request = &priv->width_requests[0];

  if (!priv->needs_width_request)
    found_in_cache = _clutter_actor_get_cached_size_request (for_height,
                                                             priv->width_requests,
                                                             &cached_size_request);

  if (!found_in_cache)
    {
      gfloat min_width, natural_width;

      min_width = natural_width = 0;

      CLUTTER_NOTE (LAYOUT, "Width request for %.2f px", for_height);

      klass->get_preferred_width (self, for_height,
                                  &min_width,
                                  &natural_width);

      /* Due to accumulated float errors, it's better not to warn
       * on this, but just fix it.
       */
      if (natural_width < min_width)
	natural_width = min_width;

      cached_size_request->min_size = min_width;
      cached_size_request->natural_size = natural_width;
      cached_size_request->for_size = for_height;
      cached_size_request->age = priv->cached_width_age;

      priv->cached_width_age ++;
      priv->needs_width_request = FALSE;
    }

  if (!priv->min_width_set)
    priv->request_min_width = cached_size_request->min_size;

  if (!priv->natural_width_set)
    priv->request_natural_width = cached_size_request->natural_size;

  if (min_width_p)
    *min_width_p = priv->request_min_width;

  if (natural_width_p)
    *natural_width_p = priv->request_natural_width;
}

/**
 * clutter_actor_get_preferred_height:
 * @self: A #ClutterActor
 * @for_width: available width to assume in computing desired height,
 *   or a negative value to indicate that no width is defined
 * @min_height_p: (out) (allow-none): return location for minimum height,
 *   or %NULL
 * @natural_height_p: (out) (allow-none): return location for natural
 *   height, or %NULL
 *
 * Computes the requested minimum and natural heights for an actor,
 * or if they are already computed, returns the cached values.
 *
 * An actor may not get its request - depending on the layout
 * manager that's in effect.
 *
 * A request should not incorporate the actor's scale or anchor point;
 * those transformations do not affect layout, only rendering.
 *
 * Since: 0.8
 */
void
clutter_actor_get_preferred_height (ClutterActor *self,
                                    gfloat        for_width,
                                    gfloat       *min_height_p,
                                    gfloat       *natural_height_p)
{
  ClutterActorClass *klass;
  ClutterActorPrivate *priv;
  gboolean found_in_cache;
  SizeRequest *cached_size_request;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  klass = CLUTTER_ACTOR_GET_CLASS (self);
  priv = self->priv;

  found_in_cache = FALSE;
  cached_size_request = &priv->height_requests[0];

  if (!priv->needs_height_request)
    found_in_cache = _clutter_actor_get_cached_size_request (for_width,
                                                             priv->height_requests,
                                                             &cached_size_request);

  if (!found_in_cache)
    {
      gfloat min_height, natural_height;

      min_height = natural_height = 0;

      CLUTTER_NOTE (LAYOUT, "Height request for %.2f px", for_width);

      klass->get_preferred_height (self, for_width,
                                   &min_height,
                                   &natural_height);

      /* Due to accumulated float errors, it's better not to warn
       * on this, but just fix it.
       */
      if (natural_height < min_height)
	natural_height = min_height;

      if (!priv->min_height_set)
        {
          priv->request_min_height = min_height;
        }

      if (!priv->natural_height_set)
        {
          priv->request_natural_height = natural_height;
        }

      cached_size_request->min_size = min_height;
      cached_size_request->natural_size = natural_height;
      cached_size_request->for_size = for_width;
      cached_size_request->age = priv->cached_height_age;

      priv->cached_height_age ++;

      priv->needs_height_request = FALSE;
    }

  if (!priv->min_height_set)
    priv->request_min_height = cached_size_request->min_size;

  if (!priv->natural_height_set)
    priv->request_natural_height = cached_size_request->natural_size;

  if (min_height_p)
    *min_height_p = priv->request_min_height;

  if (natural_height_p)
    *natural_height_p = priv->request_natural_height;
}

/**
 * clutter_actor_get_allocation_box:
 * @self: A #ClutterActor
 * @box: (out): the function fills this in with the actor's allocation
 *
 * Gets the layout box an actor has been assigned. The allocation can
 * only be assumed valid inside a paint() method; anywhere else, it
 * may be out-of-date.
 *
 * An allocation does not incorporate the actor's scale or anchor point;
 * those transformations do not affect layout, only rendering.
 *
 * <note>Do not call any of the clutter_actor_get_allocation_*() family
 * of functions inside the implementation of the get_preferred_width()
 * or get_preferred_height() virtual functions.</note>
 *
 * Since: 0.8
 */
void
clutter_actor_get_allocation_box (ClutterActor    *self,
                                  ClutterActorBox *box)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  /* XXX - if needs_allocation=TRUE, we can either 1) g_return_if_fail,
   * which limits calling get_allocation to inside paint() basically; or
   * we can 2) force a layout, which could be expensive if someone calls
   * get_allocation somewhere silly; or we can 3) just return the latest
   * value, allowing it to be out-of-date, and assume people know what
   * they are doing.
   *
   * The least-surprises approach that keeps existing code working is
   * likely to be 2). People can end up doing some inefficient things,
   * though, and in general code that requires 2) is probably broken.
   */

  /* this implements 2) */
  if (G_UNLIKELY (self->priv->needs_allocation))
    {
      ClutterActor *stage = _clutter_actor_get_stage_internal (self);

      /* do not queue a relayout on an unparented actor */
      if (stage)
        _clutter_stage_maybe_relayout (stage);
    }

  /* commenting out the code above and just keeping this assigment
   * implements 3)
   */
  *box = self->priv->allocation;
}

/**
 * clutter_actor_get_allocation_geometry:
 * @self: A #ClutterActor
 * @geom: (out): allocation geometry in pixels
 *
 * Gets the layout box an actor has been assigned.  The allocation can
 * only be assumed valid inside a paint() method; anywhere else, it
 * may be out-of-date.
 *
 * An allocation does not incorporate the actor's scale or anchor point;
 * those transformations do not affect layout, only rendering.
 *
 * The returned rectangle is in pixels.
 *
 * Since: 0.8
 */
void
clutter_actor_get_allocation_geometry (ClutterActor    *self,
                                       ClutterGeometry *geom)
{
  ClutterActorBox box;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (geom != NULL);

  clutter_actor_get_allocation_box (self, &box);

  geom->x = clutter_actor_box_get_x (&box);
  geom->y = clutter_actor_box_get_y (&box);
  geom->width = clutter_actor_box_get_width (&box);
  geom->height = clutter_actor_box_get_height (&box);
}

/**
 * clutter_actor_allocate:
 * @self: A #ClutterActor
 * @box: new allocation of the actor, in parent-relative coordinates
 * @flags: flags that control the allocation
 *
 * Called by the parent of an actor to assign the actor its size.
 * Should never be called by applications (except when implementing
 * a container or layout manager).
 *
 * Actors can know from their allocation box whether they have moved
 * with respect to their parent actor. The @flags parameter describes
 * additional information about the allocation, for instance whether
 * the parent has moved with respect to the stage, for example because
 * a grandparent's origin has moved.
 *
 * Since: 0.8
 */
void
clutter_actor_allocate (ClutterActor           *self,
                        const ClutterActorBox  *box,
                        ClutterAllocationFlags  flags)
{
  ClutterActorPrivate *priv;
  ClutterActorClass *klass;
  ClutterActorBox alloc;
  gboolean child_moved;
  gboolean stage_allocation_changed;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  if (G_UNLIKELY (_clutter_actor_get_stage_internal (self) == NULL))
    {
      g_warning ("Spurious clutter_actor_allocate called for actor %p/%s "
                 "which isn't a descendent of the stage!\n",
                 self, _clutter_actor_get_debug_name (self));
      return;
    }

  priv = self->priv;

  alloc = *box;

  if (priv->constraints != NULL)
    {
      const GList *constraints, *l;

      constraints = _clutter_meta_group_peek_metas (priv->constraints);
      for (l = constraints; l != NULL; l = l->next)
        {
          ClutterConstraint *constraint = l->data;
          ClutterActorMeta *meta = l->data;

          if (clutter_actor_meta_get_enabled (meta))
            _clutter_constraint_update_allocation (constraint, self, &alloc);
        }
    }

  child_moved = (alloc.x1 != priv->allocation.x1 ||
                 alloc.y1 != priv->allocation.y1);

  if (flags & CLUTTER_ABSOLUTE_ORIGIN_CHANGED ||
      child_moved ||
      alloc.x2 != priv->allocation.x2 ||
      alloc.y2 != priv->allocation.y2)
    stage_allocation_changed = TRUE;
  else
    stage_allocation_changed = FALSE;

  /* If we get an allocation "out of the blue"
   * (we did not queue relayout), then we want to
   * ignore it. But if we have needs_allocation set,
   * we want to guarantee that allocate() virtual
   * method is always called, i.e. that queue_relayout()
   * always results in an allocate() invocation on
   * an actor.
   *
   * The optimization here is to avoid re-allocating
   * actors that did not queue relayout and were
   * not moved.
   */

  if (!priv->needs_allocation && !stage_allocation_changed)
    {
      CLUTTER_NOTE (LAYOUT, "No allocation needed");
      return;
    }

  /* When ABSOLUTE_ORIGIN_CHANGED is passed in to
   * clutter_actor_allocate(), it indicates whether the parent has its
   * absolute origin moved; when passed in to ClutterActor::allocate()
   * virtual method though, it indicates whether the child has its
   * absolute origin moved.  So we set it when child_moved is TRUE
   */
  if (child_moved)
    flags |= CLUTTER_ABSOLUTE_ORIGIN_CHANGED;

  CLUTTER_SET_PRIVATE_FLAGS (self, CLUTTER_IN_RELAYOUT);

  klass = CLUTTER_ACTOR_GET_CLASS (self);
  klass->allocate (self, &alloc, flags);

  CLUTTER_UNSET_PRIVATE_FLAGS (self, CLUTTER_IN_RELAYOUT);

  if (stage_allocation_changed)
    clutter_actor_queue_redraw (self);
}

/**
 * clutter_actor_set_geometry:
 * @self: A #ClutterActor
 * @geometry: A #ClutterGeometry
 *
 * Sets the actor's fixed position and forces its minimum and natural
 * size, in pixels. This means the untransformed actor will have the
 * given geometry. This is the same as calling clutter_actor_set_position()
 * and clutter_actor_set_size().
 */
void
clutter_actor_set_geometry (ClutterActor          *self,
			    const ClutterGeometry *geometry)
{
  g_object_freeze_notify (G_OBJECT (self));

  clutter_actor_set_position (self, geometry->x, geometry->y);
  clutter_actor_set_size (self, geometry->width, geometry->height);

  g_object_thaw_notify (G_OBJECT (self));
}

/**
 * clutter_actor_get_geometry:
 * @self: A #ClutterActor
 * @geometry: (out): A location to store actors #ClutterGeometry
 *
 * Gets the size and position of an actor relative to its parent
 * actor. This is the same as calling clutter_actor_get_position() and
 * clutter_actor_get_size(). It tries to "do what you mean" and get the
 * requested size and position if the actor's allocation is invalid.
 */
void
clutter_actor_get_geometry (ClutterActor    *self,
			    ClutterGeometry *geometry)
{
  gfloat x, y, width, height;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (geometry != NULL);

  clutter_actor_get_position (self, &x, &y);
  clutter_actor_get_size (self, &width, &height);

  geometry->x = (int) x;
  geometry->y = (int) y;
  geometry->width = (int) width;
  geometry->height = (int) height;
}

/**
 * clutter_actor_set_position
 * @self: A #ClutterActor
 * @x: New left position of actor in pixels.
 * @y: New top position of actor in pixels.
 *
 * Sets the actor's fixed position in pixels relative to any parent
 * actor.
 *
 * If a layout manager is in use, this position will override the
 * layout manager and force a fixed position.
 */
void
clutter_actor_set_position (ClutterActor *self,
			    gfloat        x,
			    gfloat        y)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  g_object_freeze_notify (G_OBJECT (self));

  clutter_actor_set_x (self, x);
  clutter_actor_set_y (self, y);

  g_object_thaw_notify (G_OBJECT (self));
}

/**
 * clutter_actor_get_fixed_position_set:
 * @self: A #ClutterActor
 *
 * Checks whether an actor has a fixed position set (and will thus be
 * unaffected by any layout manager).
 *
 * Return value: %TRUE if the fixed position is set on the actor
 *
 * Since: 0.8
 */
gboolean
clutter_actor_get_fixed_position_set (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), FALSE);

  return self->priv->position_set;
}

/**
 * clutter_actor_set_fixed_position_set:
 * @self: A #ClutterActor
 * @is_set: whether to use fixed position
 *
 * Sets whether an actor has a fixed position set (and will thus be
 * unaffected by any layout manager).
 *
 * Since: 0.8
 */
void
clutter_actor_set_fixed_position_set (ClutterActor *self,
                                      gboolean      is_set)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  if (self->priv->position_set == (is_set != FALSE))
    return;

  self->priv->position_set = is_set != FALSE;
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_FIXED_POSITION_SET]);

  clutter_actor_queue_relayout (self);
}

/**
 * clutter_actor_move_by:
 * @self: A #ClutterActor
 * @dx: Distance to move Actor on X axis.
 * @dy: Distance to move Actor on Y axis.
 *
 * Moves an actor by the specified distance relative to its current
 * position in pixels.
 *
 * This function modifies the fixed position of an actor and thus removes
 * it from any layout management. Another way to move an actor is with an
 * anchor point, see clutter_actor_set_anchor_point().
 *
 * Since: 0.2
 */
void
clutter_actor_move_by (ClutterActor *self,
		       gfloat        dx,
		       gfloat        dy)
{
  ClutterActorPrivate *priv;
  gfloat x, y;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  x = priv->fixed_x;
  y = priv->fixed_y;

  clutter_actor_set_position (self, x + dx, y + dy);
}

static void
clutter_actor_set_min_width (ClutterActor *self,
                             gfloat        min_width)
{
  ClutterActorPrivate *priv = self->priv;
  ClutterActorBox old = { 0, };

  /* if we are setting the size on a top-level actor and the
   * backend only supports static top-levels (e.g. framebuffers)
   * then we ignore the passed value and we override it with
   * the stage implementation's preferred size.
   */
  if (CLUTTER_ACTOR_IS_TOPLEVEL (self) &&
      clutter_feature_available (CLUTTER_FEATURE_STAGE_STATIC))
    return;

  if (priv->min_width_set && min_width == priv->request_min_width)
    return;

  g_object_freeze_notify (G_OBJECT (self));

  clutter_actor_store_old_geometry (self, &old);

  priv->request_min_width = min_width;
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_MIN_WIDTH]);
  clutter_actor_set_min_width_set (self, TRUE);

  clutter_actor_notify_if_geometry_changed (self, &old);

  g_object_thaw_notify (G_OBJECT (self));

  clutter_actor_queue_relayout (self);
}

static void
clutter_actor_set_min_height (ClutterActor *self,
                              gfloat        min_height)

{
  ClutterActorPrivate *priv = self->priv;
  ClutterActorBox old = { 0, };

  /* if we are setting the size on a top-level actor and the
   * backend only supports static top-levels (e.g. framebuffers)
   * then we ignore the passed value and we override it with
   * the stage implementation's preferred size.
   */
  if (CLUTTER_ACTOR_IS_TOPLEVEL (self) &&
      clutter_feature_available (CLUTTER_FEATURE_STAGE_STATIC))
    return;

  if (priv->min_height_set && min_height == priv->request_min_height)
    return;

  g_object_freeze_notify (G_OBJECT (self));

  clutter_actor_store_old_geometry (self, &old);

  priv->request_min_height = min_height;
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_MIN_HEIGHT]);
  clutter_actor_set_min_height_set (self, TRUE);

  clutter_actor_notify_if_geometry_changed (self, &old);

  g_object_thaw_notify (G_OBJECT (self));

  clutter_actor_queue_relayout (self);
}

static void
clutter_actor_set_natural_width (ClutterActor *self,
                                 gfloat        natural_width)
{
  ClutterActorPrivate *priv = self->priv;
  ClutterActorBox old = { 0, };

  /* if we are setting the size on a top-level actor and the
   * backend only supports static top-levels (e.g. framebuffers)
   * then we ignore the passed value and we override it with
   * the stage implementation's preferred size.
   */
  if (CLUTTER_ACTOR_IS_TOPLEVEL (self) &&
      clutter_feature_available (CLUTTER_FEATURE_STAGE_STATIC))
    return;

  if (priv->natural_width_set &&
      natural_width == priv->request_natural_width)
    return;

  g_object_freeze_notify (G_OBJECT (self));

  clutter_actor_store_old_geometry (self, &old);

  priv->request_natural_width = natural_width;
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_NATURAL_WIDTH]);
  clutter_actor_set_natural_width_set (self, TRUE);

  clutter_actor_notify_if_geometry_changed (self, &old);

  g_object_thaw_notify (G_OBJECT (self));

  clutter_actor_queue_relayout (self);
}

static void
clutter_actor_set_natural_height (ClutterActor *self,
                                  gfloat        natural_height)
{
  ClutterActorPrivate *priv = self->priv;
  ClutterActorBox old = { 0, };

  /* if we are setting the size on a top-level actor and the
   * backend only supports static top-levels (e.g. framebuffers)
   * then we ignore the passed value and we override it with
   * the stage implementation's preferred size.
   */
  if (CLUTTER_ACTOR_IS_TOPLEVEL (self) &&
      clutter_feature_available (CLUTTER_FEATURE_STAGE_STATIC))
    return;

  if (priv->natural_height_set &&
      natural_height == priv->request_natural_height)
    return;

  g_object_freeze_notify (G_OBJECT (self));

  clutter_actor_store_old_geometry (self, &old);

  priv->request_natural_height = natural_height;
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_NATURAL_HEIGHT]);
  clutter_actor_set_natural_height_set (self, TRUE);

  clutter_actor_notify_if_geometry_changed (self, &old);

  g_object_thaw_notify (G_OBJECT (self));

  clutter_actor_queue_relayout (self);
}

static void
clutter_actor_set_min_width_set (ClutterActor *self,
                                 gboolean      use_min_width)
{
  ClutterActorPrivate *priv = self->priv;
  ClutterActorBox old = { 0, };

  if (priv->min_width_set == (use_min_width != FALSE))
    return;

  clutter_actor_store_old_geometry (self, &old);

  priv->min_width_set = use_min_width != FALSE;
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_MIN_WIDTH_SET]);

  clutter_actor_notify_if_geometry_changed (self, &old);

  clutter_actor_queue_relayout (self);
}

static void
clutter_actor_set_min_height_set (ClutterActor *self,
                                  gboolean      use_min_height)
{
  ClutterActorPrivate *priv = self->priv;
  ClutterActorBox old = { 0, };

  if (priv->min_height_set == (use_min_height != FALSE))
    return;

  clutter_actor_store_old_geometry (self, &old);

  priv->min_height_set = use_min_height != FALSE;
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_MIN_HEIGHT_SET]);

  clutter_actor_notify_if_geometry_changed (self, &old);

  clutter_actor_queue_relayout (self);
}

static void
clutter_actor_set_natural_width_set (ClutterActor *self,
                                     gboolean      use_natural_width)
{
  ClutterActorPrivate *priv = self->priv;
  ClutterActorBox old = { 0, };

  if (priv->natural_width_set == (use_natural_width != FALSE))
    return;

  clutter_actor_store_old_geometry (self, &old);

  priv->natural_width_set = use_natural_width != FALSE;
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_NATURAL_WIDTH_SET]);

  clutter_actor_notify_if_geometry_changed (self, &old);

  clutter_actor_queue_relayout (self);
}

static void
clutter_actor_set_natural_height_set (ClutterActor *self,
                                      gboolean      use_natural_height)
{
  ClutterActorPrivate *priv = self->priv;
  ClutterActorBox old = { 0, };

  if (priv->natural_height_set == (use_natural_height != FALSE))
    return;

  clutter_actor_store_old_geometry (self, &old);

  priv->natural_height_set = use_natural_height != FALSE;
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_NATURAL_HEIGHT_SET]);

  clutter_actor_notify_if_geometry_changed (self, &old);

  clutter_actor_queue_relayout (self);
}

/**
 * clutter_actor_set_request_mode:
 * @self: a #ClutterActor
 * @mode: the request mode
 *
 * Sets the geometry request mode of @self.
 *
 * The @mode determines the order for invoking
 * clutter_actor_get_preferred_width() and
 * clutter_actor_get_preferred_height()
 *
 * Since: 1.2
 */
void
clutter_actor_set_request_mode (ClutterActor       *self,
                                ClutterRequestMode  mode)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  if (priv->request_mode == mode)
    return;

  priv->request_mode = mode;

  priv->needs_width_request = TRUE;
  priv->needs_height_request = TRUE;

  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_REQUEST_MODE]);

  clutter_actor_queue_relayout (self);
}

/**
 * clutter_actor_get_request_mode:
 * @self: a #ClutterActor
 *
 * Retrieves the geometry request mode of @self
 *
 * Return value: the request mode for the actor
 *
 * Since: 1.2
 */
ClutterRequestMode
clutter_actor_get_request_mode (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self),
                        CLUTTER_REQUEST_HEIGHT_FOR_WIDTH);

  return self->priv->request_mode;
}

/* variant of set_width() without checks and without notification
 * freeze+thaw, for internal usage only
 */
static inline void
clutter_actor_set_width_internal (ClutterActor *self,
                                  gfloat        width)
{
  if (width >= 0)
    {
      /* the Stage will use the :min-width to control the minimum
       * width to be resized to, so we should not be setting it
       * along with the :natural-width
       */
      if (!CLUTTER_ACTOR_IS_TOPLEVEL (self))
        clutter_actor_set_min_width (self, width);

      clutter_actor_set_natural_width (self, width);
    }
  else
    {
      /* we only unset the :natural-width for the Stage */
      if (!CLUTTER_ACTOR_IS_TOPLEVEL (self))
        clutter_actor_set_min_width_set (self, FALSE);

      clutter_actor_set_natural_width_set (self, FALSE);
    }
}

/* variant of set_height() without checks and without notification
 * freeze+thaw, for internal usage only
 */
static inline void
clutter_actor_set_height_internal (ClutterActor *self,
                                   gfloat        height)
{
  if (height >= 0)
    {
      /* see the comment above in set_width_internal() */
      if (!CLUTTER_ACTOR_IS_TOPLEVEL (self))
        clutter_actor_set_min_height (self, height);

      clutter_actor_set_natural_height (self, height);
    }
  else
    {
      /* see the comment above in set_width_internal() */
      if (!CLUTTER_ACTOR_IS_TOPLEVEL (self))
        clutter_actor_set_min_height_set (self, FALSE);

      clutter_actor_set_natural_height_set (self, FALSE);
    }
}

/**
 * clutter_actor_set_size
 * @self: A #ClutterActor
 * @width: New width of actor in pixels, or -1
 * @height: New height of actor in pixels, or -1
 *
 * Sets the actor's size request in pixels. This overrides any
 * "normal" size request the actor would have. For example
 * a text actor might normally request the size of the text;
 * this function would force a specific size instead.
 *
 * If @width and/or @height are -1 the actor will use its
 * "normal" size request instead of overriding it, i.e.
 * you can "unset" the size with -1.
 *
 * This function sets or unsets both the minimum and natural size.
 */
void
clutter_actor_set_size (ClutterActor *self,
			gfloat        width,
			gfloat        height)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  g_object_freeze_notify (G_OBJECT (self));

  clutter_actor_set_width_internal (self, width);
  clutter_actor_set_height_internal (self, height);

  g_object_thaw_notify (G_OBJECT (self));
}

/**
 * clutter_actor_get_size:
 * @self: A #ClutterActor
 * @width: (out) (allow-none): return location for the width, or %NULL.
 * @height: (out) (allow-none): return location for the height, or %NULL.
 *
 * This function tries to "do what you mean" and return
 * the size an actor will have. If the actor has a valid
 * allocation, the allocation will be returned; otherwise,
 * the actors natural size request will be returned.
 *
 * If you care whether you get the request vs. the allocation, you
 * should probably call a different function like
 * clutter_actor_get_allocation_box() or
 * clutter_actor_get_preferred_width().
 *
 * Since: 0.2
 */
void
clutter_actor_get_size (ClutterActor *self,
			gfloat       *width,
			gfloat       *height)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  if (width)
    *width = clutter_actor_get_width (self);

  if (height)
    *height = clutter_actor_get_height (self);
}

/**
 * clutter_actor_get_position:
 * @self: a #ClutterActor
 * @x: (out) (allow-none): return location for the X coordinate, or %NULL
 * @y: (out) (allow-none): return location for the Y coordinate, or %NULL
 *
 * This function tries to "do what you mean" and tell you where the
 * actor is, prior to any transformations. Retrieves the fixed
 * position of an actor in pixels, if one has been set; otherwise, if
 * the allocation is valid, returns the actor's allocated position;
 * otherwise, returns 0,0.
 *
 * The returned position is in pixels.
 *
 * Since: 0.6
 */
void
clutter_actor_get_position (ClutterActor *self,
                            gfloat       *x,
                            gfloat       *y)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  if (x)
    *x = clutter_actor_get_x (self);

  if (y)
    *y = clutter_actor_get_y (self);
}

/**
 * clutter_actor_get_transformed_position:
 * @self: A #ClutterActor
 * @x: (out) (allow-none): return location for the X coordinate, or %NULL
 * @y: (out) (allow-none): return location for the Y coordinate, or %NULL
 *
 * Gets the absolute position of an actor, in pixels relative to the stage.
 *
 * Since: 0.8
 */
void
clutter_actor_get_transformed_position (ClutterActor *self,
                                        gfloat       *x,
                                        gfloat       *y)
{
  ClutterVertex v1;
  ClutterVertex v2;

  v1.x = v1.y = v1.z = 0;
  clutter_actor_apply_transform_to_point (self, &v1, &v2);

  if (x)
    *x = v2.x;

  if (y)
    *y = v2.y;
}

/**
 * clutter_actor_get_transformed_size:
 * @self: A #ClutterActor
 * @width: (out) (allow-none): return location for the width, or %NULL
 * @height: (out) (allow-none): return location for the height, or %NULL
 *
 * Gets the absolute size of an actor in pixels, taking into account the
 * scaling factors.
 *
 * If the actor has a valid allocation, the allocated size will be used.
 * If the actor has not a valid allocation then the preferred size will
 * be transformed and returned.
 *
 * If you want the transformed allocation, see
 * clutter_actor_get_abs_allocation_vertices() instead.
 *
 * <note>When the actor (or one of its ancestors) is rotated around the
 * X or Y axis, it no longer appears as on the stage as a rectangle, but
 * as a generic quadrangle; in that case this function returns the size
 * of the smallest rectangle that encapsulates the entire quad. Please
 * note that in this case no assumptions can be made about the relative
 * position of this envelope to the absolute position of the actor, as
 * returned by clutter_actor_get_transformed_position(); if you need this
 * information, you need to use clutter_actor_get_abs_allocation_vertices()
 * to get the coords of the actual quadrangle.</note>
 *
 * Since: 0.8
 */
void
clutter_actor_get_transformed_size (ClutterActor *self,
                                    gfloat       *width,
                                    gfloat       *height)
{
  ClutterActorPrivate *priv;
  ClutterVertex v[4];
  gfloat x_min, x_max, y_min, y_max;
  gint i;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  /* if the actor hasn't been allocated yet, get the preferred
   * size and transform that
   */
  if (priv->needs_allocation)
    {
      gfloat natural_width, natural_height;
      ClutterActorBox box;

      /* Make a fake allocation to transform.
       *
       * NB: _clutter_actor_transform_and_project_box expects a box in
       * the actor's coordinate space... */

      box.x1 = 0;
      box.y1 = 0;

      natural_width = natural_height = 0;
      clutter_actor_get_preferred_size (self, NULL, NULL,
                                        &natural_width,
                                        &natural_height);

      box.x2 = natural_width;
      box.y2 = natural_height;

      _clutter_actor_transform_and_project_box (self, &box, v);
    }
  else
    clutter_actor_get_abs_allocation_vertices (self, v);

  x_min = x_max = v[0].x;
  y_min = y_max = v[0].y;

  for (i = 1; i < G_N_ELEMENTS (v); ++i)
    {
      if (v[i].x < x_min)
	x_min = v[i].x;

      if (v[i].x > x_max)
	x_max = v[i].x;

      if (v[i].y < y_min)
	y_min = v[i].y;

      if (v[i].y > y_max)
	y_max = v[i].y;
    }

  if (width)
    *width  = x_max - x_min;

  if (height)
    *height = y_max - y_min;
}

/**
 * clutter_actor_get_width:
 * @self: A #ClutterActor
 *
 * Retrieves the width of a #ClutterActor.
 *
 * If the actor has a valid allocation, this function will return the
 * width of the allocated area given to the actor.
 *
 * If the actor does not have a valid allocation, this function will
 * return the actor's natural width, that is the preferred width of
 * the actor.
 *
 * If you care whether you get the preferred width or the width that
 * has been assigned to the actor, you should probably call a different
 * function like clutter_actor_get_allocation_box() to retrieve the
 * allocated size or clutter_actor_get_preferred_width() to retrieve the
 * preferred width.
 *
 * If an actor has a fixed width, for instance a width that has been
 * assigned using clutter_actor_set_width(), the width returned will
 * be the same value.
 *
 * Return value: the width of the actor, in pixels
 */
gfloat
clutter_actor_get_width (ClutterActor *self)
{
  ClutterActorPrivate *priv;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), 0);

  priv = self->priv;

  if (priv->needs_allocation)
    {
      gfloat natural_width = 0;

      if (self->priv->request_mode == CLUTTER_REQUEST_HEIGHT_FOR_WIDTH)
        clutter_actor_get_preferred_width (self, -1, NULL, &natural_width);
      else
        {
          gfloat natural_height = 0;

          clutter_actor_get_preferred_height (self, -1, NULL, &natural_height);
          clutter_actor_get_preferred_width (self, natural_height,
                                             NULL,
                                             &natural_width);
        }

      return natural_width;
    }
  else
    return priv->allocation.x2 - priv->allocation.x1;
}

/**
 * clutter_actor_get_height:
 * @self: A #ClutterActor
 *
 * Retrieves the height of a #ClutterActor.
 *
 * If the actor has a valid allocation, this function will return the
 * height of the allocated area given to the actor.
 *
 * If the actor does not have a valid allocation, this function will
 * return the actor's natural height, that is the preferred height of
 * the actor.
 *
 * If you care whether you get the preferred height or the height that
 * has been assigned to the actor, you should probably call a different
 * function like clutter_actor_get_allocation_box() to retrieve the
 * allocated size or clutter_actor_get_preferred_height() to retrieve the
 * preferred height.
 *
 * If an actor has a fixed height, for instance a height that has been
 * assigned using clutter_actor_set_height(), the height returned will
 * be the same value.
 *
 * Return value: the height of the actor, in pixels
 */
gfloat
clutter_actor_get_height (ClutterActor *self)
{
  ClutterActorPrivate *priv;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), 0);

  priv = self->priv;

  if (priv->needs_allocation)
    {
      gfloat natural_height = 0;

      if (priv->request_mode == CLUTTER_REQUEST_HEIGHT_FOR_WIDTH)
        {
          gfloat natural_width = 0;

          clutter_actor_get_preferred_width (self, -1, NULL, &natural_width);
          clutter_actor_get_preferred_height (self, natural_width,
                                              NULL, &natural_height);
        }
      else
        clutter_actor_get_preferred_height (self, -1, NULL, &natural_height);

      return natural_height;
    }
  else
    return priv->allocation.y2 - priv->allocation.y1;
}

/**
 * clutter_actor_set_width
 * @self: A #ClutterActor
 * @width: Requested new width for the actor, in pixels, or -1
 *
 * Forces a width on an actor, causing the actor's preferred width
 * and height (if any) to be ignored.
 *
 * If @width is -1 the actor will use its preferred width request
 * instead of overriding it, i.e. you can "unset" the width with -1.
 *
 * This function sets both the minimum and natural size of the actor.
 *
 * since: 0.2
 */
void
clutter_actor_set_width (ClutterActor *self,
                         gfloat        width)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  g_object_freeze_notify (G_OBJECT (self));

  clutter_actor_set_width_internal (self, width);

  g_object_thaw_notify (G_OBJECT (self));
}

/**
 * clutter_actor_set_height
 * @self: A #ClutterActor
 * @height: Requested new height for the actor, in pixels, or -1
 *
 * Forces a height on an actor, causing the actor's preferred width
 * and height (if any) to be ignored.
 *
 * If @height is -1 the actor will use its preferred height instead of
 * overriding it, i.e. you can "unset" the height with -1.
 *
 * This function sets both the minimum and natural size of the actor.
 *
 * since: 0.2
 */
void
clutter_actor_set_height (ClutterActor *self,
                          gfloat        height)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  g_object_freeze_notify (G_OBJECT (self));

  clutter_actor_set_height_internal (self, height);

  g_object_thaw_notify (G_OBJECT (self));
}

/**
 * clutter_actor_set_x:
 * @self: a #ClutterActor
 * @x: the actor's position on the X axis
 *
 * Sets the actor's X coordinate, relative to its parent, in pixels.
 *
 * Overrides any layout manager and forces a fixed position for
 * the actor.
 *
 * Since: 0.6
 */
void
clutter_actor_set_x (ClutterActor *self,
                     gfloat        x)
{
  ClutterActorBox old = { 0, };
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  if (priv->position_set && priv->fixed_x == x)
    return;

  clutter_actor_store_old_geometry (self, &old);

  priv->fixed_x = x;
  clutter_actor_set_fixed_position_set (self, TRUE);

  clutter_actor_notify_if_geometry_changed (self, &old);

  clutter_actor_queue_relayout (self);
}

/**
 * clutter_actor_set_y:
 * @self: a #ClutterActor
 * @y: the actor's position on the Y axis
 *
 * Sets the actor's Y coordinate, relative to its parent, in pixels.#
 *
 * Overrides any layout manager and forces a fixed position for
 * the actor.
 *
 * Since: 0.6
 */
void
clutter_actor_set_y (ClutterActor *self,
                     gfloat        y)
{
  ClutterActorBox old = { 0, };
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  if (priv->position_set && priv->fixed_y == y)
    return;

  clutter_actor_store_old_geometry (self, &old);

  priv->fixed_y = y;
  clutter_actor_set_fixed_position_set (self, TRUE);

  clutter_actor_notify_if_geometry_changed (self, &old);

  clutter_actor_queue_relayout (self);
}

/**
 * clutter_actor_get_x
 * @self: A #ClutterActor
 *
 * Retrieves the X coordinate of a #ClutterActor.
 *
 * This function tries to "do what you mean", by returning the
 * correct value depending on the actor's state.
 *
 * If the actor has a valid allocation, this function will return
 * the X coordinate of the origin of the allocation box.
 *
 * If the actor has any fixed coordinate set using clutter_actor_set_x(),
 * clutter_actor_set_position() or clutter_actor_set_geometry(), this
 * function will return that coordinate.
 *
 * If both the allocation and a fixed position are missing, this function
 * will return 0.
 *
 * Return value: the X coordinate, in pixels, ignoring any
 *   transformation (i.e. scaling, rotation)
 */
gfloat
clutter_actor_get_x (ClutterActor *self)
{
  ClutterActorPrivate *priv;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), 0);

  priv = self->priv;

  if (priv->needs_allocation)
    {
      if (priv->position_set)
        return priv->fixed_x;
      else
        return 0;
    }
  else
    return priv->allocation.x1;
}

/**
 * clutter_actor_get_y
 * @self: A #ClutterActor
 *
 * Retrieves the Y coordinate of a #ClutterActor.
 *
 * This function tries to "do what you mean", by returning the
 * correct value depending on the actor's state.
 *
 * If the actor has a valid allocation, this function will return
 * the Y coordinate of the origin of the allocation box.
 *
 * If the actor has any fixed coordinate set using clutter_actor_set_y(),
 * clutter_actor_set_position() or clutter_actor_set_geometry(), this
 * function will return that coordinate.
 *
 * If both the allocation and a fixed position are missing, this function
 * will return 0.
 *
 * Return value: the Y coordinate, in pixels, ignoring any
 *   transformation (i.e. scaling, rotation)
 */
gfloat
clutter_actor_get_y (ClutterActor *self)
{
  ClutterActorPrivate *priv;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), 0);

  priv = self->priv;

  if (priv->needs_allocation)
    {
      if (priv->position_set)
        return priv->fixed_y;
      else
        return 0;
    }
  else
    return priv->allocation.y1;
}

/**
 * clutter_actor_set_scale:
 * @self: A #ClutterActor
 * @scale_x: double factor to scale actor by horizontally.
 * @scale_y: double factor to scale actor by vertically.
 *
 * Scales an actor with the given factors. The scaling is relative to
 * the scale center and the anchor point. The scale center is
 * unchanged by this function and defaults to 0,0.
 *
 * Since: 0.2
 */
void
clutter_actor_set_scale (ClutterActor *self,
                         gdouble       scale_x,
                         gdouble       scale_y)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  priv->transform_valid = FALSE;

  g_object_freeze_notify (G_OBJECT (self));

  priv->scale_x = scale_x;
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_SCALE_X]);

  priv->scale_y = scale_y;
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_SCALE_Y]);

  clutter_actor_queue_redraw (self);

  g_object_thaw_notify (G_OBJECT (self));
}

/**
 * clutter_actor_set_scale_full:
 * @self: A #ClutterActor
 * @scale_x: double factor to scale actor by horizontally.
 * @scale_y: double factor to scale actor by vertically.
 * @center_x: X coordinate of the center of the scale.
 * @center_y: Y coordinate of the center of the scale
 *
 * Scales an actor with the given factors around the given center
 * point. The center point is specified in pixels relative to the
 * anchor point (usually the top left corner of the actor).
 *
 * Since: 1.0
 */
void
clutter_actor_set_scale_full (ClutterActor *self,
                              gdouble       scale_x,
                              gdouble       scale_y,
                              gfloat        center_x,
                              gfloat        center_y)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  g_object_freeze_notify (G_OBJECT (self));

  clutter_actor_set_scale (self, scale_x, scale_y);

  priv->transform_valid = FALSE;

  if (priv->scale_center.is_fractional)
    g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_SCALE_GRAVITY]);

  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_SCALE_CENTER_X]);
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_SCALE_CENTER_Y]);

  clutter_anchor_coord_set_units (&priv->scale_center, center_x, center_y, 0);

  g_object_thaw_notify (G_OBJECT (self));
}

/**
 * clutter_actor_set_scale_with_gravity:
 * @self: A #ClutterActor
 * @scale_x: double factor to scale actor by horizontally.
 * @scale_y: double factor to scale actor by vertically.
 * @gravity: the location of the scale center expressed as a compass
 * direction.
 *
 * Scales an actor with the given factors around the given
 * center point. The center point is specified as one of the compass
 * directions in #ClutterGravity. For example, setting it to north
 * will cause the top of the actor to remain unchanged and the rest of
 * the actor to expand left, right and downwards.
 *
 * Since: 1.0
 */
void
clutter_actor_set_scale_with_gravity (ClutterActor   *self,
                                      gdouble         scale_x,
                                      gdouble         scale_y,
                                      ClutterGravity  gravity)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  if (gravity == CLUTTER_GRAVITY_NONE)
    clutter_actor_set_scale_full (self, scale_x, scale_y, 0, 0);
  else
    {
      g_object_freeze_notify (G_OBJECT (self));

      clutter_actor_set_scale (self, scale_x, scale_y);

      priv->transform_valid = FALSE;

      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_SCALE_GRAVITY]);
      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_SCALE_CENTER_X]);
      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_SCALE_CENTER_Y]);

      clutter_anchor_coord_set_gravity (&priv->scale_center, gravity);

      g_object_thaw_notify (G_OBJECT (self));
    }
}

/**
 * clutter_actor_get_scale:
 * @self: A #ClutterActor
 * @scale_x: (out) (allow-none): Location to store horizonal
 *   scale factor, or %NULL.
 * @scale_y: (out) (allow-none): Location to store vertical
 *   scale factor, or %NULL.
 *
 * Retrieves an actors scale factors.
 *
 * Since: 0.2
 */
void
clutter_actor_get_scale (ClutterActor *self,
			 gdouble      *scale_x,
			 gdouble      *scale_y)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  if (scale_x)
    *scale_x = self->priv->scale_x;

  if (scale_y)
    *scale_y = self->priv->scale_y;
}

/**
 * clutter_actor_get_scale_center:
 * @self: A #ClutterActor
 * @center_x: (out) (allow-none): Location to store the X position
 *   of the scale center, or %NULL.
 * @center_y: (out) (allow-none): Location to store the Y position
 *   of the scale center, or %NULL.
 *
 * Retrieves the scale center coordinate in pixels relative to the top
 * left corner of the actor. If the scale center was specified using a
 * #ClutterGravity this will calculate the pixel offset using the
 * current size of the actor.
 *
 * Since: 1.0
 */
void
clutter_actor_get_scale_center (ClutterActor *self,
                                gfloat       *center_x,
                                gfloat       *center_y)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  clutter_anchor_coord_get_units (self, &self->priv->scale_center,
                                  center_x,
                                  center_y,
                                  NULL);
}

/**
 * clutter_actor_get_scale_gravity:
 * @self: A #ClutterActor
 *
 * Retrieves the scale center as a compass direction. If the scale
 * center was specified in pixels or units this will return
 * %CLUTTER_GRAVITY_NONE.
 *
 * Return value: the scale gravity
 *
 * Since: 1.0
 */
ClutterGravity
clutter_actor_get_scale_gravity (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), CLUTTER_GRAVITY_NONE);

  return clutter_anchor_coord_get_gravity (&self->priv->scale_center);
}

/**
 * clutter_actor_set_opacity:
 * @self: A #ClutterActor
 * @opacity: New opacity value for the actor.
 *
 * Sets the actor's opacity, with zero being completely transparent and
 * 255 (0xff) being fully opaque.
 */
void
clutter_actor_set_opacity (ClutterActor *self,
			   guint8        opacity)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  if (priv->opacity != opacity)
    {
      priv->opacity = opacity;

      /* Queue a redraw from the flatten effect so that it can use
         its cached image if available instead of having to redraw the
         actual actor. If it doesn't end up using the FBO then the
         effect is still able to continue the paint anyway. If there
         is no flatten effect yet then this is equivalent to queueing
         a full redraw */
      _clutter_actor_queue_redraw_full (self,
                                        0, /* flags */
                                        NULL, /* clip */
                                        priv->flatten_effect);

      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_OPACITY]);
    }
}

/*
 * clutter_actor_get_paint_opacity_internal:
 * @self: a #ClutterActor
 *
 * Retrieves the absolute opacity of the actor, as it appears on the stage
 *
 * This function does not do type checks
 *
 * Return value: the absolute opacity of the actor
 */
static guint8
clutter_actor_get_paint_opacity_internal (ClutterActor *self)
{
  ClutterActorPrivate *priv = self->priv;
  ClutterActor *parent;

  /* override the top-level opacity to always be 255; even in
   * case of ClutterStage:use-alpha being TRUE we want the rest
   * of the scene to be painted
   */
  if (CLUTTER_ACTOR_IS_TOPLEVEL (self))
    return 255;

  if (priv->opacity_override >= 0)
    return priv->opacity_override;

  parent = priv->parent_actor;

  /* Factor in the actual actors opacity with parents */
  if (parent != NULL)
    {
      guint8 opacity = clutter_actor_get_paint_opacity_internal (parent);

      if (opacity != 0xff)
        return (opacity * priv->opacity) / 0xff;
    }

  return priv->opacity;

}

/**
 * clutter_actor_get_paint_opacity:
 * @self: A #ClutterActor
 *
 * Retrieves the absolute opacity of the actor, as it appears on the stage.
 *
 * This function traverses the hierarchy chain and composites the opacity of
 * the actor with that of its parents.
 *
 * This function is intended for subclasses to use in the paint virtual
 * function, to paint themselves with the correct opacity.
 *
 * Return value: The actor opacity value.
 *
 * Since: 0.8
 */
guint8
clutter_actor_get_paint_opacity (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), 0);

  return clutter_actor_get_paint_opacity_internal (self);
}

/**
 * clutter_actor_get_opacity:
 * @self: a #ClutterActor
 *
 * Retrieves the opacity value of an actor, as set by
 * clutter_actor_set_opacity().
 *
 * For retrieving the absolute opacity of the actor inside a paint
 * virtual function, see clutter_actor_get_paint_opacity().
 *
 * Return value: the opacity of the actor
 */
guint8
clutter_actor_get_opacity (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), 0);

  return self->priv->opacity;
}

/**
 * clutter_actor_set_offscreen_redirect:
 * @self: A #ClutterActor
 * @redirect: New offscreen redirect value for the actor.
 *
 * Sets whether to redirect the actor into an offscreen image. The
 * offscreen image is used to flatten the actor into a single image
 * while painting for two main reasons. Firstly, when the actor is
 * painted a second time without any of its contents changing it can
 * simply repaint the cached image without descending further down the
 * actor hierarchy. Secondly, it will make the opacity look correct
 * even if there are overlapping primitives in the actor.
 *
 * Caching the actor could in some cases be a performance win and in
 * some cases be a performance lose so it is important to determine
 * which value is right for an actor before modifying this value. For
 * example, there is never any reason to flatten an actor that is just
 * a single texture (such as a #ClutterTexture) because it is
 * effectively already cached in an image so the offscreen would be
 * redundant. Also if the actor contains primitives that are far apart
 * with a large transparent area in the middle (such as a large
 * CluterGroup with a small actor in the top left and a small actor in
 * the bottom right) then the cached image will contain the entire
 * image of the large area and the paint will waste time blending all
 * of the transparent pixels in the middle.
 *
 * The default method of implementing opacity on a container simply
 * forwards on the opacity to all of the children. If the children are
 * overlapping then it will appear as if they are two separate glassy
 * objects and there will be a break in the color where they
 * overlap. By redirecting to an offscreen buffer it will be as if the
 * two opaque objects are combined into one and then made transparent
 * which is usually what is expected.
 *
 * The image below demonstrates the difference between redirecting and
 * not. The image shows two Clutter groups, each containing a red and
 * a green rectangle which overlap. The opacity on the group is set to
 * 128 (which is 50%). When the offscreen redirect is not used, the
 * red rectangle can be seen through the blue rectangle as if the two
 * rectangles were separately transparent. When the redirect is used
 * the group as a whole is transparent instead so the red rectangle is
 * not visible where they overlap.
 *
 * <figure id="offscreen-redirect">
 *   <title>Sample of using an offscreen redirect for transparency</title>
 *   <graphic fileref="offscreen-redirect.png" format="PNG"/>
 * </figure>
 *
 * The default behaviour is
 * %CLUTTER_OFFSCREEN_REDIRECT_AUTOMATIC_FOR_OPACITY. This will end up
 * redirecting actors whenever they are semi-transparent unless their
 * has_overlaps() virtual returns %FALSE. This should mean that
 * generally all actors will be rendered with the correct opacity and
 * certain actors that don't need the offscreen redirect (such as
 * #ClutterTexture) will paint directly for efficiency.
 *
 * Custom actors that don't contain any overlapping primitives are
 * recommended to override the has_overlaps() virtual to return %FALSE
 * for maximum efficiency.
 *
 * Since: 1.8
 */
void
clutter_actor_set_offscreen_redirect (ClutterActor *self,
                                      ClutterOffscreenRedirect redirect)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  if (priv->offscreen_redirect != redirect)
    {
      priv->offscreen_redirect = redirect;

      /* Queue a redraw from the effect so that it can use its cached
         image if available instead of having to redraw the actual
         actor. If it doesn't end up using the FBO then the effect is
         still able to continue the paint anyway. If there is no
         effect then this is equivalent to queuing a full redraw */
      _clutter_actor_queue_redraw_full (self,
                                        0, /* flags */
                                        NULL, /* clip */
                                        priv->flatten_effect);

      g_object_notify_by_pspec (G_OBJECT (self),
                                obj_props[PROP_OFFSCREEN_REDIRECT]);
    }
}

/**
 * clutter_actor_get_offscreen_redirect:
 * @self: a #ClutterActor
 *
 * Retrieves whether to redirect the actor to an offscreen buffer, as
 * set by clutter_actor_set_offscreen_redirect().
 *
 * Return value: the value of the offscreen-redirect property of the actor
 *
 * Since: 1.8
 */
ClutterOffscreenRedirect
clutter_actor_get_offscreen_redirect (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), 0);

  return self->priv->offscreen_redirect;
}

/**
 * clutter_actor_set_name:
 * @self: A #ClutterActor
 * @name: Textual tag to apply to actor
 *
 * Sets the given name to @self. The name can be used to identify
 * a #ClutterActor.
 */
void
clutter_actor_set_name (ClutterActor *self,
			const gchar  *name)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  g_free (self->priv->name);
  self->priv->name = g_strdup (name);

  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_NAME]);
}

/**
 * clutter_actor_get_name:
 * @self: A #ClutterActor
 *
 * Retrieves the name of @self.
 *
 * Return value: the name of the actor, or %NULL. The returned string is
 *   owned by the actor and should not be modified or freed.
 */
G_CONST_RETURN gchar *
clutter_actor_get_name (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), NULL);

  return self->priv->name;
}

/**
 * clutter_actor_get_gid:
 * @self: A #ClutterActor
 *
 * Retrieves the unique id for @self.
 *
 * Return value: Globally unique value for this object instance.
 *
 * Since: 0.6
 *
 * Deprecated: 1.8: The id is not used any longer.
 */
guint32
clutter_actor_get_gid (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), 0);

  return self->priv->id;
}

/**
 * clutter_actor_set_depth:
 * @self: a #ClutterActor
 * @depth: Z co-ord
 *
 * Sets the Z coordinate of @self to @depth.
 *
 * The unit used by @depth is dependant on the perspective setup. See
 * also clutter_stage_set_perspective().
 */
void
clutter_actor_set_depth (ClutterActor *self,
                         gfloat        depth)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  if (priv->z != depth)
    {
      /* Sets Z value - XXX 2.0: should we invert? */
      priv->z = depth;

      if (priv->parent_actor && CLUTTER_IS_CONTAINER (priv->parent_actor))
        {
          ClutterContainer *parent;

          /* We need to resort the container stacking order as to
           * correctly render alpha values.
           *
           * FIXME: This is sub-optimal. maybe queue the sort
           *        before stacking
           */
          parent = CLUTTER_CONTAINER (priv->parent_actor);
          clutter_container_sort_depth_order (parent);
        }

      priv->transform_valid = FALSE;

      clutter_actor_queue_redraw (self);

      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_DEPTH]);
    }
}

/**
 * clutter_actor_get_depth:
 * @self: a #ClutterActor
 *
 * Retrieves the depth of @self.
 *
 * Return value: the depth of the actor
 */
gfloat
clutter_actor_get_depth (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), -1);

  return self->priv->z;
}

/**
 * clutter_actor_set_rotation:
 * @self: a #ClutterActor
 * @axis: the axis of rotation
 * @angle: the angle of rotation
 * @x: X coordinate of the rotation center
 * @y: Y coordinate of the rotation center
 * @z: Z coordinate of the rotation center
 *
 * Sets the rotation angle of @self around the given axis.
 *
 * The rotation center coordinates used depend on the value of @axis:
 * <itemizedlist>
 *   <listitem><para>%CLUTTER_X_AXIS requires @y and @z</para></listitem>
 *   <listitem><para>%CLUTTER_Y_AXIS requires @x and @z</para></listitem>
 *   <listitem><para>%CLUTTER_Z_AXIS requires @x and @y</para></listitem>
 * </itemizedlist>
 *
 * The rotation coordinates are relative to the anchor point of the
 * actor, set using clutter_actor_set_anchor_point(). If no anchor
 * point is set, the upper left corner is assumed as the origin.
 *
 * Since: 0.8
 */
void
clutter_actor_set_rotation (ClutterActor      *self,
                            ClutterRotateAxis  axis,
                            gdouble            angle,
                            gfloat             x,
                            gfloat             y,
                            gfloat             z)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  g_object_freeze_notify (G_OBJECT (self));

  clutter_actor_set_rotation_internal (self, axis, angle);

  switch (axis)
    {
    case CLUTTER_X_AXIS:
      clutter_anchor_coord_set_units (&priv->rx_center, x, y, z);
      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ROTATION_CENTER_X]);
      break;

    case CLUTTER_Y_AXIS:
      clutter_anchor_coord_set_units (&priv->ry_center, x, y, z);
      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ROTATION_CENTER_Y]);
      break;

    case CLUTTER_Z_AXIS:
      if (priv->rz_center.is_fractional)
        g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ROTATION_CENTER_Z_GRAVITY]);
      clutter_anchor_coord_set_units (&priv->rz_center, x, y, z);
      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ROTATION_CENTER_Z]);
      break;
    }

  priv->transform_valid = FALSE;

  g_object_thaw_notify (G_OBJECT (self));
}

/**
 * clutter_actor_set_z_rotation_from_gravity:
 * @self: a #ClutterActor
 * @angle: the angle of rotation
 * @gravity: the center point of the rotation
 *
 * Sets the rotation angle of @self around the Z axis using the center
 * point specified as a compass point. For example to rotate such that
 * the center of the actor remains static you can use
 * %CLUTTER_GRAVITY_CENTER. If the actor changes size the center point
 * will move accordingly.
 *
 * Since: 1.0
 */
void
clutter_actor_set_z_rotation_from_gravity (ClutterActor   *self,
                                           gdouble         angle,
                                           ClutterGravity  gravity)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  if (gravity == CLUTTER_GRAVITY_NONE)
    clutter_actor_set_rotation (self, CLUTTER_Z_AXIS, angle, 0, 0, 0);
  else
    {
      priv = self->priv;

      g_object_freeze_notify (G_OBJECT (self));

      clutter_actor_set_rotation_internal (self, CLUTTER_Z_AXIS, angle);

      clutter_anchor_coord_set_gravity (&priv->rz_center, gravity);
      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ROTATION_CENTER_Z_GRAVITY]);
      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ROTATION_CENTER_Z]);

      g_object_thaw_notify (G_OBJECT (self));
    }
}

/**
 * clutter_actor_get_rotation:
 * @self: a #ClutterActor
 * @axis: the axis of rotation
 * @x: (out): return value for the X coordinate of the center of rotation
 * @y: (out): return value for the Y coordinate of the center of rotation
 * @z: (out): return value for the Z coordinate of the center of rotation
 *
 * Retrieves the angle and center of rotation on the given axis,
 * set using clutter_actor_set_rotation().
 *
 * Return value: the angle of rotation
 *
 * Since: 0.8
 */
gdouble
clutter_actor_get_rotation (ClutterActor      *self,
                            ClutterRotateAxis  axis,
                            gfloat            *x,
                            gfloat            *y,
                            gfloat            *z)
{
  ClutterActorPrivate *priv;
  gdouble retval = 0;
  AnchorCoord *anchor_coord = NULL;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), 0);

  priv = self->priv;

  switch (axis)
    {
    case CLUTTER_X_AXIS:
      anchor_coord = &priv->rx_center;
      retval = priv->rxang;
      break;

    case CLUTTER_Y_AXIS:
      anchor_coord = &priv->ry_center;
      retval = priv->ryang;
      break;

    case CLUTTER_Z_AXIS:
      anchor_coord = &priv->rz_center;
      retval = priv->rzang;
      break;
    }

  clutter_anchor_coord_get_units (self, anchor_coord, x, y, z);

  return retval;
}

/**
 * clutter_actor_get_z_rotation_gravity:
 * @self: A #ClutterActor
 *
 * Retrieves the center for the rotation around the Z axis as a
 * compass direction. If the center was specified in pixels or units
 * this will return %CLUTTER_GRAVITY_NONE.
 *
 * Return value: the Z rotation center
 *
 * Since: 1.0
 */
ClutterGravity
clutter_actor_get_z_rotation_gravity (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), 0.0);

  return clutter_anchor_coord_get_gravity (&self->priv->rz_center);
}

/**
 * clutter_actor_set_clip:
 * @self: A #ClutterActor
 * @xoff: X offset of the clip rectangle
 * @yoff: Y offset of the clip rectangle
 * @width: Width of the clip rectangle
 * @height: Height of the clip rectangle
 *
 * Sets clip area for @self. The clip area is always computed from the
 * upper left corner of the actor, even if the anchor point is set
 * otherwise.
 *
 * Since: 0.6
 */
void
clutter_actor_set_clip (ClutterActor *self,
                        gfloat        xoff,
                        gfloat        yoff,
                        gfloat        width,
                        gfloat        height)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  if (priv->has_clip &&
      priv->clip[0] == xoff &&
      priv->clip[1] == yoff &&
      priv->clip[2] == width &&
      priv->clip[3] == height)
    return;

  priv->clip[0] = xoff;
  priv->clip[1] = yoff;
  priv->clip[2] = width;
  priv->clip[3] = height;

  priv->has_clip = TRUE;

  clutter_actor_queue_redraw (self);

  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_HAS_CLIP]);
  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_CLIP]);
}

/**
 * clutter_actor_remove_clip
 * @self: A #ClutterActor
 *
 * Removes clip area from @self.
 */
void
clutter_actor_remove_clip (ClutterActor *self)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  if (!self->priv->has_clip)
    return;

  self->priv->has_clip = FALSE;

  clutter_actor_queue_redraw (self);

  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_HAS_CLIP]);
}

/**
 * clutter_actor_has_clip:
 * @self: a #ClutterActor
 *
 * Determines whether the actor has a clip area set or not.
 *
 * Return value: %TRUE if the actor has a clip area set.
 *
 * Since: 0.1.1
 */
gboolean
clutter_actor_has_clip (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), FALSE);

  return self->priv->has_clip;
}

/**
 * clutter_actor_get_clip:
 * @self: a #ClutterActor
 * @xoff: (out) (allow-none): return location for the X offset of
 *   the clip rectangle, or %NULL
 * @yoff: (out) (allow-none): return location for the Y offset of
 *   the clip rectangle, or %NULL
 * @width: (out) (allow-none): return location for the width of
 *   the clip rectangle, or %NULL
 * @height: (out) (allow-none): return location for the height of
 *   the clip rectangle, or %NULL
 *
 * Gets the clip area for @self, if any is set
 *
 * Since: 0.6
 */
void
clutter_actor_get_clip (ClutterActor *self,
                        gfloat       *xoff,
                        gfloat       *yoff,
                        gfloat       *width,
                        gfloat       *height)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  if (!priv->has_clip)
    return;

  if (xoff)
    *xoff = priv->clip[0];

  if (yoff)
    *yoff = priv->clip[1];

  if (width)
    *width = priv->clip[2];

  if (height)
    *height = priv->clip[3];
}

/**
 * clutter_actor_set_parent:
 * @self: A #ClutterActor
 * @parent: A new #ClutterActor parent
 *
 * Sets the parent of @self to @parent.  The opposite function is
 * clutter_actor_unparent().
 *
 * This function should not be used by applications, but by custom
 * container actor subclasses.
 */
void
clutter_actor_set_parent (ClutterActor *self,
		          ClutterActor *parent)
{
  ClutterActorPrivate *priv;
  ClutterActorPrivate *parent_priv;
  ClutterTextDirection text_dir;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (CLUTTER_IS_ACTOR (parent));
  g_return_if_fail (self != parent);

  priv = self->priv;

  if (priv->parent_actor != NULL)
    {
      g_warning ("Cannot set a parent on an actor which has a parent.\n"
		 "You must use clutter_actor_unparent() first.\n");
      return;
    }

  if (CLUTTER_ACTOR_IS_TOPLEVEL (self))
    {
      g_warning ("Cannot set a parent on a toplevel actor\n");
      return;
    }

  if (CLUTTER_ACTOR_IN_DESTRUCTION (self))
    {
      g_warning ("Cannot set a parent currently being destroyed");
      return;
    }

  g_object_ref_sink (self);
  priv->parent_actor = parent;

  /* Maintain an explicit list of children for every actor... */
  parent_priv = parent->priv;
  parent_priv->children =
    g_list_prepend (parent_priv->children, self);
  parent_priv->n_children++;

  /* if push_internal() has been called then we automatically set
   * the flag on the actor
   */
  if (parent->priv->internal_child)
    CLUTTER_SET_PRIVATE_FLAGS (self, CLUTTER_INTERNAL_CHILD);

  /* clutter_actor_reparent() will emit ::parent-set for us */
  if (!CLUTTER_ACTOR_IN_REPARENT (self))
    g_signal_emit (self, actor_signals[PARENT_SET], 0, NULL);

  /* If parent is mapped or realized, we need to also be mapped or
   * realized once we're inside the parent.
   */
  clutter_actor_update_map_state (self, MAP_STATE_CHECK);

  /* propagate the parent's text direction to the child */
  text_dir = clutter_actor_get_text_direction (parent);
  clutter_actor_set_text_direction (self, text_dir);

  if (priv->show_on_set_parent)
    clutter_actor_show (self);

  if (CLUTTER_ACTOR_IS_MAPPED (self))
    clutter_actor_queue_redraw (self);

  /* maintain the invariant that if an actor needs layout,
   * its parents do as well
   */
  if (priv->needs_width_request ||
      priv->needs_height_request ||
      priv->needs_allocation)
    {
      /* we work around the short-circuiting we do
       * in clutter_actor_queue_relayout() since we
       * want to force a relayout
       */
      priv->needs_width_request = TRUE;
      priv->needs_height_request = TRUE;
      priv->needs_allocation = TRUE;

      clutter_actor_queue_relayout (priv->parent_actor);
    }
}

/**
 * clutter_actor_get_parent:
 * @self: A #ClutterActor
 *
 * Retrieves the parent of @self.
 *
 * Return Value: (transfer none): The #ClutterActor parent, or %NULL
 *  if no parent is set
 */
ClutterActor *
clutter_actor_get_parent (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), NULL);

  return self->priv->parent_actor;
}

/**
 * clutter_actor_get_paint_visibility:
 * @self: A #ClutterActor
 *
 * Retrieves the 'paint' visibility of an actor recursively checking for non
 * visible parents.
 *
 * This is by definition the same as CLUTTER_ACTOR_IS_MAPPED().
 *
 * Return Value: TRUE if the actor is visibile and will be painted.
 *
 * Since: 0.8.4
 */
gboolean
clutter_actor_get_paint_visibility (ClutterActor *actor)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (actor), FALSE);

  return CLUTTER_ACTOR_IS_MAPPED (actor);
}

static ClutterActorTraverseVisitFlags
invalidate_queue_redraw_entry (ClutterActor *self,
                               int           depth,
                               gpointer      user_data)
{
  ClutterActorPrivate *priv = self->priv;

  if (priv->queue_redraw_entry != NULL)
    {
      _clutter_stage_queue_redraw_entry_invalidate (priv->queue_redraw_entry);
      priv->queue_redraw_entry = NULL;
    }

  return CLUTTER_ACTOR_TRAVERSE_VISIT_CONTINUE;
}

/**
 * clutter_actor_unparent:
 * @self: a #ClutterActor
 *
 * Removes the parent of @self.
 *
 * This function should not be used in applications.
 *
 * This function should only be called by implementations of the
 * #ClutterContainer interface, or by composite actors that do
 * not implicitly create their children.
 *
 * Since: 0.1.1
 */
void
clutter_actor_unparent (ClutterActor *self)
{
  ClutterActorPrivate *priv;
  ClutterActor *old_parent;
  ClutterActorPrivate *old_parent_priv;
  gboolean was_mapped;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  if (priv->parent_actor == NULL)
    return;

  was_mapped = CLUTTER_ACTOR_IS_MAPPED (self);

  /* we need to unrealize *before* we set parent_actor to NULL,
   * because in an unrealize method actors are dissociating from the
   * stage, which means they need to be able to
   * clutter_actor_get_stage(). This should unmap and unrealize,
   * unless we're reparenting.
   */
  clutter_actor_update_map_state (self, MAP_STATE_MAKE_UNREALIZED);

   /* We take this opportunity to invalidate any queue redraw entry
    * associated with the actor and descendants since we won't be able to
    * determine the appropriate stage after this. */
  _clutter_actor_traverse (self,
                           0,
                           invalidate_queue_redraw_entry,
                           NULL,
                           NULL);

  old_parent = priv->parent_actor;
  priv->parent_actor = NULL;

  /* clutter_actor_reparent() will emit ::parent-set for us */
  if (!CLUTTER_ACTOR_IN_REPARENT (self))
    g_signal_emit (self, actor_signals[PARENT_SET], 0, old_parent);

  old_parent_priv = old_parent->priv;
  old_parent_priv->children = g_list_remove (old_parent_priv->children, self);
  old_parent_priv->n_children--;

  /* Queue a redraw on old_parent only if we were painted in the first
   * place. Will be no-op if old parent is not shown.
   */
  if (was_mapped && !CLUTTER_ACTOR_IS_MAPPED (self))
    clutter_actor_queue_redraw (old_parent);

  /* remove the reference we acquired in clutter_actor_set_parent() */
  g_object_unref (self);
}

/**
 * clutter_actor_reparent:
 * @self: a #ClutterActor
 * @new_parent: the new #ClutterActor parent
 *
 * This function resets the parent actor of @self.  It is
 * logically equivalent to calling clutter_actor_unparent()
 * and clutter_actor_set_parent(), but more efficiently
 * implemented, ensures the child is not finalized
 * when unparented, and emits the parent-set signal only
 * one time.
 *
 * Since: 0.2
 */
void
clutter_actor_reparent (ClutterActor *self,
                        ClutterActor *new_parent)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (CLUTTER_IS_ACTOR (new_parent));
  g_return_if_fail (self != new_parent);

  if (CLUTTER_ACTOR_IS_TOPLEVEL (self))
    {
      g_warning ("Cannot set a parent on a toplevel actor");
      return;
    }

  if (CLUTTER_ACTOR_IN_DESTRUCTION (self))
    {
      g_warning ("Cannot set a parent currently being destroyed");
      return;
    }

  priv = self->priv;

  if (priv->parent_actor != new_parent)
    {
      ClutterActor *old_parent;

      CLUTTER_SET_PRIVATE_FLAGS (self, CLUTTER_IN_REPARENT);

      old_parent = priv->parent_actor;

      g_object_ref (self);

      /* go through the Container implementation if this is a regular
       * child and not an internal one
       */
      if (CLUTTER_IS_CONTAINER (priv->parent_actor) &&
          !CLUTTER_ACTOR_IS_INTERNAL_CHILD (self))
        {
          ClutterContainer *parent = CLUTTER_CONTAINER (priv->parent_actor);

          /* this will have to call unparent() */
          clutter_container_remove_actor (parent, self);
        }
      else
        clutter_actor_unparent (self);

      /* Note, will call parent() */
      if (CLUTTER_IS_CONTAINER (new_parent))
        clutter_container_add_actor (CLUTTER_CONTAINER (new_parent), self);
      else
        clutter_actor_set_parent (self, new_parent);

      /* we emit the ::parent-set signal once */
      g_signal_emit (self, actor_signals[PARENT_SET], 0, old_parent);

      g_object_unref (self);

      CLUTTER_UNSET_PRIVATE_FLAGS (self, CLUTTER_IN_REPARENT);

      /* the IN_REPARENT flag suspends state updates */
      clutter_actor_update_map_state (self, MAP_STATE_CHECK);
   }
}

/**
 * clutter_actor_contains:
 * @self: A #ClutterActor
 * @descendant: A #ClutterActor, possibly contained in @self
 *
 * Determines if @descendant is contained inside @self (either as an
 * immediate child, or as a deeper descendant). If @self and
 * @descendant point to the same actor then it will also return %TRUE.
 *
 * Return value: whether @descendent is contained within @self
 *
 * Since: 1.4
 */
gboolean
clutter_actor_contains (ClutterActor *self,
			ClutterActor *descendant)
{
  ClutterActor *actor;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), FALSE);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (descendant), FALSE);

  for (actor = descendant; actor; actor = actor->priv->parent_actor)
    if (actor == self)
      return TRUE;

  return FALSE;
}

/**
 * clutter_actor_raise:
 * @self: A #ClutterActor
 * @below: (allow-none): A #ClutterActor to raise above.
 *
 * Puts @self above @below.
 *
 * Both actors must have the same parent, and the parent must implement
 * the #ClutterContainer interface
 *
 * This function is the equivalent of clutter_container_raise_child().
 */
void
clutter_actor_raise (ClutterActor *self,
                     ClutterActor *below)
{
  ClutterActor *parent;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  parent = clutter_actor_get_parent (self);
  if (parent == NULL || !CLUTTER_IS_CONTAINER (parent))
    {
      g_warning ("%s: Actor '%s' is not inside a container",
                 G_STRFUNC,
                 _clutter_actor_get_debug_name (self));
      return;
    }

  if (below != NULL)
    {
      if (parent != clutter_actor_get_parent (below))
        {
          g_warning ("%s Actor '%s' is not in the same container as "
                     "actor '%s'",
                     G_STRFUNC,
                     _clutter_actor_get_debug_name (self),
                     _clutter_actor_get_debug_name (below));
          return;
        }
    }

  clutter_container_raise_child (CLUTTER_CONTAINER (parent), self, below);
}

/**
 * clutter_actor_lower:
 * @self: A #ClutterActor
 * @above: (allow-none): A #ClutterActor to lower below
 *
 * Puts @self below @above.
 *
 * Both actors must have the same parent, and the parent must implement
 * the #ClutterContainer interface.
 *
 * This function is the equivalent of clutter_container_lower_child().
 */
void
clutter_actor_lower (ClutterActor *self,
                     ClutterActor *above)
{
  ClutterActor *parent;

  g_return_if_fail (CLUTTER_IS_ACTOR(self));

  parent = clutter_actor_get_parent (self);
  if (parent == NULL || !CLUTTER_IS_CONTAINER (parent))
    {
      g_warning ("%s: Actor of type %s is not inside a container",
                 G_STRFUNC,
                 _clutter_actor_get_debug_name (self));
      return;
    }

  if (above)
    {
      if (parent != clutter_actor_get_parent (above))
        {
          g_warning ("%s: Actor '%s' is not in the same container as "
                     "actor '%s'",
                     G_STRFUNC,
                     _clutter_actor_get_debug_name (self),
                     _clutter_actor_get_debug_name (above));
          return;
        }
    }

  clutter_container_lower_child (CLUTTER_CONTAINER (parent), self, above);
}

/**
 * clutter_actor_raise_top:
 * @self: A #ClutterActor
 *
 * Raises @self to the top.
 *
 * This function calls clutter_actor_raise() internally.
 */
void
clutter_actor_raise_top (ClutterActor *self)
{
  clutter_actor_raise (self, NULL);
}

/**
 * clutter_actor_lower_bottom:
 * @self: A #ClutterActor
 *
 * Lowers @self to the bottom.
 *
 * This function calls clutter_actor_lower() internally.
 */
void
clutter_actor_lower_bottom (ClutterActor *self)
{
  clutter_actor_lower (self, NULL);
}

/*
 * Event handling
 */

/**
 * clutter_actor_event:
 * @actor: a #ClutterActor
 * @event: a #ClutterEvent
 * @capture: TRUE if event in in capture phase, FALSE otherwise.
 *
 * This function is used to emit an event on the main stage.
 * You should rarely need to use this function, except for
 * synthetising events.
 *
 * Return value: the return value from the signal emission: %TRUE
 *   if the actor handled the event, or %FALSE if the event was
 *   not handled
 *
 * Since: 0.6
 */
gboolean
clutter_actor_event (ClutterActor *actor,
                     ClutterEvent *event,
		     gboolean      capture)
{
  gboolean retval = FALSE;
  gint signal_num = -1;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (actor), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  g_object_ref (actor);

  if (capture)
    {
      g_signal_emit (actor, actor_signals[CAPTURED_EVENT], 0,
		     event,
                     &retval);
      goto out;
    }

  g_signal_emit (actor, actor_signals[EVENT], 0, event, &retval);

  if (!retval)
    {
      switch (event->type)
	{
	case CLUTTER_NOTHING:
	  break;
	case CLUTTER_BUTTON_PRESS:
	  signal_num = BUTTON_PRESS_EVENT;
	  break;
	case CLUTTER_BUTTON_RELEASE:
	  signal_num = BUTTON_RELEASE_EVENT;
	  break;
	case CLUTTER_SCROLL:
	  signal_num = SCROLL_EVENT;
	  break;
	case CLUTTER_KEY_PRESS:
	  signal_num = KEY_PRESS_EVENT;
	  break;
	case CLUTTER_KEY_RELEASE:
	  signal_num = KEY_RELEASE_EVENT;
	  break;
	case CLUTTER_MOTION:
	  signal_num = MOTION_EVENT;
	  break;
	case CLUTTER_ENTER:
	  signal_num = ENTER_EVENT;
	  break;
	case CLUTTER_LEAVE:
	  signal_num = LEAVE_EVENT;
	  break;
	case CLUTTER_DELETE:
	case CLUTTER_DESTROY_NOTIFY:
	case CLUTTER_CLIENT_MESSAGE:
	default:
	  signal_num = -1;
	  break;
	}

      if (signal_num != -1)
	g_signal_emit (actor, actor_signals[signal_num], 0,
		       event, &retval);
    }

out:
  g_object_unref (actor);

  return retval;
}

/**
 * clutter_actor_set_reactive:
 * @actor: a #ClutterActor
 * @reactive: whether the actor should be reactive to events
 *
 * Sets @actor as reactive. Reactive actors will receive events.
 *
 * Since: 0.6
 */
void
clutter_actor_set_reactive (ClutterActor *actor,
                            gboolean      reactive)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (actor));

  if (reactive == CLUTTER_ACTOR_IS_REACTIVE (actor))
    return;

  if (reactive)
    CLUTTER_ACTOR_SET_FLAGS (actor, CLUTTER_ACTOR_REACTIVE);
  else
    CLUTTER_ACTOR_UNSET_FLAGS (actor, CLUTTER_ACTOR_REACTIVE);

  g_object_notify_by_pspec (G_OBJECT (actor), obj_props[PROP_REACTIVE]);
}

/**
 * clutter_actor_get_reactive:
 * @actor: a #ClutterActor
 *
 * Checks whether @actor is marked as reactive.
 *
 * Return value: %TRUE if the actor is reactive
 *
 * Since: 0.6
 */
gboolean
clutter_actor_get_reactive (ClutterActor *actor)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (actor), FALSE);

  return CLUTTER_ACTOR_IS_REACTIVE (actor) ? TRUE : FALSE;
}

/**
 * clutter_actor_get_anchor_point:
 * @self: a #ClutterActor
 * @anchor_x: (out): return location for the X coordinate of the anchor point
 * @anchor_y: (out): return location for the Y coordinate of the anchor point
 *
 * Gets the current anchor point of the @actor in pixels.
 *
 * Since: 0.6
 */
void
clutter_actor_get_anchor_point (ClutterActor *self,
				gfloat       *anchor_x,
                                gfloat       *anchor_y)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  clutter_anchor_coord_get_units (self, &priv->anchor,
                                  anchor_x,
                                  anchor_y,
                                  NULL);
}

/**
 * clutter_actor_set_anchor_point:
 * @self: a #ClutterActor
 * @anchor_x: X coordinate of the anchor point
 * @anchor_y: Y coordinate of the anchor point
 *
 * Sets an anchor point for @self. The anchor point is a point in the
 * coordinate space of an actor to which the actor position within its
 * parent is relative; the default is (0, 0), i.e. the top-left corner
 * of the actor.
 *
 * Since: 0.6
 */
void
clutter_actor_set_anchor_point (ClutterActor *self,
                                gfloat        anchor_x,
                                gfloat        anchor_y)
{
  ClutterActorPrivate *priv;
  gboolean changed = FALSE;
  gfloat old_anchor_x, old_anchor_y;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  g_object_freeze_notify (G_OBJECT (self));

  clutter_anchor_coord_get_units (self, &priv->anchor,
                                  &old_anchor_x,
                                  &old_anchor_y,
                                  NULL);

  if (priv->anchor.is_fractional)
    g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ANCHOR_GRAVITY]);

  if (old_anchor_x != anchor_x)
    {
      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ANCHOR_X]);
      changed = TRUE;
    }

  if (old_anchor_y != anchor_y)
    {
      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ANCHOR_Y]);
      changed = TRUE;
    }

  clutter_anchor_coord_set_units (&priv->anchor, anchor_x, anchor_y, 0);

  if (changed)
    {
      priv->transform_valid = FALSE;
      clutter_actor_queue_redraw (self);
    }

  g_object_thaw_notify (G_OBJECT (self));
}

/**
 * clutter_actor_get_anchor_point_gravity:
 * @self: a #ClutterActor
 *
 * Retrieves the anchor position expressed as a #ClutterGravity. If
 * the anchor point was specified using pixels or units this will
 * return %CLUTTER_GRAVITY_NONE.
 *
 * Return value: the #ClutterGravity used by the anchor point
 *
 * Since: 1.0
 */
ClutterGravity
clutter_actor_get_anchor_point_gravity (ClutterActor *self)
{
  ClutterActorPrivate *priv;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), CLUTTER_GRAVITY_NONE);

  priv = self->priv;

  return clutter_anchor_coord_get_gravity (&priv->anchor);
}

/**
 * clutter_actor_move_anchor_point:
 * @self: a #ClutterActor
 * @anchor_x: X coordinate of the anchor point
 * @anchor_y: Y coordinate of the anchor point
 *
 * Sets an anchor point for the actor, and adjusts the actor postion so that
 * the relative position of the actor toward its parent remains the same.
 *
 * Since: 0.6
 */
void
clutter_actor_move_anchor_point (ClutterActor *self,
                                 gfloat        anchor_x,
                                 gfloat        anchor_y)
{
  ClutterActorPrivate *priv;
  gfloat old_anchor_x, old_anchor_y;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  clutter_anchor_coord_get_units (self, &priv->anchor,
                                  &old_anchor_x,
                                  &old_anchor_y,
                                  NULL);

  g_object_freeze_notify (G_OBJECT (self));

  clutter_actor_set_anchor_point (self, anchor_x, anchor_y);

  if (priv->position_set)
    clutter_actor_move_by (self,
                           anchor_x - old_anchor_x,
                           anchor_y - old_anchor_y);

  g_object_thaw_notify (G_OBJECT (self));
}

/**
 * clutter_actor_move_anchor_point_from_gravity:
 * @self: a #ClutterActor
 * @gravity: #ClutterGravity.
 *
 * Sets an anchor point on the actor based on the given gravity, adjusting the
 * actor postion so that its relative position within its parent remains
 * unchanged.
 *
 * Since version 1.0 the anchor point will be stored as a gravity so
 * that if the actor changes size then the anchor point will move. For
 * example, if you set the anchor point to %CLUTTER_GRAVITY_SOUTH_EAST
 * and later double the size of the actor, the anchor point will move
 * to the bottom right.
 *
 * Since: 0.6
 */
void
clutter_actor_move_anchor_point_from_gravity (ClutterActor   *self,
					      ClutterGravity  gravity)
{
  gfloat old_anchor_x, old_anchor_y, new_anchor_x, new_anchor_y;
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  g_object_freeze_notify (G_OBJECT (self));

  clutter_anchor_coord_get_units (self, &priv->anchor,
                                  &old_anchor_x,
                                  &old_anchor_y,
                                  NULL);
  clutter_actor_set_anchor_point_from_gravity (self, gravity);
  clutter_anchor_coord_get_units (self, &priv->anchor,
                                  &new_anchor_x,
                                  &new_anchor_y,
                                  NULL);

  if (priv->position_set)
    clutter_actor_move_by (self,
                           new_anchor_x - old_anchor_x,
                           new_anchor_y - old_anchor_y);

  g_object_thaw_notify (G_OBJECT (self));
}

/**
 * clutter_actor_set_anchor_point_from_gravity:
 * @self: a #ClutterActor
 * @gravity: #ClutterGravity.
 *
 * Sets an anchor point on the actor, based on the given gravity (this is a
 * convenience function wrapping clutter_actor_set_anchor_point()).
 *
 * Since version 1.0 the anchor point will be stored as a gravity so
 * that if the actor changes size then the anchor point will move. For
 * example, if you set the anchor point to %CLUTTER_GRAVITY_SOUTH_EAST
 * and later double the size of the actor, the anchor point will move
 * to the bottom right.
 *
 * Since: 0.6
 */
void
clutter_actor_set_anchor_point_from_gravity (ClutterActor   *self,
					     ClutterGravity  gravity)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  if (gravity == CLUTTER_GRAVITY_NONE)
    clutter_actor_set_anchor_point (self, 0, 0);
  else
    {
      clutter_anchor_coord_set_gravity (&self->priv->anchor, gravity);

      self->priv->transform_valid = FALSE;

      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ANCHOR_GRAVITY]);
      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ANCHOR_X]);
      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ANCHOR_Y]);
    }
}

typedef enum
{
  PARSE_X,
  PARSE_Y,
  PARSE_WIDTH,
  PARSE_HEIGHT,
  PARSE_ANCHOR_X,
  PARSE_ANCHOR_Y
} ParseDimension;

static gfloat
parse_units (ClutterActor   *self,
             ParseDimension  dimension,
             JsonNode       *node)
{
  GValue value = { 0, };
  gfloat retval = 0;

  if (JSON_NODE_TYPE (node) != JSON_NODE_VALUE)
    return 0;

  json_node_get_value (node, &value);

  if (G_VALUE_HOLDS (&value, G_TYPE_INT64))
    {
      retval = (gfloat) g_value_get_int64 (&value);
    }
  else if (G_VALUE_HOLDS (&value, G_TYPE_DOUBLE))
    {
      retval = g_value_get_double (&value);
    }
  else if (G_VALUE_HOLDS (&value, G_TYPE_STRING))
    {
      ClutterUnits units;
      gboolean res;

      res = clutter_units_from_string (&units, g_value_get_string (&value));
      if (res)
        retval = clutter_units_to_pixels (&units);
      else
        {
          g_warning ("Invalid value '%s': integers, strings or floating point "
                     "values can be used for the x, y, width and height "
                     "properties. Valid modifiers for strings are 'px', 'mm', "
                     "'pt' and 'em'.",
                     g_value_get_string (&value));
          retval = 0;
        }
    }
  else
    {
      g_warning ("Invalid value of type '%s': integers, strings of floating "
                 "point values can be used for the x, y, width, height "
                 "anchor-x and anchor-y properties.",
                 g_type_name (G_VALUE_TYPE (&value)));
    }

  g_value_unset (&value);

  return retval;
}

typedef struct {
  ClutterRotateAxis axis;

  gdouble angle;

  gfloat center_x;
  gfloat center_y;
  gfloat center_z;
} RotationInfo;

static inline gboolean
parse_rotation_array (ClutterActor *actor,
                      JsonArray    *array,
                      RotationInfo *info)
{
  JsonNode *element;

  if (json_array_get_length (array) != 2)
    return FALSE;

  /* angle */
  element = json_array_get_element (array, 0);
  if (JSON_NODE_TYPE (element) == JSON_NODE_VALUE)
    info->angle = json_node_get_double (element);
  else
    return FALSE;

  /* center */
  element = json_array_get_element (array, 1);
  if (JSON_NODE_TYPE (element) == JSON_NODE_ARRAY)
    {
      JsonArray *center = json_node_get_array (element);

      if (json_array_get_length (center) != 2)
        return FALSE;

      switch (info->axis)
        {
        case CLUTTER_X_AXIS:
          info->center_y = parse_units (actor, PARSE_Y,
                                        json_array_get_element (center, 0));
          info->center_z = parse_units (actor, PARSE_Y,
                                        json_array_get_element (center, 1));
          return TRUE;

        case CLUTTER_Y_AXIS:
          info->center_x = parse_units (actor, PARSE_X,
                                        json_array_get_element (center, 0));
          info->center_z = parse_units (actor, PARSE_X,
                                        json_array_get_element (center, 1));
          return TRUE;

        case CLUTTER_Z_AXIS:
          info->center_x = parse_units (actor, PARSE_X,
                                        json_array_get_element (center, 0));
          info->center_y = parse_units (actor, PARSE_Y,
                                        json_array_get_element (center, 1));
          return TRUE;
        }
    }

  return FALSE;
}

static gboolean
parse_rotation (ClutterActor *actor,
                JsonNode     *node,
                RotationInfo *info)
{
  JsonArray *array;
  guint len, i;
  gboolean retval = FALSE;

  if (JSON_NODE_TYPE (node) != JSON_NODE_ARRAY)
    {
      g_warning ("Invalid node of type '%s' found, expecting an array",
                 json_node_type_name (node));
      return FALSE;
    }

  array = json_node_get_array (node);
  len = json_array_get_length (array);

  for (i = 0; i < len; i++)
    {
      JsonNode *element = json_array_get_element (array, i);
      JsonObject *object;
      JsonNode *member;

      if (JSON_NODE_TYPE (element) != JSON_NODE_OBJECT)
        {
          g_warning ("Invalid node of type '%s' found, expecting an object",
                     json_node_type_name (element));
          return FALSE;
        }

      object = json_node_get_object (element);

      if (json_object_has_member (object, "x-axis"))
        {
          member = json_object_get_member (object, "x-axis");

          info->axis = CLUTTER_X_AXIS;

          if (JSON_NODE_TYPE (member) == JSON_NODE_VALUE)
            {
              info->angle = json_node_get_double (member);
              retval = TRUE;
            }
          else if (JSON_NODE_TYPE (member) == JSON_NODE_ARRAY)
            retval = parse_rotation_array (actor,
                                           json_node_get_array (member),
                                           info);
          else
            retval = FALSE;
        }
      else if (json_object_has_member (object, "y-axis"))
        {
          member = json_object_get_member (object, "y-axis");

          info->axis = CLUTTER_Y_AXIS;

          if (JSON_NODE_TYPE (member) == JSON_NODE_VALUE)
            {
              info->angle = json_node_get_double (member);
              retval = TRUE;
            }
          else if (JSON_NODE_TYPE (member) == JSON_NODE_ARRAY)
            retval = parse_rotation_array (actor,
                                           json_node_get_array (member),
                                           info);
          else
            retval = FALSE;
        }
      else if (json_object_has_member (object, "z-axis"))
        {
          member = json_object_get_member (object, "z-axis");

          info->axis = CLUTTER_Z_AXIS;

          if (JSON_NODE_TYPE (member) == JSON_NODE_VALUE)
            {
              info->angle = json_node_get_double (member);
              retval = TRUE;
            }
          else if (JSON_NODE_TYPE (member) == JSON_NODE_ARRAY)
            retval = parse_rotation_array (actor,
                                           json_node_get_array (member),
                                           info);
          else
            retval = FALSE;
        }
    }

  return retval;
}

static GSList *
parse_actor_metas (ClutterScript *script,
                   ClutterActor  *actor,
                   JsonNode      *node)
{
  GList *elements, *l;
  GSList *retval = NULL;

  if (!JSON_NODE_HOLDS_ARRAY (node))
    return NULL;

  elements = json_array_get_elements (json_node_get_array (node));

  for (l = elements; l != NULL; l = l->next)
    {
      JsonNode *element = l->data;
      const gchar *id_ = _clutter_script_get_id_from_node (element);
      GObject *meta;

      if (id_ == NULL || *id_ == '\0')
        continue;

      meta = clutter_script_get_object (script, id_);
      if (meta == NULL)
        continue;

      retval = g_slist_prepend (retval, meta);
    }

  g_list_free (elements);

  return g_slist_reverse (retval);
}

static GSList *
parse_behaviours (ClutterScript *script,
                  ClutterActor  *actor,
                  JsonNode      *node)
{
  GList *elements, *l;
  GSList *retval = NULL;

  if (!JSON_NODE_HOLDS_ARRAY (node))
    return NULL;

  elements = json_array_get_elements (json_node_get_array (node));

  for (l = elements; l != NULL; l = l->next)
    {
      JsonNode *element = l->data;
      const gchar *id_ = _clutter_script_get_id_from_node (element);
      GObject *behaviour;

      if (id_ == NULL || *id_ == '\0')
        continue;

      behaviour = clutter_script_get_object (script, id_);
      if (behaviour == NULL)
        continue;

      retval = g_slist_prepend (retval, behaviour);
    }

  g_list_free (elements);

  return g_slist_reverse (retval);
}

static gboolean
clutter_actor_parse_custom_node (ClutterScriptable *scriptable,
                                 ClutterScript     *script,
                                 GValue            *value,
                                 const gchar       *name,
                                 JsonNode          *node)
{
  ClutterActor *actor = CLUTTER_ACTOR (scriptable);
  gboolean retval = FALSE;

  if ((name[0] == 'x' && name[1] == '\0') ||
      (name[0] == 'y' && name[1] == '\0') ||
      (strcmp (name, "width") == 0) ||
      (strcmp (name, "height") == 0) ||
      (strcmp (name, "anchor_x") == 0) ||
      (strcmp (name, "anchor_y") == 0))
    {
      ParseDimension dimension;
      gfloat units;

      if (name[0] == 'x')
        dimension = PARSE_X;
      else if (name[0] == 'y')
        dimension = PARSE_Y;
      else if (name[0] == 'w')
        dimension = PARSE_WIDTH;
      else if (name[0] == 'h')
        dimension = PARSE_HEIGHT;
      else if (name[0] == 'a' && name[7] == 'x')
        dimension = PARSE_ANCHOR_X;
      else if (name[0] == 'a' && name[7] == 'y')
        dimension = PARSE_ANCHOR_Y;
      else
        return FALSE;

      units = parse_units (actor, dimension, node);

      /* convert back to pixels: all properties are pixel-based */
      g_value_init (value, G_TYPE_FLOAT);
      g_value_set_float (value, units);

      retval = TRUE;
    }
  else if (strcmp (name, "rotation") == 0)
    {
      RotationInfo *info;

      info = g_slice_new0 (RotationInfo);
      retval = parse_rotation (actor, node, info);

      if (retval)
        {
          g_value_init (value, G_TYPE_POINTER);
          g_value_set_pointer (value, info);
        }
      else
        g_slice_free (RotationInfo, info);
    }
  else if (strcmp (name, "behaviours") == 0)
    {
      GSList *l;

      l = parse_behaviours (script, actor, node);

      g_value_init (value, G_TYPE_POINTER);
      g_value_set_pointer (value, l);

      retval = TRUE;
    }
  else if (strcmp (name, "actions") == 0 ||
           strcmp (name, "constraints") == 0 ||
           strcmp (name, "effects") == 0)
    {
      GSList *l;

      l = parse_actor_metas (script, actor, node);

      g_value_init (value, G_TYPE_POINTER);
      g_value_set_pointer (value, l);

      retval = TRUE;
    }

  return retval;
}

static void
clutter_actor_set_custom_property (ClutterScriptable *scriptable,
                                   ClutterScript     *script,
                                   const gchar       *name,
                                   const GValue      *value)
{
  ClutterActor *actor = CLUTTER_ACTOR (scriptable);

#ifdef CLUTTER_ENABLE_DEBUG
  if (G_UNLIKELY (CLUTTER_HAS_DEBUG (SCRIPT)))
    {
      gchar *tmp = g_strdup_value_contents (value);

      CLUTTER_NOTE (SCRIPT,
                    "in ClutterActor::set_custom_property('%s') = %s",
                    name,
                    tmp);

      g_free (tmp);
    }
#endif /* CLUTTER_ENABLE_DEBUG */

  if (strcmp (name, "rotation") == 0)
    {
      RotationInfo *info;

      if (!G_VALUE_HOLDS (value, G_TYPE_POINTER))
        return;

      info = g_value_get_pointer (value);

      clutter_actor_set_rotation (actor,
                                  info->axis, info->angle,
                                  info->center_x,
                                  info->center_y,
                                  info->center_z);

      g_slice_free (RotationInfo, info);

      return;
    }

  if (strcmp (name, "behaviours") == 0)
    {
      GSList *behaviours, *l;

      if (!G_VALUE_HOLDS (value, G_TYPE_POINTER))
        return;

      behaviours = g_value_get_pointer (value);
      for (l = behaviours; l != NULL; l = l->next)
        {
          ClutterBehaviour *behaviour = l->data;

          clutter_behaviour_apply (behaviour, actor);
        }

      g_slist_free (behaviours);

      return;
    }

  if (strcmp (name, "actions") == 0 ||
      strcmp (name, "constraints") == 0 ||
      strcmp (name, "effects") == 0)
    {
      GSList *metas, *l;

      if (!G_VALUE_HOLDS (value, G_TYPE_POINTER))
        return;

      metas = g_value_get_pointer (value);
      for (l = metas; l != NULL; l = l->next)
        {
          if (name[0] == 'a')
            clutter_actor_add_action (actor, l->data);

          if (name[0] == 'c')
            clutter_actor_add_constraint (actor, l->data);

          if (name[0] == 'e')
            clutter_actor_add_effect (actor, l->data);
        }

      g_slist_free (metas);

      return;
    }

  g_object_set_property (G_OBJECT (scriptable), name, value);
}

static void
clutter_scriptable_iface_init (ClutterScriptableIface *iface)
{
  iface->parse_custom_node = clutter_actor_parse_custom_node;
  iface->set_custom_property = clutter_actor_set_custom_property;
}

static ClutterActorMeta *
get_meta_from_animation_property (ClutterActor  *actor,
                                  const gchar   *name,
                                  gchar        **name_p)
{
  ClutterActorPrivate *priv = actor->priv;
  ClutterActorMeta *meta = NULL;
  gchar **tokens;

  /* if this is not a special property, fall through */
  if (name[0] != '@')
    return NULL;

  /* detect the properties named using the following spec:
   *
   *   @<section>.<meta-name>.<property-name>
   *
   * where <section> can be one of the following:
   *
   *   - actions
   *   - constraints
   *   - effects
   *
   * and <meta-name> is the name set on a specific ActorMeta
   */

  tokens = g_strsplit (name + 1, ".", -1);
  if (tokens == NULL || g_strv_length (tokens) != 3)
    {
      CLUTTER_NOTE (ANIMATION, "Invalid property name '%s'",
                    name + 1);
      g_strfreev (tokens);
      return NULL;
    }

  if (strcmp (tokens[0], "actions") == 0)
    meta = _clutter_meta_group_get_meta (priv->actions, tokens[1]);

  if (strcmp (tokens[0], "constraints") == 0)
    meta = _clutter_meta_group_get_meta (priv->constraints, tokens[1]);

  if (strcmp (tokens[0], "effects") == 0)
    meta = _clutter_meta_group_get_meta (priv->effects, tokens[1]);

  if (name_p != NULL)
    *name_p = g_strdup (tokens[2]);

  CLUTTER_NOTE (ANIMATION,
                "Looking for property '%s' of object '%s' in section '%s'",
                tokens[2],
                tokens[1],
                tokens[0]);

  g_strfreev (tokens);

  return meta;
}

static GParamSpec *
clutter_actor_find_property (ClutterAnimatable *animatable,
                             const gchar       *property_name)
{
  ClutterActorMeta *meta = NULL;
  GObjectClass *klass = NULL;
  GParamSpec *pspec = NULL;
  gchar *p_name = NULL;

  meta = get_meta_from_animation_property (CLUTTER_ACTOR (animatable),
                                           property_name,
                                           &p_name);

  if (meta != NULL)
    {
      klass = G_OBJECT_GET_CLASS (meta);

      pspec = g_object_class_find_property (klass, p_name);

      g_free (p_name);
    }
  else
    {
      klass = G_OBJECT_GET_CLASS (animatable);

      pspec = g_object_class_find_property (klass, property_name);
    }

  return pspec;
}

static void
clutter_actor_get_initial_state (ClutterAnimatable *animatable,
                                 const gchar       *property_name,
                                 GValue            *initial)
{
  ClutterActorMeta *meta = NULL;
  gchar *p_name = NULL;

  meta = get_meta_from_animation_property (CLUTTER_ACTOR (animatable),
                                           property_name,
                                           &p_name);

  if (meta != NULL)
    g_object_get_property (G_OBJECT (meta), p_name, initial);
  else
    g_object_get_property (G_OBJECT (animatable), property_name, initial);

  g_free (p_name);
}

static void
clutter_actor_set_final_state (ClutterAnimatable *animatable,
                               const gchar       *property_name,
                               const GValue      *final)
{
  ClutterActorMeta *meta = NULL;
  gchar *p_name = NULL;

  meta = get_meta_from_animation_property (CLUTTER_ACTOR (animatable),
                                           property_name,
                                           &p_name);
  if (meta != NULL)
    g_object_set_property (G_OBJECT (meta), p_name, final);
  else
    g_object_set_property (G_OBJECT (animatable), property_name, final);

  g_free (p_name);
}

static gboolean
clutter_actor_animate_property (ClutterAnimatable *animatable,
                                ClutterAnimation  *animation,
                                const gchar       *property_name,
                                const GValue      *initial,
                                const GValue      *final,
                                gdouble            progress,
                                GValue            *new_value)
{
  ClutterInterval *interval;

  interval = clutter_animation_get_interval (animation, property_name);

  return clutter_interval_compute_value (interval, progress, new_value);
}

static void
clutter_animatable_iface_init (ClutterAnimatableIface *iface)
{
  iface->animate_property = clutter_actor_animate_property;
  iface->find_property = clutter_actor_find_property;
  iface->get_initial_state = clutter_actor_get_initial_state;
  iface->set_final_state = clutter_actor_set_final_state;
}

/**
 * clutter_actor_transform_stage_point:
 * @self: A #ClutterActor
 * @x: (in): x screen coordinate of the point to unproject
 * @y: (in): y screen coordinate of the point to unproject
 * @x_out: (out): return location for the unprojected x coordinance
 * @y_out: (out): return location for the unprojected y coordinance
 *
 * This function translates screen coordinates (@x, @y) to
 * coordinates relative to the actor. For example, it can be used to translate
 * screen events from global screen coordinates into actor-local coordinates.
 *
 * The conversion can fail, notably if the transform stack results in the
 * actor being projected on the screen as a mere line.
 *
 * The conversion should not be expected to be pixel-perfect due to the
 * nature of the operation. In general the error grows when the skewing
 * of the actor rectangle on screen increases.
 *
 * <note><para>This function can be computationally intensive.</para></note>
 *
 * <note><para>This function only works when the allocation is up-to-date,
 * i.e. inside of paint().</para></note>
 *
 * Return value: %TRUE if conversion was successful.
 *
 * Since: 0.6
 */
gboolean
clutter_actor_transform_stage_point (ClutterActor *self,
				     gfloat        x,
				     gfloat        y,
				     gfloat       *x_out,
				     gfloat       *y_out)
{
  ClutterVertex v[4];
  float ST[3][3];
  float RQ[3][3];
  int du, dv, xi, yi;
  float px, py;
  float xf, yf, wf, det;
  ClutterActorPrivate *priv;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), FALSE);

  priv = self->priv;

  /* This implementation is based on the quad -> quad projection algorithm
   * described by Paul Heckbert in:
   *
   *   http://www.cs.cmu.edu/~ph/texfund/texfund.pdf
   *
   * and the sample implementation at:
   *
   *   http://www.cs.cmu.edu/~ph/src/texfund/
   *
   * Our texture is a rectangle with origin [0, 0], so we are mapping from
   * quad to rectangle only, which significantly simplifies things; the
   * function calls have been unrolled, and most of the math is done in fixed
   * point.
   */

  clutter_actor_get_abs_allocation_vertices (self, v);

  /* Keeping these as ints simplifies the multiplication (no significant
   * loss of precision here).
   */
  du = (int) (priv->allocation.x2 - priv->allocation.x1);
  dv = (int) (priv->allocation.y2 - priv->allocation.y1);

  if (!du || !dv)
    return FALSE;

#define UX2FP(x)        (x)
#define DET2FP(a,b,c,d) (((a) * (d)) - ((b) * (c)))

  /* First, find mapping from unit uv square to xy quadrilateral; this
   * equivalent to the pmap_square_quad() functions in the sample
   * implementation, which we can simplify, since our target is always
   * a rectangle.
   */
  px = v[0].x - v[1].x + v[3].x - v[2].x;
  py = v[0].y - v[1].y + v[3].y - v[2].y;

  if (!px && !py)
    {
      /* affine transform */
      RQ[0][0] = UX2FP (v[1].x - v[0].x);
      RQ[1][0] = UX2FP (v[3].x - v[1].x);
      RQ[2][0] = UX2FP (v[0].x);
      RQ[0][1] = UX2FP (v[1].y - v[0].y);
      RQ[1][1] = UX2FP (v[3].y - v[1].y);
      RQ[2][1] = UX2FP (v[0].y);
      RQ[0][2] = 0;
      RQ[1][2] = 0;
      RQ[2][2] = 1.0;
    }
  else
    {
      /* projective transform */
      double dx1, dx2, dy1, dy2, del;

      dx1 = UX2FP (v[1].x - v[3].x);
      dx2 = UX2FP (v[2].x - v[3].x);
      dy1 = UX2FP (v[1].y - v[3].y);
      dy2 = UX2FP (v[2].y - v[3].y);

      del = DET2FP (dx1, dx2, dy1, dy2);
      if (!del)
	return FALSE;

      /*
       * The division here needs to be done in floating point for
       * precisions reasons.
       */
      RQ[0][2] = (DET2FP (UX2FP (px), dx2, UX2FP (py), dy2) / del);
      RQ[1][2] = (DET2FP (dx1, UX2FP (px), dy1, UX2FP (py)) / del);
      RQ[1][2] = (DET2FP (dx1, UX2FP (px), dy1, UX2FP (py)) / del);
      RQ[2][2] = 1.0;
      RQ[0][0] = UX2FP (v[1].x - v[0].x) + (RQ[0][2] * UX2FP (v[1].x));
      RQ[1][0] = UX2FP (v[2].x - v[0].x) + (RQ[1][2] * UX2FP (v[2].x));
      RQ[2][0] = UX2FP (v[0].x);
      RQ[0][1] = UX2FP (v[1].y - v[0].y) + (RQ[0][2] * UX2FP (v[1].y));
      RQ[1][1] = UX2FP (v[2].y - v[0].y) + (RQ[1][2] * UX2FP (v[2].y));
      RQ[2][1] = UX2FP (v[0].y);
    }

  /*
   * Now combine with transform from our rectangle (u0,v0,u1,v1) to unit
   * square. Since our rectangle is based at 0,0 we only need to scale.
   */
  RQ[0][0] /= du;
  RQ[1][0] /= dv;
  RQ[0][1] /= du;
  RQ[1][1] /= dv;
  RQ[0][2] /= du;
  RQ[1][2] /= dv;

  /*
   * Now RQ is transform from uv rectangle to xy quadrilateral; we need an
   * inverse of that.
   */
  ST[0][0] = DET2FP (RQ[1][1], RQ[1][2], RQ[2][1], RQ[2][2]);
  ST[1][0] = DET2FP (RQ[1][2], RQ[1][0], RQ[2][2], RQ[2][0]);
  ST[2][0] = DET2FP (RQ[1][0], RQ[1][1], RQ[2][0], RQ[2][1]);
  ST[0][1] = DET2FP (RQ[2][1], RQ[2][2], RQ[0][1], RQ[0][2]);
  ST[1][1] = DET2FP (RQ[2][2], RQ[2][0], RQ[0][2], RQ[0][0]);
  ST[2][1] = DET2FP (RQ[2][0], RQ[2][1], RQ[0][0], RQ[0][1]);
  ST[0][2] = DET2FP (RQ[0][1], RQ[0][2], RQ[1][1], RQ[1][2]);
  ST[1][2] = DET2FP (RQ[0][2], RQ[0][0], RQ[1][2], RQ[1][0]);
  ST[2][2] = DET2FP (RQ[0][0], RQ[0][1], RQ[1][0], RQ[1][1]);

  /*
   * Check the resulting matrix is OK.
   */
  det = (RQ[0][0] * ST[0][0])
      + (RQ[0][1] * ST[0][1])
      + (RQ[0][2] * ST[0][2]);
  if (!det)
    return FALSE;

  /*
   * Now transform our point with the ST matrix; the notional w
   * coordinate is 1, hence the last part is simply added.
   */
  xi = (int) x;
  yi = (int) y;

  xf = xi * ST[0][0] + yi * ST[1][0] + ST[2][0];
  yf = xi * ST[0][1] + yi * ST[1][1] + ST[2][1];
  wf = xi * ST[0][2] + yi * ST[1][2] + ST[2][2];

  if (x_out)
    *x_out = xf / wf;

  if (y_out)
    *y_out = yf / wf;

#undef UX2FP
#undef DET2FP

  return TRUE;
}

/*
 * ClutterGeometry
 */

static ClutterGeometry*
clutter_geometry_copy (const ClutterGeometry *geometry)
{
  return g_slice_dup (ClutterGeometry, geometry);
}

static void
clutter_geometry_free (ClutterGeometry *geometry)
{
  if (G_LIKELY (geometry != NULL))
    g_slice_free (ClutterGeometry, geometry);
}

/**
 * clutter_geometry_union:
 * @geometry_a: a #ClutterGeometry
 * @geometry_b: another #ClutterGeometry
 * @result: (out): location to store the result
 *
 * Find the union of two rectangles represented as #ClutterGeometry.
 *
 * Since: 1.4
 */
void
clutter_geometry_union (const ClutterGeometry *geometry_a,
                        const ClutterGeometry *geometry_b,
                        ClutterGeometry       *result)
{
  /* We don't try to handle rectangles that can't be represented
   * as a signed integer box */
  gint x_1 = MIN (geometry_a->x, geometry_b->x);
  gint y_1 = MIN (geometry_a->y, geometry_b->y);
  gint x_2 = MAX (geometry_a->x + (gint)geometry_a->width,
                  geometry_b->x + (gint)geometry_b->width);
  gint y_2 = MAX (geometry_a->y + (gint)geometry_a->height,
                  geometry_b->y + (gint)geometry_b->height);
  result->x = x_1;
  result->y = y_1;
  result->width = x_2 - x_1;
  result->height = y_2 - y_1;
}

/**
 * clutter_geometry_intersects:
 * @geometry0: The first geometry to test
 * @geometry1: The second geometry to test
 *
 * Determines if @geometry0 and geometry1 intersect returning %TRUE if
 * they do else %FALSE.
 *
 * Return value: %TRUE of @geometry0 and geometry1 intersect else
 * %FALSE.
 *
 * Since: 1.4
 */
gboolean
clutter_geometry_intersects (const ClutterGeometry *geometry0,
                             const ClutterGeometry *geometry1)
{
  if (geometry1->x >= (geometry0->x + (gint)geometry0->width) ||
      geometry1->y >= (geometry0->y + (gint)geometry0->height) ||
      (geometry1->x + (gint)geometry1->width) <= geometry0->x ||
      (geometry1->y + (gint)geometry1->height) <= geometry0->y)
    return FALSE;
  else
    return TRUE;
}

static gboolean
clutter_geometry_progress (const GValue *a,
                           const GValue *b,
                           gdouble       progress,
                           GValue       *retval)
{
  const ClutterGeometry *a_geom = g_value_get_boxed (a);
  const ClutterGeometry *b_geom = g_value_get_boxed (b);
  ClutterGeometry res = { 0, };
  gint a_width = a_geom->width;
  gint b_width = b_geom->width;
  gint a_height = a_geom->height;
  gint b_height = b_geom->height;

  res.x = a_geom->x + (b_geom->x - a_geom->x) * progress;
  res.y = a_geom->y + (b_geom->y - a_geom->y) * progress;

  res.width = a_width + (b_width - a_width) * progress;
  res.height = a_height + (b_height - a_height) * progress;

  g_value_set_boxed (retval, &res);

  return TRUE;
}

G_DEFINE_BOXED_TYPE_WITH_CODE (ClutterGeometry, clutter_geometry,
                               clutter_geometry_copy,
                               clutter_geometry_free,
                               CLUTTER_REGISTER_INTERVAL_PROGRESS (clutter_geometry_progress));

/*
 * ClutterVertices
 */

/**
 * clutter_vertex_new:
 * @x: X coordinate
 * @y: Y coordinate
 * @z: Z coordinate
 *
 * Creates a new #ClutterVertex for the point in 3D space
 * identified by the 3 coordinates @x, @y, @z
 *
 * Return value: the newly allocate #ClutterVertex. Use
 *   clutter_vertex_free() to free the resources
 *
 * Since: 1.0
 */
ClutterVertex *
clutter_vertex_new (gfloat x,
                    gfloat y,
                    gfloat z)
{
  ClutterVertex *vertex;

  vertex = g_slice_new (ClutterVertex);
  vertex->x = x;
  vertex->y = y;
  vertex->z = z;

  return vertex;
}

/**
 * clutter_vertex_copy:
 * @vertex: a #ClutterVertex
 *
 * Copies @vertex
 *
 * Return value: a newly allocated copy of #ClutterVertex. Use
 *   clutter_vertex_free() to free the allocated resources
 *
 * Since: 1.0
 */
ClutterVertex *
clutter_vertex_copy (const ClutterVertex *vertex)
{
  if (G_LIKELY (vertex != NULL))
    return g_slice_dup (ClutterVertex, vertex);

  return NULL;
}

/**
 * clutter_vertex_free:
 * @vertex: a #ClutterVertex
 *
 * Frees a #ClutterVertex allocated using clutter_vertex_copy()
 *
 * Since: 1.0
 */
void
clutter_vertex_free (ClutterVertex *vertex)
{
  if (G_UNLIKELY (vertex != NULL))
    g_slice_free (ClutterVertex, vertex);
}

/**
 * clutter_vertex_equal:
 * @vertex_a: a #ClutterVertex
 * @vertex_b: a #ClutterVertex
 *
 * Compares @vertex_a and @vertex_b for equality
 *
 * Return value: %TRUE if the passed #ClutterVertex are equal
 *
 * Since: 1.0
 */
gboolean
clutter_vertex_equal (const ClutterVertex *vertex_a,
                      const ClutterVertex *vertex_b)
{
  g_return_val_if_fail (vertex_a != NULL && vertex_b != NULL, FALSE);

  if (vertex_a == vertex_b)
    return TRUE;

  return vertex_a->x == vertex_b->x &&
         vertex_a->y == vertex_b->y &&
         vertex_a->z == vertex_b->z;
}

static gboolean
clutter_vertex_progress (const GValue *a,
                         const GValue *b,
                         gdouble       progress,
                         GValue       *retval)
{
  const ClutterVertex *av = g_value_get_boxed (a);
  const ClutterVertex *bv = g_value_get_boxed (b);
  ClutterVertex res = { 0, };

  res.x = av->x + (bv->x - av->x) * progress;
  res.y = av->y + (bv->y - av->y) * progress;
  res.z = av->z + (bv->z - av->z) * progress;

  g_value_set_boxed (retval, &res);

  return TRUE;
}

G_DEFINE_BOXED_TYPE_WITH_CODE (ClutterVertex, clutter_vertex,
                               clutter_vertex_copy,
                               clutter_vertex_free,
                               CLUTTER_REGISTER_INTERVAL_PROGRESS (clutter_vertex_progress));

struct _ShaderData
{
  ClutterShader *shader;

  /* back pointer to the actor */
  ClutterActor *actor;

  /* list of values that should be set on the shader
   * before each paint cycle
   */
  GHashTable *value_hash;
};

static void
shader_value_free (gpointer data)
{
  GValue *var = data;
  g_value_unset (var);
  g_slice_free (GValue, var);
}

static void
destroy_shader_data (gpointer data)
{
  ShaderData *shader_data = data;

  if (shader_data == NULL)
    return;

  if (shader_data->shader != NULL)
    {
      g_object_unref (shader_data->shader);
      shader_data->shader = NULL;
    }

  if (shader_data->value_hash != NULL)
    {
      g_hash_table_destroy (shader_data->value_hash);
      shader_data->value_hash = NULL;
    }

  g_slice_free (ShaderData, shader_data);
}


/**
 * clutter_actor_get_shader:
 * @self: a #ClutterActor
 *
 * Queries the currently set #ClutterShader on @self.
 *
 * Return value: (transfer none): The currently set #ClutterShader
 *   or %NULL if no shader is set.
 *
 * Since: 0.6
 */
ClutterShader *
clutter_actor_get_shader (ClutterActor *self)
{
  ShaderData *shader_data;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), NULL);

  shader_data = g_object_get_qdata (G_OBJECT (self), quark_shader_data);
  if (shader_data != NULL)
    return shader_data->shader;

  return NULL;
}

/**
 * clutter_actor_set_shader:
 * @self: a #ClutterActor
 * @shader: (allow-none): a #ClutterShader or %NULL to unset the shader.
 *
 * Sets the #ClutterShader to be used when rendering @self.
 *
 * If @shader is %NULL this function will unset any currently set shader
 * for the actor.
 *
 * <note>Any #ClutterEffect applied to @self will take the precedence
 * over the #ClutterShader set using this function.</note>
 *
 * Return value: %TRUE if the shader was successfully applied
 *   or removed
 *
 * Since: 0.6
 */
gboolean
clutter_actor_set_shader (ClutterActor  *self,
                          ClutterShader *shader)
{
  ShaderData *shader_data;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), FALSE);
  g_return_val_if_fail (shader == NULL || CLUTTER_IS_SHADER (shader), FALSE);

  if (shader != NULL)
    g_object_ref (shader);
  else
    {
      /* if shader passed in is NULL we destroy the shader */
      g_object_set_qdata (G_OBJECT (self), quark_shader_data, NULL);
      return TRUE;
    }

  shader_data = g_object_get_qdata (G_OBJECT (self), quark_shader_data);
  if (shader_data == NULL)
    {
      shader_data = g_slice_new (ShaderData);
      shader_data->actor = self;
      shader_data->shader = NULL;
      shader_data->value_hash =
        g_hash_table_new_full (g_str_hash, g_str_equal,
                               g_free,
                               shader_value_free);

      g_object_set_qdata_full (G_OBJECT (self), quark_shader_data,
                               shader_data,
                               destroy_shader_data);
    }

  if (shader_data->shader != NULL)
    g_object_unref (shader_data->shader);

  shader_data->shader = shader;

  clutter_actor_queue_redraw (self);

  return TRUE;
}


static void
set_each_param (gpointer key,
                gpointer value,
                gpointer user_data)
{
  ClutterShader *shader = user_data;
  const gchar *uniform = key;
  GValue *var = value;

  clutter_shader_set_uniform (shader, uniform, var);
}

static void
clutter_actor_shader_pre_paint (ClutterActor *actor,
                                gboolean      repeat)
{
  ShaderData *shader_data;
  ClutterShader *shader;

  shader_data = g_object_get_qdata (G_OBJECT (actor), quark_shader_data);
  if (shader_data == NULL)
    return;

  shader = shader_data->shader;
  if (shader != NULL)
    {
      clutter_shader_set_is_enabled (shader, TRUE);

      g_hash_table_foreach (shader_data->value_hash, set_each_param, shader);

      if (!repeat)
        _clutter_context_push_shader_stack (actor);
    }
}

static void
clutter_actor_shader_post_paint (ClutterActor *actor)
{
  ShaderData *shader_data;
  ClutterShader *shader;

  shader_data = g_object_get_qdata (G_OBJECT (actor), quark_shader_data);
  if (shader_data == NULL)
    return;

  shader = shader_data->shader;
  if (shader != NULL)
    {
      ClutterActor *head;

      clutter_shader_set_is_enabled (shader, FALSE);

      /* remove the actor from the shaders stack; if there is another
       * actor inside it, then call pre-paint again to set its shader
       * but this time with the second argument being TRUE, indicating
       * that we are re-applying an existing shader and thus should it
       * not be prepended to the stack
       */
      head = _clutter_context_pop_shader_stack (actor);
      if (head != NULL)
        clutter_actor_shader_pre_paint (head, TRUE);
    }
}

/**
 * clutter_actor_set_shader_param:
 * @self: a #ClutterActor
 * @param: the name of the parameter
 * @value: the value of the parameter
 *
 * Sets the value for a named parameter of the shader applied
 * to @actor.
 *
 * Since: 1.0
 */
void
clutter_actor_set_shader_param (ClutterActor *self,
                                const gchar  *param,
                                const GValue *value)
{
  ShaderData *shader_data;
  GValue *var;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (param != NULL);
  g_return_if_fail (CLUTTER_VALUE_HOLDS_SHADER_FLOAT (value) ||
                    CLUTTER_VALUE_HOLDS_SHADER_INT (value) ||
                    CLUTTER_VALUE_HOLDS_SHADER_MATRIX (value) ||
                    G_VALUE_HOLDS_FLOAT (value) ||
                    G_VALUE_HOLDS_INT (value));

  shader_data = g_object_get_qdata (G_OBJECT (self), quark_shader_data);
  if (shader_data == NULL)
    return;

  var = g_slice_new0 (GValue);
  g_value_init (var, G_VALUE_TYPE (value));
  g_value_copy (value, var);
  g_hash_table_insert (shader_data->value_hash, g_strdup (param), var);

  clutter_actor_queue_redraw (self);
}

/**
 * clutter_actor_set_shader_param_float:
 * @self: a #ClutterActor
 * @param: the name of the parameter
 * @value: the value of the parameter
 *
 * Sets the value for a named float parameter of the shader applied
 * to @actor.
 *
 * Since: 0.8
 */
void
clutter_actor_set_shader_param_float (ClutterActor *self,
                                      const gchar  *param,
                                      gfloat        value)
{
  GValue var = { 0, };

  g_value_init (&var, G_TYPE_FLOAT);
  g_value_set_float (&var, value);

  clutter_actor_set_shader_param (self, param, &var);

  g_value_unset (&var);
}

/**
 * clutter_actor_set_shader_param_int:
 * @self: a #ClutterActor
 * @param: the name of the parameter
 * @value: the value of the parameter
 *
 * Sets the value for a named int parameter of the shader applied to
 * @actor.
 *
 * Since: 0.8
 */
void
clutter_actor_set_shader_param_int (ClutterActor *self,
                                    const gchar  *param,
                                    gint          value)
{
  GValue var = { 0, };

  g_value_init (&var, G_TYPE_INT);
  g_value_set_int (&var, value);

  clutter_actor_set_shader_param (self, param, &var);

  g_value_unset (&var);
}

/**
 * clutter_actor_is_rotated:
 * @self: a #ClutterActor
 *
 * Checks whether any rotation is applied to the actor.
 *
 * Return value: %TRUE if the actor is rotated.
 *
 * Since: 0.6
 */
gboolean
clutter_actor_is_rotated (ClutterActor *self)
{
  ClutterActorPrivate *priv;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), FALSE);

  priv = self->priv;

  if (priv->rxang || priv->ryang || priv->rzang)
    return TRUE;

  return FALSE;
}

/**
 * clutter_actor_is_scaled:
 * @self: a #ClutterActor
 *
 * Checks whether the actor is scaled in either dimension.
 *
 * Return value: %TRUE if the actor is scaled.
 *
 * Since: 0.6
 */
gboolean
clutter_actor_is_scaled (ClutterActor *self)
{
  ClutterActorPrivate *priv;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), FALSE);

  priv = self->priv;

  if (priv->scale_x != 1.0 || priv->scale_y != 1.0)
    return TRUE;

  return FALSE;
}

ClutterActor *
_clutter_actor_get_stage_internal (ClutterActor *actor)
{
  while (actor && !CLUTTER_ACTOR_IS_TOPLEVEL (actor))
    actor = actor->priv->parent_actor;

  return actor;
}

/**
 * clutter_actor_get_stage:
 * @actor: a #ClutterActor
 *
 * Retrieves the #ClutterStage where @actor is contained.
 *
 * Return value: (transfer none): the stage containing the actor, or %NULL
 *
 * Since: 0.8
 */
ClutterActor *
clutter_actor_get_stage (ClutterActor *actor)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (actor), NULL);

  return _clutter_actor_get_stage_internal (actor);
}

/**
 * clutter_actor_allocate_available_size:
 * @self: a #ClutterActor
 * @x: the actor's X coordinate
 * @y: the actor's Y coordinate
 * @available_width: the maximum available width, or -1 to use the
 *   actor's natural width
 * @available_height: the maximum available height, or -1 to use the
 *   actor's natural height
 * @flags: flags controlling the allocation
 *
 * Allocates @self taking into account the #ClutterActor<!-- -->'s
 * preferred size, but limiting it to the maximum available width
 * and height provided.
 *
 * This function will do the right thing when dealing with the
 * actor's request mode.
 *
 * The implementation of this function is equivalent to:
 *
 * |[
 *   if (request_mode == CLUTTER_REQUEST_HEIGHT_FOR_WIDTH)
 *     {
 *       clutter_actor_get_preferred_width (self, available_height,
 *                                          &amp;min_width,
 *                                          &amp;natural_width);
 *       width = CLAMP (natural_width, min_width, available_width);
 *
 *       clutter_actor_get_preferred_height (self, width,
 *                                           &amp;min_height,
 *                                           &amp;natural_height);
 *       height = CLAMP (natural_height, min_height, available_height);
 *     }
 *   else
 *     {
 *       clutter_actor_get_preferred_height (self, available_width,
 *                                           &amp;min_height,
 *                                           &amp;natural_height);
 *       height = CLAMP (natural_height, min_height, available_height);
 *
 *       clutter_actor_get_preferred_width (self, height,
 *                                          &amp;min_width,
 *                                          &amp;natural_width);
 *       width = CLAMP (natural_width, min_width, available_width);
 *     }
 *
 *   box.x1 = x; box.y1 = y;
 *   box.x2 = box.x1 + available_width;
 *   box.y2 = box.y1 + available_height;
 *   clutter_actor_allocate (self, &amp;box, flags);
 * ]|
 *
 * This function can be used by fluid layout managers to allocate
 * an actor's preferred size without making it bigger than the area
 * available for the container.
 *
 * Since: 1.0
 */
void
clutter_actor_allocate_available_size (ClutterActor           *self,
                                       gfloat                  x,
                                       gfloat                  y,
                                       gfloat                  available_width,
                                       gfloat                  available_height,
                                       ClutterAllocationFlags  flags)
{
  ClutterActorPrivate *priv;
  gfloat width, height;
  gfloat min_width, min_height;
  gfloat natural_width, natural_height;
  ClutterActorBox box;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  width = height = 0.0;

  switch (priv->request_mode)
    {
    case CLUTTER_REQUEST_HEIGHT_FOR_WIDTH:
      clutter_actor_get_preferred_width (self, available_height,
                                         &min_width,
                                         &natural_width);
      width  = CLAMP (natural_width, min_width, available_width);

      clutter_actor_get_preferred_height (self, width,
                                          &min_height,
                                          &natural_height);
      height = CLAMP (natural_height, min_height, available_height);
      break;

    case CLUTTER_REQUEST_WIDTH_FOR_HEIGHT:
      clutter_actor_get_preferred_height (self, available_width,
                                          &min_height,
                                          &natural_height);
      height = CLAMP (natural_height, min_height, available_height);

      clutter_actor_get_preferred_width (self, height,
                                         &min_width,
                                         &natural_width);
      width  = CLAMP (natural_width, min_width, available_width);
      break;
    }


  box.x1 = x;
  box.y1 = y;
  box.x2 = box.x1 + width;
  box.y2 = box.y1 + height;
  clutter_actor_allocate (self, &box, flags);
}

/**
 * clutter_actor_allocate_preferred_size:
 * @self: a #ClutterActor
 * @flags: flags controlling the allocation
 *
 * Allocates the natural size of @self.
 *
 * This function is a utility call for #ClutterActor implementations
 * that allocates the actor's preferred natural size. It can be used
 * by fixed layout managers (like #ClutterGroup or so called
 * 'composite actors') inside the ClutterActor::allocate
 * implementation to give each child exactly how much space it
 * requires.
 *
 * This function is not meant to be used by applications. It is also
 * not meant to be used outside the implementation of the
 * ClutterActor::allocate virtual function.
 *
 * Since: 0.8
 */
void
clutter_actor_allocate_preferred_size (ClutterActor           *self,
                                       ClutterAllocationFlags  flags)
{
  gfloat actor_x, actor_y;
  gfloat natural_width, natural_height;
  ClutterActorBox actor_box;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  actor_x = clutter_actor_get_x (self);
  actor_y = clutter_actor_get_y (self);

  clutter_actor_get_preferred_size (self,
                                    NULL, NULL,
                                    &natural_width,
                                    &natural_height);

  actor_box.x1 = actor_x;
  actor_box.y1 = actor_y;
  actor_box.x2 = actor_box.x1 + natural_width;
  actor_box.y2 = actor_box.y1 + natural_height;

  clutter_actor_allocate (self, &actor_box, flags);
}

/**
 * clutter_actor_allocate_align_fill:
 * @self: a #ClutterActor
 * @box: a #ClutterActorBox, containing the available width and height
 * @x_align: the horizontal alignment, between 0 and 1
 * @y_align: the vertical alignment, between 0 and 1
 * @x_fill: whether the actor should fill horizontally
 * @y_fill: whether the actor should fill vertically
 * @flags: allocation flags to be passed to clutter_actor_allocate()
 *
 * Allocates @self by taking into consideration the available allocation
 * area; an alignment factor on either axis; and whether the actor should
 * fill the allocation on either axis.
 *
 * The @box should contain the available allocation width and height;
 * if the x1 and y1 members of #ClutterActorBox are not set to 0, the
 * allocation will be offset by their value.
 *
 * This function takes into consideration the geometry request specified by
 * the #ClutterActor:request-mode property, and the text direction.
 *
 * This function is useful for fluid layout managers, like #ClutterBinLayout
 * or #ClutterTableLayout
 *
 * Since: 1.4
 */
void
clutter_actor_allocate_align_fill (ClutterActor           *self,
                                   const ClutterActorBox  *box,
                                   gdouble                 x_align,
                                   gdouble                 y_align,
                                   gboolean                x_fill,
                                   gboolean                y_fill,
                                   ClutterAllocationFlags  flags)
{
  ClutterActorPrivate *priv;
  ClutterActorBox allocation = { 0, };
  gfloat x_offset, y_offset;
  gfloat available_width, available_height;
  gfloat child_width, child_height;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (box != NULL);
  g_return_if_fail (x_align >= 0.0 && x_align <= 1.0);
  g_return_if_fail (y_align >= 0.0 && y_align <= 1.0);

  priv = self->priv;

  clutter_actor_box_get_origin (box, &x_offset, &y_offset);
  clutter_actor_box_get_size (box, &available_width, &available_height);

  if (available_width < 0)
    available_width = 0;

  if (available_height < 0)
    available_height = 0;

  if (x_fill)
    {
      allocation.x1 = x_offset;
      allocation.x2 = allocation.x1 + available_width;
    }

  if (y_fill)
    {
      allocation.y1 = y_offset;
      allocation.y2 = allocation.y1 + available_height;
    }

  /* if we are filling horizontally and vertically then we're done */
  if (x_fill && y_fill)
    goto out;

  child_width = child_height = 0.0f;

  if (priv->request_mode == CLUTTER_REQUEST_HEIGHT_FOR_WIDTH)
    {
      gfloat min_width, natural_width;
      gfloat min_height, natural_height;

      clutter_actor_get_preferred_width (self, available_height,
                                         &min_width,
                                         &natural_width);

      child_width = CLAMP (natural_width, min_width, available_width);

      if (!y_fill)
        {
          clutter_actor_get_preferred_height (self, child_width,
                                              &min_height,
                                              &natural_height);

          child_height = CLAMP (natural_height, min_height, available_height);
        }
    }
  else
    {
      gfloat min_width, natural_width;
      gfloat min_height, natural_height;

      clutter_actor_get_preferred_height (self, available_width,
                                          &min_height,
                                          &natural_height);

      child_height = CLAMP (natural_height, min_height, available_height);

      if (!x_fill)
        {
          clutter_actor_get_preferred_width (self, child_height,
                                             &min_width,
                                             &natural_width);

          child_width = CLAMP (natural_width, min_width, available_width);
        }
    }

  /* invert the horizontal alignment for RTL languages */
  if (priv->text_direction == CLUTTER_TEXT_DIRECTION_RTL)
    x_align = 1.0 - x_align;

  if (!x_fill)
    {
      allocation.x1 = x_offset
                    + ((available_width - child_width) * x_align);
      allocation.x2 = allocation.x1 + child_width;
    }

  if (!y_fill)
    {
      allocation.y1 = y_offset
                    + ((available_height - child_height) * y_align);
      allocation.y2 = allocation.y1 + child_height;
    }

out:
  clutter_actor_box_clamp_to_pixel (&allocation);
  clutter_actor_allocate (self, &allocation, flags);
}

/**
 * clutter_actor_grab_key_focus:
 * @self: a #ClutterActor
 *
 * Sets the key focus of the #ClutterStage including @self
 * to this #ClutterActor.
 *
 * Since: 1.0
 */
void
clutter_actor_grab_key_focus (ClutterActor *self)
{
  ClutterActor *stage;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  stage = _clutter_actor_get_stage_internal (self);
  if (stage != NULL)
    clutter_stage_set_key_focus (CLUTTER_STAGE (stage), self);
}

/**
 * clutter_actor_get_pango_context:
 * @self: a #ClutterActor
 *
 * Retrieves the #PangoContext for @self. The actor's #PangoContext
 * is already configured using the appropriate font map, resolution
 * and font options.
 *
 * Unlike clutter_actor_create_pango_context(), this context is owend
 * by the #ClutterActor and it will be updated each time the options
 * stored by the #ClutterBackend change.
 *
 * You can use the returned #PangoContext to create a #PangoLayout
 * and render text using cogl_pango_render_layout() to reuse the
 * glyphs cache also used by Clutter.
 *
 * Return value: (transfer none): the #PangoContext for a #ClutterActor.
 *   The returned #PangoContext is owned by the actor and should not be
 *   unreferenced by the application code
 *
 * Since: 1.0
 */
PangoContext *
clutter_actor_get_pango_context (ClutterActor *self)
{
  ClutterActorPrivate *priv;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), NULL);

  priv = self->priv;

  if (priv->pango_context != NULL)
    return priv->pango_context;

  priv->pango_context = _clutter_context_get_pango_context ();
  g_object_ref (priv->pango_context);

  return priv->pango_context;
}

/**
 * clutter_actor_create_pango_context:
 * @self: a #ClutterActor
 *
 * Creates a #PangoContext for the given actor. The #PangoContext
 * is already configured using the appropriate font map, resolution
 * and font options.
 *
 * See also clutter_actor_get_pango_context().
 *
 * Return value: (transfer full): the newly created #PangoContext.
 *   Use g_object_unref() on the returned value to deallocate its
 *   resources
 *
 * Since: 1.0
 */
PangoContext *
clutter_actor_create_pango_context (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), NULL);

  return _clutter_context_create_pango_context ();
}

/**
 * clutter_actor_create_pango_layout:
 * @self: a #ClutterActor
 * @text: (allow-none) the text to set on the #PangoLayout, or %NULL
 *
 * Creates a new #PangoLayout from the same #PangoContext used
 * by the #ClutterActor. The #PangoLayout is already configured
 * with the font map, resolution and font options, and the
 * given @text.
 *
 * If you want to keep around a #PangoLayout created by this
 * function you will have to connect to the #ClutterBackend::font-changed
 * and #ClutterBackend::resolution-changed signals, and call
 * pango_layout_context_changed() in response to them.
 *
 * Return value: (transfer full): the newly created #PangoLayout.
 *   Use g_object_unref() when done
 *
 * Since: 1.0
 */
PangoLayout *
clutter_actor_create_pango_layout (ClutterActor *self,
                                   const gchar  *text)
{
  PangoContext *context;
  PangoLayout *layout;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), NULL);

  context = clutter_actor_get_pango_context (self);
  layout = pango_layout_new (context);

  if (text)
    pango_layout_set_text (layout, text, -1);

  return layout;
}

/* Allows overriding the calculated paint opacity. Used by ClutterClone and
 * ClutterOffscreenEffect.
 */
void
_clutter_actor_set_opacity_override (ClutterActor *self,
                                     gint          opacity)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  self->priv->opacity_override = opacity;
}

gint
_clutter_actor_get_opacity_override (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), -1);

  return self->priv->opacity_override;
}

/* Allows you to disable applying the actors model view transform during
 * a paint. Used by ClutterClone. */
void
_clutter_actor_set_enable_model_view_transform (ClutterActor *self,
                                                gboolean      enable)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  self->priv->enable_model_view_transform = enable;
}

void
_clutter_actor_set_enable_paint_unmapped (ClutterActor *self,
                                          gboolean      enable)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  priv->enable_paint_unmapped = enable;

  if (priv->enable_paint_unmapped)
    {
      /* Make sure that the parents of the widget are realized first;
       * otherwise checks in clutter_actor_update_map_state() will
       * fail.
       */
      clutter_actor_realize (self);

      clutter_actor_update_map_state (self, MAP_STATE_MAKE_MAPPED);
    }
  else
    {
      clutter_actor_update_map_state (self, MAP_STATE_MAKE_UNMAPPED);
    }
}

static void
clutter_anchor_coord_get_units (ClutterActor      *self,
                                const AnchorCoord *coord,
                                gfloat            *x,
                                gfloat            *y,
                                gfloat            *z)
{
  if (coord->is_fractional)
    {
      gfloat actor_width, actor_height;

      clutter_actor_get_size (self, &actor_width, &actor_height);

      if (x)
        *x = actor_width * coord->v.fraction.x;

      if (y)
        *y = actor_height * coord->v.fraction.y;

      if (z)
        *z = 0;
    }
  else
    {
      if (x)
        *x = coord->v.units.x;

      if (y)
        *y = coord->v.units.y;

      if (z)
        *z = coord->v.units.z;
    }
}

static void
clutter_anchor_coord_set_units (AnchorCoord *coord,
                                gfloat       x,
                                gfloat       y,
                                gfloat       z)
{
  coord->is_fractional = FALSE;
  coord->v.units.x = x;
  coord->v.units.y = y;
  coord->v.units.z = z;
}

static ClutterGravity
clutter_anchor_coord_get_gravity (AnchorCoord *coord)
{
  if (coord->is_fractional)
    {
      if (coord->v.fraction.x == 0.0)
        {
          if (coord->v.fraction.y == 0.0)
            return CLUTTER_GRAVITY_NORTH_WEST;
          else if (coord->v.fraction.y == 0.5)
            return CLUTTER_GRAVITY_WEST;
          else if (coord->v.fraction.y == 1.0)
            return CLUTTER_GRAVITY_SOUTH_WEST;
          else
            return CLUTTER_GRAVITY_NONE;
        }
      else if (coord->v.fraction.x == 0.5)
        {
          if (coord->v.fraction.y == 0.0)
            return CLUTTER_GRAVITY_NORTH;
          else if (coord->v.fraction.y == 0.5)
            return CLUTTER_GRAVITY_CENTER;
          else if (coord->v.fraction.y == 1.0)
            return CLUTTER_GRAVITY_SOUTH;
          else
            return CLUTTER_GRAVITY_NONE;
        }
      else if (coord->v.fraction.x == 1.0)
        {
          if (coord->v.fraction.y == 0.0)
            return CLUTTER_GRAVITY_NORTH_EAST;
          else if (coord->v.fraction.y == 0.5)
            return CLUTTER_GRAVITY_EAST;
          else if (coord->v.fraction.y == 1.0)
            return CLUTTER_GRAVITY_SOUTH_EAST;
          else
            return CLUTTER_GRAVITY_NONE;
        }
      else
        return CLUTTER_GRAVITY_NONE;
    }
  else
    return CLUTTER_GRAVITY_NONE;
}

static void
clutter_anchor_coord_set_gravity (AnchorCoord    *coord,
                                  ClutterGravity  gravity)
{
  switch (gravity)
    {
    case CLUTTER_GRAVITY_NORTH:
      coord->v.fraction.x = 0.5;
      coord->v.fraction.y = 0.0;
      break;

    case CLUTTER_GRAVITY_NORTH_EAST:
      coord->v.fraction.x = 1.0;
      coord->v.fraction.y = 0.0;
      break;

    case CLUTTER_GRAVITY_EAST:
      coord->v.fraction.x = 1.0;
      coord->v.fraction.y = 0.5;
      break;

    case CLUTTER_GRAVITY_SOUTH_EAST:
      coord->v.fraction.x = 1.0;
      coord->v.fraction.y = 1.0;
      break;

    case CLUTTER_GRAVITY_SOUTH:
      coord->v.fraction.x = 0.5;
      coord->v.fraction.y = 1.0;
      break;

    case CLUTTER_GRAVITY_SOUTH_WEST:
      coord->v.fraction.x = 0.0;
      coord->v.fraction.y = 1.0;
      break;

    case CLUTTER_GRAVITY_WEST:
      coord->v.fraction.x = 0.0;
      coord->v.fraction.y = 0.5;
      break;

    case CLUTTER_GRAVITY_NORTH_WEST:
      coord->v.fraction.x = 0.0;
      coord->v.fraction.y = 0.0;
      break;

    case CLUTTER_GRAVITY_CENTER:
      coord->v.fraction.x = 0.5;
      coord->v.fraction.y = 0.5;
      break;

    default:
      coord->v.fraction.x = 0.0;
      coord->v.fraction.y = 0.0;
      break;
    }

  coord->is_fractional = TRUE;
}

static gboolean
clutter_anchor_coord_is_zero (const AnchorCoord *coord)
{
  if (coord->is_fractional)
    return coord->v.fraction.x == 0.0 && coord->v.fraction.y == 0.0;
  else
    return (coord->v.units.x == 0.0
            && coord->v.units.y == 0.0
            && coord->v.units.z == 0.0);
}

/**
 * clutter_actor_get_flags:
 * @self: a #ClutterActor
 *
 * Retrieves the flags set on @self
 *
 * Return value: a bitwise or of #ClutterActorFlags or 0
 *
 * Since: 1.0
 */
ClutterActorFlags
clutter_actor_get_flags (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), 0);

  return self->flags;
}

/**
 * clutter_actor_set_flags:
 * @self: a #ClutterActor
 * @flags: the flags to set
 *
 * Sets @flags on @self
 *
 * This function will emit notifications for the changed properties
 *
 * Since: 1.0
 */
void
clutter_actor_set_flags (ClutterActor      *self,
                         ClutterActorFlags  flags)
{
  ClutterActorFlags old_flags;
  GObject *obj;
  gboolean was_reactive_set, reactive_set;
  gboolean was_realized_set, realized_set;
  gboolean was_mapped_set, mapped_set;
  gboolean was_visible_set, visible_set;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  if (self->flags == flags)
    return;

  obj = G_OBJECT (self);
  g_object_ref (obj);
  g_object_freeze_notify (obj);

  old_flags = self->flags;

  was_reactive_set = ((old_flags & CLUTTER_ACTOR_REACTIVE) != 0);
  was_realized_set = ((old_flags & CLUTTER_ACTOR_REALIZED) != 0);
  was_mapped_set   = ((old_flags & CLUTTER_ACTOR_MAPPED)   != 0);
  was_visible_set  = ((old_flags & CLUTTER_ACTOR_VISIBLE)  != 0);

  self->flags |= flags;

  reactive_set = ((self->flags & CLUTTER_ACTOR_REACTIVE) != 0);
  realized_set = ((self->flags & CLUTTER_ACTOR_REALIZED) != 0);
  mapped_set   = ((self->flags & CLUTTER_ACTOR_MAPPED)   != 0);
  visible_set  = ((self->flags & CLUTTER_ACTOR_VISIBLE)  != 0);

  if (reactive_set != was_reactive_set)
    g_object_notify_by_pspec (obj, obj_props[PROP_REACTIVE]);

  if (realized_set != was_realized_set)
    g_object_notify_by_pspec (obj, obj_props[PROP_REALIZED]);

  if (mapped_set != was_mapped_set)
    g_object_notify_by_pspec (obj, obj_props[PROP_MAPPED]);

  if (visible_set != was_visible_set)
    g_object_notify_by_pspec (obj, obj_props[PROP_VISIBLE]);

  g_object_thaw_notify (obj);
  g_object_unref (obj);
}

/**
 * clutter_actor_unset_flags:
 * @self: a #ClutterActor
 * @flags: the flags to unset
 *
 * Unsets @flags on @self
 *
 * This function will emit notifications for the changed properties
 *
 * Since: 1.0
 */
void
clutter_actor_unset_flags (ClutterActor      *self,
                           ClutterActorFlags  flags)
{
  ClutterActorFlags old_flags;
  GObject *obj;
  gboolean was_reactive_set, reactive_set;
  gboolean was_realized_set, realized_set;
  gboolean was_mapped_set, mapped_set;
  gboolean was_visible_set, visible_set;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  obj = G_OBJECT (self);
  g_object_freeze_notify (obj);

  old_flags = self->flags;

  was_reactive_set = ((old_flags & CLUTTER_ACTOR_REACTIVE) != 0);
  was_realized_set = ((old_flags & CLUTTER_ACTOR_REALIZED) != 0);
  was_mapped_set   = ((old_flags & CLUTTER_ACTOR_MAPPED)   != 0);
  was_visible_set  = ((old_flags & CLUTTER_ACTOR_VISIBLE)  != 0);

  self->flags &= ~flags;

  if (self->flags == old_flags)
    return;

  reactive_set = ((self->flags & CLUTTER_ACTOR_REACTIVE) != 0);
  realized_set = ((self->flags & CLUTTER_ACTOR_REALIZED) != 0);
  mapped_set   = ((self->flags & CLUTTER_ACTOR_MAPPED)   != 0);
  visible_set  = ((self->flags & CLUTTER_ACTOR_VISIBLE)  != 0);

  if (reactive_set != was_reactive_set)
    g_object_notify_by_pspec (obj, obj_props[PROP_REACTIVE]);

  if (realized_set != was_realized_set)
    g_object_notify_by_pspec (obj, obj_props[PROP_REALIZED]);

  if (mapped_set != was_mapped_set)
    g_object_notify_by_pspec (obj, obj_props[PROP_MAPPED]);

  if (visible_set != was_visible_set)
    g_object_notify_by_pspec (obj, obj_props[PROP_VISIBLE]);

  g_object_thaw_notify (obj);
}

/**
 * clutter_actor_get_transformation_matrix:
 * @self: a #ClutterActor
 * @matrix: (out): the return location for a #CoglMatrix
 *
 * Retrieves the transformations applied to @self relative to its
 * parent.
 *
 * Since: 1.0
 */
void
clutter_actor_get_transformation_matrix (ClutterActor *self,
                                         CoglMatrix   *matrix)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  cogl_matrix_init_identity (matrix);

  _clutter_actor_apply_modelview_transform (self, matrix);
}

void
_clutter_actor_set_in_clone_paint (ClutterActor *self,
                                   gboolean      is_in_clone_paint)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  self->priv->in_clone_paint = is_in_clone_paint;
}

/**
 * clutter_actor_is_in_clone_paint:
 * @self: a #ClutterActor
 *
 * Checks whether @self is being currently painted by a #ClutterClone
 *
 * This function is useful only inside the ::paint virtual function
 * implementations or within handlers for the #ClutterActor::paint
 * signal
 *
 * This function should not be used by applications
 *
 * Return value: %TRUE if the #ClutterActor is currently being painted
 *   by a #ClutterClone, and %FALSE otherwise
 *
 * Since: 1.0
 */
gboolean
clutter_actor_is_in_clone_paint (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), FALSE);

  return self->priv->in_clone_paint;
}

static void
set_direction_recursive (ClutterActor *actor,
                         gpointer      user_data)
{
  ClutterTextDirection text_dir = GPOINTER_TO_INT (user_data);

  clutter_actor_set_text_direction (actor, text_dir);
}

/**
 * clutter_actor_set_text_direction:
 * @self: a #ClutterActor
 * @text_dir: the text direction for @self
 *
 * Sets the #ClutterTextDirection for an actor
 *
 * The passed text direction must not be %CLUTTER_TEXT_DIRECTION_DEFAULT
 *
 * If @self implements #ClutterContainer then this function will recurse
 * inside all the children of @self (including the internal ones).
 *
 * Composite actors not implementing #ClutterContainer, or actors requiring
 * special handling when the text direction changes, should connect to
 * the #GObject::notify signal for the #ClutterActor:text-direction property
 *
 * Since: 1.2
 */
void
clutter_actor_set_text_direction (ClutterActor         *self,
                                  ClutterTextDirection  text_dir)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (text_dir != CLUTTER_TEXT_DIRECTION_DEFAULT);

  priv = self->priv;

  if (priv->text_direction != text_dir)
    {
      priv->text_direction = text_dir;

      /* we need to emit the notify::text-direction first, so that
       * the sub-classes can catch that and do specific handling of
       * the text direction; see clutter_text_direction_changed_cb()
       * inside clutter-text.c
       */
      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_TEXT_DIRECTION]);

      /* if this is a container we need to recurse */
      if (CLUTTER_IS_CONTAINER (self))
        {
          ClutterContainer *container = CLUTTER_CONTAINER (self);

          clutter_container_foreach_with_internals (container,
                                                    set_direction_recursive,
                                                    GINT_TO_POINTER (text_dir));
        }

      clutter_actor_queue_relayout (self);
    }
}

void
_clutter_actor_set_has_pointer (ClutterActor *self,
                                gboolean      has_pointer)
{
  ClutterActorPrivate *priv = self->priv;

  if (priv->has_pointer != has_pointer)
    {
      priv->has_pointer = has_pointer;

      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_HAS_POINTER]);
    }
}

/**
 * clutter_actor_get_text_direction:
 * @self: a #ClutterActor
 *
 * Retrieves the value set using clutter_actor_set_text_direction()
 *
 * If no text direction has been previously set, the default text
 * direction, as returned by clutter_get_default_text_direction(), will
 * be returned instead
 *
 * Return value: the #ClutterTextDirection for the actor
 *
 * Since: 1.2
 */
ClutterTextDirection
clutter_actor_get_text_direction (ClutterActor *self)
{
  ClutterActorPrivate *priv;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self),
                        CLUTTER_TEXT_DIRECTION_LTR);

  priv = self->priv;

  /* if no direction has been set yet use the default */
  if (priv->text_direction == CLUTTER_TEXT_DIRECTION_DEFAULT)
    priv->text_direction = clutter_get_default_text_direction ();

  return priv->text_direction;
}

/**
 * clutter_actor_push_internal:
 * @self: a #ClutterActor
 *
 * Should be used by actors implementing the #ClutterContainer and with
 * internal children added through clutter_actor_set_parent(), for instance:
 *
 * |[
 *   static void
 *   my_actor_init (MyActor *self)
 *   {
 *     self->priv = SELF_ACTOR_GET_PRIVATE (self);
 *
 *     clutter_actor_push_internal (CLUTTER_ACTOR (self));
 *
 *     /&ast; calling clutter_actor_set_parent() now will result in
 *      &ast; the internal flag being set on a child of MyActor
 *      &ast;/
 *
 *     /&ast; internal child - a background texture &ast;/
 *     self->priv->background_tex = clutter_texture_new ();
 *     clutter_actor_set_parent (self->priv->background_tex,
 *                               CLUTTER_ACTOR (self));
 *
 *     /&ast; internal child - a label &ast;/
 *     self->priv->label = clutter_text_new ();
 *     clutter_actor_set_parent (self->priv->label,
 *                               CLUTTER_ACTOR (self));
 *
 *     clutter_actor_pop_internal (CLUTTER_ACTOR (self));
 *
 *     /&ast; calling clutter_actor_set_parent() now will not result in
 *      &ast; the internal flag being set on a child of MyActor
 *      &ast;/
 *   }
 * ]|
 *
 * This function will be used by Clutter to toggle an "internal child"
 * flag whenever clutter_actor_set_parent() is called; internal children
 * are handled differently by Clutter, specifically when destroying their
 * parent.
 *
 * Call clutter_actor_pop_internal() when you finished adding internal
 * children.
 *
 * Nested calls to clutter_actor_push_internal() are allowed, but each
 * one must by followed by a clutter_actor_pop_internal() call.
 *
 * Since: 1.2
 */
void
clutter_actor_push_internal (ClutterActor *self)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  self->priv->internal_child += 1;
}

/**
 * clutter_actor_pop_internal:
 * @self: a #ClutterActor
 *
 * Disables the effects of clutter_actor_push_internal()
 *
 * Since: 1.2
 */
void
clutter_actor_pop_internal (ClutterActor *self)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  priv = self->priv;

  if (priv->internal_child == 0)
    {
      g_warning ("Mismatched %s: you need to call "
                 "clutter_actor_push_composite() at least once before "
                 "calling this function", G_STRFUNC);
      return;
    }

  priv->internal_child -= 1;
}

/**
 * clutter_actor_has_pointer:
 * @self: a #ClutterActor
 *
 * Checks whether an actor contains the pointer of a
 * #ClutterInputDevice
 *
 * Return value: %TRUE if the actor contains the pointer, and
 *   %FALSE otherwise
 *
 * Since: 1.2
 */
gboolean
clutter_actor_has_pointer (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), FALSE);

  return self->priv->has_pointer;
}

/* XXX: This is a workaround for not being able to break the ABI of
 * the QUEUE_REDRAW signal. It is an out-of-band argument.  See
 * clutter_actor_queue_clipped_redraw() for details.
 */
ClutterPaintVolume *
_clutter_actor_get_queue_redraw_clip (ClutterActor *self)
{
  return self->priv->oob_queue_redraw_clip;
}

void
_clutter_actor_set_queue_redraw_clip (ClutterActor *self,
                                      ClutterPaintVolume *clip)
{
  self->priv->oob_queue_redraw_clip = clip;
}

/**
 * clutter_actor_has_allocation:
 * @self: a #ClutterActor
 *
 * Checks if the actor has an up-to-date allocation assigned to
 * it. This means that the actor should have an allocation: it's
 * visible and has a parent. It also means that there is no
 * outstanding relayout request in progress for the actor or its
 * children (There might be other outstanding layout requests in
 * progress that will cause the actor to get a new allocation
 * when the stage is laid out, however).
 *
 * If this function returns %FALSE, then the actor will normally
 * be allocated before it is next drawn on the screen.
 *
 * Return value: %TRUE if the actor has an up-to-date allocation
 *
 * Since: 1.4
 */
gboolean
clutter_actor_has_allocation (ClutterActor *self)
{
  ClutterActorPrivate *priv;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), FALSE);

  priv = self->priv;

  return priv->parent_actor != NULL &&
         CLUTTER_ACTOR_IS_VISIBLE (self) &&
         !priv->needs_allocation;
}

/**
 * clutter_actor_add_action:
 * @self: a #ClutterActor
 * @action: a #ClutterAction
 *
 * Adds @action to the list of actions applied to @self
 *
 * A #ClutterAction can only belong to one actor at a time
 *
 * The #ClutterActor will hold a reference on @action until either
 * clutter_actor_remove_action() or clutter_actor_clear_actions()
 * is called
 *
 * Since: 1.4
 */
void
clutter_actor_add_action (ClutterActor  *self,
                          ClutterAction *action)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (CLUTTER_IS_ACTION (action));

  priv = self->priv;

  if (priv->actions == NULL)
    {
      priv->actions = g_object_new (CLUTTER_TYPE_META_GROUP, NULL);
      priv->actions->actor = self;
    }

  _clutter_meta_group_add_meta (priv->actions, CLUTTER_ACTOR_META (action));

  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ACTIONS]);
}

/**
 * clutter_actor_add_action_with_name:
 * @self: a #ClutterActor
 * @name: the name to set on the action
 * @action: a #ClutterAction
 *
 * A convenience function for setting the name of a #ClutterAction
 * while adding it to the list of actions applied to @self
 *
 * This function is the logical equivalent of:
 *
 * |[
 *   clutter_actor_meta_set_name (CLUTTER_ACTOR_META (action), name);
 *   clutter_actor_add_action (self, action);
 * ]|
 *
 * Since: 1.4
 */
void
clutter_actor_add_action_with_name (ClutterActor  *self,
                                    const gchar   *name,
                                    ClutterAction *action)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (name != NULL);
  g_return_if_fail (CLUTTER_IS_ACTION (action));

  clutter_actor_meta_set_name (CLUTTER_ACTOR_META (action), name);
  clutter_actor_add_action (self, action);
}

/**
 * clutter_actor_remove_action:
 * @self: a #ClutterActor
 * @action: a #ClutterAction
 *
 * Removes @action from the list of actions applied to @self
 *
 * The reference held by @self on the #ClutterAction will be released
 *
 * Since: 1.4
 */
void
clutter_actor_remove_action (ClutterActor  *self,
                             ClutterAction *action)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (CLUTTER_IS_ACTION (action));

  priv = self->priv;

  if (priv->actions == NULL)
    return;

  _clutter_meta_group_remove_meta (priv->actions, CLUTTER_ACTOR_META (action));

  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ACTIONS]);
}

/**
 * clutter_actor_remove_action_by_name:
 * @self: a #ClutterActor
 * @name: the name of the action to remove
 *
 * Removes the #ClutterAction with the given name from the list
 * of actions applied to @self
 *
 * Since: 1.4
 */
void
clutter_actor_remove_action_by_name (ClutterActor *self,
                                     const gchar  *name)
{
  ClutterActorPrivate *priv;
  ClutterActorMeta *meta;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (name != NULL);

  priv = self->priv;

  if (priv->actions == NULL)
    return;

  meta = _clutter_meta_group_get_meta (priv->actions, name);
  if (meta == NULL)
    return;

  _clutter_meta_group_remove_meta (priv->actions, meta);

  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_ACTIONS]);
}

/**
 * clutter_actor_get_actions:
 * @self: a #ClutterActor
 *
 * Retrieves the list of actions applied to @self
 *
 * Return value: (transfer container) (element-type Clutter.Action): a copy
 *   of the list of #ClutterAction<!-- -->s. The contents of the list are
 *   owned by the #ClutterActor. Use g_list_free() to free the resources
 *   allocated by the returned #GList
 *
 * Since: 1.4
 */
GList *
clutter_actor_get_actions (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), NULL);

  if (self->priv->actions == NULL)
    return NULL;

  return _clutter_meta_group_get_metas_no_internal (self->priv->actions);
}

/**
 * clutter_actor_get_action:
 * @self: a #ClutterActor
 * @name: the name of the action to retrieve
 *
 * Retrieves the #ClutterAction with the given name in the list
 * of actions applied to @self
 *
 * Return value: (transfer none): a #ClutterAction for the given
 *   name, or %NULL. The returned #ClutterAction is owned by the
 *   actor and it should not be unreferenced directly
 *
 * Since: 1.4
 */
ClutterAction *
clutter_actor_get_action (ClutterActor *self,
                          const gchar  *name)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), NULL);
  g_return_val_if_fail (name != NULL, NULL);

  if (self->priv->actions == NULL)
    return NULL;

  return CLUTTER_ACTION (_clutter_meta_group_get_meta (self->priv->actions, name));
}

/**
 * clutter_actor_clear_actions:
 * @self: a #ClutterActor
 *
 * Clears the list of actions applied to @self
 *
 * Since: 1.4
 */
void
clutter_actor_clear_actions (ClutterActor *self)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  if (self->priv->actions == NULL)
    return;

  _clutter_meta_group_clear_metas_no_internal (self->priv->actions);
}

/**
 * clutter_actor_add_constraint:
 * @self: a #ClutterActor
 * @constraint: a #ClutterConstraint
 *
 * Adds @constraint to the list of #ClutterConstraint<!-- -->s applied
 * to @self
 *
 * The #ClutterActor will hold a reference on the @constraint until
 * either clutter_actor_remove_constraint() or
 * clutter_actor_clear_constraints() is called.
 *
 * Since: 1.4
 */
void
clutter_actor_add_constraint (ClutterActor      *self,
                              ClutterConstraint *constraint)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (CLUTTER_IS_CONSTRAINT (constraint));

  priv = self->priv;

  if (priv->constraints == NULL)
    {
      priv->constraints = g_object_new (CLUTTER_TYPE_META_GROUP, NULL);
      priv->constraints->actor = self;
    }

  _clutter_meta_group_add_meta (priv->constraints,
                                CLUTTER_ACTOR_META (constraint));
  clutter_actor_queue_relayout (self);

  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_CONSTRAINTS]);
}

/**
 * clutter_actor_add_constraint_with_name:
 * @self: a #ClutterActor
 * @name: the name to set on the constraint
 * @constraint: a #ClutterConstraint
 *
 * A convenience function for setting the name of a #ClutterConstraint
 * while adding it to the list of constraints applied to @self
 *
 * This function is the logical equivalent of:
 *
 * |[
 *   clutter_actor_meta_set_name (CLUTTER_ACTOR_META (constraint), name);
 *   clutter_actor_add_constraint (self, constraint);
 * ]|
 *
 * Since: 1.4
 */
void
clutter_actor_add_constraint_with_name (ClutterActor      *self,
                                        const gchar       *name,
                                        ClutterConstraint *constraint)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (name != NULL);
  g_return_if_fail (CLUTTER_IS_CONSTRAINT (constraint));

  clutter_actor_meta_set_name (CLUTTER_ACTOR_META (constraint), name);
  clutter_actor_add_constraint (self, constraint);
}

/**
 * clutter_actor_remove_constraint:
 * @self: a #ClutterActor
 * @constraint: a #ClutterConstraint
 *
 * Removes @constraint from the list of constraints applied to @self
 *
 * The reference held by @self on the #ClutterConstraint will be released
 *
 * Since: 1.4
 */
void
clutter_actor_remove_constraint (ClutterActor      *self,
                                 ClutterConstraint *constraint)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (CLUTTER_IS_CONSTRAINT (constraint));

  priv = self->priv;

  if (priv->constraints == NULL)
    return;

  _clutter_meta_group_remove_meta (priv->constraints,
                                   CLUTTER_ACTOR_META (constraint));
  clutter_actor_queue_relayout (self);

  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_CONSTRAINTS]);
}

/**
 * clutter_actor_remove_constraint_by_name:
 * @self: a #ClutterActor
 * @name: the name of the constraint to remove
 *
 * Removes the #ClutterConstraint with the given name from the list
 * of constraints applied to @self
 *
 * Since: 1.4
 */
void
clutter_actor_remove_constraint_by_name (ClutterActor *self,
                                         const gchar  *name)
{
  ClutterActorPrivate *priv;
  ClutterActorMeta *meta;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (name != NULL);

  priv = self->priv;

  if (priv->constraints == NULL)
    return;

  meta = _clutter_meta_group_get_meta (priv->constraints, name);
  if (meta == NULL)
    return;

  _clutter_meta_group_remove_meta (priv->constraints, meta);
  clutter_actor_queue_relayout (self);
}

/**
 * clutter_actor_get_constraints:
 * @self: a #ClutterActor
 *
 * Retrieves the list of constraints applied to @self
 *
 * Return value: (transfer container) (element-type Clutter.Constraint): a copy
 *   of the list of #ClutterConstraint<!-- -->s. The contents of the list are
 *   owned by the #ClutterActor. Use g_list_free() to free the resources
 *   allocated by the returned #GList
 *
 * Since: 1.4
 */
GList *
clutter_actor_get_constraints (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), NULL);

  if (self->priv->constraints == NULL)
    return NULL;

  return _clutter_meta_group_get_metas_no_internal (self->priv->constraints);
}

/**
 * clutter_actor_get_constraint:
 * @self: a #ClutterActor
 * @name: the name of the constraint to retrieve
 *
 * Retrieves the #ClutterConstraint with the given name in the list
 * of constraints applied to @self
 *
 * Return value: (transfer none): a #ClutterConstraint for the given
 *   name, or %NULL. The returned #ClutterConstraint is owned by the
 *   actor and it should not be unreferenced directly
 *
 * Since: 1.4
 */
ClutterConstraint *
clutter_actor_get_constraint (ClutterActor *self,
                              const gchar  *name)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), NULL);
  g_return_val_if_fail (name != NULL, NULL);

  if (self->priv->constraints == NULL)
    return NULL;

  return CLUTTER_CONSTRAINT (_clutter_meta_group_get_meta (self->priv->constraints, name));
}

/**
 * clutter_actor_clear_constraints:
 * @self: a #ClutterActor
 *
 * Clears the list of constraints applied to @self
 *
 * Since: 1.4
 */
void
clutter_actor_clear_constraints (ClutterActor *self)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  if (self->priv->constraints == NULL)
    return;

  _clutter_meta_group_clear_metas_no_internal (self->priv->constraints);

  clutter_actor_queue_relayout (self);
}

/**
 * clutter_actor_set_clip_to_allocation:
 * @self: a #ClutterActor
 * @clip_set: %TRUE to apply a clip tracking the allocation
 *
 * Sets whether @self should be clipped to the same size as its
 * allocation
 *
 * Since: 1.4
 */
void
clutter_actor_set_clip_to_allocation (ClutterActor *self,
                                      gboolean      clip_set)
{
  ClutterActorPrivate *priv;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  clip_set = !!clip_set;

  priv = self->priv;

  if (priv->clip_to_allocation != clip_set)
    {
      priv->clip_to_allocation = clip_set;

      clutter_actor_queue_redraw (self);

      g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_CLIP_TO_ALLOCATION]);
    }
}

/**
 * clutter_actor_get_clip_to_allocation:
 * @self: a #ClutterActor
 *
 * Retrieves the value set using clutter_actor_set_clip_to_allocation()
 *
 * Return value: %TRUE if the #ClutterActor is clipped to its allocation
 *
 * Since: 1.4
 */
gboolean
clutter_actor_get_clip_to_allocation (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), FALSE);

  return self->priv->clip_to_allocation;
}

/**
 * clutter_actor_add_effect:
 * @self: a #ClutterActor
 * @effect: a #ClutterEffect
 *
 * Adds @effect to the list of #ClutterEffect<!-- -->s applied to @self
 *
 * The #ClutterActor will hold a reference on the @effect until either
 * clutter_actor_remove_effect() or clutter_actor_clear_effects() is
 * called.
 *
 * Since: 1.4
 */
void
clutter_actor_add_effect (ClutterActor  *self,
                          ClutterEffect *effect)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (CLUTTER_IS_EFFECT (effect));

  _clutter_actor_add_effect_internal (self, effect);

  clutter_actor_queue_redraw (self);

  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_EFFECT]);
}

/**
 * clutter_actor_add_effect_with_name:
 * @self: a #ClutterActor
 * @name: the name to set on the effect
 * @effect: a #ClutterEffect
 *
 * A convenience function for setting the name of a #ClutterEffect
 * while adding it to the list of effectss applied to @self
 *
 * This function is the logical equivalent of:
 *
 * |[
 *   clutter_actor_meta_set_name (CLUTTER_ACTOR_META (effect), name);
 *   clutter_actor_add_effect (self, effect);
 * ]|
 *
 * Since: 1.4
 */
void
clutter_actor_add_effect_with_name (ClutterActor  *self,
                                    const gchar   *name,
                                    ClutterEffect *effect)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (name != NULL);
  g_return_if_fail (CLUTTER_IS_EFFECT (effect));

  clutter_actor_meta_set_name (CLUTTER_ACTOR_META (effect), name);
  clutter_actor_add_effect (self, effect);
}

/**
 * clutter_actor_remove_effect:
 * @self: a #ClutterActor
 * @effect: a #ClutterEffect
 *
 * Removes @effect from the list of effects applied to @self
 *
 * The reference held by @self on the #ClutterEffect will be released
 *
 * Since: 1.4
 */
void
clutter_actor_remove_effect (ClutterActor  *self,
                             ClutterEffect *effect)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (CLUTTER_IS_EFFECT (effect));

  _clutter_actor_remove_effect_internal (self, effect);

  clutter_actor_queue_redraw (self);

  g_object_notify_by_pspec (G_OBJECT (self), obj_props[PROP_EFFECT]);
}

/**
 * clutter_actor_remove_effect_by_name:
 * @self: a #ClutterActor
 * @name: the name of the effect to remove
 *
 * Removes the #ClutterEffect with the given name from the list
 * of effects applied to @self
 *
 * Since: 1.4
 */
void
clutter_actor_remove_effect_by_name (ClutterActor *self,
                                     const gchar  *name)
{
  ClutterActorPrivate *priv;
  ClutterActorMeta *meta;

  g_return_if_fail (CLUTTER_IS_ACTOR (self));
  g_return_if_fail (name != NULL);

  priv = self->priv;

  if (priv->effects == NULL)
    return;

  meta = _clutter_meta_group_get_meta (priv->effects, name);
  if (meta == NULL)
    return;

  _clutter_meta_group_remove_meta (priv->effects, meta);
}

/**
 * clutter_actor_get_effects:
 * @self: a #ClutterActor
 *
 * Retrieves the #ClutterEffect<!-- -->s applied on @self, if any
 *
 * Return value: (transfer container) (element-type Clutter.Effect): a list
 *   of #ClutterEffect<!-- -->s, or %NULL. The elements of the returned
 *   list are owned by Clutter and they should not be freed. You should
 *   free the returned list using g_list_free() when done
 *
 * Since: 1.4
 */
GList *
clutter_actor_get_effects (ClutterActor *self)
{
  ClutterActorPrivate *priv;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), NULL);

  priv = self->priv;

  if (priv->effects == NULL)
    return NULL;

  return _clutter_meta_group_get_metas_no_internal (priv->effects);
}

/**
 * clutter_actor_get_effect:
 * @self: a #ClutterActor
 * @name: the name of the effect to retrieve
 *
 * Retrieves the #ClutterEffect with the given name in the list
 * of effects applied to @self
 *
 * Return value: (transfer none): a #ClutterEffect for the given
 *   name, or %NULL. The returned #ClutterEffect is owned by the
 *   actor and it should not be unreferenced directly
 *
 * Since: 1.4
 */
ClutterEffect *
clutter_actor_get_effect (ClutterActor *self,
                          const gchar  *name)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), NULL);
  g_return_val_if_fail (name != NULL, NULL);

  if (self->priv->effects == NULL)
    return NULL;

  return CLUTTER_EFFECT (_clutter_meta_group_get_meta (self->priv->effects, name));
}

/**
 * clutter_actor_clear_effects:
 * @self: a #ClutterActor
 *
 * Clears the list of effects applied to @self
 *
 * Since: 1.4
 */
void
clutter_actor_clear_effects (ClutterActor *self)
{
  g_return_if_fail (CLUTTER_IS_ACTOR (self));

  if (self->priv->effects == NULL)
    return;

  _clutter_meta_group_clear_metas_no_internal (self->priv->effects);

  clutter_actor_queue_redraw (self);
}

/**
 * clutter_actor_has_key_focus:
 * @self: a #ClutterActor
 *
 * Checks whether @self is the #ClutterActor that has key focus
 *
 * Return value: %TRUE if the actor has key focus, and %FALSE otherwise
 *
 * Since: 1.4
 */
gboolean
clutter_actor_has_key_focus (ClutterActor *self)
{
  ClutterActor *stage;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), FALSE);

  stage = _clutter_actor_get_stage_internal (self);
  if (stage == NULL)
    return FALSE;

  return clutter_stage_get_key_focus (CLUTTER_STAGE (stage)) == self;
}

static gboolean
_clutter_actor_get_paint_volume_real (ClutterActor *self,
                                      ClutterPaintVolume *pv)
{
  ClutterActorPrivate *priv = self->priv;

  /* Actors are only expected to report a valid paint volume
   * while they have a valid allocation. */
  if (G_UNLIKELY (priv->needs_allocation))
    {
      CLUTTER_NOTE (CLIPPING, "Bail from get_paint_volume (%s): "
                    "Actor needs allocation",
                    _clutter_actor_get_debug_name (self));
      return FALSE;
    }

  /* Check if there are any handlers connected to the paint
   * signal. If there are then all bets are off for what the paint
   * volume for this actor might possibly be!
   *
   * XXX: It's expected that this is going to end up being quite a
   * costly check to have to do here, but we haven't come up with
   * another solution that can reliably catch paint signal handlers at
   * the right time to either avoid artefacts due to invalid stage
   * clipping or due to incorrect culling.
   *
   * Previously we checked in clutter_actor_paint(), but at that time
   * we may already be using a stage clip that could be derived from
   * an invalid paint-volume. We used to try and handle that by
   * queuing a follow up, unclipped, redraw but still the previous
   * checking wasn't enough to catch invalid volumes involved in
   * culling (considering that containers may derive their volume from
   * children that haven't yet been painted)
   *
   * Longer term, improved solutions could be:
   * - Disallow painting in the paint signal, only allow using it
   *   for tracking when paints happen. We can add another API that
   *   allows monkey patching the paint of arbitrary actors but in a
   *   more controlled way and that also supports modifying the
   *   paint-volume.
   * - If we could be notified somehow when signal handlers are
   *   connected we wouldn't have to poll for handlers like this.
   */
  if (g_signal_has_handler_pending (self,
                                    actor_signals[PAINT],
                                    0,
                                    TRUE))
    {
      CLUTTER_NOTE (CLIPPING, "Bail from get_paint_volume (%s): "
                    "Actor has \"paint\" signal handlers",
                    _clutter_actor_get_debug_name (self));
      return FALSE;
    }

  _clutter_paint_volume_init_static (pv, self);

  if (!CLUTTER_ACTOR_GET_CLASS (self)->get_paint_volume (self, pv))
    {
      clutter_paint_volume_free (pv);
      CLUTTER_NOTE (CLIPPING, "Bail from get_paint_volume (%s): "
                    "Actor failed to report a volume",
                    _clutter_actor_get_debug_name (self));
      return FALSE;
    }

  /* since effects can modify the paint volume, we allow them to actually
   * do this by making get_paint_volume() "context sensitive"
   */
  if (priv->effects != NULL)
    {
      if (priv->current_effect != NULL)
        {
          const GList *effects, *l;

          /* if we are being called from within the paint sequence of
           * an actor, get the paint volume up to the current effect
           */
          effects = _clutter_meta_group_peek_metas (priv->effects);
          for (l = effects;
               l != NULL || (l != NULL && l->data != priv->current_effect);
               l = l->next)
            {
              if (!_clutter_effect_get_paint_volume (l->data, pv))
                {
                  clutter_paint_volume_free (pv);
                  CLUTTER_NOTE (CLIPPING, "Bail from get_paint_volume (%s): "
                                "Effect (%s) failed to report a volume",
                                _clutter_actor_get_debug_name (self),
                                _clutter_actor_meta_get_debug_name (l->data));
                  return FALSE;
                }
            }
        }
      else
        {
          const GList *effects, *l;

          /* otherwise, get the cumulative volume */
          effects = _clutter_meta_group_peek_metas (priv->effects);
          for (l = effects; l != NULL; l = l->next)
            if (!_clutter_effect_get_paint_volume (l->data, pv))
              {
                clutter_paint_volume_free (pv);
                CLUTTER_NOTE (CLIPPING, "Bail from get_paint_volume (%s): "
                              "Effect (%s) failed to report a volume",
                              _clutter_actor_get_debug_name (self),
                              _clutter_actor_meta_get_debug_name (l->data));
                return FALSE;
              }
        }
    }

  return TRUE;
}

/* The public clutter_actor_get_paint_volume API returns a const
 * pointer since we return a pointer directly to the cached
 * PaintVolume associated with the actor and don't want the user to
 * inadvertently modify it, but for internal uses we sometimes need
 * access to the same PaintVolume but need to apply some book-keeping
 * modifications to it so we don't want a const pointer.
 */
static ClutterPaintVolume *
_clutter_actor_get_paint_volume_mutable (ClutterActor *self)
{
  ClutterActorPrivate *priv;

  priv = self->priv;

  if (priv->paint_volume_valid)
    clutter_paint_volume_free (&priv->paint_volume);

  if (_clutter_actor_get_paint_volume_real (self, &priv->paint_volume))
    {
      priv->paint_volume_valid = TRUE;
      return &priv->paint_volume;
    }
  else
    {
      priv->paint_volume_valid = FALSE;
      return NULL;
    }
}

/**
 * clutter_actor_get_paint_volume:
 * @self: a #ClutterActor
 *
 * Retrieves the paint volume of the passed #ClutterActor, or %NULL
 * when a paint volume can't be determined.
 *
 * The paint volume is defined as the 3D space occupied by an actor
 * when being painted.
 *
 * This function will call the <function>get_paint_volume()</function>
 * virtual function of the #ClutterActor class. Sub-classes of #ClutterActor
 * should not usually care about overriding the default implementation,
 * unless they are, for instance: painting outside their allocation, or
 * actors with a depth factor (not in terms of #ClutterActor:depth but real
 * 3D depth).
 *
 * <note>2D actors overriding <function>get_paint_volume()</function>
 * ensure their volume has a depth of 0. (This will be true so long as
 * you don't call clutter_paint_volume_set_depth().)</note>
 *
 * Return value: (transfer none): a pointer to a #ClutterPaintVolume
 *   or %NULL if no volume could be determined.
 *
 * Since: 1.6
 */
const ClutterPaintVolume *
clutter_actor_get_paint_volume (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), NULL);

  return _clutter_actor_get_paint_volume_mutable (self);
}

/**
 * clutter_actor_get_transformed_paint_volume:
 * @self: a #ClutterActor
 * @relative_to_ancestor: A #ClutterActor that is an ancestor of @self
 *    (or %NULL for the stage)
 *
 * Retrieves the 3D paint volume of an actor like
 * clutter_actor_get_paint_volume() does (Please refer to the
 * documentation of clutter_actor_get_paint_volume() for more
 * details.) and it additionally transforms the paint volume into the
 * coordinate space of @relative_to_ancestor. (Or the stage if %NULL
 * is passed for @relative_to_ancestor)
 *
 * This can be used by containers that base their paint volume on
 * the volume of their children. Such containers can query the
 * transformed paint volume of all of its children and union them
 * together using clutter_paint_volume_union().
 *
 * Return value: (transfer none): a pointer to a #ClutterPaintVolume
 *   or %NULL if no volume could be determined.
 *
 * Since: 1.6
 */
const ClutterPaintVolume *
clutter_actor_get_transformed_paint_volume (ClutterActor *self,
                                            ClutterActor *relative_to_ancestor)
{
  const ClutterPaintVolume *volume;
  ClutterActor *stage;
  ClutterPaintVolume *transformed_volume;

  stage = _clutter_actor_get_stage_internal (self);
  if (G_UNLIKELY (stage == NULL))
    return NULL;

  if (relative_to_ancestor == NULL)
    relative_to_ancestor = stage;

  volume = clutter_actor_get_paint_volume (self);
  if (volume == NULL)
    return NULL;

  transformed_volume =
    _clutter_stage_paint_volume_stack_allocate (CLUTTER_STAGE (stage));

  _clutter_paint_volume_copy_static (volume, transformed_volume);

  _clutter_paint_volume_transform_relative (transformed_volume,
                                            relative_to_ancestor);

  return transformed_volume;
}

/**
 * clutter_actor_get_paint_box:
 * @self: a #ClutterActor
 * @box: (out): return location for a #ClutterActorBox
 *
 * Retrieves the paint volume of the passed #ClutterActor, and
 * transforms it into a 2D bounding box in stage coordinates.
 *
 * This function is useful to determine the on screen area occupied by
 * the actor. The box is only an approximation and may often be
 * considerably larger due to the optimizations used to calculate the
 * box. The box is never smaller though, so it can reliably be used
 * for culling.
 *
 * There are times when a 2D paint box can't be determined, e.g.
 * because the actor isn't yet parented under a stage or because
 * the actor is unable to determine a paint volume.
 *
 * Return value: %TRUE if a 2D paint box could be determined, else
 * %FALSE.
 *
 * Since: 1.6
 */
gboolean
clutter_actor_get_paint_box (ClutterActor    *self,
                             ClutterActorBox *box)
{
  ClutterActor *stage;
  ClutterPaintVolume *pv;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), FALSE);
  g_return_val_if_fail (box != NULL, FALSE);

  stage = _clutter_actor_get_stage_internal (self);
  if (G_UNLIKELY (!stage))
    return FALSE;

  pv = _clutter_actor_get_paint_volume_mutable (self);
  if (G_UNLIKELY (!pv))
    return FALSE;

  _clutter_paint_volume_get_stage_paint_box (pv, CLUTTER_STAGE (stage), box);

  return TRUE;
}

/**
 * clutter_actor_has_overlaps:
 * @self: A #ClutterActor
 *
 * Return value: whether the actor may contain overlapping
 * primitives. Clutter uses this to determine whether the painting
 * should be redirected to an offscreen buffer to correctly implement
 * the opacity property. Custom actors can override this by
 * implementing the has_overlaps virtual. See
 * clutter_actor_set_offscreen_redirect() for more information.
 *
 * Since: 1.8
 */
gboolean
clutter_actor_has_overlaps (ClutterActor *self)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (self), TRUE);

  return CLUTTER_ACTOR_GET_CLASS (self)->has_overlaps (self);
}

gint
_clutter_actor_get_n_children (ClutterActor *self)
{
  return self->priv->n_children;
}

/* _clutter_actor_foreach_child:
 * @actor: The actor whos children you want to iterate
 * @callback: The function to call for each child
 * @user_data: Private data to pass to @callback
 *
 * Calls a given @callback once for each child of the specified @actor and
 * passing the @user_data pointer each time.
 *
 * Return value: returns %TRUE if all children were iterated, else
 *    %FALSE if a callback broke out of iteration early.
 */
gboolean
_clutter_actor_foreach_child (ClutterActor *self,
                              ClutterForeachCallback callback,
                              void *user_data)
{
  ClutterActorPrivate *priv = self->priv;
  gboolean cont;
  GList *l;

  for (cont = TRUE, l = priv->children; cont && l; l = l->next)
    cont = callback (l->data, user_data);

  return cont;
}

/* For debugging purposes this gives us a simple way to print out
 * the scenegraph e.g in gdb using:
 * [|
 *   _clutter_actor_traverse (clutter_stage_get_default (),
 *                            0,
 *                            _clutter_debug_print_actor_cb,
 *                            NULL,
 *                            NULL);
 * |]
 */
ClutterActorTraverseVisitFlags
_clutter_debug_print_actor_cb (ClutterActor *actor,
                               int depth,
                               void *user_data)
{
  g_print ("%*s%s:%p\n",
           depth * 2, "",
           _clutter_actor_get_debug_name (actor),
           actor);

  return CLUTTER_ACTOR_TRAVERSE_VISIT_CONTINUE;
}

static void
_clutter_actor_traverse_breadth (ClutterActor           *actor,
                                 ClutterTraverseCallback callback,
                                 gpointer                user_data)
{
  GQueue *queue = g_queue_new ();
  ClutterActor dummy;
  int current_depth = 0;

  g_queue_push_tail (queue, actor);
  g_queue_push_tail (queue, &dummy); /* use to delimit depth changes */

  while ((actor = g_queue_pop_head (queue)))
    {
      ClutterActorTraverseVisitFlags flags;

      if (actor == &dummy)
        {
          current_depth++;
          g_queue_push_tail (queue, &dummy);
          continue;
        }

      flags = callback (actor, current_depth, user_data);
      if (flags & CLUTTER_ACTOR_TRAVERSE_VISIT_BREAK)
        break;
      else if (!(flags & CLUTTER_ACTOR_TRAVERSE_VISIT_SKIP_CHILDREN))
        {
          GList *l;
          for (l = actor->priv->children; l; l = l->next)
            g_queue_push_tail (queue, l->data);
        }
    }

  g_queue_free (queue);
}

static ClutterActorTraverseVisitFlags
_clutter_actor_traverse_depth (ClutterActor           *actor,
                               ClutterTraverseCallback before_children_callback,
                               ClutterTraverseCallback after_children_callback,
                               int                     current_depth,
                               gpointer                user_data)
{
  ClutterActorTraverseVisitFlags flags;

  flags = before_children_callback (actor, current_depth, user_data);
  if (flags & CLUTTER_ACTOR_TRAVERSE_VISIT_BREAK)
    return CLUTTER_ACTOR_TRAVERSE_VISIT_BREAK;

  if (!(flags & CLUTTER_ACTOR_TRAVERSE_VISIT_SKIP_CHILDREN))
    {
      GList *l;
      for (l = actor->priv->children; l; l = l->next)
        {
          flags = _clutter_actor_traverse_depth (l->data,
                                                 before_children_callback,
                                                 after_children_callback,
                                                 current_depth + 1,
                                                 user_data);
          if (flags & CLUTTER_ACTOR_TRAVERSE_VISIT_BREAK)
            return CLUTTER_ACTOR_TRAVERSE_VISIT_BREAK;
        }
    }

  if (after_children_callback)
    return after_children_callback (actor, current_depth, user_data);
  else
    return CLUTTER_ACTOR_TRAVERSE_VISIT_CONTINUE;
}

/* _clutter_actor_traverse:
 * @actor: The actor to start traversing the graph from
 * @flags: These flags may affect how the traversal is done
 * @before_children_callback: A function to call before visiting the
 *   children of the current actor.
 * @after_children_callback: A function to call after visiting the
 *   children of the current actor. (Ignored if
 *   %CLUTTER_ACTOR_TRAVERSE_BREADTH_FIRST is passed to @flags.)
 * @user_data: The private data to pass to the callbacks
 *
 * Traverses the scenegraph starting at the specified @actor and
 * descending through all its children and its children's children.
 * For each actor traversed @before_children_callback and
 * @after_children_callback are called with the specified
 * @user_data, before and after visiting that actor's children.
 *
 * The callbacks can return flags that affect the ongoing traversal
 * such as by skipping over an actors children or bailing out of
 * any further traversing.
 */
void
_clutter_actor_traverse (ClutterActor              *actor,
                         ClutterActorTraverseFlags  flags,
                         ClutterTraverseCallback    before_children_callback,
                         ClutterTraverseCallback    after_children_callback,
                         gpointer                   user_data)
{
  if (flags & CLUTTER_ACTOR_TRAVERSE_BREADTH_FIRST)
    _clutter_actor_traverse_breadth (actor,
                                     before_children_callback,
                                     user_data);
  else /* DEPTH_FIRST */
    _clutter_actor_traverse_depth (actor,
                                   before_children_callback,
                                   after_children_callback,
                                   0, /* start depth */
                                   user_data);
}
