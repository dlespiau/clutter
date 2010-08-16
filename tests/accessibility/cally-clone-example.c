/* CALLY - The Clutter Accessibility Implementation Library
 *
 * Copyright (C) 2009 Igalia, S.L.
 *
 * Author: Alejandro Piñeiro Iglesias <apinheiro@igalia.com>
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
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <atk/atk.h>
#include <clutter/clutter.h>

#include "cally-examples-util.h"

#define WIDTH 800
#define HEIGHT 600
#define HEIGHT_STEP 100
#define NUM_ENTRIES 3

static void
make_ui (ClutterActor *stage)
{
  ClutterActor    *editable      = NULL;
  ClutterActor    *rectangle     = NULL;
  ClutterActor    *label         = NULL;
  ClutterColor     color_stage   = { 0x00, 0x00, 0x00, 0xff };
  ClutterColor     color_text    = { 0xff, 0x00, 0x00, 0xff };
  ClutterColor     color_sel     = { 0x00, 0xff, 0x00, 0x55 };
  ClutterColor     color_label   = { 0x00, 0xff, 0x55, 0xff };
  ClutterColor     color_rect    = { 0x00, 0xff, 0xff, 0x55 };
  ClutterGeometry  editable_geom = {150, 50, 100, 75};
  ClutterActor    *full_entry    = NULL;
  ClutterActor    *cloned_entry  = NULL;


  clutter_stage_set_color (CLUTTER_STAGE (stage), &color_stage);
  clutter_actor_set_size (stage, WIDTH, HEIGHT);

  label = clutter_text_new_full ("Sans Bold 32px",
                                 "Entry",
                                 &color_label);
  clutter_actor_set_position (label, 0, 50);

  /* editable */
  editable = clutter_text_new_full ("Sans Bold 32px",
                                    "ddd",
                                    &color_text);
  clutter_actor_set_position (editable, 150, 50);
  clutter_text_set_editable (CLUTTER_TEXT (editable), TRUE);
  clutter_text_set_selectable (CLUTTER_TEXT (editable), TRUE);
  clutter_text_set_selection_color (CLUTTER_TEXT (editable),
                                    &color_sel);
  clutter_actor_grab_key_focus (editable);
  clutter_actor_set_reactive (editable, TRUE);

  /* rectangle: to create a entry "feeling" */
  rectangle = clutter_rectangle_new_with_color (&color_rect);
  clutter_actor_set_geometry (rectangle, &editable_geom);

  full_entry = clutter_group_new ();
  clutter_actor_set_position (full_entry, 0, 50);
  clutter_actor_set_size (full_entry, 100, 75);
  clutter_group_add (CLUTTER_GROUP (full_entry), label);
  clutter_group_add (CLUTTER_GROUP (full_entry), editable);
  clutter_group_add (CLUTTER_GROUP (full_entry), rectangle);
  clutter_actor_show_all (full_entry);
  clutter_actor_set_scale (full_entry, 2, 1);
  clutter_group_add (CLUTTER_GROUP (stage), full_entry);

  /* Cloning! */
  cloned_entry = clutter_clone_new (full_entry);
  clutter_actor_set_position (cloned_entry, 50, 200);
  clutter_actor_set_scale (cloned_entry, 1, 2);
  clutter_actor_show_all (cloned_entry);
  clutter_actor_set_reactive (cloned_entry, TRUE);

  clutter_group_add (CLUTTER_GROUP (stage), cloned_entry);
}

int
main (int argc, char *argv[])
{
  ClutterActor *stage         = NULL;

  g_set_application_name ("Clone Example");

  clutter_init (&argc, &argv);

  cally_util_a11y_init (&argc, &argv);

  stage = clutter_stage_get_default ();
  make_ui (stage);

  clutter_actor_show_all (stage);

  clutter_main ();

  return 0;
}