/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Copyright (C) 2011 Intel Corporation.
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
 * Authors:
 *   Damien Lespiau <damien.lespiau@intel.com>
 */

#include <android_native_app_glue.h>
#include <android/input.h>

#include <cogl/cogl.h>
#include <glib-android/glib-android.h>

#include "clutter-main.h"
#include "clutter-marshal.h"
#include "clutter-private.h"

#include "clutter-android-application.h"

G_DEFINE_TYPE (ClutterAndroidApplication,
               clutter_android_application,
               G_TYPE_OBJECT)

#define ANDROID_APPLICATION_PRIVATE(o)                            \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o),                              \
                                CLUTTER_TYPE_ANDROID_APPLICATION, \
                                ClutterAndroidApplicationPrivate))

enum
{
  READY,

  LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0, };

struct _ClutterAndroidApplicationPrivate
{
  struct android_app* android_application;

  gint have_window : 1;
  GMainLoop *wait_for_window;
};

static gboolean
clutter_android_application_ready (ClutterAndroidApplication *application)
{
  ClutterAndroidApplicationPrivate *priv = application->priv;
  g_message ("ready!");

  cogl_android_set_native_window (priv->android_application->window);

  return TRUE;
}

static void
clutter_android_application_finalize (GObject *object)
{
  G_OBJECT_CLASS (clutter_android_application_parent_class)->finalize (object);
}

static void
clutter_android_application_class_init (ClutterAndroidApplicationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (ClutterAndroidApplicationPrivate));

  object_class->finalize = clutter_android_application_finalize;

  klass->ready = clutter_android_application_ready;

  signals[READY] =
    g_signal_new (I_("ready"),
                  G_TYPE_FROM_CLASS (object_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ClutterAndroidApplicationClass, ready),
                  NULL, NULL,
                  _clutter_marshal_BOOLEAN__VOID,
                  G_TYPE_BOOLEAN, 0);
}

static void
clutter_android_application_init (ClutterAndroidApplication *self)
{
  self->priv = ANDROID_APPLICATION_PRIVATE (self);
}

static ClutterAndroidApplication *
clutter_android_application_new (void)
{
  return g_object_new (CLUTTER_TYPE_ANDROID_APPLICATION, NULL);
}

/*
 * Process the next main command.
 */
static void
clutter_android_handle_cmd (struct android_app *app,
                            int32_t             cmd)
{
  ClutterAndroidApplication *application;
  ClutterAndroidApplicationPrivate *priv;

  application = CLUTTER_ANDROID_APPLICATION (app->userData);
  priv = application->priv;

  switch (cmd)
    {
    case APP_CMD_INIT_WINDOW:
      /* The window is being shown, get it ready */
      g_message ("command: INIT_WINDOW");
      if (app->window != NULL)
        {
          gboolean initialized;

          g_signal_emit (application, signals[READY], 0, &initialized);

          if (initialized)
            priv->have_window = TRUE;

          if (priv->wait_for_window)
            {
              g_message ("Waking up the waiting main loop");
              g_main_loop_quit (priv->wait_for_window);
            }
        }
      break;

    case APP_CMD_TERM_WINDOW:
      /* The window is being hidden or closed, clean it up */
      g_message ("command: TERM_WINDOW");
      if (priv->wait_for_window)
        g_main_loop_quit (priv->wait_for_window);
      else
        clutter_main_quit ();
      //test_fini (data);
      break;

    case APP_CMD_GAINED_FOCUS:
      g_message ("command: GAINED_FOCUS");
      break;

    case APP_CMD_LOST_FOCUS:
      /* When our app loses focus, we stop monitoring the accelerometer.
       * This is to avoid consuming battery while not being used. */
      g_message ("command: LOST_FOCUS");
      break;
    }
}

/**
 * Process the next input event
 */
static int32_t
clutter_android_handle_input (struct android_app *app,
                              AInputEvent        *a_event)
{
  ClutterEvent *event;
  ClutterButtonEvent *button_event;
  ClutterDeviceManager *manager;
  ClutterInputDevice *pointer_device;
  ClutterActor *stage;

  if (AInputEvent_getType (a_event) == AINPUT_EVENT_TYPE_MOTION)
    {
      int32_t action;

      g_message ("motion event: (%.02lf,%0.2lf)",
                 AMotionEvent_getX (a_event, 0),
                 AMotionEvent_getY (a_event, 0));

      action = AMotionEvent_getAction (a_event);

      if ((action & AMOTION_EVENT_ACTION_MASK) == AMOTION_EVENT_ACTION_DOWN)
        {
          g_message ("Press");
          event = clutter_event_new (CLUTTER_BUTTON_PRESS);
        }
      else if ((action & AMOTION_EVENT_ACTION_MASK) == AMOTION_EVENT_ACTION_UP)
        {
          g_message ("Release");
          event = clutter_event_new (CLUTTER_BUTTON_RELEASE);
        }
      else
        {
          g_warning ("meh? %x", action);
          return 0;
        }

      stage = clutter_stage_get_default ();
      manager = clutter_device_manager_get_default ();
      pointer_device =
        clutter_device_manager_get_core_device (manager,
                                                CLUTTER_POINTER_DEVICE);

      button_event = (ClutterButtonEvent *) event;
      button_event->stage = CLUTTER_STAGE (stage);
      button_event->device = pointer_device;
      button_event->button = 1;
      button_event->click_count = 1;
      button_event->x = AMotionEvent_getX (a_event, 0);
      button_event->y = AMotionEvent_getY (a_event, 0);

      clutter_do_event (event);
      clutter_event_free (event);

      return 1;
    }

  return 0;
}

/* XXX: We should be able to get rid of that */
static gboolean
check_ready (gpointer user_data)
{
  ClutterAndroidApplication *application = user_data;
  ClutterAndroidApplicationPrivate *priv = application->priv;

  if (priv->have_window && priv->wait_for_window)
    {
      g_main_loop_quit (priv->wait_for_window);
      return FALSE;
    }

  if (priv->have_window)
    return FALSE;

  return TRUE;
}

void
clutter_android_application_run (ClutterAndroidApplication *application)
{
  ClutterAndroidApplicationPrivate *priv = application->priv;;

  g_return_if_fail (CLUTTER_IS_ANDROID_APPLICATION (application));

  /* XXX: eeew. We wait to have a window to initialize Clutter and thus to
   * enter the clutter main loop */
  if (!priv->have_window)
    {
      g_message ("Waiting for the window");
      priv->wait_for_window = g_main_loop_new (NULL, FALSE);
      g_timeout_add (1000, check_ready, application);
      g_main_loop_run (priv->wait_for_window);
      g_main_loop_unref (priv->wait_for_window);
      priv->wait_for_window = NULL;
    }

  g_message ("entering main loop");
  clutter_main ();
}

AAssetManager *
clutter_android_application_get_asset_manager (ClutterAndroidApplication *application)
{
  g_return_val_if_fail (CLUTTER_IS_ANDROID_APPLICATION (application), NULL);

  return application->priv->android_application->activity->assetManager;
}


/*
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void
android_main (struct android_app* android_application)
{
  ClutterAndroidApplication *clutter_application;
  ClutterAndroidApplicationPrivate *priv;

  /* Make sure glue isn't stripped */
  app_dummy ();

  g_type_init ();
  g_android_init ();

  clutter_application = clutter_android_application_new ();
  priv = clutter_application->priv;

  android_application->userData = clutter_application;
  android_application->onAppCmd = clutter_android_handle_cmd;
  android_application->onInputEvent = clutter_android_handle_input;

  priv->android_application = android_application;

  clutter_android_main (clutter_application);
}
