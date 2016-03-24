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

static int _ds_close(void * context)
{
	s_erc error;
	SDatasource * ds = (SDatasource *) context;
	S_CLR_ERR(&error);
	if (ds != NULL)
		S_DELETE(ds, "s_read_utt_maryxml", &error);
	if (S_CHK_ERR(&error, S_CONTERR,
		      "s_read_utt_maryxml",
		      "Call to \"xmlInputClose\" failed"))
		return -1;
	return 0;
}

S_LOCAL SUtterance* s_read_utt_maryxml(const SDatasource *ds, s_erc *error)
{
	SUtterance * utt;
	char * input = NULL;
	SRelation * tokenRelation = NULL;
	SRelation * phraseRelation = NULL;
	SRelation * wordRelation = NULL;
	SRelation * syllableRelation = NULL;
	SRelation * sylStructRelation = NULL;
	SRelation * segmentRelation = NULL;
	xmlTextReaderPtr reader;
	int ret;

	S_CLR_ERR(error);

	/* Create SUtterance and relations */
	utt = S_NEW(SUtterance, error);
	if(S_CHK_ERR(error, S_CONTERR,
		       "s_read_utt_maryxml",
		       "Call to \"SUtteranceInit\" failed"))
		return NULL;

	tokenRelation = SUtteranceNewRelation(utt, "Token", error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "s_read_utt_maryxml",
		      "Call to \"SUtteranceNewRelation\" failed"))
		return NULL;

	wordRelation = SUtteranceNewRelation(utt, "Word", error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "s_read_utt_maryxml",
		      "Call to \"SUtteranceNewRelation\" failed"))
		return NULL;

	phraseRelation = SUtteranceNewRelation(utt, "Phrase", error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "s_read_utt_maryxml",
		      "Call to \"SUtteranceNewRelation\" failed"))
		return NULL;

	sylStructRelation = SUtteranceNewRelation(utt, "SylStructure", error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "s_read_utt_maryxml",
		      "Call to \"SUtteranceNewRelation\" failed"))
		return NULL;

	syllableRelation = SUtteranceNewRelation(utt, "Syllable", error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "s_read_utt_maryxml",
		      "Call to \"SUtteranceNewRelation\" failed"))
		return NULL;

	segmentRelation = SUtteranceNewRelation(utt, "Segment", error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "s_read_utt_maryxml",
		      "Call to \"SUtteranceNewRelation\" failed"))
		return NULL;

	/* Init reader to read from a datasource */
	reader = xmlReaderForIO(_ds_read,
				_ds_close,
				(void *) ds,
				"http://mary.dfki.de/2002/MaryXML",
				"UTF-8",
				0);

	if (reader == NULL)
	{
		S_CTX_ERR(error, S_CONTERR,
			  "s_read_utt_maryxml",
			  "Call to \"xmlReaderForIo\" failed");
		return NULL;
	}

	/* Start reading */
	ret = xmlTextReaderRead(reader);
	while (ret == 1)
	{
		int nodeType;
		const xmlChar * nameElement = xmlTextReaderConstName(reader);

		if (s_strcmp(nameElement, "s", error) == 1)
		{
			nodeType = xmlTextReaderNodeType(reader);
			if (nodeType == 1)
			{

			} else {
				if (nodeType == 15) {

				}
				else
					return NULL;
			}
		}

		if (S_CHK_ERR(error, S_CONTERR,
			      "s_read_utt_maryxml",
			      "Call to \"s_strcmp\" failed"))
			return NULL;




		ret = xmlTextReaderRead(reader);
	}

	xmlFreeTextReader(reader);
	if (ret != 0)
	{
		S_CTX_ERR(error, S_IOEOF,
			  "s_read_utt_maryxml",
			  "Call to \"xmlTextReaderRead\" failed");
		return NULL;
	}

	return utt;
}
