/************************************************************************************/
/* Copyright (c) Mivoq SRL                                                          */
/*                                                                                  */
/* Contributors:  Giovanni Mazzocchin, Giulio Paci                                  */
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
/* AUTHOR  : Giovanni Mazzocchin, Giulio Paci                                       */
/* DATE    : June 2016                                                              */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* A Standard Italian (ita-it) syllabification class                                */
/* implementation for the Mivoq project. Based on Pietro Maturi,                    */
/* "I suoni delle lingue, i suoni dell'italiano"                                    */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* Modules used                                                                     */
/*                                                                                  */
/************************************************************************************/

#include <string.h>
#include "ita_it_mivoq.h"
#include "phoneset.h"


/************************************************************************************/
/*                                                                                  */
/* Data Types                                                                       */
/*                                                                                  */
/************************************************************************************/

/************************************************************************************/
/*                                                                                  */
/* Static variables                                                                 */
/*                                                                                  */
/************************************************************************************/
/* SSyllabItaItMivoq class declaration. */
static SSyllabItaItMivoqClass SyllabItaItMivoqClass;


/************************************************************************************/
/*                                                                                  */
/*  Static function prototypes and types                                            */
/*                                                                                  */
/************************************************************************************/

enum PhoneLevels {occlusive, fricative, nasal, lateral, s, vibrant, approximant, vowel, error_level};

static enum PhoneLevels phone_sonority_level(const SPhoneset* phoneset, const char* phone, s_erc *error);

/************************************************************************************/
/*                                                                                  */
/* Plug-in class registration/free                                                  */
/*                                                                                  */
/************************************************************************************/


/* local functions to register and free classes */
S_LOCAL void _s_syllab_ita_it_mivoq_class_reg(s_erc *error)
{
	S_CLR_ERR(error);
	s_class_reg(S_OBJECTCLASS(&SyllabItaItMivoqClass), error);
	S_CHK_ERR(error, S_CONTERR,
		  "_s_syllab_ita_it_mivoq_class_reg",
		  "Failed to register SSyllabItaItMivoqClass");
}


S_LOCAL void _s_syllab_ita_it_mivoq_class_free(s_erc *error)
{
	S_CLR_ERR(error);
	s_class_free(S_OBJECTCLASS(&SyllabItaItMivoqClass), error);
	S_CHK_ERR(error, S_CONTERR,
		  "_s_syllab_ita_it_mivoq_class_free",
		  "Failed to free SSyllabItaItMivoqClass");
}


/************************************************************************************/
/*                                                                                  */
/*  Static function implementations                                                 */
/*                                                                                  */
/************************************************************************************/

static s_bool phone_is_vowel(const SPhoneset *phoneset, const char *phone, s_erc *error)
{
	s_bool present;

	S_CLR_ERR(error);
	present = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
							       "vowel",
							       error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "phone_is_vowel",
		      "Call to method \"phone_has_feature\" failed"))
		return FALSE;

	if (present)
		return TRUE;

	return FALSE;
}
/**
 * This function return sonority level of phones, according to some of their features
 * the sonority level is mostly in increasing order, with the major exception of
 * strident + alveolar sounds (s and z) for which sonority level is higher than
 * expected. The exception has been introduced in order to cope with Italian
 * syllabification.
 *
 * Sonority scale:
 *    << occlusives       (0)
 *    << fricatives       (1)
 *    << nasals           (2)
 *    << laterals         (3)
 *    << 's'              (4)
 *    << vibrants         (5)
 *    << approximants     (6)
 *    << vowels           (7)
 *
 * By using this scale, Italian syllabification algorithm can be
 * simplified as follow: given a list of 0 or more consonants between
 * two vowels v1 and v2, the syllable containing v2 starts when the
 * sonority level does not decrease while moving backward on the
 * consonants sequence to v1.
 * With the following exceptions:
 *  - all approximants belong to the same syllable
 *  - occlusive + nasal (mainly in words of foreign origin) should be splitted
 *  - occlusive + occlusive
 *  - lateral   + 's' should be splitted
 *  - nasal + 's' should be splitted
 * */
static enum PhoneLevels phone_sonority_level(const SPhoneset* phoneset, const char* phone, s_erc *error)
{
	s_bool has_feature;
	S_CLR_ERR(error);

	/* Check if phone is a vowel */
	has_feature = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
							       "vowel",
							       error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "phone_sonority_level",
		      "Call to method \"phone_has_feature\" failed"))
		return error_level;
	if (has_feature)
		return vowel;
	/* Check if phone is an occlusive */
	has_feature = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
							       "manner_plosive",
							       error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "phone_sonority_level",
		      "Call to method \"phone_has_feature\" failed"))
		return error_level;
	if (has_feature)
		return occlusive;
	/* Check if phone is an nasal */
	has_feature = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
							       "manner_nasal",
							       error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "phone_sonority_level",
		      "Call to method \"phone_has_feature\" failed"))
		return error_level;
	if (has_feature)
		return nasal;
	/* Check if phone is an lateral */
	has_feature = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
							       "manner_lateral",
							       error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "phone_sonority_level",
		      "Call to method \"phone_has_feature\" failed"))
		return error_level;
	if (has_feature)
		return lateral;
	/* Check if phone is a vibrant (liquid+not lateral) */
	has_feature = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
							       "manner_liquid",
							       error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "phone_sonority_level",
		      "Call to method \"phone_has_feature\" failed"))
		return error_level;
	if (has_feature)
		return vibrant;
	/* Check if phone is an approximant (approximant+not vibrant+not lateral) */
	has_feature = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
							       "manner_approximant",
							       error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "phone_sonority_level",
		      "Call to method \"phone_has_feature\" failed"))
		return error_level;
	if (has_feature)
		return approximant;
	/* Check if phone is an s (strident+alveolar) */
	has_feature = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
							       "manner_strident",
							       error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "phone_sonority_level",
		      "Call to method \"phone_has_feature\" failed"))
		return error_level;
	if (has_feature) {
		has_feature = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
									   "place_alveolar",
									   error);
		if (S_CHK_ERR(error, S_CONTERR,
			      "phone_sonority_level",
			      "Call to method \"phone_has_feature\" failed"))
			return error_level;
		if (has_feature)
			return s;
	}
	/* Check if phone is a fricative ((fricative or affricate)+not s) */
	has_feature = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
							       "manner_fricative",
							       error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "phone_sonority_level",
		      "Call to method \"phone_has_feature\" failed"))
		return error_level;
	if (has_feature)
		return fricative;
	has_feature = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
							       "manner_affricate",
							       error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "phone_sonority_level",
		      "Call to method \"phone_has_feature\" failed"))
		return error_level;
	if (has_feature)
		return fricative;

	return error_level;
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


static const char *GetName(const SSyllabification *self, s_erc *error)
{
	S_CLR_ERR(error);

	if (self->info == NULL)
		return NULL;

	return (const char*)self->info->name;
}


static const char *GetDescription(const SSyllabification *self, s_erc *error)
{
	S_CLR_ERR(error);

	if (self->info == NULL)
		return NULL;

	return (const char*)self->info->description;
}


static const char *GetLanguage(const SSyllabification *self, s_erc *error)
{
	S_CLR_ERR(error);

	if (self->info == NULL)
		return NULL;

	return (const char*)self->info->language;
}


static const char *GetLangCode(const SSyllabification *self, s_erc *error)
{
	S_CLR_ERR(error);

	if (self->info == NULL)
		return NULL;

	return (const char*)self->info->lang_code;
}


static const s_version *SGetVersion(const SSyllabification *self, s_erc *error)
{
	S_CLR_ERR(error);

	if (self->info == NULL)
		return NULL;

	return (const s_version*)&(self->info->version);
}


static const SObject *GetFeature(const SSyllabification *self, const char *key,
				 s_erc *error)
{
	const SObject *feature;


	S_CLR_ERR(error);
	if (key == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
			  "GetFeature",
			  "Argument \"key\" is NULL");
		return NULL;
	}

	if (self->features == NULL)
		return NULL;

	feature = SMapGetObjectDef(self->features, key, NULL, error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "GetFeature",
		      "Call to \"SMapGetObjectDef\" failed"))
		return NULL;

	return feature;
}


static SList *_get_syllable(const SList*phoneList, sint32 start, sint32 end, s_erc *error) {
	SList *syl = NULL;
	SObject* segm_object = NULL;
	syl = S_LIST(S_NEW(SListList, error));
	if (S_CHK_ERR(error, S_CONTERR,
		      "_get_syllable",
		      "Call to \"S_LIST\" failed"))
		goto quit_error;

	sint32 k = start;
	while (k < end)
	{
		/* get the k-th element in phoneList as a SObject* */
		const SObject *tmp = SListNth(phoneList, k, error);
		if (S_CHK_ERR(error, S_CONTERR,
			      "_get_syllable",
			      "Call to \"SListNth\" failed"))
			goto quit_error;

		/* get the string from the k-th element in phoneList*/
		const char *phone_string = SObjectGetString(tmp, error);
		if (S_CHK_ERR(error, S_CONTERR,
			      "_get_syllable",
			      "Call to \"SObjectGetString\" failed"))
			goto quit_error;

		/* create a SObject* with the k-th phone in phoneList, then push
		 * this SObject* to 'syl' SList, which should contains all the phonemes
		 * in the current syllable */

		segm_object = SObjectSetString(phone_string, error);
		SListPush(syl, segm_object, error);
		if (S_CHK_ERR(error, S_CONTERR,
			      "_get_syllable",
			      "Call to \"SListPush/SObjectSetString\" failed"))
			goto quit_error;
		k++;
	}  //end of inner while
	return syl;
quit_error:
	if(segm_object != NULL) {
		S_DELETE(segm_object, "_get_syllable", error);
	}
	if (syl != NULL) {
		S_DELETE(syl, "_get_syllable", error);
	}
	return NULL;
}

/**
 * return a vallist of vallists where the primary list is syllables and the secondary
 * lists are the phones in the syllables. for example :
 * for mathematics phonelist is : (m , ae , th, ax, m, ae, t, ih, k, s)
 * syllfunc returns : ((m, ae), (th, ax), (m, ae), (t, ih, k, s))
 */

static SList *Syllabify(const SSyllabification *self, const SItem *word,
			const SList *phoneList, s_erc *error)
{
	const SPhoneset *phoneset;
	const SVoice *voice;

	/* should be returned */
	SList *syllables = NULL;
	SList *syl;
	const SObject *tmp;
	const char *phone_string;

	/* store for 'is_...' functions */
	s_bool test_phone = FALSE;


	S_CLR_ERR(error);

	if (word == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
			  "Syllabify",
			  "Argument \"word\" is NULL");
		goto quit_error;
	}

	if (phoneList == NULL)
	{
		S_CTX_ERR(error, S_ARGERROR,
			  "Syllabify",
			  "Argument \"phoneList\" is NULL");
		goto quit_error;
	}

	/* get the voice */
	voice = SItemVoice(word, error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "Syllabify",
		      "Call to \"SItemVoice\" failed"))
		goto quit_error;

	if (voice == NULL)
	{
		S_CTX_ERR(error, S_CONTERR,
			  "Syllabify",
			  "Syllabification requires a voice, through the voice "
			  "syllabification gets access to a phoneset");
		goto quit_error;
	}

	/* get the phoneset */
	phoneset = (SPhoneset*)SVoiceGetData(voice, "phoneset", error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "Syllabify",
		      "Call to \"SVoiceGetFeature\" failed"))
		goto quit_error;

	if (phoneset == NULL)
	{
		S_CTX_ERR(error, S_CONTERR,
			  "Syllabify",
			  "Syllabification requires a phoneset, none found in voice features");
		goto quit_error;
	}

	/* create syllables list */
	syllables = S_LIST(S_NEW(SListList, error));
	if (S_CHK_ERR(error, S_CONTERR,
		      "Syllabify",
		      "Failed to create new 'SListList' object"))
		goto quit_error;


	/* counter for the loop */
	size_t i = 0;
	SIterator *itr_phoneList = S_ITERATOR_GET(phoneList, error);
	if (S_CHK_ERR(error, S_CONTERR,
		      "Syllabify",
		      "Call to \"S_ITERATOR_GET\" failed"))
		goto quit_error;

	size_t last_head = 0;

	/* first loop for finding the division points, until the first vowel */
	while (itr_phoneList != NULL  && !test_phone)
	{
		tmp = SIteratorObject(itr_phoneList, error);
		if (S_CHK_ERR(error, S_CONTERR,
				  "Syllabify",
				  "Call to method \"SIteratorObject\" failed"))
			goto quit_error;

		/* 'phone_string' is analyzed */
		phone_string = SObjectGetString(tmp, error);
		if (S_CHK_ERR(error, S_CONTERR,
			      "Syllabify",
			      "Call to \"SObjectGetString\" failed"))
			goto quit_error;

		test_phone = phone_is_vowel(phoneset, phone_string, error);

		itr_phoneList = SIteratorNext(itr_phoneList);
		i++;
	}

	/* this loop starts after the first vowel of 'phoneList' */
	while (itr_phoneList != NULL)
	{
		tmp = SIteratorObject(itr_phoneList, error);
		if (S_CHK_ERR(error, S_CONTERR,
			      "Syllabify",
			      "Call to \"SIteratorObject\" failed"))
			goto quit_error;

		/* 'phone_string' is analyzed (using 'phone_is_*' functions) */
		phone_string = SObjectGetString(tmp, error);
		if (S_CHK_ERR(error, S_CONTERR,
			      "Syllabify",
			      "Call to \"SObjectGetString\" failed"))
			goto quit_error;


		/* test for each vowel */
		s_bool is_vowel = phone_is_vowel(phoneset, phone_string, error);
		if (S_CHK_ERR(error, S_CONTERR,
			      "Syllabify",
			      "Call to \"phone_is_vowel\" failed"))
			goto quit_error;

		/* if there was a vowel in the i-th position, begin a backward going loop */
		if (is_vowel)
		{
			/* variables used for direction control */
			/* set the initial sonority level to vowel */
			enum PhoneLevels phone_level = vowel;
			enum PhoneLevels phone_level_p = phone_level;

			size_t j = i - 1;
			const SObject *tmp_1 = SListNth(phoneList, j, error);
			if (S_CHK_ERR(error, S_CONTERR,
				      "Syllabify",
				      "Call to \"SListNth\" failed"))
				goto quit_error;

			const char* phone_string_1 = SObjectGetString(tmp_1, error);
			if (S_CHK_ERR(error, S_CONTERR,
				      "Syllabify",
				      "Call to \"SObjectGetString\" failed"))
				goto quit_error;
			/* stay in the loop as long as the phone at j isn't a vowel */
			s_bool test_vowel = phone_is_vowel(phoneset, phone_string_1, error);
			if (S_CHK_ERR(error, S_CONTERR,
				      "Syllabify",
				      "Call to \"phone_is_vowel\" failed"))
				goto quit_error;

			/* this check solves the problem with adjacent vowels */
			if (test_vowel)
			{
				syl = _get_syllable(phoneList, last_head, j+1, error);
				if (S_CHK_ERR(error, S_CONTERR,
					      "Syllabify",
					      "Call to \"_get_syllable\" failed"))
					goto quit_error;

				/* push on 'syllables' list */
				SListPush(syllables, S_OBJECT(syl), error);
				if (S_CHK_ERR(error, S_CONTERR,
					      "Syllabify",
					      "Call to \"SListPush\" failed"))
					goto quit_error;

				last_head = j+1;
			}
			s_bool is_dec_c = FALSE;
			while (!test_vowel && !is_dec_c)
			{
				const SObject *tmp_2 = SListNth(phoneList, j, error);

				if (S_CHK_ERR(error, S_CONTERR,
					      "Syllabify",
					      "Call to \"SListNth\" failed"))
					goto quit_error;

				phone_string_1 = SObjectGetString(tmp_2, error);

				if (S_CHK_ERR(error, S_CONTERR,
					      "Syllabify",
					      "Call to \"SObjectGetString\" failed"))
					goto quit_error;

				test_vowel = phone_is_vowel(phoneset, phone_string_1, error);
				if (S_CHK_ERR(error, S_CONTERR,
					      "Syllabify",
					      "Call to \"SObjectGetString\" failed"))
					goto quit_error;

				phone_level_p = phone_level;
				phone_level = phone_sonority_level(phoneset, phone_string_1, error);
				if (S_CHK_ERR(error, S_CONTERR,
					      "Syllabify",
					      "Call to \"phone_sonority_level\" failed"))
					goto quit_error;

				/* check for previous direction */
				is_dec_c = phone_level > phone_level_p;
				/* two adjacent approximants should not be splitted */
				if ((phone_level == phone_level_p) && (phone_level != approximant))
				{
					is_dec_c = TRUE;
				}
				if ((phone_level == occlusive) && (phone_level_p == nasal))
				{
					is_dec_c = TRUE;
				}
				if ((phone_level == nasal) && (phone_level_p == s))
				{
					is_dec_c = TRUE;
				}
				if ((phone_level == nasal) && (phone_level_p == lateral))
				{
					is_dec_c = TRUE;
				}
				if ((phone_level == lateral) && (phone_level_p == s))
				{
					is_dec_c = TRUE;
				}
				/* the direction check only works with at least two previous elements,
				 * so it begins this control from j < i - 1 (so we have i - 1 and i) */
				if (j < i - 1)
				{
					if (is_dec_c)
					{
						syl = _get_syllable(phoneList, last_head, j+1, error);
						if (S_CHK_ERR(error, S_CONTERR,
							      "Syllabify",
							      "Call to \"_get_syllable\" failed"))
							goto quit_error;

						/* push on 'syllables' list */
						SListPush(syllables, S_OBJECT(syl), error);
						if (S_CHK_ERR(error, S_CONTERR,
							      "Syllabify",
							      "Call to \"SListPush\" failed"))
							goto quit_error;

						last_head = j+1;
					}
				}
				j--;
			}/*end of inner while*/
		}

		itr_phoneList = SIteratorNext(itr_phoneList);
		i++;
	}/* end of outer while */
	if ((size_t)last_head < i)
	{
		syl = _get_syllable(phoneList, last_head, i, error);
		if (S_CHK_ERR(error, S_CONTERR,
			      "Syllabify",
			      "Call to \"_get_syllable\" failed"))
			goto quit_error;

		/* push on 'syllables' list */
		SListPush(syllables, S_OBJECT(syl), error);
		if (S_CHK_ERR(error, S_CONTERR,
			      "Syllabify",
			      "Call to \"SListPush\" failed"))
			goto quit_error;
	}

	return syllables;

quit_error:
	if (syllables != NULL)
		S_DELETE(syllables, "Syllabify", error);

	if (itr_phoneList != NULL)
		S_DELETE(itr_phoneList, "Syllabify", error);

	return NULL;

	S_UNUSED(self);
}


/************************************************************************************/
/*                                                                                  */
/* SSyllabItaItMivoq class initialization                                           */
/*                                                                                  */
/************************************************************************************/

static SSyllabItaItMivoqClass SyllabItaItMivoqClass =
{
	/* SObjectClass */
	{
		"SSyllabification:SSyllabItaItMivoq",
		sizeof(SSyllabItaItMivoq),
		{ 0, 1},
		NULL,            /* init    */
		NULL,            /* destroy */
		Dispose,         /* dispose */
		NULL,            /* compare */
		NULL,            /* print   */
		NULL,            /* copy    */
	},
	/* SSyllabificationClass */
	GetName,             /* get_name        */
	GetDescription,      /* get_description */
	GetLanguage,         /* get_language    */
	GetLangCode,         /* get_lang_code   */
	SGetVersion,         /* get_version     */
	GetFeature,          /* get_feature     */
	Syllabify            /* syllabify       */
};
