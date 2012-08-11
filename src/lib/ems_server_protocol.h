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

#ifndef _EMS_SERVER_PROTOCOL_H_
#define _EMS_SERVER_PROTOCOL_H_
#include <Eina.h>
#include <Eet.h>

typedef struct _Match_Type Match_Type;
typedef struct _Get_Medias Get_Medias;
typedef struct _Get_Medias_Req Get_Medias_Req;
typedef struct _Get_Medias_Infos Get_Medias_Infos;
typedef struct _Get_Media_Infos_Req Get_Media_Infos_Req;
typedef struct _Get_Media_Infos Get_Media_Infos;
typedef enum _Ems_Server_Protocol_Type Ems_Server_Protocol_Type;
typedef struct _Ems_Server_Protocol Ems_Server_Protocol;

struct _Match_Type {
   const char *name;
   int type;
   Eet_Data_Descriptor *(*edd)(void);
};

extern Match_Type match_type[];

struct _Get_Medias_Req
{
   Ems_Collection *collection;
};

struct _Get_Medias
{
   Eina_List *files;
};

struct _Get_Media_Infos_Req
{
   const char *uuid;
   const char *metadata;
};


struct _Get_Media_Infos
{
   const char *value;
};


enum _Ems_Server_Protocol_Type
{
  EMS_SERVER_PROTOCOL_TYPE_GET_MEDIAS_REQ,
  EMS_SERVER_PROTOCOL_TYPE_GET_MEDIAS,
  EMS_SERVER_PROTOCOL_TYPE_GET_MEDIA_INFOS_REQ,
  EMS_SERVER_PROTOCOL_TYPE_GET_MEDIA_INFOS,
  EMS_SERVER_PROTOCOL_TYPE_UNKNOWN
};

union Specs
{
   Get_Medias get_medias;
   Get_Medias_Req get_medias_req;
   Get_Media_Infos_Req get_media_infos_req;
   Get_Media_Infos get_media_infos;
};

struct _Ems_Server_Protocol
{
   Ems_Server_Protocol_Type type;
   union Specs data;
};

Eina_Bool serialisation_init(void);

#endif /* _EMS_SERVER_PROTOCOL_H_ */
