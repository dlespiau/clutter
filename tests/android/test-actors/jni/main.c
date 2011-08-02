/*
 * Copyright (C) 2010 The Android Open Source Project
 * Copyright (C) 2011 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/*
 * This file is derived from the "native-activity" sample of the android NDK
 * r5b. The coding style has been adapted to the code style most commonly found
 * in glib/gobject based projects.
 */

#include <math.h>

#include <android_native_app_glue.h>
#include <android/asset_manager.h>
#include <android/input.h>

#include <glib.h>
#include <glib-android/glib-android.h>
#include <cogl/cogl.h>
#include <clutter/clutter.h>

#define NHANDS  6

extern unsigned char *stbi_load_from_memory (unsigned char const *buffer,
                                             int                  len,
                                             int                 *x,
                                             int                 *y,
                                             int                 *comp,
                                             int                  req_comp);

static gint n_hands = NHANDS;

typedef struct
{
  struct android_app* app;

  gint have_window : 1;
  GMainLoop *wait_for_window;

  ClutterActor **hand;
  ClutterActor  *bgtex;
  ClutterActor  *real_hand;
  ClutterActor  *group;
  ClutterActor  *stage;

  gint stage_width;
  gint stage_height;
  gfloat radius;

  ClutterBehaviour *scaler_1;
  ClutterBehaviour *scaler_2;
  ClutterTimeline *timeline;

} TestData;

ClutterActor *
clutter_texture_new_from_android_asset (TestData   *data,
                                        const char *path)
{
  const unsigned char *buffer, *pixels;
  int width, height, stb_pixel_format;
  GError *error = NULL;
  AAsset *asset;
  ClutterActor *actor;

  asset = AAssetManager_open (data->app->activity->assetManager,
                             path,
                             AASSET_MODE_BUFFER);

  if (asset == NULL)
    g_critical ("Could not open %s", path);

  buffer = AAsset_getBuffer (asset);
  pixels = stbi_load_from_memory (buffer,
                                  AAsset_getLength (asset),
                                  &width, &height,
                                  &stb_pixel_format,
                                  4 /* STBI_rgb_alpha */);

  if (pixels == NULL)
    g_critical ("Could not decompress image");

  actor = clutter_texture_new ();
  clutter_texture_set_from_rgb_data (CLUTTER_TEXTURE (actor),
                                     pixels,
                                     TRUE,
                                     width, height,
                                     width * 4,
                                     4,
                                     CLUTTER_TEXTURE_NONE,
                                     &error);
  AAsset_close (asset);

  if (error)
    g_critical ("Could not create texure: %s", error->message);


  return actor;
}

static void
on_group_destroy (ClutterActor *actor,
                  TestData     *data)
{
  data->group = NULL;
}

static void
on_hand_destroy (ClutterActor *actor,
                 TestData     *data)
{
  int i;

  for (i = 0; i < n_hands; i++)
    {
      if (data->hand[i] == actor)
        data->hand[i] = NULL;
    }
}

/* Timeline handler */
static void
frame_cb (ClutterTimeline *timeline,
	  gint             msecs,
	  gpointer         user_data)
{
  TestData *data = user_data;
  gint i;
  float rotation = clutter_timeline_get_progress (timeline) * 360.0f;

  /* Rotate everything clockwise about stage center*/
  if (data->group != NULL)
    clutter_actor_set_rotation (data->group,
                                CLUTTER_Z_AXIS,
                                rotation,
                                data->stage_width / 2,
                                data->stage_height / 2,
                                0);

  for (i = 0; i < n_hands; i++)
    {
      /* Rotate each hand around there centers - to get this we need
       * to take into account any scaling.
       */
      if (data->hand[i] != NULL)
        clutter_actor_set_rotation (data->hand[i],
                                    CLUTTER_Z_AXIS,
                                    -6.0f * rotation,
                                    0, 0, 0);
    }
}

static gdouble
my_sine_wave (ClutterAlpha *alpha,
              gpointer      dummy G_GNUC_UNUSED)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline (alpha);
  gdouble progress = clutter_timeline_get_progress (timeline);

  return sin (progress * G_PI);
}

static gboolean
on_button_press_event (ClutterActor *actor,
                       ClutterEvent *event,
                       TestData     *data)
{
  gfloat x, y;

  clutter_event_get_coords (event, &x, &y);

  g_debug ("*** button press event (button:%d) at %.2f, %.2f on %s ***\n",
           clutter_event_get_button (event),
           x, y,
           clutter_actor_get_name (actor));

  clutter_actor_hide (actor);

  return TRUE;
}
static gboolean
test_init (TestData* data)
{
  ClutterInitError init_error;
  ClutterAlpha *alpha;
  gint          i;
  ClutterActor *real_hand;

  cogl_android_set_native_window_EXP (data->app->window);
  init_error = clutter_init (NULL, NULL);
  if (init_error <= 0)
    {
      g_critical ("Could not initialize clutter");
      return FALSE;
    }

  data->stage = clutter_stage_get_default ();
  clutter_actor_set_name (data->stage, "Default Stage");
  clutter_stage_set_color (CLUTTER_STAGE (data->stage),
                           CLUTTER_COLOR_LightSkyBlue);

  /* Create a timeline to manage animation */
  data->timeline = clutter_timeline_new (6000);
  clutter_timeline_set_loop (data->timeline, TRUE);

  /* fire a callback for frame change */
  g_signal_connect (data->timeline, "new-frame", G_CALLBACK (frame_cb), data);

  /* Set up some behaviours to handle scaling  */
  alpha = clutter_alpha_new_with_func (data->timeline, my_sine_wave, NULL, NULL);

  data->scaler_1 = clutter_behaviour_scale_new (alpha, 0.5, 0.5, 1.0, 1.0);
  data->scaler_2 = clutter_behaviour_scale_new (alpha, 1.0, 1.0, 0.5, 0.5);


  real_hand = clutter_texture_new_from_android_asset (data, "redhand.png");

  /* create a new group to hold multiple actors in a group */
  data->group = clutter_group_new();
  clutter_actor_set_name (data->group, "Group");
  g_signal_connect (data->group, "destroy",
                    G_CALLBACK (on_group_destroy), data);

  data->hand = g_new (ClutterActor*, n_hands);

  data->stage_width = clutter_actor_get_width (data->stage);
  data->stage_height = clutter_actor_get_height (data->stage);
  data->radius = (data->stage_width + data->stage_height) / n_hands;

  for (i = 0; i < n_hands; i++)
    {
      gint x, y, w, h;

      if (i == 0)
        {
          data->hand[i] = real_hand;
          clutter_actor_set_name (data->hand[i], "Real Hand");
        }
      else
        {
          data->hand[i] = clutter_clone_new (real_hand);
          clutter_actor_set_name (data->hand[i], "Clone Hand");
        }

      clutter_actor_set_reactive (data->hand[i], TRUE);

      clutter_actor_set_size (data->hand[i], 200, 213);

      /* Place around a circle */
      w = clutter_actor_get_width (data->hand[i]);
      h = clutter_actor_get_height (data->hand[i]);

      x = data->stage_width / 2
	+ data->radius
	* cos (i * G_PI / (n_hands / 2))
	- w / 2;

      y = data->stage_height / 2
	+ data->radius
	* sin (i * G_PI / (n_hands / 2))
	- h / 2;

      clutter_actor_set_position (data->hand[i], x, y);

      clutter_actor_move_anchor_point_from_gravity (data->hand[i],
                                                    CLUTTER_GRAVITY_CENTER);

      /* Add to our group group */
      clutter_container_add_actor (CLUTTER_CONTAINER (data->group),
                                   data->hand[i]);

      g_signal_connect (data->hand[i], "button-press-event",
                        G_CALLBACK (on_button_press_event),
                        data);

      g_signal_connect (data->hand[i], "destroy",
                        G_CALLBACK (on_hand_destroy),
                        data);

      if (i % 2)
	clutter_behaviour_apply (data->scaler_1, data->hand[i]);
      else
	clutter_behaviour_apply (data->scaler_2, data->hand[i]);
    }

  /* Add the group to the stage */
  clutter_container_add_actor (CLUTTER_CONTAINER (data->stage), data->group);

  /* and start it */
  clutter_timeline_start (data->timeline);

  return TRUE;
}

static void
test_fini (TestData *data)
{
}

/**
 * Process the next main command.
 */
static void
test_handle_cmd (struct android_app* app,
                 int32_t             cmd)
{
  TestData *data = (TestData *) app->userData;

  switch (cmd)
    {
    case APP_CMD_INIT_WINDOW:
      /* The window is being shown, get it ready */
      g_message ("command: INIT_WINDOW");
      if (data->app->window != NULL)
        {
          gboolean initialized;

          initialized = test_init (data);

          if (initialized)
            data->have_window = TRUE;

          if (data->wait_for_window)
            {
              g_message ("Waking up the waiting main loop");
              g_main_loop_quit (data->wait_for_window);
            }
        }
      break;

    case APP_CMD_TERM_WINDOW:
      /* The window is being hidden or closed, clean it up */
      g_message ("command: TERM_WINDOW");
      if (data->wait_for_window)
        g_main_loop_quit (data->wait_for_window);
      else
        clutter_main_quit ();
      test_fini (data);
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
test_handle_input (struct android_app* app,
                   AInputEvent*        a_event)
{
  ClutterEvent *event;
  ClutterButtonEvent *button_event;
  ClutterDeviceManager *manager;
  ClutterInputDevice *pointer_device;

  manager = clutter_device_manager_get_default ();
  pointer_device =
    clutter_device_manager_get_core_device (manager,
                                            CLUTTER_POINTER_DEVICE);

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

      button_event = (ClutterButtonEvent *) event;
      button_event->x = AMotionEvent_getX (a_event, 0);
      button_event->y = AMotionEvent_getY (a_event, 0);
      button_event->button = 1;
      button_event->click_count = 1;

      clutter_event_set_device (event, pointer_device);

      clutter_event_put (event);

      return 1;
    }

  return 0;
}

static gboolean
check_ready (gpointer user_data)
{
  TestData *data = user_data;

  if (data->have_window && data->wait_for_window)
    {
      g_main_loop_quit (data->wait_for_window);
      return FALSE;
    }

  if (data->have_window)
    return FALSE;

  return TRUE;
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void
android_main (struct android_app* application)
{
  TestData data;

  /* Make sure glue isn't stripped */
  app_dummy ();

  g_setenv ("CLUTTER_DEBUG", "event", TRUE);
  g_android_init ();

  memset (&data, 0, sizeof (TestData));
  application->userData = &data;
  application->onAppCmd = test_handle_cmd;
  application->onInputEvent = test_handle_input;
  data.app = application;

  /* XXX: eeew. We wait to have a window to initialize Clutter and thus to
   * enter the clutter main loop */
  if (!data.have_window)
    {
      g_message ("Waiting for the window");
      data.wait_for_window = g_main_loop_new (NULL, FALSE);
      g_timeout_add (1000, check_ready, &data);
      g_main_loop_run (data.wait_for_window);
      g_main_loop_unref (data.wait_for_window);
      data.wait_for_window = NULL;
    }

  clutter_actor_show (data.stage);
  g_message ("entering main loop");
  clutter_main ();
}
