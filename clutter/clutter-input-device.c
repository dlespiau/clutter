/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Copyright © 2009, 2010, 2011  Intel Corp.
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
 * Author: Emmanuele Bassi <ebassi@linux.intel.com>
 */

/**
 * SECTION:clutter-input-device
 * @short_description: An input device managed by Clutter
 *
 * #ClutterInputDevice represents an input device known to Clutter.
 *
 * The #ClutterInputDevice class holds the state of the device, but
 * its contents are usually defined by the Clutter backend in use.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "clutter-input-device.h"

#include "clutter-actor-private.h"
#include "clutter-debug.h"
#include "clutter-device-manager-private.h"
#include "clutter-enum-types.h"
#include "clutter-event-private.h"
#include "clutter-marshal.h"
#include "clutter-private.h"
#include "clutter-stage-private.h"

enum
{
  PROP_0,

  PROP_BACKEND,

  PROP_ID,
  PROP_NAME,

  PROP_DEVICE_TYPE,
  PROP_DEVICE_MANAGER,
  PROP_DEVICE_MODE,

  PROP_HAS_CURSOR,
  PROP_ENABLED,

  PROP_N_AXES,

  PROP_LAST
};

static GParamSpec *obj_props[PROP_LAST] = { NULL, };

G_DEFINE_TYPE (ClutterInputDevice, clutter_input_device, G_TYPE_OBJECT);

static void
clutter_input_device_dispose (GObject *gobject)
{
  ClutterInputDevice *device = CLUTTER_INPUT_DEVICE (gobject);

  g_free (device->device_name);

  if (device->device_mode == CLUTTER_INPUT_MODE_SLAVE)
    _clutter_input_device_remove_slave (device->associated, device);

  if (device->associated != NULL)
    {
      _clutter_input_device_set_associated_device (device->associated, NULL);
      g_object_unref (device->associated);
      device->associated = NULL;
    }

  if (device->axes != NULL)
    {
      g_array_free (device->axes, TRUE);
      device->axes = NULL;
    }

  if (device->keys != NULL)
    {
      g_array_free (device->keys, TRUE);
      device->keys = NULL;
    }

  G_OBJECT_CLASS (clutter_input_device_parent_class)->dispose (gobject);
}

static void
clutter_input_device_set_property (GObject      *gobject,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  ClutterInputDevice *self = CLUTTER_INPUT_DEVICE (gobject);

  switch (prop_id)
    {
    case PROP_ID:
      self->id = g_value_get_int (value);
      break;

    case PROP_DEVICE_TYPE:
      self->device_type = g_value_get_enum (value);
      break;

    case PROP_DEVICE_MANAGER:
      self->device_manager = g_value_get_object (value);
      break;

    case PROP_DEVICE_MODE:
      self->device_mode = g_value_get_enum (value);
      break;

    case PROP_BACKEND:
      self->backend = g_value_get_object (value);
      break;

    case PROP_NAME:
      self->device_name = g_value_dup_string (value);
      break;

    case PROP_HAS_CURSOR:
      self->has_cursor = g_value_get_boolean (value);
      break;

    case PROP_ENABLED:
      clutter_input_device_set_enabled (self, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
clutter_input_device_get_property (GObject    *gobject,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  ClutterInputDevice *self = CLUTTER_INPUT_DEVICE (gobject);

  switch (prop_id)
    {
    case PROP_ID:
      g_value_set_int (value, self->id);
      break;

    case PROP_DEVICE_TYPE:
      g_value_set_enum (value, self->device_type);
      break;

    case PROP_DEVICE_MANAGER:
      g_value_set_object (value, self->device_manager);
      break;

    case PROP_DEVICE_MODE:
      g_value_set_enum (value, self->device_mode);
      break;

    case PROP_BACKEND:
      g_value_set_object (value, self->backend);
      break;

    case PROP_NAME:
      g_value_set_string (value, self->device_name);
      break;

    case PROP_HAS_CURSOR:
      g_value_set_boolean (value, self->has_cursor);
      break;

    case PROP_N_AXES:
      g_value_set_uint (value, clutter_input_device_get_n_axes (self));
      break;

    case PROP_ENABLED:
      g_value_set_boolean (value, self->is_enabled);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
clutter_input_device_class_init (ClutterInputDeviceClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  /**
   * ClutterInputDevice:id:
   *
   * The unique identifier of the device
   *
   * Since: 1.2
   */
  obj_props[PROP_ID] =
    g_param_spec_int ("id",
                      P_("Id"),
                      P_("Unique identifier of the device"),
                      -1, G_MAXINT,
                      0,
                      CLUTTER_PARAM_READWRITE |
                      G_PARAM_CONSTRUCT_ONLY);

  /**
   * ClutterInputDevice:name:
   *
   * The name of the device
   *
   * Since: 1.2
   */
  obj_props[PROP_NAME] =
    g_param_spec_string ("name",
                         P_("Name"),
                         P_("The name of the device"),
                         NULL,
                         CLUTTER_PARAM_READWRITE |
                         G_PARAM_CONSTRUCT_ONLY);

  /**
   * ClutterInputDevice:device-type:
   *
   * The type of the device
   *
   * Since: 1.2
   */
  obj_props[PROP_DEVICE_TYPE] =
    g_param_spec_enum ("device-type",
                       P_("Device Type"),
                       P_("The type of the device"),
                       CLUTTER_TYPE_INPUT_DEVICE_TYPE,
                       CLUTTER_POINTER_DEVICE,
                       CLUTTER_PARAM_READWRITE |
                       G_PARAM_CONSTRUCT_ONLY);

  /**
   * ClutterInputDevice:device-manager:
   *
   * The #ClutterDeviceManager instance which owns the device
   *
   * Since: 1.6
   */
  obj_props[PROP_DEVICE_MANAGER] =
    g_param_spec_object ("device-manager",
                         P_("Device Manager"),
                         P_("The device manager instance"),
                         CLUTTER_TYPE_DEVICE_MANAGER,
                         CLUTTER_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  /**
   * ClutterInputDevice:mode:
   *
   * The mode of the device.
   *
   * Since: 1.6
   */
  obj_props[PROP_DEVICE_MODE] =
    g_param_spec_enum ("device-mode",
                       P_("Device Mode"),
                       P_("The mode of the device"),
                       CLUTTER_TYPE_INPUT_MODE,
                       CLUTTER_INPUT_MODE_FLOATING,
                       CLUTTER_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  /**
   * ClutterInputDevice:has-cursor:
   *
   * Whether the device has an on screen cursor following its movement.
   *
   * Since: 1.6
   */
  obj_props[PROP_HAS_CURSOR] =
    g_param_spec_boolean ("has-cursor",
                          P_("Has Cursor"),
                          P_("Whether the device has a cursor"),
                          FALSE,
                          CLUTTER_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  /**
   * ClutterInputDevice:enabled:
   *
   * Whether the device is enabled.
   *
   * A device with the #ClutterInputDevice:device-mode property set
   * to %CLUTTER_INPUT_MODE_MASTER cannot be disabled.
   *
   * A device must be enabled in order to receive events from it.
   *
   * Since: 1.6
   */
  obj_props[PROP_ENABLED] =
    g_param_spec_boolean ("enabled",
                          P_("Enabled"),
                          P_("Whether the device is enabled"),
                          FALSE,
                          CLUTTER_PARAM_READWRITE);

  /**
   * ClutterInputDevice:n-axes:
   *
   * The number of axes of the device.
   *
   * Since: 1.6
   */
  obj_props[PROP_N_AXES] =
    g_param_spec_uint ("n-axes",
                       P_("Number of Axes"),
                       P_("The number of axes on the device"),
                       0, G_MAXUINT,
                       0,
                       CLUTTER_PARAM_READABLE);

  /**
   * ClutterInputDevice:backend:
   *
   * The #ClutterBackend that created the device.
   *
   * Since: 1.6
   */
  obj_props[PROP_BACKEND] =
    g_param_spec_object ("backend",
                         P_("Backend"),
                         P_("The backend instance"),
                         CLUTTER_TYPE_BACKEND,
                         CLUTTER_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  gobject_class->dispose = clutter_input_device_dispose;
  gobject_class->set_property = clutter_input_device_set_property;
  gobject_class->get_property = clutter_input_device_get_property;
  g_object_class_install_properties (gobject_class, PROP_LAST, obj_props);
}

static void
clutter_input_device_init (ClutterInputDevice *self)
{
  self->id = -1;
  self->device_type = CLUTTER_POINTER_DEVICE;

  self->click_count = 0;

  self->current_time = self->previous_time = CLUTTER_CURRENT_TIME;
  self->current_x = self->previous_x = -1;
  self->current_y = self->previous_y = -1;
  self->current_button_number = self->previous_button_number = -1;
  self->current_state = self->previous_state = 0;
}

/*< private >
 * clutter_input_device_set_coords:
 * @device: a #ClutterInputDevice
 * @x: X coordinate of the device
 * @y: Y coordinate of the device
 *
 * Stores the last known coordinates of the device
 */
void
_clutter_input_device_set_coords (ClutterInputDevice *device,
                                  gint                x,
                                  gint                y)
{
  g_return_if_fail (CLUTTER_IS_INPUT_DEVICE (device));

  if (device->current_x != x)
    device->current_x = x;

  if (device->current_y != y)
    device->current_y = y;
}

/*< private >
 * clutter_input_device_set_state:
 * @device: a #ClutterInputDevice
 * @state: a bitmask of modifiers
 *
 * Stores the last known modifiers state of the device
 */
void
_clutter_input_device_set_state (ClutterInputDevice  *device,
                                 ClutterModifierType  state)
{
  g_return_if_fail (CLUTTER_IS_INPUT_DEVICE (device));

  device->current_state = state;
}

/*< private >
 * clutter_input_device_set_time:
 * @device: a #ClutterInputDevice
 * @time_: the time
 *
 * Stores the last known event time of the device
 */
void
_clutter_input_device_set_time (ClutterInputDevice *device,
                                guint32             time_)
{
  g_return_if_fail (CLUTTER_IS_INPUT_DEVICE (device));

  if (device->current_time != time_)
    device->current_time = time_;
}

/* #ClutterInputDevice keeps a weak reference on the actor
 * under its pointer; this function unsets the reference on
 * the actor to avoid keeping around stale pointers
 */
static void
cursor_weak_unref (gpointer  user_data,
                   GObject  *object_pointer)
{
  ClutterInputDevice *device = user_data;

  device->cursor_actor = NULL;
}

/*< private >
 * clutter_input_device_set_stage:
 * @device: a #ClutterInputDevice
 * @stage: a #ClutterStage or %NULL
 *
 * Stores the stage under the device
 */
void
_clutter_input_device_set_stage (ClutterInputDevice *device,
                                 ClutterStage       *stage)
{
  if (device->stage == stage)
    return;

  device->stage = stage;

  /* we leave the ->cursor_actor in place in order to check
   * if we left the stage without crossing it again; this way
   * we can emit a leave event on the cursor actor right before
   * we emit the leave event on the stage.
   */
}

/*< private >
 * clutter_input_device_set_actor:
 * @device: a #ClutterInputDevice
 * @actor: a #ClutterActor
 * @emit_crossing: %TRUE to emit crossing events
 *
 * Sets the actor under the pointer coordinates of @device
 *
 * This function is called by _clutter_input_device_update()
 * and it will:
 *
 *   - queue a %CLUTTER_LEAVE event on the previous pointer actor
 *     of @device, if any
 *   - set to %FALSE the :has-pointer property of the previous
 *     pointer actor of @device, if any
 *   - queue a %CLUTTER_ENTER event on the new pointer actor
 *   - set to %TRUE the :has-pointer property of the new pointer
 *     actor
 */
void
_clutter_input_device_set_actor (ClutterInputDevice *device,
                                 ClutterActor       *actor,
                                 gboolean            emit_crossing)
{
  ClutterActor *old_actor;

  if (device->cursor_actor == actor)
    return;

  old_actor = device->cursor_actor;

  if (old_actor != NULL)
    {
      if (emit_crossing)
        {
          ClutterEvent *event;

          event = clutter_event_new (CLUTTER_LEAVE);
          event->crossing.time = device->current_time;
          event->crossing.flags = 0;
          event->crossing.stage = device->stage;
          event->crossing.source = device->cursor_actor;
          event->crossing.x = device->current_x;
          event->crossing.y = device->current_y;
          event->crossing.related = actor;
          clutter_event_set_device (event, device);

          /* we need to make sure that this event is processed
           * before any other event we might have queued up until
           * now, so we go on, and synthesize the event emission
           * ourselves
           */
          _clutter_process_event (event);

          clutter_event_free (event);
        }

      /* processing the event might have destroyed the actor */
      if (device->cursor_actor != NULL)
        {
          _clutter_actor_set_has_pointer (device->cursor_actor, FALSE);
          g_object_weak_unref (G_OBJECT (device->cursor_actor),
                               cursor_weak_unref,
                               device);

          device->cursor_actor = NULL;
        }
    }

  if (actor != NULL)
    {
      if (emit_crossing)
        {
          ClutterEvent *event;

          event = clutter_event_new (CLUTTER_ENTER);
          event->crossing.time = device->current_time;
          event->crossing.flags = 0;
          event->crossing.stage = device->stage;
          event->crossing.x = device->current_x;
          event->crossing.y = device->current_y;
          event->crossing.source = actor;
          event->crossing.related = old_actor;
          clutter_event_set_device (event, device);

          /* see above */
          _clutter_process_event (event);

          clutter_event_free (event);
        }
    }

  device->cursor_actor = actor;
  if (device->cursor_actor != NULL)
    {
      g_object_weak_ref (G_OBJECT (device->cursor_actor),
                         cursor_weak_unref,
                         device);
      _clutter_actor_set_has_pointer (device->cursor_actor, TRUE);
    }
}

/**
 * clutter_input_device_get_device_type:
 * @device: a #ClutterInputDevice
 *
 * Retrieves the type of @device
 *
 * Return value: the type of the device
 *
 * Since: 1.0
 */
ClutterInputDeviceType
clutter_input_device_get_device_type (ClutterInputDevice *device)
{
  g_return_val_if_fail (CLUTTER_IS_INPUT_DEVICE (device),
                        CLUTTER_POINTER_DEVICE);

  return device->device_type;
}

/**
 * clutter_input_device_get_device_id:
 * @device: a #ClutterInputDevice
 *
 * Retrieves the unique identifier of @device
 *
 * Return value: the identifier of the device
 *
 * Since: 1.0
 */
gint
clutter_input_device_get_device_id (ClutterInputDevice *device)
{
  g_return_val_if_fail (CLUTTER_IS_INPUT_DEVICE (device), -1);

  return device->id;
}

/**
 * clutter_input_device_set_enabled:
 * @device: a #ClutterInputDevice
 * @enabled: %TRUE to enable the @device
 *
 * Enables or disables a #ClutterInputDevice.
 *
 * Only devices with a #ClutterInputDevice:device-mode property set
 * to %CLUTTER_INPUT_MODE_SLAVE or %CLUTTER_INPUT_MODE_FLOATING can
 * be disabled.
 *
 * Since: 1.6
 */
void
clutter_input_device_set_enabled (ClutterInputDevice *device,
                                  gboolean            enabled)
{
  g_return_if_fail (CLUTTER_IS_INPUT_DEVICE (device));

  enabled = !!enabled;

  if (!enabled && device->device_mode == CLUTTER_INPUT_MODE_MASTER)
    return;

  if (device->is_enabled == enabled)
    return;

  device->is_enabled = enabled;

  g_object_notify_by_pspec (G_OBJECT (device), obj_props[PROP_ENABLED]);
}

/**
 * clutter_input_device_get_enabled:
 * @device: a #ClutterInputDevice
 *
 * Retrieves whether @device is enabled.
 *
 * Return value: %TRUE if the device is enabled
 *
 * Since: 1.6
 */
gboolean
clutter_input_device_get_enabled (ClutterInputDevice *device)
{
  g_return_val_if_fail (CLUTTER_IS_INPUT_DEVICE (device), FALSE);

  return device->is_enabled;
}

/**
 * clutter_input_device_get_device_coords:
 * @device: a #ClutterInputDevice of type %CLUTTER_POINTER_DEVICE
 * @x: (out): return location for the X coordinate
 * @y: (out): return location for the Y coordinate
 *
 * Retrieves the latest coordinates of the pointer of @device
 *
 * Since: 1.2
 */
void
clutter_input_device_get_device_coords (ClutterInputDevice *device,
                                        gint               *x,
                                        gint               *y)
{
  g_return_if_fail (CLUTTER_IS_INPUT_DEVICE (device));

  if (x)
    *x = device->current_x;

  if (y)
    *y = device->current_y;
}

/*
 * _clutter_input_device_update:
 * @device: a #ClutterInputDevice
 *
 * Updates the input @device by determining the #ClutterActor underneath the
 * pointer's cursor
 *
 * This function calls _clutter_input_device_set_actor() if needed.
 *
 * This function only works for #ClutterInputDevice of type
 * %CLUTTER_POINTER_DEVICE.
 *
 * Since: 1.2
 */
ClutterActor *
_clutter_input_device_update (ClutterInputDevice *device,
                              gboolean            emit_crossing)
{
  ClutterStage *stage;
  ClutterActor *new_cursor_actor;
  ClutterActor *old_cursor_actor;
  gint x, y;

  if (device->device_type == CLUTTER_KEYBOARD_DEVICE)
    return NULL;

  stage = device->stage;
  if (G_UNLIKELY (stage == NULL))
    {
      CLUTTER_NOTE (EVENT, "No stage defined for device '%s'",
                    clutter_input_device_get_device_name (device));
      return NULL;
    }

  clutter_input_device_get_device_coords (device, &x, &y);

  old_cursor_actor = device->cursor_actor;
  new_cursor_actor =
    _clutter_stage_do_pick (stage, x, y, CLUTTER_PICK_REACTIVE);

  /* if the pick could not find an actor then we do not update the
   * input device, to avoid ghost enter/leave events; the pick should
   * never fail, except for bugs in the glReadPixels() implementation
   * in which case this is the safest course of action anyway
   */
  if (new_cursor_actor == NULL)
    return NULL;

  CLUTTER_NOTE (EVENT,
                "Actor under cursor (device %d, at %d, %d): %s",
                clutter_input_device_get_device_id (device),
                x, y,
                clutter_actor_get_name (new_cursor_actor) != NULL
                  ? clutter_actor_get_name (new_cursor_actor)
                  : G_OBJECT_TYPE_NAME (new_cursor_actor));

  /* short-circuit here */
  if (new_cursor_actor == old_cursor_actor)
    return old_cursor_actor;

  _clutter_input_device_set_actor (device, new_cursor_actor, emit_crossing);

  return device->cursor_actor;
}

/**
 * clutter_input_device_get_pointer_actor:
 * @device: a #ClutterInputDevice of type %CLUTTER_POINTER_DEVICE
 *
 * Retrieves the #ClutterActor underneath the pointer of @device
 *
 * Return value: (transfer none): a pointer to the #ClutterActor or %NULL
 *
 * Since: 1.2
 */
ClutterActor *
clutter_input_device_get_pointer_actor (ClutterInputDevice *device)
{
  g_return_val_if_fail (CLUTTER_IS_INPUT_DEVICE (device), NULL);
  g_return_val_if_fail (device->device_type == CLUTTER_POINTER_DEVICE, NULL);

  return device->cursor_actor;
}

/**
 * clutter_input_device_get_pointer_stage:
 * @device: a #ClutterInputDevice of type %CLUTTER_POINTER_DEVICE
 *
 * Retrieves the #ClutterStage underneath the pointer of @device
 *
 * Return value: (transfer none): a pointer to the #ClutterStage or %NULL
 *
 * Since: 1.2
 */
ClutterStage *
clutter_input_device_get_pointer_stage (ClutterInputDevice *device)
{
  g_return_val_if_fail (CLUTTER_IS_INPUT_DEVICE (device), NULL);
  g_return_val_if_fail (device->device_type == CLUTTER_POINTER_DEVICE, NULL);

  return device->stage;
}

/**
 * clutter_input_device_get_device_name:
 * @device: a #ClutterInputDevice
 *
 * Retrieves the name of the @device
 *
 * Return value: the name of the device, or %NULL. The returned string
 *   is owned by the #ClutterInputDevice and should never be modified
 *   or freed
 *
 * Since: 1.2
 */
G_CONST_RETURN gchar *
clutter_input_device_get_device_name (ClutterInputDevice *device)
{
  g_return_val_if_fail (CLUTTER_IS_INPUT_DEVICE (device), NULL);

  return device->device_name;
}

/**
 * clutter_input_device_get_has_cursor:
 * @device: a #ClutterInputDevice
 *
 * Retrieves whether @device has a pointer that follows the
 * device motion.
 *
 * Return value: %TRUE if the device has a cursor
 *
 * Since: 1.6
 */
gboolean
clutter_input_device_get_has_cursor (ClutterInputDevice *device)
{
  g_return_val_if_fail (CLUTTER_IS_INPUT_DEVICE (device), FALSE);

  return device->has_cursor;
}

/**
 * clutter_input_device_get_device_mode:
 * @device: a #ClutterInputDevice
 *
 * Retrieves the #ClutterInputMode of @device.
 *
 * Return value: the device mode
 *
 * Since: 1.6
 */
ClutterInputMode
clutter_input_device_get_device_mode (ClutterInputDevice *device)
{
  g_return_val_if_fail (CLUTTER_IS_INPUT_DEVICE (device),
                        CLUTTER_INPUT_MODE_FLOATING);

  return device->device_mode;
}

/**
 * clutter_input_device_update_from_event:
 * @device: a #ClutterInputDevice
 * @event: a #ClutterEvent
 * @update_stage: whether to update the #ClutterStage of the @device
 *   using the stage of the event
 *
 * Forcibly updates the state of the @device using a #ClutterEvent
 *
 * This function should never be used by applications: it is meant
 * for integration with embedding toolkits, like clutter-gtk
 *
 * Embedding toolkits that disable the event collection inside Clutter
 * need to use this function to update the state of input devices depending
 * on a #ClutterEvent that they are going to submit to the event handling code
 * in Clutter though clutter_do_event(). Since the input devices hold the state
 * that is going to be used to fill in fields like the #ClutterButtonEvent
 * click count, or to emit synthesized events like %CLUTTER_ENTER and
 * %CLUTTER_LEAVE, it is necessary for embedding toolkits to also be
 * responsible of updating the input device state.
 *
 * For instance, this might be the code to translate an embedding toolkit
 * native motion notification into a Clutter #ClutterMotionEvent and ask
 * Clutter to process it:
 *
 * |[
 *   ClutterEvent c_event;
 *
 *   translate_native_event_to_clutter (native_event, &amp;c_event);
 *
 *   clutter_do_event (&amp;c_event);
 * ]|
 *
 * Before letting clutter_do_event() process the event, it is necessary to call
 * clutter_input_device_update_from_event():
 *
 * |[
 *   ClutterEvent c_event;
 *   ClutterDeviceManager *manager;
 *   ClutterInputDevice *device;
 *
 *   translate_native_event_to_clutter (native_event, &amp;c_event);
 *
 *   /&ast; get the device manager &ast;/
 *   manager = clutter_device_manager_get_default ();
 *
 *   /&ast; use the default Core Pointer that Clutter
 *    &ast; backends register by default
 *    &ast;/
 *   device = clutter_device_manager_get_core_device (manager, %CLUTTER_POINTER_DEVICE);
 *
 *   /&ast; update the state of the input device &ast;/
 *   clutter_input_device_update_from_event (device, &amp;c_event, FALSE);
 *
 *   clutter_do_event (&amp;c_event);
 * ]|
 *
 * The @update_stage boolean argument should be used when the input device
 * enters and leaves a #ClutterStage; it will use the #ClutterStage field
 * of the passed @event to update the stage associated to the input device.
 *
 * Since: 1.2
 */
void
clutter_input_device_update_from_event (ClutterInputDevice *device,
                                        ClutterEvent       *event,
                                        gboolean            update_stage)
{
  ClutterModifierType event_state;
  ClutterStage *event_stage;
  gfloat event_x, event_y;
  guint32 event_time;

  g_return_if_fail (CLUTTER_IS_INPUT_DEVICE (device));
  g_return_if_fail (event != NULL);

  event_state = clutter_event_get_state (event);
  event_time = clutter_event_get_time (event);
  event_stage = clutter_event_get_stage (event);
  clutter_event_get_coords (event, &event_x, &event_y);

  _clutter_input_device_set_coords (device, event_x, event_y);
  _clutter_input_device_set_state (device, event_state);
  _clutter_input_device_set_time (device, event_time);

  if (update_stage)
    _clutter_input_device_set_stage (device, event_stage);
}

/*< private >
 * clutter_input_device_reset_axes:
 * @device: a #ClutterInputDevice
 *
 * Resets the axes on @device
 */
void
_clutter_input_device_reset_axes (ClutterInputDevice *device)
{
  if (device->axes != NULL)
    {
      g_array_free (device->axes, TRUE);
      device->axes = NULL;

      g_object_notify_by_pspec (G_OBJECT (device), obj_props[PROP_N_AXES]);
    }
}

/*< private >
 * clutter_input_device_add_axis:
 * @device: a #ClutterInputDevice
 * @axis: the axis type
 * @minimum: the minimum axis value
 * @maximum: the maximum axis value
 * @resolution: the axis resolution
 *
 * Adds an axis of type @axis on @device.
 */
guint
_clutter_input_device_add_axis (ClutterInputDevice *device,
                                ClutterInputAxis    axis,
                                gdouble             minimum,
                                gdouble             maximum,
                                gdouble             resolution)
{
  ClutterAxisInfo info;
  guint pos;

  if (device->axes == NULL)
    device->axes = g_array_new (FALSE, TRUE, sizeof (ClutterAxisInfo));

  info.axis = axis;
  info.min_value = minimum;
  info.max_value = maximum;
  info.resolution = resolution;

  switch (axis)
    {
    case CLUTTER_INPUT_AXIS_X:
    case CLUTTER_INPUT_AXIS_Y:
      info.min_axis = 0;
      info.max_axis = 0;
      break;

    case CLUTTER_INPUT_AXIS_XTILT:
    case CLUTTER_INPUT_AXIS_YTILT:
      info.min_axis = -1;
      info.max_axis = 1;
      break;

    default:
      info.min_axis = 0;
      info.max_axis = 1;
      break;
    }

  device->axes = g_array_append_val (device->axes, info);
  pos = device->axes->len - 1;

  g_object_notify_by_pspec (G_OBJECT (device), obj_props[PROP_N_AXES]);

  return pos;
}

/*< private >
 * clutter_input_translate_axis:
 * @device: a #ClutterInputDevice
 * @index_: the index of the axis
 * @gint: the absolute value of the axis
 * @axis_value: (out): the translated value of the axis
 *
 * Performs a conversion from the absolute value of the axis
 * to a relative value.
 *
 * The axis at @index_ must not be %CLUTTER_INPUT_AXIS_X or
 * %CLUTTER_INPUT_AXIS_Y.
 *
 * Return value: %TRUE if the conversion was successful
 */
gboolean
_clutter_input_device_translate_axis (ClutterInputDevice *device,
                                      guint               index_,
                                      gdouble             value,
                                      gdouble            *axis_value)
{
  ClutterAxisInfo *info;
  gdouble width;
  gdouble real_value;

  if (device->axes == NULL || index_ >= device->axes->len)
    return FALSE;

  info = &g_array_index (device->axes, ClutterAxisInfo, index_);

  if (info->axis == CLUTTER_INPUT_AXIS_X ||
      info->axis == CLUTTER_INPUT_AXIS_Y)
    return FALSE;

  width = info->max_value - info->min_value;
  real_value = (info->max_axis * (value - info->min_value)
             + info->min_axis * (info->max_value - value))
             / width;

  if (axis_value)
    *axis_value = real_value;

  return TRUE;
}

/**
 * clutter_input_device_get_axis:
 * @device: a #ClutterInputDevice
 * @index_: the index of the axis
 *
 * Retrieves the type of axis on @device at the given index.
 *
 * Return value: the axis type
 *
 * Since: 1.6
 */
ClutterInputAxis
clutter_input_device_get_axis (ClutterInputDevice *device,
                               guint               index_)
{
  ClutterAxisInfo *info;

  g_return_val_if_fail (CLUTTER_IS_INPUT_DEVICE (device),
                        CLUTTER_INPUT_AXIS_IGNORE);

  if (device->axes == NULL)
    return CLUTTER_INPUT_AXIS_IGNORE;

  if (index_ >= device->axes->len)
    return CLUTTER_INPUT_AXIS_IGNORE;

  info = &g_array_index (device->axes, ClutterAxisInfo, index_);

  return info->axis;
}

/**
 * clutter_input_device_get_axis_value:
 * @device: a #ClutterInputDevice
 * @axes: (array): an array of axes values, typically
 *   coming from clutter_event_get_axes()
 * @axis: the axis to extract
 * @value: (out): return location for the axis value
 *
 * Extracts the value of the given @axis of a #ClutterInputDevice from
 * an array of axis values.
 *
 * An example of typical usage for this function is:
 *
 * |[
 *   ClutterInputDevice *device = clutter_event_get_device (event);
 *   gdouble *axes = clutter_event_get_axes (event, NULL);
 *   gdouble pressure_value = 0;
 *
 *   clutter_input_device_get_axis_value (device, axes,
 *                                        CLUTTER_INPUT_AXIS_PRESSURE,
 *                                        &amp;pressure_value);
 * ]|
 *
 * Return value: %TRUE if the value was set, and %FALSE otherwise
 *
 * Since: 1.6
 */
gboolean
clutter_input_device_get_axis_value (ClutterInputDevice *device,
                                     gdouble            *axes,
                                     ClutterInputAxis    axis,
                                     gdouble            *value)
{
  gint i;

  g_return_val_if_fail (CLUTTER_IS_INPUT_DEVICE (device), FALSE);
  g_return_val_if_fail (device->axes != NULL, FALSE);

  for (i = 0; i < device->axes->len; i++)
    {
      ClutterAxisInfo *info;

      info = &g_array_index (device->axes, ClutterAxisInfo, i);

      if (info->axis == axis)
        {
          if (value)
            *value = axes[i];

          return TRUE;
        }
    }

  return FALSE;
}

/**
 * clutter_input_device_get_n_axes:
 * @device: a #ClutterInputDevice
 *
 * Retrieves the number of axes available on @device.
 *
 * Return value: the number of axes on the device
 *
 * Since: 1.6
 */
guint
clutter_input_device_get_n_axes (ClutterInputDevice *device)
{
  g_return_val_if_fail (CLUTTER_IS_INPUT_DEVICE (device), 0);

  if (device->axes != NULL)
    return device->axes->len;

  return 0;
}

/*< private >
 * clutter_input_device_set_n_keys:
 * @device: a #ClutterInputDevice
 * @n_keys: the number of keys of the device
 *
 * Initializes the keys of @device.
 *
 * Call clutter_input_device_set_key() on each key to set the keyval
 * and modifiers.
 */
void
_clutter_input_device_set_n_keys (ClutterInputDevice *device,
                                  guint               n_keys)
{
  if (device->keys != NULL)
    g_array_free (device->keys, TRUE);

  device->n_keys = n_keys;
  device->keys = g_array_sized_new (FALSE, TRUE,
                                    sizeof (ClutterKeyInfo),
                                    n_keys);
}

/**
 * clutter_input_device_get_n_keys:
 * @device: a #ClutterInputDevice
 *
 * Retrieves the number of keys registered for @device.
 *
 * Return value: the number of registered keys
 *
 * Since: 1.6
 */
guint
clutter_input_device_get_n_keys (ClutterInputDevice *device)
{
  g_return_val_if_fail (CLUTTER_IS_INPUT_DEVICE (device), 0);

  return device->n_keys;
}

/**
 * clutter_input_device_set_key:
 * @device: a #ClutterInputDevice
 * @index_: the index of the key
 * @keyval: the keyval
 * @modifiers: a bitmask of modifiers
 *
 * Sets the keyval and modifiers at the given @index_ for @device.
 *
 * Clutter will use the keyval and modifiers set when filling out
 * an event coming from the same input device.
 *
 * Since: 1.6
 */
void
clutter_input_device_set_key (ClutterInputDevice  *device,
                              guint                index_,
                              guint                keyval,
                              ClutterModifierType  modifiers)
{
  ClutterKeyInfo *key_info;

  g_return_if_fail (CLUTTER_IS_INPUT_DEVICE (device));
  g_return_if_fail (index_ < device->n_keys);

  key_info = &g_array_index (device->keys, ClutterKeyInfo, index_);
  key_info->keyval = keyval;
  key_info->modifiers = modifiers;
}

/**
 * clutter_input_device_get_key:
 * @device: a #ClutterInputDevice
 * @index_: the index of the key
 * @keyval: (out): return location for the keyval at @index_
 * @modifiers: (out): return location for the modifiers at @index_
 *
 * Retrieves the key set using clutter_input_device_set_key()
 *
 * Return value: %TRUE if a key was set at the given index
 *
 * Since: 1.6
 */
gboolean
clutter_input_device_get_key (ClutterInputDevice  *device,
                              guint                index_,
                              guint               *keyval,
                              ClutterModifierType *modifiers)
{
  ClutterKeyInfo *key_info;

  g_return_val_if_fail (CLUTTER_IS_INPUT_DEVICE (device), FALSE);

  if (device->keys == NULL)
    return FALSE;

  if (index_ > device->keys->len)
    return FALSE;

  key_info = &g_array_index (device->keys, ClutterKeyInfo, index_);

  if (!key_info->keyval && !key_info->modifiers)
    return FALSE;

  if (keyval)
    *keyval = key_info->keyval;

  if (modifiers)
    *modifiers = key_info->modifiers;

  return TRUE;
}

/*< private >
 * clutter_input_device_add_slave:
 * @master: a #ClutterInputDevice
 * @slave: a #ClutterInputDevice
 *
 * Adds @slave to the list of slave devices of @master
 *
 * This function does not increase the reference count of either @master
 * or @slave.
 */
void
_clutter_input_device_add_slave (ClutterInputDevice *master,
                                 ClutterInputDevice *slave)
{
  if (g_list_find (master->slaves, slave) == NULL)
    master->slaves = g_list_prepend (master->slaves, slave);
}

/*< private >
 * clutter_input_device_remove_slave:
 * @master: a #ClutterInputDevice
 * @slave: a #ClutterInputDevice
 *
 * Removes @slave from the list of slave devices of @master.
 *
 * This function does not decrease the reference count of either @master
 * or @slave.
 */
void
_clutter_input_device_remove_slave (ClutterInputDevice *master,
                                    ClutterInputDevice *slave)
{
  if (g_list_find (master->slaves, slave) != NULL)
    master->slaves = g_list_remove (master->slaves, slave);
}

/**
 * clutter_input_device_get_slave_devices:
 * @device: a #ClutterInputDevice
 *
 * Retrieves the slave devices attached to @device.
 *
 * Return value: (transfer container) (element-type Clutter.InputDevice): a
 *   list of #ClutterInputDevice, or %NULL. The contents of the list are
 *   owned by the device. Use g_list_free() when done
 *
 * Since: 1.6
 */
GList *
clutter_input_device_get_slave_devices (ClutterInputDevice *device)
{
  g_return_val_if_fail (CLUTTER_IS_INPUT_DEVICE (device), NULL);

  return g_list_copy (device->slaves);
}

/*< internal >
 * clutter_input_device_set_associated_device:
 * @device: a #ClutterInputDevice
 * @associated: (allow-none): a #ClutterInputDevice, or %NULL
 *
 * Sets the associated device for @device.
 *
 * This function keeps a reference on the associated device.
 */
void
_clutter_input_device_set_associated_device (ClutterInputDevice *device,
                                             ClutterInputDevice *associated)
{
  if (device->associated == associated)
    return;

  if (device->associated != NULL)
    g_object_unref (device->associated);

  device->associated = associated;
  if (device->associated != NULL)
    g_object_ref (device->associated);

  CLUTTER_NOTE (MISC, "Associating device '%s' to device '%s'",
                clutter_input_device_get_device_name (device),
                device->associated != NULL
                  ? clutter_input_device_get_device_name (device->associated)
                  : "(none)");

  if (device->device_mode != CLUTTER_INPUT_MODE_MASTER)
    {
      if (device->associated != NULL)
        device->device_mode = CLUTTER_INPUT_MODE_SLAVE;
      else
        device->device_mode = CLUTTER_INPUT_MODE_FLOATING;

      g_object_notify_by_pspec (G_OBJECT (device), obj_props[PROP_DEVICE_MODE]);
    }
}

/**
 * clutter_input_device_get_associated_device:
 * @device: a #ClutterInputDevice
 *
 * Retrieves a pointer to the #ClutterInputDevice that has been
 * associated to @device.
 *
 * If the #ClutterInputDevice:device-mode property of @device is
 * set to %CLUTTER_INPUT_MODE_MASTER, this function will return
 * %NULL.
 *
 * Return value: (transfer none): a #ClutterInputDevice, or %NULL
 *
 * Since: 1.6
 */
ClutterInputDevice *
clutter_input_device_get_associated_device (ClutterInputDevice *device)
{
  g_return_val_if_fail (CLUTTER_IS_INPUT_DEVICE (device), NULL);

  return device->associated;
}

/*< internal >
 * clutter_input_device_select_stage_events:
 * @device: a #ClutterInputDevice
 * @stage: the #ClutterStage to select events on
 * @event_mask: platform-specific mask of events
 *
 * Selects input device events on @stage.
 *
 * The implementation of this function depends on the backend used.
 */
void
_clutter_input_device_select_stage_events (ClutterInputDevice *device,
                                           ClutterStage       *stage,
                                           gint                event_mask)
{
  ClutterInputDeviceClass *device_class;

  device_class = CLUTTER_INPUT_DEVICE_GET_CLASS (device);
  if (device_class->select_stage_events != NULL)
    device_class->select_stage_events (device, stage, event_mask);
}
