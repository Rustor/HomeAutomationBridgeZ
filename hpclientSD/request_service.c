/*Copyright 2011 Aalborg University. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY Aalborg University ''AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Aalborg University OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed*/

/*
 * @author Thibaut Le Guilly
 */
#include "request_service.h"

int destroy_request_service_list( RequestService *request_service_head )
{
	RequestService *tmp, *iterator;
	
	if( request_service_head == NULL )
		return -1;

	LL_FOREACH_SAFE( request_service_head, iterator, tmp )
	{
		LL_DELETE( request_service_head, iterator );
		destroy_request_service_struct ( iterator );
	}

	return 0;
}

int destroy_request_service_struct (RequestService *rs_tmp)
{
	OnValueChange *tmp, *iterator;
	
	if( rs_tmp == NULL )
		return -1;
	
	if ( rs_tmp->ID )
		free( rs_tmp->ID );

	if ( rs_tmp->value_url )
		free( rs_tmp->value_url );

	if( rs_tmp->type )
		free( rs_tmp->type );

	if( rs_tmp->RSID )
		free( rs_tmp->RSID );

	if( rs_tmp->device_type )
		free( rs_tmp->device_type );

	if ( rs_tmp->device_ID )
		free( rs_tmp->device_ID );

	if( rs_tmp->on_value_change_head )
	{
		LL_FOREACH_SAFE( rs_tmp->on_value_change_head, iterator, tmp )
		{
			LL_DELETE( rs_tmp->on_value_change_head, iterator );
			destroy_on_value_change_struct ( iterator );
		}
	}
	
    free( rs_tmp );

	rs_tmp = NULL;

	return 0;
}

int destroy_on_value_change_struct( OnValueChange *ovc_tmp )
{
	Action *tmp, *iterator;
	
	if( ovc_tmp == NULL )
		return -1;

	if( ovc_tmp->value )
		free( ovc_tmp->value );

	if( ovc_tmp->action_head )
	{
		LL_FOREACH_SAFE( ovc_tmp->action_head, iterator, tmp )
		{
			LL_DELETE( ovc_tmp->action_head, iterator );
			destroy_action_struct ( iterator );
		}
	}

	free( ovc_tmp );

	return 0;
}

int destroy_action_struct( Action *action_tmp )
{
	if( action_tmp == NULL )
		return -1;
	
	if( action_tmp->type )
		free( action_tmp->type );

	if( action_tmp->RSID )
		free( action_tmp->RSID );

	if( action_tmp->value )
		free( action_tmp->value );

	free( action_tmp );

	return 0;

}

int copy_request_service( RequestService *to_copy, RequestService *to_create )
{

	to_create->ID = to_copy->ID;
	to_create->type = to_copy->type;
	to_create->device_ID = to_copy->device_ID;
	to_create->device_type = to_copy->device_type;
	to_create->RSID = to_copy->RSID;

	return 0;

}

int init_request_service_struct( RequestService *rs_tmp )
{
	if( rs_tmp == NULL )
	{
		return -1;
	}
	
	rs_tmp->ID = NULL;
	rs_tmp->value_url = NULL;
	rs_tmp->type = NULL;
	rs_tmp->RSID = NULL;
	rs_tmp->device_type = NULL;
	rs_tmp->device_ID = NULL;
	rs_tmp->on_value_change_head = NULL;
	rs_tmp->next = NULL;

	return 0;
}

int init_on_value_change_struct( OnValueChange *ovc_tmp )
{
	if ( ovc_tmp == NULL )
		return -1;

	ovc_tmp->value = NULL;
	ovc_tmp->action_head = NULL;

	return 0;
}

int init_action_struct( Action *action_tmp )
{
	if ( action_tmp == NULL )
		return -1;

	action_tmp->type = NULL;
	action_tmp->RSID = NULL;
	action_tmp->value = NULL;

	return 0;
}

