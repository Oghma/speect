/************************************************************************************/
/* Copyright (c) 2009 The Department of Arts and Culture,                           */
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
/* DATE    : December 2009                                                          */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* A Normalization utterance processor.                                             */
/*                                                                                  */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* Modules used                                                                     */
/*                                                                                  */
/************************************************************************************/

#include "normalizer_proc.h"


/************************************************************************************/
/*                                                                                  */
/* Static variables                                                                 */
/*                                                                                  */
/************************************************************************************/

/* SNormalizationUttProc class declaration. */
static SNormalizationUttProcClass NormalizationUttProcClass;


/************************************************************************************/
/*                                                                                  */
/* Plug-in class registration/free                                                  */
/*                                                                                  */
/************************************************************************************/

/* local functions to register and free classes */
S_LOCAL void _s_normalization_utt_proc_class_reg(s_erc *error)
{
	S_CLR_ERR(error);
	s_class_reg(&NormalizationUttProcClass, error);
	S_CHK_ERR(error, S_CONTERR,
			  "_s_normalization_utt_proc_class_reg",
			  "Failed to register SNormalizationUttProcClass");
}


S_LOCAL void _s_normalization_utt_proc_class_free(s_erc *error)
{
	S_CLR_ERR(error);
	s_class_free(&NormalizationUttProcClass, error);
	S_CHK_ERR(error, S_CONTERR,
			  "_s_normalization_utt_proc_class_free",
			  "Failed to free SNormalizationUttProcClass");
}

/************************************************************************************/
/*                                                                                  */
/* Static class function implementations                                            */
/*                                                                                  */
/************************************************************************************/

static void Dispose(void *obj, s_erc *error)
{
	S_CLR_ERR(error);
	SObjectDecRef(obj);
}


static void Run(const SUttProcessor *self, SUtterance *utt,
				s_erc *error)
{
	const SVoice *voice;
	const SFeatProcessor *normProcessor = NULL;
	const SRelation *tokenRel;
	SRelation *wordRel = NULL;
	s_bool is_present;
	SItem *tokenItem;
	const SObject *token;
	SItem *wordItem;


	S_CLR_ERR(error);

	/* we require the token relation */
	is_present = SUtteranceRelationIsPresent(utt, "Token", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SUtteranceRelationIsPresent\" failed"))
		goto quit_error;

	if (!is_present)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "Run",
				  "Failed to find 'Token' relation in utterance");
		goto quit_error;
	}

	tokenRel = SUtteranceGetRelation(utt, "Token", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SUtteranceGetRelation\" failed"))
		goto quit_error;

	/* see if there is a normalization feature processor in the voice */
	voice = SUtteranceVoice(utt, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SUtteranceVoice\" failed"))
		goto quit_error;

	if (voice != NULL)
	{
		normProcessor = SVoiceGetFeatProc(voice, "word-normalization", error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "Run",
					  "Call to \"SVoiceGetFeatProc\" failed"))
			goto quit_error;
	}

	/* create Word relation */
	wordRel = SUtteranceNewRelation(utt, "Word", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SUtteranceNewRelation\" failed"))
		goto quit_error;

	/* start at the first item in the token relation, cast away
	 * const, we want to add daughter items
	 */
	tokenItem = (SItem*)SRelationHead(tokenRel, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SRelationHead\" failed"))
		goto quit_error;

	while (tokenItem != NULL)
	{
		/* Give the item to the normalization feature processor, if
		 * any. Normalization feature processor can set daughter items
		 * in Word relation, return would be NULL.
		 */
		if (normProcessor)
		{
			SFeatProcessorRun(normProcessor, tokenItem, error);
			if (S_CHK_ERR(error, S_CONTERR,
						  "Run",
						  "Call to \"SFeatProcessorRun\" failed"))
				goto quit_error;
		}
		else
		{
			/* we can't do normalization, hope for the best and set
			 * the token as the word. Token is name feature of token
			 * item.
			 */
			token = SItemGetObject(tokenItem, "name", error);
			if (S_CHK_ERR(error, S_CONTERR,
						  "Run",
						  "Call to \"SItemGetObject\" failed"))
				goto quit_error;

			/* create word item as daughter of token item, NULL
			 * shared content
			 */
			wordItem = SItemAddDaughter(tokenItem, NULL, error);
			if (S_CHK_ERR(error, S_CONTERR,
						  "Run",
						  "Call to \"SItemAddDaughter\" failed"))
				goto quit_error;

			/* set the token as the word item's name */
			SItemSetObject(wordItem, "name", token, error);
			if (S_CHK_ERR(error, S_CONTERR,
						  "Run",
						  "Call to \"SItemSetObject\" failed"))
				goto quit_error;

			/* and create a new word item in word relation, shared
			 * content it token relation's word item.
			 */
			SRelationAppend(wordRel, wordItem, error);
			if (S_CHK_ERR(error, S_CONTERR,
						  "Run",
						  "Call to \"SItemSetObject\" failed"))
				goto quit_error;
		}

		tokenItem = SItemNext(tokenItem, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "Run",
					  "Call to \"SItemNext\" failed"))
			goto quit_error;
	}

	/* here all is OK */
	return;

	/* error clean-up code */
quit_error:
	if (wordRel != NULL)
	{
		SUtteranceDelRelation(utt, "Word", error);
		S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SUtteranceDelRelation\" failed");
	}

	self = NULL; /* to suppress noise about unused parameters */
}


/************************************************************************************/
/*                                                                                  */
/* SNormalizationUttProc class initialization                                       */
/*                                                                                  */
/************************************************************************************/

static SNormalizationUttProcClass NormalizationUttProcClass =
{
	/* SObjectClass */
	{
		"SUttProcessor:SNormalizationUttProc",
		sizeof(SNormalizationUttProc),
		{ 0, 1},
		NULL,            /* init    */
		NULL,            /* destroy */
		Dispose,         /* dispose */
		NULL,            /* compare */
		NULL,            /* print   */
		NULL,            /* copy    */
	},
	/* SUttProcessorClass */
	NULL,                /* initialize */
	Run                  /* run        */
};
