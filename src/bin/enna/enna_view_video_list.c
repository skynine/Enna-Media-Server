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

#include <Evas.h>
#include <Elementary.h>
#include <Ems.h>

#include "enna_private.h"
#include "enna_view_video_list.h"
#include "enna_config.h"
#include "enna_activity.h"
#include "enna_input.h"
#include "enna_view_player_video.h"
#include "enna_keyboard.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define TIMER_VALUE 0.1

typedef struct _Enna_View_Video_List_Group Enna_View_Video_List_Group;
typedef struct _Enna_View_Video_List_Item Enna_View_Video_List_Item;
typedef struct _Enna_View_Video_List Enna_View_Video_List;

struct _Enna_View_Video_List_Group
{
    Elm_Object_Item *it;
    Ems_Node *node;
    Enna_View_Video_List *act;
};

struct _Enna_View_Video_List_Item
{
   Elm_Object_Item *it;
   Ems_Node *node;
   Enna_View_Video_List *act;
   const char *name;
   const char *description;
   const char *cover;
   const char *backdrop;
   Ems_Video *media;
   Evas_Object *o_cover;
   Evas_Object *o_backdrop;
};

struct _Enna_View_Video_List
{
   Eina_Hash *nodes;
   Evas_Object *list;
   Evas_Object *ly;
   Evas_Object *btn_trailer;
   Evas_Object *btn_play;
   Ecore_Timer *show_timer;
   int nb_items;
   Enna *enna;
   Enna_Input_Listener *il;
};
static Eina_Bool _timer_cb(void *data);

static Elm_Genlist_Item_Class itc_group;
static Elm_Genlist_Item_Class itc_item;

static char *_genlist_text_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   Enna_View_Video_List_Group *gr = data;

   return strdup(ems_node_name_get(gr->node));
}

static char *_media_text_get(void *data, Evas_Object *obj __UNUSED__, const char *part)
{
   Enna_View_Video_List_Item *it = data;

   if (!strcmp(part, "name"))
     return it->name ? strdup(it->name) : NULL;
   else if (!strcmp(part, "description"))
     return it->description ? strdup(it->description) : NULL;
   else
     return NULL;
}

static void _group_del(Enna_View_Video_List_Group *gr)
{
   if (!gr)
     return;

   DBG("Free GROUP");

   free(gr);
}

static void
_node_add_cb(void *data, Ems_Node *node)
{
   Enna_View_Video_List *act = data;

   DBG("Node %s added", ems_node_name_get(node));

   if (!ems_node_connect(node))
     DBG("An error occured during connection with node %s",
         ems_node_name_get(node));
   else
     {
        Enna_View_Video_List_Group *gr;

        gr = calloc(1, sizeof(Enna_View_Video_List_Group));
        gr->node = node;
        gr->act = act;
        eina_hash_add(act->nodes, node, gr);
     }

}

static void
_node_del_cb(void *data __UNUSED__, Ems_Node *node)
{
   DBG("Node %s deleted", ems_node_name_get(node));
}


static void
_node_update_cb(void *data __UNUSED__, Ems_Node *node)
{
   DBG("Node %s updated", ems_node_name_get(node));
}

static void
_add_item_file_name_cb(void *data, Ems_Node *node __UNUSED__, const char *value)
{
   Enna_View_Video_List_Item *it = data;

   if (value && value[0])
     {
        it->name = eina_stringshare_add(value);
        elm_genlist_item_fields_update(it->it, "name",
                                       ELM_GENLIST_ITEM_FIELD_TEXT);
     }
   else
     {
        it->name = eina_stringshare_add("Unknown");
//        elm_genlist_item_fields_update(it->it, "name",
//                                       ELM_GENLIST_ITEM_FIELD_TEXT);
        ERR("Cannot find a title for this file: :'(");
     }
}


static void
_add_item_name_cb(void *data, Ems_Node *node, const char *value)
{
   Enna_View_Video_List_Item *it = data;

   if (value && value[0])
     {
        it->name = eina_stringshare_add(value);
        elm_genlist_item_fields_update(it->it, "name",
                                       ELM_GENLIST_ITEM_FIELD_TEXT);
     }
   else
     {
         ems_node_media_info_get(node, ems_video_hash_key_get(it->media), "clean_name", _add_item_file_name_cb,
                             NULL, NULL, it);
     }
}

static void
_add_item_poster_cb(void *data, Ems_Node *node __UNUSED__, const char *value)
{
   Enna_View_Video_List_Item *it = data;

   if (value)
     {
        it->cover = eina_stringshare_add(value);
        if(elm_genlist_item_selected_get(it->it))
             _timer_cb(it);
     }
}

static void
_add_item_backdrop_cb(void *data, Ems_Node *node __UNUSED__, const char *value)
{
   Enna_View_Video_List_Item *it = data;

   if (value)
     {
        it->backdrop = eina_stringshare_add(value);
        if(elm_genlist_item_selected_get(it->it))
          _timer_cb(it);
     }
}

static void
_add_item_cb(void *data, Ems_Node *node, Ems_Video *media)
{
   Enna_View_Video_List *act = data;
   Enna_View_Video_List_Item *it;
   Enna_View_Video_List_Group *gr;

   gr = eina_hash_find(act->nodes, node);

   it = calloc(1, sizeof (Enna_View_Video_List_Item));
   it->node = node;
   it->act = act;
   it->media =  media;
   it->it = elm_genlist_item_append(act->list, &itc_item,
                                    it,
                                    gr->it/* parent */,
                                    ELM_GENLIST_ITEM_NONE,
                                    NULL,
                                    NULL);
   if (!act->nb_items)
     elm_genlist_item_selected_set(it->it, EINA_TRUE);

   act->nb_items++;

   ems_node_media_info_get(node, ems_video_hash_key_get(it->media), "name", _add_item_name_cb,
                             NULL, NULL, it);
   ems_node_media_info_get(node, ems_video_hash_key_get(it->media), "poster", _add_item_poster_cb,
                             NULL, NULL, it);
   ems_node_media_info_get(node, ems_video_hash_key_get(it->media), "backdrop", _add_item_backdrop_cb,
                             NULL, NULL, it);


}

static void
_node_connected_cb(void *data, Ems_Node *node)
{
   Enna_View_Video_List_Group *gr;
   Enna_View_Video_List *act = data;
   Ems_Collection *collection;

   DBG("Node %s connected", ems_node_name_get(node));

   gr = eina_hash_find(act->nodes, node);
   gr->it = elm_genlist_item_append(act->list, &itc_group,
                                    gr,
                                    NULL/* parent */,
                                    ELM_GENLIST_ITEM_GROUP,
                                    NULL,
                                    NULL);
   elm_genlist_item_select_mode_set(gr->it, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

   collection = ems_collection_new(EMS_MEDIA_TYPE_VIDEO, "films", "*", NULL);
   ems_node_media_get(node, collection, _add_item_cb, act);
}

static void
_node_disconnected_cb(void *data, Ems_Node *node)
{
   Enna_View_Video_List_Group *gr;
   Enna_View_Video_List *act = data;

   DBG("Node %s disconnected", ems_node_name_get(node));

   gr = eina_hash_find(act->nodes, node);
   if (!gr)
     DBG("gt is null");
   else
     {
        elm_object_item_del(gr->it);
        eina_hash_del(act->nodes, node, gr);
     }
}

static void
_cover_del(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Enna_View_Video_List_Item *item = data;

   item->o_cover = NULL;
}

static void
_backdrop_del(void *data, Evas *e  __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Enna_View_Video_List_Item *item = data;

   item->o_backdrop = NULL;
}

static Eina_Bool
_timer_cb(void *data)
{
   Enna_View_Video_List_Item *item = data;

   if (!item || !item->act)
     return EINA_FALSE;

   if (item->o_cover)
     evas_object_del(item->o_cover);
   if (item->o_backdrop)
     evas_object_del(item->o_backdrop);

   if (item->cover)
     {
        item->o_cover = elm_icon_add(item->act->ly);
        elm_image_file_set(item->o_cover, item->cover, NULL);
        elm_image_preload_disabled_set(item->o_cover, EINA_FALSE);
        elm_object_part_content_set(item->act->ly, "cover.swallow", item->o_cover);

        evas_object_event_callback_add(item->o_cover, EVAS_CALLBACK_DEL, _cover_del, item);
     }

   if (item->backdrop)
     {
        item->o_backdrop = elm_icon_add(item->act->ly);
        elm_image_file_set(item->o_backdrop, item->backdrop, NULL);
        elm_image_preload_disabled_set(item->o_backdrop, EINA_FALSE);
        elm_object_part_content_set(item->act->ly, "backdrop.swallow", item->o_backdrop);

        evas_object_event_callback_add(item->o_backdrop, EVAS_CALLBACK_DEL, _backdrop_del, item);
     }

   if (item->act->show_timer)
     item->act->show_timer = NULL;
   return EINA_FALSE;
}

static void
_list_item_selected_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *it = event_info;
   Enna_View_Video_List_Item *item;

   if (elm_genlist_item_type_get(it) != ELM_GENLIST_ITEM_NONE)
     return;

   item = elm_object_item_data_get(it);
   if (!item || !item->act)
     return;

   if (item->act->show_timer)
     {
        ecore_timer_del(item->act->show_timer);
     }
   item->act->show_timer = ecore_timer_add(TIMER_VALUE, _timer_cb, item);

}

static void
_play_pressed_cb(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Enna_View_Video_List *act = data;
   Elm_Object_Item *it;
   Enna_View_Video_List_Item *item;
   Evas_Object *player;

   it = elm_genlist_selected_item_get(act->list);
   if (elm_genlist_item_type_get(it) != ELM_GENLIST_ITEM_NONE)
     return;

   item = elm_object_item_data_get(it);
   if (!item || !item->act)
     return;

   player = enna_activity_select(act->enna, "VideoPlayer");
   enna_view_player_video_uri_set(player, item->node, item->media);
   enna_view_player_video_play(player);
}

static void
_layout_object_del(void *data, Evas *e  __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
   Enna_View_Video_List *act = data;

   DBG("delete Enna_View_Video_List object (%p)", obj);

   FREE_NULL_FUNC(evas_object_del, act->list);
   FREE_NULL_FUNC(evas_object_del, act->btn_trailer);
   FREE_NULL_FUNC(evas_object_del, act->btn_play);
   FREE_NULL_FUNC(ecore_timer_del, act->show_timer);
   FREE_NULL_FUNC(eina_hash_free, act->nodes);
   FREE_NULL_FUNC(enna_input_listener_del, act->il);

   free(act);
}

static void
_list_select_do(Evas_Object *obj, const char direction)
{
   Elm_Object_Item *it;

   /* Get the current selected item */
   it = elm_genlist_selected_item_get(obj);
   if (it)
     {
	/* Get the previous or next item */
	if (direction)
	  it = elm_genlist_item_next_get(it);
	else
	  it = elm_genlist_item_prev_get(it);
	/* Item is the first one or the last one in the list */
	if (!it)
	  {
	     /* Try to select the last or the first element */
	     if (direction)
	       it = elm_genlist_first_item_get(obj);
	     else
	       it = elm_genlist_last_item_get(obj);
	     if (it)
	       {
		  /* Select this item and show it */
		  elm_genlist_item_selected_set(it, EINA_TRUE);
		  elm_genlist_item_bring_in(it, ELM_GENLIST_ITEM_SCROLLTO_MIDDLE);
	       }
	     else
	       {
		  ERR("item can't be selected, the list is void ?");
	       }
	  }
	/* Select previous or next item and show it */
	else
	  {
	     elm_genlist_item_selected_set(it, EINA_TRUE);
	     elm_genlist_item_bring_in(it, ELM_GENLIST_ITEM_SCROLLTO_MIDDLE);
	  }
     }
   /* There is no selected item, select the fist one */
   else
     {
	if (direction)
	  it = elm_genlist_first_item_get(obj);
	else
	  it = elm_genlist_last_item_get(obj);
	if (it)
	  {
	     /* Select this item and show it */
	     elm_genlist_item_selected_set(it, EINA_TRUE);
	     elm_genlist_item_bring_in(it, ELM_GENLIST_ITEM_SCROLLTO_MIDDLE);
	  }
	else
	  {
	     ERR("item can't be selected, the list is void ?");
	  }
     }

}

static Eina_Bool
_input_event(void *data, Enna_Input event)
{
    Enna_View_Video_List *act = data;
    const char *tmp;

    tmp = enna_keyboard_input_name_get(event);
    INF("view video list input event %s", tmp);

    switch(event)
    {
    case ENNA_INPUT_DOWN:
        _list_select_do(act->list, 1);
        return ENNA_EVENT_BLOCK;
    case ENNA_INPUT_UP:
        _list_select_do(act->list, 0);
        return ENNA_EVENT_BLOCK;
    case ENNA_INPUT_OK:
    {
        Evas_Object *o;
        Elm_Object_Item *it;
        Enna_View_Video_List_Item *list_it;

        it = elm_genlist_selected_item_get(act->list);
        list_it = elm_object_item_data_get(it);

        o = enna_activity_select(act->enna, "VideoPlayer");
        enna_view_player_video_uri_set(o, list_it->node, list_it->media);

        return ENNA_EVENT_BLOCK;
    }
    default:
        break;
    }

    return ENNA_EVENT_CONTINUE;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

Evas_Object *enna_view_video_list_add(Enna *enna, Evas_Object *parent)
{
   Evas_Object *ly;
   Evas_Object *list;
   Ems_Node *node;
   Eina_List *l;
   Enna_View_Video_List *act;

   act = calloc(1, sizeof(Enna_View_Video_List));
   if (!act)
       return NULL;

   act->il = enna_input_listener_add("enna_view_video_list", _input_event, act);

   ly = elm_layout_add(parent);
   if (!ly)
       goto err1;

   elm_layout_file_set(ly, enna_config_theme_get(), "activity/layout/list");
   evas_object_data_set(ly, "view_video_list", act);
   act->enna = enna;

   list = elm_genlist_add(ly);
   if (!list)
       goto err2;

   elm_object_part_content_set(ly, "list.swallow", list);
   elm_scroller_bounce_set(list, EINA_FALSE, EINA_TRUE);
   elm_object_style_set(list, "media");

   itc_group.item_style       = "group_index_media";
   itc_group.func.text_get    = _genlist_text_get;
   itc_group.func.content_get = NULL;
   itc_group.func.state_get   = NULL;
   itc_group.func.del         = NULL;

   itc_item.item_style        = "media";
   itc_item.func.text_get     = _media_text_get;
   itc_item.func.content_get  = NULL;
   itc_item.func.state_get    = NULL;
   itc_item.func.del          = NULL;

   act->list = list;
   act->ly = ly;

   act->nodes = eina_hash_pointer_new((Eina_Free_Cb)_group_del);
   ems_node_cb_set(_node_add_cb, _node_del_cb, _node_update_cb,
                     _node_connected_cb, _node_disconnected_cb,
                     act);

   EINA_LIST_FOREACH(ems_node_list_get(), l, node)
   {

      if (!ems_node_connect(node))
        DBG("An error occured during connection with node %s",
            ems_node_name_get(node));
      else
        {
           Enna_View_Video_List_Group *gr;

           gr = calloc(1, sizeof(Enna_View_Video_List_Group));
           gr->node = node;
           gr->act = act;
           eina_hash_add(act->nodes, node, gr);

           DBG("Found new node: %s", ems_node_name_get(node));
        }
   }

   elm_object_focus_set(list, EINA_TRUE);

   /* Create Trailer and Play buttons */
   act->btn_play = elm_button_add(ly);
   elm_object_text_set(act->btn_play, "Play");
   evas_object_show(act->btn_play);
   elm_object_part_content_set(ly, "play.swallow", act->btn_play);
   elm_object_style_set(act->btn_play, "enna");
   evas_object_smart_callback_add(act->btn_play, "pressed", _play_pressed_cb, act);

   act->btn_trailer = elm_button_add(ly);
   elm_object_text_set(act->btn_trailer, "Trailer");
   evas_object_show(act->btn_trailer);
   elm_object_part_content_set(ly, "trailer.swallow", act->btn_trailer);
   elm_object_style_set(act->btn_trailer, "enna");

   evas_object_smart_callback_add(act->list, "selected", _list_item_selected_cb, act);

   evas_object_event_callback_add(ly, EVAS_CALLBACK_DEL, _layout_object_del, act);

   return ly;

err2:
   evas_object_del(ly);
err1:
   free(act);
   return NULL;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
