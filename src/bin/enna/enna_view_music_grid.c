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
#include "enna_view_music_grid.h"
#include "enna_config.h"


/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define TIMER_VALUE 0.1

typedef struct _Enna_View_Music_Grid_Group Enna_View_Music_Grid_Group;
typedef struct _Enna_View_Music_Grid_Item Enna_View_Music_Grid_Item;
typedef struct _Enna_View_Music_Grid Enna_View_Music_Grid;

struct _Enna_View_Music_Grid_Group
{
    Elm_Object_Item *it;
    Ems_Node *node;
    Enna_View_Music_Grid *act;
};

struct _Enna_View_Music_Grid_Item
{
   Elm_Object_Item *it;
   Ems_Node *node;
   Enna_View_Music_Grid *act;
   const char *album;
   const char *artist;
   const char *cover;
   const char *media;
   Evas_Object *o_cover;
   Evas_Object *o_backdrop;
};

struct _Enna_View_Music_Grid
{
   Eina_Hash *nodes;
   Evas_Object *grid;
   Evas_Object *ly;
   int nb_items;
};

static Elm_Gengrid_Item_Class itc_group;
static Elm_Gengrid_Item_Class itc_item;

static char *_gengrid_text_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   Enna_View_Music_Grid_Group *gr = data;

   return strdup(ems_node_name_get(gr->node));
}

static char *_media_text_get(void *data, Evas_Object *obj __UNUSED__, const char *part)
{
   Enna_View_Music_Grid_Item *it = data;

   if (!strcmp(part, "album"))
     return it->album ? strdup(it->album) : NULL;
   else if (!strcmp(part, "artist"))
     return it->artist ? strdup(it->artist) : NULL;
   else
     return NULL;
}

static Evas_Object
*_media_content_get(void *data, Evas_Object *obj, const char *part)
{
   Enna_View_Music_Grid_Item *it = data;

   if (!strcmp(part, "elm.swallow.icon"))
     {
        Evas_Object *cover;
        cover = evas_object_image_filled_add(evas_object_evas_get(obj));
        evas_object_image_file_set(cover, it->cover, NULL);
        evas_object_show(cover);
        return cover;
     }
   else
     return NULL;
}

static void _group_del(Enna_View_Music_Grid_Group *gr)
{
   if (!gr)
     return;

   DBG("Free GROUP");

   free(gr);
}

static void
_node_add_cb(void *data, Ems_Node *node)
{
   Enna_View_Music_Grid *act = data;

   DBG("Node %s added", ems_node_name_get(node));

   if (!ems_node_connect(node))
     DBG("An error occured during connection with node %s",
         ems_node_name_get(node));
   else
     {
        Enna_View_Music_Grid_Group *gr;

        gr = calloc(1, sizeof(Enna_View_Music_Grid_Group));
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
   Enna_View_Music_Grid_Item *it = data;

   if (value && value[0])
     {
        it->album = eina_stringshare_add(value);
     }
   else
     {
        it->album = eina_stringshare_add("Unknown");
        ERR("Cannot find a title for this file: :'(");
     }
   elm_gengrid_realized_items_update(it->act->grid);
}


static void
_add_item_album_cb(void *data, Ems_Node *node, const char *value)
{
   Enna_View_Music_Grid_Item *it = data;

   if (value && value[0])
     {
        it->album = eina_stringshare_add(value);
        elm_gengrid_realized_items_update(it->act->grid);
     }
   else
     {
         ems_node_media_info_get(node, it->media, "name", _add_item_file_name_cb,
                                   NULL, NULL, it);
     }
}

static void
_add_item_poster_cb(void *data, Ems_Node *node __UNUSED__, const char *value)
{
   Enna_View_Music_Grid_Item *it = data;

   if (value)
     {
        it->cover = eina_stringshare_add(value);
        elm_gengrid_realized_items_update(it->act->grid);
     }
}

static void
_add_item_cb(void *data, Ems_Node *node, const char *media)
{
   Enna_View_Music_Grid *act = data;
   Enna_View_Music_Grid_Item *it;


   it = calloc(1, sizeof (Enna_View_Music_Grid_Item));
   it->node = node;
   it->act = act;
   it->media = eina_stringshare_add(media);

   it->it = elm_gengrid_item_append(act->grid, &itc_item,
                                    it,
                                    NULL,
                                    NULL);
   if (!act->nb_items)
     elm_gengrid_item_selected_set(it->it, EINA_TRUE);

   act->nb_items++;

   ems_node_media_info_get(node, media, "album", _add_item_album_cb,
                             NULL, NULL, it);
   ems_node_media_info_get(node, media, "poster", _add_item_poster_cb,
                             NULL, NULL, it);
}

static void
_node_connected_cb(void *data, Ems_Node *node)
{
   Enna_View_Music_Grid_Group *gr;
   Enna_View_Music_Grid *act = data;
   Ems_Collection *collection;

   DBG("Node %s connected", ems_node_name_get(node));

   gr = eina_hash_find(act->nodes, node);
   gr->it = elm_gengrid_item_append(act->grid, &itc_group,
                                    gr,
                                    NULL,
                                    NULL);
   elm_gengrid_item_select_mode_set(gr->it, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

   collection = ems_collection_new(EMS_MEDIA_TYPE_VIDEO, "music", "*", NULL);
   ems_node_media_get(node, collection, _add_item_cb, act);
}

static void
_node_disconnected_cb(void *data, Ems_Node *node)
{
   Enna_View_Music_Grid_Group *gr;
   Enna_View_Music_Grid *act = data;

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
_grid_item_selected_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *it = event_info;
   Enna_View_Music_Grid_Item *item;

   item = elm_object_item_data_get(it);
   if (!item || !item->act)
     return;


}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

Evas_Object *enna_view_music_grid_add(Enna *enna __UNUSED__, Evas_Object *parent)
{
   Evas_Object *ly;
   Evas_Object *grid;
   Ems_Node *node;
   Eina_List *l;
   Enna_View_Music_Grid *act;

   act = calloc(1, sizeof(Enna_View_Music_Grid));
   if (!act)
       return NULL;

   ly = elm_layout_add(parent);
   if (!ly)
       goto err1;

   elm_layout_file_set(ly, enna_config_theme_get(), "activity/layout/grid");
   evas_object_data_set(ly, "view_music_grid", act);

   grid = elm_gengrid_add(ly);
   if (!grid)
       goto err2;

   elm_object_part_content_set(ly, "grid.swallow", grid);
   elm_scroller_bounce_set(grid, EINA_FALSE, EINA_TRUE);
   elm_object_style_set(grid, "media");

   itc_group.item_style       = "group_index";
   itc_group.func.text_get    = _gengrid_text_get;
   itc_group.func.content_get = NULL;
   itc_group.func.state_get   = NULL;
   itc_group.func.del         = NULL;

   itc_item.item_style        = "default";
   itc_item.func.text_get     = _media_text_get;
   itc_item.func.content_get  = _media_content_get;
   itc_item.func.state_get    = NULL;
   itc_item.func.del          = NULL;

   act->grid = grid;
   act->ly = ly;

   act->nodes = eina_hash_pointer_new((Eina_Free_Cb)_group_del);
   ems_node_cb_set(_node_add_cb, _node_del_cb, _node_update_cb,
                     _node_connected_cb, _node_disconnected_cb,
                     act);

   elm_gengrid_item_size_set(grid, 160, 200);
   elm_gengrid_group_item_size_set(grid, 31, 31);

   EINA_LIST_FOREACH(ems_node_list_get(), l, node)
   {

      if (!ems_node_connect(node))
        DBG("An error occured during connection with node %s",
            ems_node_name_get(node));
      else
        {
           Enna_View_Music_Grid_Group *gr;

           gr = calloc(1, sizeof(Enna_View_Music_Grid_Group));
           gr->node = node;
           gr->act = act;
           eina_hash_add(act->nodes, node, gr);
        }
   }

   elm_object_focus_set(grid, EINA_TRUE);

   evas_object_smart_callback_add(act->grid, "selected", _grid_item_selected_cb, act);

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
