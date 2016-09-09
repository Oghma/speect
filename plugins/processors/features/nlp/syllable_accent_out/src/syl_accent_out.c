/************************************************************************************/
/* Copyright (c) 2010-2011 The Department of Arts and Culture,                      */
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
/* DATE    : March 2010                                                             */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* Syllable accent out feature processor.                                           */
/*                                                                                  */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* Modules used                                                                     */
/*                                                                                  */
/************************************************************************************/

#include "syl_accent_out.h"


/************************************************************************************/
/*                                                                                  */
/* Static variables                                                                 */
/*                                                                                  */
/************************************************************************************/

static SSylAccentOutFeatProcClass SylAccentOutFeatProcClass; /* SSylAccentOutFeatProc class declaration. */


/************************************************************************************/
/*                                                                                  */
/* Static function prototypes                                                       */
/*                                                                                  */
/************************************************************************************/

static s_bool syl_is_accented(const SItem *syl, s_erc *error);


/************************************************************************************/
/*                                                                                  */
/* Plug-in class registration/free                                                  */
/*                                                                                  */
/************************************************************************************/


/* local functions to register and free classes */
S_LOCAL void _s_syl_accent_out_class_reg(s_erc *error)
{
	S_CLR_ERR(error);
	s_class_reg(S_OBJECTCLASS(&SylAccentOutFeatProcClass), error);
	S_CHK_ERR(error, S_CONTERR,
			  "_s_syl_accent_out_class_reg",
			  "Failed to register SSylAccentOutFeatProcClass");
}


S_LOCAL void _s_syl_accent_out_class_free(s_erc *error)
{
	S_CLR_ERR(error);
	s_class_free(S_OBJECTCLASS(&SylAccentOutFeatProcClass), error);
	S_CHK_ERR(error, S_CONTERR,
			  "_s_syl_accent_out_class_free",
			  "Failed to free SSylAccentOutFeatProcClass");
}


/************************************************************************************/
/*                                                                                  */
/* Static function implementations                                                  */
/*                                                                                  */
/************************************************************************************/

/*
 * this is for Georg's accent levels:
 * unaccented : FALSE
 * accented   : TRUE
 */
static s_bool syl_is_accented(const SItem *syl, s_erc *error)
{
	const char *accent_feat;


	S_CLR_ERR(error);
	accent_feat = SItemGetString(syl, "accent", error);
	if (S_CHK_ERR(error, S_CONTERR,
					"syl_is_accented",
					"Call to \"SItemGetString\" failed"))
		return FALSE;

	if (s_strcmp(accent_feat, "unaccented", error) == 0)
	{
		S_CHK_ERR(error, S_CONTERR,
				  "syl_is_accented",
				  "Call to \"s_strcmp\" failed");
		return FALSE;
	}

	if (s_strcmp(accent_feat, "accented", error) == 0)
	{
		if (S_CHK_ERR(error, S_CONTERR,
					  "syl_is_accented",
					  "Call to \"s_strcmp\" failed"))
			return FALSE;

		return TRUE;
	}

	return FALSE; /* unknown */
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


static SObject *Run(const SFeatProcessor *self, const SItem *item,
					s_erc *error)
{
	SObject *extractedFeat = NULL;
	const SItem *phraseItem;
	const SItem *wordItem;
	const SItem *sylStructWordItem;
	const SItem *syllableItem;
	sint32 num_accented = 0;
	s_bool is_current_syl = FALSE;


	S_CLR_ERR(error);

	if (item == NULL)
		return NULL;

	/* get current phrase */
	phraseItem = SItemPathToItem(item, "R:SylStructure.parent.R:Phrase.parent",
								 error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SItemPathToItem\" failed"))
		goto quit_error;

	if (phraseItem == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "Run",
				  "Failed to get phrase of given syllable");
		goto quit_error;
	}

	/* last word in phrase */
	wordItem = SItemLastDaughter(phraseItem, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SItemLastDaughter\" failed"))
		goto quit_error;

	while (wordItem != NULL)
	{
		sylStructWordItem = SItemAs(wordItem, "SylStructure", error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "Run",
					  "Call to \"SItemAs\" failed"))
			goto quit_error;

		/* get syllables */
		syllableItem = SItemLastDaughter(sylStructWordItem, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "Run",
					  "Call to \"SItemLastDaughter\" failed"))
			goto quit_error;

		while (syllableItem != NULL)
		{
			s_bool is_accented;


			is_current_syl = SItemEqual(syllableItem, item, error);
			if (S_CHK_ERR(error, S_CONTERR,
						  "Run",
						  "Call to \"SItemEqual\" failed"))
				goto quit_error;

			if (is_current_syl)
				break;

			is_accented = syl_is_accented(syllableItem, error);
			if (S_CHK_ERR(error, S_CONTERR,
						  "Run",
						  "Call to \"syl_is_accented\" failed"))
				goto quit_error;

			if (is_accented)
				num_accented++;

			syllableItem = SItemPrev(syllableItem, error);
			if (S_CHK_ERR(error, S_CONTERR,
						  "Run",
						  "Call to \"SItemPrev\" failed"))
				goto quit_error;
		}

		if (is_current_syl)
			break;

		wordItem = SItemPrev(wordItem, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "Run",
					  "Call to \"SItemPrev\" failed"))
			goto quit_error;
	}

	extractedFeat = SObjectSetInt(num_accented, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SObjectSetInt\" failed"))
		goto quit_error;

	/* all OK here */
	return extractedFeat;

	/* error cleanup */
quit_error:
	if (extractedFeat != NULL)
		S_DELETE(extractedFeat, "Run", error);

	return NULL;

	S_UNUSED(self);
}


/************************************************************************************/
/*                                                                                  */
/* SwordDurFeatProc class initialization                                            */
/*                                                                                  */
/************************************************************************************/

static SSylAccentOutFeatProcClass SylAccentOutFeatProcClass =
{
	/* SObjectClass */
	{
		"SFeatProcessor:SSylAccentOutFeatProc",
		sizeof(SSylAccentOutFeatProc),
		{ 0, 1},
		NULL,            /* init    */
		NULL,            /* destroy */
		Dispose,         /* dispose */
		NULL,            /* compare */
		NULL,            /* print   */
		NULL,            /* copy    */
	},
	/* SFeatProcessorClass */
	NULL,               /* initialize */
	Run                  /* run     */
};
