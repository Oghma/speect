/************************************************************************************/
/* Copyright (c) 2008-2011 The Department of Arts and Culture,                      */
/* The Government of the Republic of South Africa.                                  */
/*                                                                                  */
/* Contributors:  Meraka Institute, CSIR, South Africa.                             */
/*                                                                                  */
/* Permission is hereby granted, free of charge, to any person obtaining a copy     */
/* of this software and associated documentation files (the "Software"), to deal    */
/* in the Software without restriction, including without limitation the rights     */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        */
/* copies of the Software, and to permit persons to whom the Software is            */
/* furnished to do so, subject to the following conditions:                         */
/* The above copyright notice and this permission notice shall be included in       */
/* all copies or substantial portions of the Software.                              */
/*                                                                                  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR       */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,         */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE      */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER           */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,    */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN        */
/* THE SOFTWARE.                                                                    */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* AUTHOR  : Aby Louw                                                               */
/* DATE    : 5 October 2009                                                         */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* Intialize and quit containers module.                                            */
/*                                                                                  */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* Modules used                                                                     */
/*                                                                                  */
/************************************************************************************/

#include "base/objsystem/class.h"
#include "containers/containers.h"


/************************************************************************************/
/*                                                                                  */
/*  Function implementations                                                        */
/*                                                                                  */
/************************************************************************************/

S_LOCAL void _s_containers_init(s_erc *error)
{
	s_erc local_err;


	S_CLR_ERR(&local_err);
	S_CLR_ERR(error);

	_s_iterator_class_add(error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "_s_containers_init",
		      "Failed to intialize SIterator class"))
		local_err = *error;

	_s_container_class_add(error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "_s_containers_init",
		      "Failed to intialize SContainer class"))
		local_err = *error;

	_s_containers_maps_init(error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "_s_containers_init",
		      "Failed to intialize Map containers"))
		local_err = *error;

	_s_containers_lists_init(error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "_s_containers_init",
		      "Failed to intialize List containers"))
		local_err = *error;

	/* if there was an error local_err will have it */
	if ((local_err != S_SUCCESS) && (*error == S_SUCCESS))
		*error = local_err;
}


S_LOCAL void _s_containers_quit(s_erc *error)
{
	S_CLR_ERR(error);
	/* NOP */
}
