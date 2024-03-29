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
 */

#ifndef __CLUTTER_STAGE_PRIVATE_H__
#define __CLUTTER_STAGE_PRIVATE_H__

#include <clutter/clutter-stage-window.h>
#include <clutter/clutter-stage.h>
#include <clutter/clutter-input-device.h>
#include <clutter/clutter-private.h>

#include <cogl/cogl.h>

G_BEGIN_DECLS

typedef struct _ClutterStageQueueRedrawEntry ClutterStageQueueRedrawEntry;

/* stage */
ClutterStageWindow *_clutter_stage_get_default_window    (void);
void                _clutter_stage_do_paint              (ClutterStage          *stage,
                                                          const ClutterGeometry *clip);
void                _clutter_stage_set_window            (ClutterStage          *stage,
                                                          ClutterStageWindow    *stage_window);
ClutterStageWindow *_clutter_stage_get_window            (ClutterStage          *stage);
void                _clutter_stage_get_projection_matrix (ClutterStage          *stage,
                                                          CoglMatrix            *projection);
void                _clutter_stage_dirty_projection      (ClutterStage          *stage);
void                _clutter_stage_set_viewport          (ClutterStage          *stage,
                                                          float                  x,
                                                          float                  y,
                                                          float                  width,
                                                          float                  height);
void                _clutter_stage_get_viewport          (ClutterStage          *stage,
                                                          float                 *x,
                                                          float                 *y,
                                                          float                 *width,
                                                          float                 *height);
void                _clutter_stage_dirty_viewport        (ClutterStage          *stage);
void                _clutter_stage_maybe_setup_viewport  (ClutterStage          *stage);
void                _clutter_stage_maybe_relayout        (ClutterActor          *stage);
gboolean            _clutter_stage_needs_update          (ClutterStage          *stage);
gboolean            _clutter_stage_do_update             (ClutterStage          *stage);

void     _clutter_stage_queue_event                       (ClutterStage *stage,
					                   ClutterEvent *event);
gboolean _clutter_stage_has_queued_events                 (ClutterStage *stage);
void     _clutter_stage_process_queued_events             (ClutterStage *stage);
void     _clutter_stage_update_input_devices              (ClutterStage *stage);
int      _clutter_stage_get_pending_swaps                 (ClutterStage *stage);
gboolean _clutter_stage_has_full_redraw_queued            (ClutterStage *stage);

ClutterActor *_clutter_stage_do_pick (ClutterStage    *stage,
                                      gint             x,
                                      gint             y,
                                      ClutterPickMode  mode);

ClutterPaintVolume *_clutter_stage_paint_volume_stack_allocate (ClutterStage *stage);
void                _clutter_stage_paint_volume_stack_free_all (ClutterStage *stage);

const ClutterPlane *_clutter_stage_get_clip (ClutterStage *stage);

ClutterStageQueueRedrawEntry *_clutter_stage_queue_actor_redraw            (ClutterStage                 *stage,
                                                                            ClutterStageQueueRedrawEntry *entry,
                                                                            ClutterActor                 *actor,
                                                                            ClutterPaintVolume           *clip);
void                          _clutter_stage_queue_redraw_entry_invalidate (ClutterStageQueueRedrawEntry *entry);

void            _clutter_stage_add_device       (ClutterStage       *stage,
                                                 ClutterInputDevice *device);
void            _clutter_stage_remove_device    (ClutterStage       *stage,
                                                 ClutterInputDevice *device);
gboolean        _clutter_stage_has_device       (ClutterStage       *stage,
                                                 ClutterInputDevice *device);

void     _clutter_stage_set_motion_events_enabled (ClutterStage *stage,
                                                   gboolean      enabled);
gboolean _clutter_stage_get_motion_events_enabled (ClutterStage *stage);

CoglFramebuffer *_clutter_stage_get_active_framebuffer (ClutterStage *stage);

gint32          _clutter_stage_acquire_pick_id          (ClutterStage *stage,
                                                         ClutterActor *actor);
void            _clutter_stage_release_pick_id          (ClutterStage *stage,
                                                         gint32        pick_id);
ClutterActor *  _clutter_stage_get_actor_by_pick_id     (ClutterStage *stage,
                                                         gint32        pick_id);

G_END_DECLS

#endif /* __CLUTTER_STAGE_PRIVATE_H__ */
