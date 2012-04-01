/* Enna Media Server - a library and daemon for medias indexation and streaming
 *
 * Copyright (C) 2012 Enna Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Elementary.h>

#include "enna_private.h"
#include "enna_config.h"
#include "enna_mainmenu.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void
_list_item_activated_cb(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
    Elm_Object_Item *it = event_info;
    const char *label;

    label = elm_object_item_text_get(it);
    DBG("Item activated : %s", label);

    evas_object_smart_callback_call(data, "selected", label);

}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

Evas_Object *
enna_mainmenu_add(Evas_Object *parent)
{
    Evas_Object *ly;
    Evas_Object *list;
    Elm_Object_Item *it;

    ly = elm_layout_add(parent);
    elm_layout_file_set(ly, enna_config_theme_get(), "mainmenu/layout");


    list = elm_list_add(ly);
    it = elm_list_item_append(list, "Videos", NULL, NULL, NULL, NULL);
    elm_list_item_selected_set(it, EINA_TRUE);
    elm_list_item_append(list, "TV Shows", NULL, NULL, NULL, NULL);
    elm_list_item_append(list, "Music", NULL, NULL, NULL, NULL);
    elm_list_item_append(list, "Photos", NULL, NULL, NULL, NULL);
    elm_list_item_append(list, "Settings", NULL, NULL, NULL, NULL);
    elm_list_item_append(list, "Exit", NULL, NULL, NULL, NULL);
    elm_object_part_content_set(ly, "list.swallow", list);
    evas_object_size_hint_align_set(list, -1, 0.5);
    elm_list_bounce_set(list, EINA_FALSE, EINA_FALSE);

    evas_object_smart_callback_add(list, "activated", _list_item_activated_cb, ly);

    evas_object_show(list);

    return ly;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
