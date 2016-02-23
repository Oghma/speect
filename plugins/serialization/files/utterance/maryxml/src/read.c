/************************************************************************************/
/* Copyright (c) 2016 Matteo Lisotto <matteo.lisotto@gmail.com>                     */
/*                                                                                  */
/* Contributors:  Matteo Lisotto                                                    */
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
/* AUTHOR  : Matteo Lisotto <matteo.lisotto@gmail.com>                              */
/* DATE    : 23 February 2015                                                       */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* Read an utterance to a maryxml format file.                                      */
/*                                                                                  */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* Modules used                                                                     */
/*                                                                                  */
/************************************************************************************/

#include "utt_maryxml.h"
#include <libxml/xmlreader.h>
#include <string.h>
#define ENCODING "UTF-8"

/************************************************************************************/
/*                                                                                  */
/* Function implementations                                                         */
/*                                                                                  */
/************************************************************************************/

static int _ds_read(void * context, char * buffer, int len)
{
	int ret = 0;
	s_erc error;
	S_CLR_ERR(&error);
	ret = SDatasourceRead((SDatasource *) context, (void *) buffer, sizeof(char), len, &error);
	if (error == S_IOEOF)
		return ret;
	if (S_CHK_ERR(&error, S_CONTERR,
		      "s_read_utt_maryxml",
		      "Call to \"SDatasourceRead\" failed"))
		return -1;

	return ret;
}

