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
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *      Robert Bragg <robert@linux.intel.com>
 *      Emmanuele Bassi <ebassi@linux.intel.com>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <glib-object.h>

#include "clutter-actor-private.h"
#include "clutter-paint-volume-private.h"
#include "clutter-private.h"
#include "clutter-stage-private.h"

G_DEFINE_BOXED_TYPE (ClutterPaintVolume, clutter_paint_volume,
                     clutter_paint_volume_copy,
                     clutter_paint_volume_free);

/*<private>
 * _clutter_paint_volume_new:
 * @actor: a #ClutterActor
 *
 * Creates a new #ClutterPaintVolume for the given @actor.
 *
 * Return value: the newly allocated #ClutterPaintVolume. Use
 *   clutter_paint_volume_free() to free the resources it uses
 *
 * Since: 1.6
 */
ClutterPaintVolume *
_clutter_paint_volume_new (ClutterActor *actor)
{
  ClutterPaintVolume *pv;

  g_return_val_if_fail (actor != NULL, NULL);

  pv = g_slice_new (ClutterPaintVolume);

  pv->actor = actor;

  memset (pv->vertices, 0, 8 * sizeof (ClutterVertex));

  pv->is_static = FALSE;
  pv->is_empty = TRUE;
  pv->is_axis_aligned = TRUE;
  pv->is_complete = TRUE;
  pv->is_2d = TRUE;

  return pv;
}

/* Since paint volumes are used so heavily in a typical paint
 * traversal of a Clutter scene graph and since paint volumes often
 * have a very short life cycle that maps well to stack allocation we
 * allow initializing a static ClutterPaintVolume variable to avoid
 * hammering the slice allocator.
 *
 * We were seeing slice allocation take about 1% cumulative CPU time
 * for some very simple clutter tests which although it isn't a *lot*
 * this is an easy way to basically drop that to 0%.
 *
 * The PaintVolume will be internally marked as static and
 * clutter_paint_volume_free should still be used to "free" static
 * volumes. This allows us to potentially store dynamically allocated
 * data inside paint volumes in the future since we would be able to
 * free it during _paint_volume_free().
 */
void
_clutter_paint_volume_init_static (ClutterPaintVolume *pv,
                                   ClutterActor *actor)
{
  pv->actor = actor;

  memset (pv->vertices, 0, 8 * sizeof (ClutterVertex));

  pv->is_static = TRUE;
  pv->is_empty = TRUE;
  pv->is_axis_aligned = TRUE;
  pv->is_complete = TRUE;
  pv->is_2d = TRUE;
}

void
_clutter_paint_volume_copy_static (const ClutterPaintVolume *src_pv,
                                   ClutterPaintVolume       *dst_pv)
{

  g_return_if_fail (src_pv != NULL && dst_pv != NULL);

  memcpy (dst_pv, src_pv, sizeof (ClutterPaintVolume));
  dst_pv->is_static = TRUE;
}

/**
 * clutter_paint_volume_copy:
 * @pv: a #ClutterPaintVolume
 *
 * Copies @pv into a new #ClutterPaintVolume
 *
 * Return value: a newly allocated copy of a #ClutterPaintVolume
 *
 * Since: 1.6
 */
ClutterPaintVolume *
clutter_paint_volume_copy (const ClutterPaintVolume *pv)
{
  ClutterPaintVolume *copy;

  g_return_val_if_fail (pv != NULL, NULL);

  copy = g_slice_dup (ClutterPaintVolume, pv);
  copy->is_static = FALSE;

  return copy;
}

void
_clutter_paint_volume_set_from_volume (ClutterPaintVolume       *pv,
                                       const ClutterPaintVolume *src)
{
  memcpy (pv, src, sizeof (ClutterPaintVolume));
}

/**
 * clutter_paint_volume_free:
 * @pv: a #ClutterPaintVolume
 *
 * Frees the resources allocated by @pv
 *
 * Since: 1.6
 */
void
clutter_paint_volume_free (ClutterPaintVolume *pv)
{
  g_return_if_fail (pv != NULL);

  if (G_LIKELY (pv->is_static))
    return;

  g_slice_free (ClutterPaintVolume, pv);
}

/**
 * clutter_paint_volume_set_origin:
 * @pv: a #ClutterPaintVolume
 * @origin: a #ClutterVertex
 *
 * Sets the origin of the paint volume.
 *
 * The origin is defined as the X, Y and Z coordinates of the top-left
 * corner of an actor's paint volume, in actor coordinates.
 *
 * The default is origin is assumed at: (0, 0, 0)
 *
 * Since: 1.6
 */
void
clutter_paint_volume_set_origin (ClutterPaintVolume  *pv,
                                 const ClutterVertex *origin)
{
  static const int key_vertices[4] = { 0, 1, 3, 4 };
  float dx, dy, dz;
  int i;

  g_return_if_fail (pv != NULL);
  g_return_if_fail (pv->is_axis_aligned);

  dx = origin->x - pv->vertices[0].x;
  dy = origin->y - pv->vertices[0].y;
  dz = origin->z - pv->vertices[0].z;

  /* If we change the origin then all the key vertices of the paint
   * volume need to be shifted too... */
  for (i = 0; i < 4; i++)
    {
      pv->vertices[key_vertices[i]].x += dx;
      pv->vertices[key_vertices[i]].y += dy;
      pv->vertices[key_vertices[i]].z += dz;
    }

  pv->is_complete = FALSE;
}

/**
 * clutter_paint_volume_get_origin:
 * @pv: a #ClutterPaintVolume
 * @vertex: (out): the return location for a #ClutterVertex
 *
 * Retrieves the origin of the #ClutterPaintVolume.
 *
 * Since: 1.6
 */
void
clutter_paint_volume_get_origin (const ClutterPaintVolume *pv,
                                 ClutterVertex            *vertex)
{
  g_return_if_fail (pv != NULL);
  g_return_if_fail (vertex != NULL);

  *vertex = pv->vertices[0];
}

static void
_clutter_paint_volume_update_is_empty (ClutterPaintVolume *pv)
{
  if (pv->vertices[0].x == pv->vertices[1].x &&
      pv->vertices[0].y == pv->vertices[3].y &&
      pv->vertices[0].z == pv->vertices[4].z)
    pv->is_empty = TRUE;
  else
    pv->is_empty = FALSE;
}

/**
 * clutter_paint_volume_set_width:
 * @pv: a #ClutterPaintVolume
 * @width: the width of the paint volume, in pixels
 *
 * Sets the width of the paint volume.
 *
 * Since: 1.6
 */
void
clutter_paint_volume_set_width (ClutterPaintVolume *pv,
                                gfloat              width)
{
  gfloat right_xpos;

  g_return_if_fail (pv != NULL);
  g_return_if_fail (pv->is_axis_aligned);
  g_return_if_fail (width >= 0.0f);

  /* If the volume is currently empty then only the origin is
   * currently valid */
  if (pv->is_empty)
    pv->vertices[1] = pv->vertices[3] = pv->vertices[4] = pv->vertices[0];

  right_xpos = pv->vertices[0].x + width;

  /* Move the right vertices of the paint box relative to the
   * origin... */
  pv->vertices[1].x = right_xpos;
  /* pv->vertices[2].x = right_xpos; NB: updated lazily */
  /* pv->vertices[5].x = right_xpos; NB: updated lazily */
  /* pv->vertices[6].x = right_xpos; NB: updated lazily */

  pv->is_complete = FALSE;

  _clutter_paint_volume_update_is_empty (pv);
}

/**
 * clutter_paint_volume_get_width:
 * @pv: a #ClutterPaintVolume
 *
 * Retrieves the width set using clutter_paint_volume_get_width()
 *
 * Return value: the width, in pixels
 *
 * Since: 1.6
 */
gfloat
clutter_paint_volume_get_width (const ClutterPaintVolume *pv)
{
  g_return_val_if_fail (pv != NULL, 0.0);
  g_return_val_if_fail (pv->is_axis_aligned, 0);

  if (pv->is_empty)
    return 0;
  else
    return pv->vertices[1].x - pv->vertices[0].x;
}

/**
 * clutter_paint_volume_set_height:
 * @pv: a #ClutterPaintVolume
 * @height: the height of the paint volume, in pixels
 *
 * Sets the height of the paint volume.
 *
 * Since: 1.6
 */
void
clutter_paint_volume_set_height (ClutterPaintVolume *pv,
                                 gfloat              height)
{
  gfloat height_ypos;

  g_return_if_fail (pv != NULL);
  g_return_if_fail (pv->is_axis_aligned);
  g_return_if_fail (height >= 0.0f);

  /* If the volume is currently empty then only the origin is
   * currently valid */
  if (pv->is_empty)
    pv->vertices[1] = pv->vertices[3] = pv->vertices[4] = pv->vertices[0];

  height_ypos = pv->vertices[0].y + height;

  /* Move the bottom vertices of the paint box relative to the
   * origin... */
  /* pv->vertices[2].y = height_ypos; NB: updated lazily */
  pv->vertices[3].y = height_ypos;
  /* pv->vertices[6].y = height_ypos; NB: updated lazily */
  /* pv->vertices[7].y = height_ypos; NB: updated lazily */
  pv->is_complete = FALSE;

  _clutter_paint_volume_update_is_empty (pv);
}

/**
 * clutter_paint_volume_get_height:
 * @pv: a #ClutterPaintVolume
 *
 * Retrieves the height of the paint volume set using
 * clutter_paint_volume_get_height()
 *
 * Return value: the height of the paint volume, in pixels
 *
 * Since: 1.6
 */
gfloat
clutter_paint_volume_get_height (const ClutterPaintVolume *pv)
{
  g_return_val_if_fail (pv != NULL, 0.0);
  g_return_val_if_fail (pv->is_axis_aligned, 0);

  if (pv->is_empty)
    return 0;
  else
    return pv->vertices[3].y - pv->vertices[0].y;
}

/**
 * clutter_paint_volume_set_depth:
 * @pv: a #ClutterPaintVolume
 * @depth: the depth of the paint volume, in pixels
 *
 * Sets the depth of the paint volume.
 *
 * Since: 1.6
 */
void
clutter_paint_volume_set_depth (ClutterPaintVolume *pv,
                                gfloat              depth)
{
  gfloat depth_zpos;

  g_return_if_fail (pv != NULL);
  g_return_if_fail (pv->is_axis_aligned);
  g_return_if_fail (depth >= 0.0f);

  /* If the volume is currently empty then only the origin is
   * currently valid */
  if (pv->is_empty)
    pv->vertices[1] = pv->vertices[3] = pv->vertices[4] = pv->vertices[0];

  depth_zpos = pv->vertices[0].z + depth;

  /* Move the back vertices of the paint box relative to the
   * origin... */
  pv->vertices[4].z = depth_zpos;
  /* pv->vertices[5].z = depth_zpos; NB: updated lazily */
  /* pv->vertices[6].z = depth_zpos; NB: updated lazily */
  /* pv->vertices[7].z = depth_zpos; NB: updated lazily */

  pv->is_complete = FALSE;
  pv->is_2d = depth ? FALSE : TRUE;
  _clutter_paint_volume_update_is_empty (pv);
}

/**
 * clutter_paint_volume_get_depth:
 * @pv: a #ClutterPaintVolume
 *
 * Retrieves the depth of the paint volume set using
 * clutter_paint_volume_get_depth()
 *
 * Return value: the depth
 *
 * Since: 1.6
 */
gfloat
clutter_paint_volume_get_depth (const ClutterPaintVolume *pv)
{
  g_return_val_if_fail (pv != NULL, 0.0);
  g_return_val_if_fail (pv->is_axis_aligned, 0);

  if (pv->is_empty)
    return 0;
  else
    return pv->vertices[4].z - pv->vertices[0].z;
}

/**
 * clutter_paint_volume_union:
 * @pv: The first #ClutterPaintVolume and destination for resulting
 *      union
 * @another_pv: A second #ClutterPaintVolume to union with @pv
 *
 * Updates the geometry of @pv to be the union bounding box that
 * encompases @pv and @another_pv.
 *
 * Since: 1.6
 */
void
clutter_paint_volume_union (ClutterPaintVolume *pv,
                            const ClutterPaintVolume *another_pv)
{
  ClutterPaintVolume aligned_pv;
  static const int key_vertices[4] = { 0, 1, 3, 4 };

  g_return_if_fail (pv != NULL);
  g_return_if_fail (pv->is_axis_aligned);
  g_return_if_fail (another_pv != NULL);

  /* NB: we only have to update vertices 0, 1, 3 and 4
   * (See the ClutterPaintVolume typedef for more details) */

  /* We special case empty volumes because otherwise we'd end up
   * calculating a bounding box that would enclose the origin of
   * the empty volume which isn't desired.
   */
  if (another_pv->is_empty)
    return;

  if (pv->is_empty)
    {
      int i;
      for (i = 0; i < 4; i++)
        pv->vertices[key_vertices[i]] = another_pv->vertices[key_vertices[i]];
      pv->is_2d = another_pv->is_2d;
      goto done;
    }

  if (!another_pv->is_axis_aligned)
    {
      _clutter_paint_volume_copy_static (another_pv, &aligned_pv);
      _clutter_paint_volume_axis_align (&aligned_pv);
      another_pv = &aligned_pv;
    }

  /* grow left*/
  /* left vertices 0, 3, 4, 7 */
  if (another_pv->vertices[0].x < pv->vertices[0].x)
    {
      int min_x = another_pv->vertices[0].x;
      pv->vertices[0].x = min_x;
      pv->vertices[3].x = min_x;
      pv->vertices[4].x = min_x;
      /* pv->vertices[7].x = min_x; */
    }

  /* grow right */
  /* right vertices 1, 2, 5, 6 */
  if (another_pv->vertices[1].x > pv->vertices[1].x)
    {
      int max_x = another_pv->vertices[1].x;
      pv->vertices[1].x = max_x;
      /* pv->vertices[2].x = max_x; */
      /* pv->vertices[5].x = max_x; */
      /* pv->vertices[6].x = max_x; */
    }

  /* grow up */
  /* top vertices 0, 1, 4, 5 */
  if (another_pv->vertices[0].y < pv->vertices[0].y)
    {
      int min_y = another_pv->vertices[0].y;
      pv->vertices[0].y = min_y;
      pv->vertices[1].y = min_y;
      pv->vertices[4].y = min_y;
      /* pv->vertices[5].y = min_y; */
    }

  /* grow down */
  /* bottom vertices 2, 3, 6, 7 */
  if (another_pv->vertices[3].y > pv->vertices[3].y)
    {
      int may_y = another_pv->vertices[3].y;
      /* pv->vertices[2].y = may_y; */
      pv->vertices[3].y = may_y;
      /* pv->vertices[6].y = may_y; */
      /* pv->vertices[7].y = may_y; */
    }

  /* grow forward */
  /* front vertices 0, 1, 2, 3 */
  if (another_pv->vertices[0].z < pv->vertices[0].z)
    {
      int min_z = another_pv->vertices[0].z;
      pv->vertices[0].z = min_z;
      pv->vertices[1].z = min_z;
      /* pv->vertices[2].z = min_z; */
      pv->vertices[3].z = min_z;
    }

  /* grow backward */
  /* back vertices 4, 5, 6, 7 */
  if (another_pv->vertices[4].z > pv->vertices[4].z)
    {
      int maz_z = another_pv->vertices[4].z;
      pv->vertices[4].z = maz_z;
      /* pv->vertices[5].z = maz_z; */
      /* pv->vertices[6].z = maz_z; */
      /* pv->vertices[7].z = maz_z; */
    }

  if (pv->vertices[4].z == pv->vertices[0].z)
    pv->is_2d = TRUE;
  else
    pv->is_2d = FALSE;

done:
  pv->is_empty = FALSE;
  pv->is_complete = FALSE;
}

/* The paint_volume setters only update vertices 0, 1, 3 and
 * 4 since the others can be drived from them.
 *
 * This will set pv->completed = TRUE;
 */
void
_clutter_paint_volume_complete (ClutterPaintVolume *pv)
{
  if (pv->is_complete || pv->is_empty)
    return;

  /* TODO: it is possible to complete non axis aligned volumes too. */
  g_return_if_fail (pv->is_axis_aligned);

  /* front-bottom-right */
  pv->vertices[2].x = pv->vertices[1].x;
  pv->vertices[2].y = pv->vertices[3].y;
  pv->vertices[2].z = pv->vertices[0].z;

  if (G_UNLIKELY (!pv->is_2d))
    {
      /* back-top-right */
      pv->vertices[5].x = pv->vertices[1].x;
      pv->vertices[5].y = pv->vertices[0].y;
      pv->vertices[5].z = pv->vertices[4].z;

      /* back-bottom-right */
      pv->vertices[6].x = pv->vertices[1].x;
      pv->vertices[6].y = pv->vertices[3].y;
      pv->vertices[6].z = pv->vertices[4].z;

      /* back-bottom-left */
      pv->vertices[7].x = pv->vertices[0].x;
      pv->vertices[7].y = pv->vertices[3].y;
      pv->vertices[7].z = pv->vertices[4].z;
    }

  pv->is_complete = TRUE;
}

/*<private>
 * _clutter_paint_volume_get_box:
 * @pv: a #ClutterPaintVolume
 * @box: a pixel aligned #ClutterGeometry
 *
 * Transforms a 3D paint volume into a 2D bounding box in the
 * same coordinate space as the 3D paint volume.
 *
 * To get an actors "paint box" you should first project
 * the paint volume into window coordinates before getting
 * the 2D bounding box.
 *
 * <note>The coordinates of the returned box are not clamped to
 * integer pixel values, if you need them to be clamped you can use
 * clutter_actor_box_clamp_to_pixel()</note>
 *
 * Since: 1.6
 */
void
_clutter_paint_volume_get_bounding_box (ClutterPaintVolume *pv,
                                        ClutterActorBox *box)
{
  gfloat x_min, y_min, x_max, y_max;
  ClutterVertex *vertices;
  int count;
  gint i;

  g_return_if_fail (pv != NULL);
  g_return_if_fail (box != NULL);

  if (pv->is_empty)
    {
      box->x1 = box->x2 = pv->vertices[0].x;
      box->y1 = box->y2 = pv->vertices[0].y;
      return;
    }

  /* Updates the vertices we calculate lazily
   * (See ClutterPaintVolume typedef for more details) */
  _clutter_paint_volume_complete (pv);

  vertices = pv->vertices;

  x_min = x_max = vertices[0].x;
  y_min = y_max = vertices[0].y;

  /* Most actors are 2D so we only have to look at the front 4
   * vertices of the paint volume... */
  if (G_LIKELY (pv->is_2d))
    count = 4;
  else
    count = 8;

  for (i = 1; i < count; i++)
    {
      if (vertices[i].x < x_min)
        x_min = vertices[i].x;
      else if (vertices[i].x > x_max)
        x_max = vertices[i].x;

      if (vertices[i].y < y_min)
        y_min = vertices[i].y;
      else if (vertices[i].y > y_max)
        y_max = vertices[i].y;
    }

  box->x1 = x_min;
  box->y1 = y_min;
  box->x2 = x_max;
  box->y2 = y_max;
}

void
_clutter_paint_volume_project (ClutterPaintVolume *pv,
                               const CoglMatrix *modelview,
                               const CoglMatrix *projection,
                               const float *viewport)
{
  int transform_count;

  if (pv->is_empty)
    {
      /* Just transform the origin... */
      _clutter_util_fully_transform_vertices (modelview,
                                              projection,
                                              viewport,
                                              pv->vertices,
                                              pv->vertices,
                                              1);
      return;
    }

  /* All the vertices must be up to date, since after the projection
   * it wont be trivial to derive the other vertices. */
  _clutter_paint_volume_complete (pv);

  /* Most actors are 2D so we only have to transform the front 4
   * vertices of the paint volume... */
  if (G_LIKELY (pv->is_2d))
    transform_count = 4;
  else
    transform_count = 8;

  _clutter_util_fully_transform_vertices (modelview,
                                          projection,
                                          viewport,
                                          pv->vertices,
                                          pv->vertices,
                                          transform_count);

  pv->is_axis_aligned = FALSE;
}

void
_clutter_paint_volume_transform (ClutterPaintVolume *pv,
                                 const CoglMatrix *matrix)
{
  int transform_count;

  if (pv->is_empty)
    {
      gfloat w = 1;
      /* Just transform the origin */
      cogl_matrix_transform_point (matrix,
                                   &pv->vertices[0].x,
                                   &pv->vertices[0].y,
                                   &pv->vertices[0].z,
                                   &w);
      return;
    }

  /* All the vertices must be up to date, since after the transform
   * it wont be trivial to derive the other vertices. */
  _clutter_paint_volume_complete (pv);

  /* Most actors are 2D so we only have to transform the front 4
   * vertices of the paint volume... */
  if (G_LIKELY (pv->is_2d))
    transform_count = 4;
  else
    transform_count = 8;

  cogl_matrix_transform_points (matrix,
                                3,
                                sizeof (ClutterVertex),
                                pv->vertices,
                                sizeof (ClutterVertex),
                                pv->vertices,
                                transform_count);

  pv->is_axis_aligned = FALSE;
}


/* Given a paint volume that has been transformed by an arbitrary
 * modelview and is no longer axis aligned, this derives a replacement
 * that is axis aligned. */
void
_clutter_paint_volume_axis_align (ClutterPaintVolume *pv)
{
  int count;
  int i;
  ClutterVertex origin;
  float max_x;
  float max_y;
  float max_z;

  g_return_if_fail (pv != NULL);

  if (pv->is_empty)
    return;

  g_return_if_fail (pv->is_complete);

  if (G_LIKELY (pv->is_axis_aligned))
    return;

  if (G_LIKELY (pv->vertices[0].x == pv->vertices[1].x &&
                pv->vertices[0].y == pv->vertices[3].y &&
                pv->vertices[0].z == pv->vertices[4].y))
    {
      pv->is_axis_aligned = TRUE;
      return;
    }

  origin = pv->vertices[0];
  max_x = pv->vertices[0].x;
  max_y = pv->vertices[0].y;
  max_z = pv->vertices[0].z;

  count = pv->is_2d ? 4 : 8;
  for (i = 1; i < count; i++)
    {
      if (pv->vertices[i].x < origin.x)
        origin.x = pv->vertices[i].x;
      else if (pv->vertices[i].x > max_x)
        max_x = pv->vertices[i].x;

      if (pv->vertices[i].y < origin.y)
        origin.y = pv->vertices[i].y;
      else if (pv->vertices[i].y > max_y)
        max_y = pv->vertices[i].y;

      if (pv->vertices[i].z < origin.z)
        origin.z = pv->vertices[i].z;
      else if (pv->vertices[i].z > max_z)
        max_z = pv->vertices[i].z;
    }

  pv->vertices[0] = origin;

  pv->vertices[1].x = max_x;
  pv->vertices[1].y = origin.y;
  pv->vertices[1].z = origin.z;

  pv->vertices[3].x = origin.x;
  pv->vertices[3].y = max_y;
  pv->vertices[3].z = origin.z;

  pv->vertices[4].x = origin.x;
  pv->vertices[4].y = origin.y;
  pv->vertices[4].z = max_z;

  pv->is_complete = FALSE;
  pv->is_axis_aligned = TRUE;

  if (pv->vertices[4].z == pv->vertices[0].z)
    pv->is_2d = TRUE;
  else
    pv->is_2d = FALSE;
}

/*<private>
 * _clutter_actor_set_default_paint_volume:
 * @self: a #ClutterActor
 * @check_gtype: if not %G_TYPE_INVALID, match the type of @self against
 *   this type
 * @volume: the #ClutterPaintVolume to set
 *
 * Sets the default paint volume for @self.
 *
 * This function should be called by #ClutterActor sub-classes that follow
 * the default assumption that their paint volume is defined by their
 * allocation.
 *
 * If @check_gtype is not %G_TYPE_INVALID, this function will check the
 * type of @self and only compute the paint volume if the type matches;
 * this can be used to avoid computing the paint volume for sub-classes
 * of an actor class
 *
 * Return value: %TRUE if the paint volume was set, and %FALSE otherwise
 */
gboolean
_clutter_actor_set_default_paint_volume (ClutterActor       *self,
                                         GType               check_gtype,
                                         ClutterPaintVolume *volume)
{
  ClutterGeometry geometry = { 0, };

  if (check_gtype != G_TYPE_INVALID)
    {
      if (G_OBJECT_TYPE (self) != check_gtype)
        return FALSE;
    }

  /* calling clutter_actor_get_allocation_* can potentially be very
   * expensive, as it can result in a synchronous full stage relayout
   * and redraw
   */
  if (!clutter_actor_has_allocation (self))
    return FALSE;

  clutter_actor_get_allocation_geometry (self, &geometry);

  /* a zero-sized actor has no paint volume */
  if (geometry.width == 0 || geometry.height == 0)
    return FALSE;

  clutter_paint_volume_set_width (volume, geometry.width);
  clutter_paint_volume_set_height (volume, geometry.height);

  return TRUE;
}

/**
 * clutter_paint_volume_set_from_allocation:
 * @pv: a #ClutterPaintVolume
 * @actor: a #ClutterActor
 *
 * Sets the #ClutterPaintVolume from the allocation of @actor.
 *
 * This function should be used when overriding the
 * <function>get_paint_volume()</function> by #ClutterActor sub-classes that do
 * not paint outside their allocation.
 *
 * A typical example is:
 *
 * |[
 * static gboolean
 * my_actor_get_paint_volume (ClutterActor       *self,
 *                            ClutterPaintVolume *volume)
 * {
 *   return clutter_paint_volume_set_from_allocation (volume, self);
 * }
 * ]|
 *
 * Return value: %TRUE if the paint volume was successfully set, and %FALSE
 *   otherwise
 *
 * Since: 1.6
 */
gboolean
clutter_paint_volume_set_from_allocation (ClutterPaintVolume *pv,
                                          ClutterActor       *actor)
{
  g_return_val_if_fail (pv != NULL, FALSE);
  g_return_val_if_fail (CLUTTER_IS_ACTOR (actor), FALSE);

  return _clutter_actor_set_default_paint_volume (actor, G_TYPE_INVALID, pv);
}

/* Currently paint volumes are defined relative to a given actor, but
 * in some cases it is desireable to be able to change the actor that
 * a volume relates too (For instance for ClutterClone actors where we
 * need to masquarade the source actors volume as the volume for the
 * clone). */
void
_clutter_paint_volume_set_reference_actor (ClutterPaintVolume *pv,
                                           ClutterActor       *actor)
{
  g_return_if_fail (pv != NULL);

  pv->actor = actor;
}

ClutterCullResult
_clutter_paint_volume_cull (ClutterPaintVolume *pv,
                            const ClutterPlane *planes)
{
  int vertex_count;
  ClutterVertex *vertices = pv->vertices;
  gboolean partial = FALSE;
  int i;
  int j;

  if (pv->is_empty)
    return CLUTTER_CULL_RESULT_OUT;

  /* We expect the volume to already be transformed into eye coordinates
   */
  g_return_val_if_fail (pv->is_complete == TRUE, CLUTTER_CULL_RESULT_IN);
  g_return_val_if_fail (pv->actor == NULL, CLUTTER_CULL_RESULT_IN);

  /* Most actors are 2D so we only have to transform the front 4
   * vertices of the paint volume... */
  if (G_LIKELY (pv->is_2d))
    vertex_count = 4;
  else
    vertex_count = 8;

  for (i = 0; i < 4; i++)
    {
      int out = 0;
      for (j = 0; j < vertex_count; j++)
        {
          ClutterVertex p;
          float distance;

          /* XXX: for perspective projections this can be optimized
           * out because all the planes should pass through the origin
           * so (0,0,0) is a valid v0. */
          p.x = vertices[j].x - planes[i].v0.x;
          p.y = vertices[j].y - planes[i].v0.y;
          p.z = vertices[j].z - planes[i].v0.z;

          distance =
            planes[i].n.x * p.x + planes[i].n.y * p.y + planes[i].n.z * p.z;

          if (distance < 0)
            out++;
        }

      if (out == 4)
        return CLUTTER_CULL_RESULT_OUT;
      else if (out != 0)
        partial = TRUE;
    }

  if (partial)
    return CLUTTER_CULL_RESULT_PARTIAL;
  else
    return CLUTTER_CULL_RESULT_IN;
}

void
_clutter_paint_volume_get_stage_paint_box (ClutterPaintVolume *pv,
                                           ClutterStage *stage,
                                           ClutterActorBox *box)
{
  ClutterPaintVolume projected_pv;
  CoglMatrix modelview;
  CoglMatrix projection;
  float viewport[4];

  _clutter_paint_volume_copy_static (pv, &projected_pv);

  /* NB: _clutter_actor_apply_modelview_transform_recursive will never
   * include the transformation between stage coordinates and OpenGL
   * eye coordinates, we have to explicitly use the
   * stage->apply_transform to get that... */
  cogl_matrix_init_identity (&modelview);

  /* If the paint volume isn't already in eye coordinates... */
  if (pv->actor)
    {
      ClutterActor *stage_actor = CLUTTER_ACTOR (stage);
      _clutter_actor_apply_modelview_transform (stage_actor, &modelview);
      _clutter_actor_apply_modelview_transform_recursive (pv->actor,
                                                          stage_actor,
                                                          &modelview);
    }

  _clutter_stage_get_projection_matrix (stage, &projection);
  _clutter_stage_get_viewport (stage,
                               &viewport[0],
                               &viewport[1],
                               &viewport[2],
                               &viewport[3]);

  _clutter_paint_volume_project (&projected_pv,
                                 &modelview,
                                 &projection,
                                 viewport);

  _clutter_paint_volume_get_bounding_box (&projected_pv, box);
  clutter_actor_box_clamp_to_pixel (box);

  clutter_paint_volume_free (&projected_pv);
}

void
_clutter_paint_volume_transform_relative (ClutterPaintVolume *pv,
                                          ClutterActor *relative_to_ancestor)
{
  CoglMatrix matrix;
  ClutterActor *actor;

  actor = pv->actor;

  g_return_if_fail (actor != NULL);

  _clutter_paint_volume_set_reference_actor (pv, relative_to_ancestor);

  cogl_matrix_init_identity (&matrix);

  if (relative_to_ancestor == NULL)
    {
      /* NB: _clutter_actor_apply_modelview_transform_recursive will never
       * include the transformation between stage coordinates and OpenGL
       * eye coordinates, we have to explicitly use the
       * stage->apply_transform to get that... */
      ClutterActor *stage = _clutter_actor_get_stage_internal (actor);

      /* We really can't do anything meaningful in this case so don't try
       * to do any transform */
      if (G_UNLIKELY (stage == NULL))
        return;

      _clutter_actor_apply_modelview_transform (stage, &matrix);

      relative_to_ancestor = stage;
    }

  _clutter_actor_apply_modelview_transform_recursive (actor,
                                                      relative_to_ancestor,
                                                      &matrix);

  _clutter_paint_volume_transform (pv, &matrix);
}
