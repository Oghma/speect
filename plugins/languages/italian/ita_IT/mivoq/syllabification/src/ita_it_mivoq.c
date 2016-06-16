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
/* AUTHOR  : Giovanni Mazzocchin                                                    */
/* DATE    : June 2016                                                              */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* A Standard Italian (ita-it) syllabification class                                */
/* implementation for the Mivoq project. Based on Pietro Maturi,                    */
/* "I suoni delle lingue, i suoni dell'italiano"	                                */
/*                        							                                */
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

static SSyllabItaItMivoqClass SyllabItaItMivoqClass; /* SSyllabItaItMivoq class declaration. */


/************************************************************************************/
/*                                                                                  */
/*  Static function prototypes and types                                            */
/*                                                                                  */
/************************************************************************************/

static enum PhoneLevels {occlusive, fricative, nasal, lateral, s, vibrant, approximant, vowel, error_level};

static s_bool phone_is_vowel(const SPhoneset *phoneset, const char *phone, s_erc *error);

static s_bool phone_is_occlusive(const SPhoneset *phoneset, const char *phone, s_erc *error);

static s_bool phone_is_fricative(const SPhoneset *phoneset, const char *phone, s_erc *error);

static s_bool phone_is_nasal(const SPhoneset *phoneset, const char *phone, s_erc *error);

static s_bool phone_is_lateral(const SPhoneset *phoneset, const char *phone, s_erc *error);

static s_bool phone_is_s(const SPhoneset *phoneset, const char *phone, s_erc *error);

static s_bool phone_is_vibrant(const SPhoneset *phoneset, const char *phone, s_erc *error);

static s_bool phone_is_approximant(const SPhoneset *phoneset, const char *phone, s_erc *error);

static enum PhoneLevels phone_is_what(const SPhoneset* phoneset, const char* phone, s_erc *error);

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

/* b, bb, p, pp, k, kk, g, gg */
static s_bool phone_is_occlusive(const SPhoneset *phoneset, const char *phone, s_erc *error)
{
	s_bool present;
	
	S_CLR_ERR(error);
	present = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
														   "manner_plosive",
															error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "phone_is_occlusive",
				  "Call to method \"phone_has_feature\" failed"))
		return FALSE;
	
	if (present)
		return TRUE;
	
	return FALSE;
}


/* m, n, gn */
static s_bool phone_is_nasal(const SPhoneset *phoneset, const char *phone, s_erc *error)
{
	s_bool present;
	
	S_CLR_ERR(error);
	present = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
														   "manner_nasal",
														   error);
    if (S_CHK_ERR(error, S_CONTERR,
				  "phone_is_nasal",
				  "Call to method \"phone_has_feature\" failed"))
		return FALSE;
		
	if (present)
		return TRUE;
	
	return FALSE;
}

/* f, v, h, tS, etc... -> fricatives + affricates
 * manner_fricative but not 's' */
static s_bool phone_is_fricative(const SPhoneset *phoneset, const char *phone, s_erc *error)
{
	s_bool present;
	/* used for affricates */
	s_bool present_0;
	/* use to keep out 's', using phone_is_s internal function */
	s_bool present_1;
	
	
	S_CLR_ERR(error);
	present = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
														   "manner_fricative",
														    error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "phone_is_fricative",
				  "Call to method \"phone_has_feature\" failed"))
		return FALSE;
		
	present_0 = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
															 "manner_affricate",
															  error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "phone_is_fricative",
				  "Call to method \"phone_has_feature\" failed"))
		return FALSE;
														
	
	present_1 = phone_is_s(phoneset, phone, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "phone_is_fricative",
				  "Call to method \"phone_is_s\" failed"))
		return FALSE;
		
	if ((present || present_0) && !present_1)
		return TRUE;
		
	return FALSE;
}

/* l, LL, l, ll */
static s_bool phone_is_lateral(const SPhoneset *phoneset, const char *phone, s_erc *error)
{
	s_bool present;
	
	S_CLR_ERR(error);
	present = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
														   "manner_lateral",
															error);
															
	if (S_CHK_ERR(error, S_CONTERR,
			      "phone_is_lateral",
				  "Call to method \"phone_has_feature\" failed"))
		return FALSE;
	
	if (present)
		return TRUE;
	
	return FALSE;
}

/* phone_is_vibrant (r): here r is considered liquid and not lateral */
/* r */
static s_bool phone_is_vibrant(const SPhoneset *phoneset, const char *phone, s_erc *error)
{
	s_bool present;
	s_bool present_1;
	
	S_CLR_ERR(error);
	present = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
														   "manner_liquid",
															error);
															
	if (S_CHK_ERR(error, S_CONTERR,
			      "phone_is_vibrant",
				  "Call to method \"phone_has_feature\" failed"))
		return FALSE;
		
	present_1 = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
														     "manner_lateral",
															  error);															
	if (S_CHK_ERR(error, S_CONTERR,
			      "phone_is_vibrant",
				  "Call to method \"phone_has_feature\" failed"))
		return FALSE;
	
	
	
	if (present && !present_1)
		return TRUE;
	
	return FALSE;
}

/* s (voiceless and voiced (represented through 'z')) 
 * place_alveolar AND manner_strident */
static s_bool phone_is_s(const SPhoneset *phoneset, const char *phone, s_erc *error)
{
	s_bool present;
	s_bool present_1;
	
	S_CLR_ERR(error);
	present = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
														   "place_alveolar",
														   error);
	if (S_CHK_ERR(error, S_CONTERR,
			      "phone_is_s",
				  "Call to method \"phone_has_feature\" failed"))
		return FALSE;
	
	present_1 = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
														     "manner_strident",
															  error);															
	if (S_CHK_ERR(error, S_CONTERR,
			      "phone_is_s",
				  "Call to method \"phone_has_feature\" failed"))
		return FALSE;
		
	if (present && present_1)	
		return TRUE;
		
	return FALSE;										   
}

/* check for 'manner_approximant', but keep out r (using phone_is_vibrant) and laterals
 *  j, w */
static s_bool phone_is_approximant(const SPhoneset *phoneset, const char *phone, s_erc *error)
{
	s_bool present;
	s_bool present_1;
	s_bool present_2;
	
	S_CLR_ERR(error);
	present = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
														   "manner_approximant",
														   error);
	if (S_CHK_ERR(error, S_CONTERR,
			      "phone_is_approximant",
				  "Call to method \"phone_has_feature\" failed"))
		return FALSE;
	
	present_1 = phone_is_vibrant(phoneset, phone, error);
																
	if (S_CHK_ERR(error, S_CONTERR,
			      "phone_is_approximant",
				  "Call to method \"phone_has_feature\" failed"))
		return FALSE;		
		
	present_2 = S_PHONESET_CALL(phoneset, phone_has_feature)(phoneset, phone,
														     "manner_lateral",
														      error);
	if (S_CHK_ERR(error, S_CONTERR,
			      "phone_is_approximant",
				  "Call to method \"phone_has_feature\" failed"))
		return FALSE;
		
		
	if (present && !present_1 && !present_2)	
		return TRUE;
		
	return FALSE;										   
}

static enum PhoneLevels phone_is_what(const SPhoneset* phoneset, const char* phone, s_erc *error)
{
	/*  *** ITALIAN SOUNDS SERIES (in increasing intensity, with a symbolic number for comparing them):
	 * 	-consonants
	 * 	  << occlusives   (0) 
	 *    << fricatives   (1) 
	 *    << nasals 	  (2) 
	 *    << laterals 	  (3)
	 *    << 's' 		  (4)
	 *    << vibrants 	  (5) 
	 *    << approximants (6) 
	 *    << vowels		  (7)	
	 * 
	 * 	 EXCEPTIONS: 
	 * 		- occlusive + nasal (mainly in words of foreign origin)
	 * 		- occlusive + occlusive
	 * 		- lateral   + 's'
	 * 		- 'n'		+ 's'
	 * */
	
	enum PhoneLevels phone_level;
	
	s_bool is_vowel = phone_is_vowel(phoneset, phone, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "phone_is_what",
				  "Call to \"phone_is_vowel\" failed"))
		return error_level;
	
	s_bool is_occlusive = phone_is_occlusive(phoneset, phone, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "phone_is_what",
				  "Call to \"phone_is_occlusive\" failed"))
		return error_level;
	
	s_bool is_fricative = phone_is_fricative(phoneset, phone, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "phone_is_what",
				  "Call to \"phone_is_fricative\" failed"))
		return error_level;
	
	s_bool is_nasal = phone_is_nasal(phoneset, phone, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "phone_is_what",
				  "Call to \"phone_is_nasal\" failed"))
		return error_level;
		
	s_bool is_lateral = phone_is_lateral(phoneset, phone, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "phone_is_what",
				  "Call to \"phone_is_lateral\" failed"))
		return error_level;
	
	s_bool is_vibrant = phone_is_vibrant(phoneset, phone, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "phone_is_what",
				  "Call to \"phone_is_vibrant\" failed"))
		return error_level;
	
	s_bool is_s = phone_is_s(phoneset, phone, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "phone_is_what",
				  "Call to \"phone_is_s\" failed"))
		return error_level;
		
	s_bool is_approximant = phone_is_approximant(phoneset, phone, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "phone_is_what",
				  "Call to \"phone_is_approximant\" failed"))
		return error_level;

	if (is_vowel)
	{
		phone_level = vowel;
	}
	else if (is_occlusive)
	{
		phone_level = occlusive;
	}
	else if (is_fricative)
	{
		phone_level = fricative;
	}
	else if (is_nasal)
	{
		phone_level = nasal;
	}	
	else if (is_lateral)
	{
		phone_level = lateral;
	}
	else if (is_s)
	{
		phone_level = s;
	}
	else if (is_vibrant)
	{
		phone_level = vibrant;
	}
	else if (is_approximant)
	{
		phone_level = approximant;
	}
	
	return phone_level;
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
		
	SList *syllables = NULL;      /* should be returned */
	SList *syl;
	const SObject *tmp;
	const char *phone_string;
	
	/* size used for the first loop */
	size_t list_size;

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

	if (S_CHK_ERR(error, S_CONTERR,
				  "Syllabify",
				  "Call to \"SListCopy\" failed"))
		goto quit_error;
	
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
				  "Failed to create new 'SList' object"))
		goto quit_error;
    
    list_size = SListSize(phoneList, error);
    
    if (S_CHK_ERR(error, S_CONTERR,
				  "Syllabify",
				  "Call to \"SListSize\" failed"))
		goto quit_error;
	
	/* counter for the loop */
	sint32 i = 0;
	/* I need a list to keep the indexes marking the beginning of the syllables */
	SList* indexes = S_LIST(S_NEW(SListList, error)); 
    if (S_CHK_ERR(error, S_CONTERR,
				  "main",
				  "Call to \"S_NEW\" failed"))
	   goto quit_error;

	/* create an object containing the sint32 index */
	SObject* obj_ind = SObjectSetInt(i, error);
	SListAppend(indexes, obj_ind, error);
	if (S_CHK_ERR(error, S_CONTERR,
					  "Syllabify",
					  "Call to \"SListAppend\" failed"))
		goto quit_error;
			
	/* first loop for finding the division points, until the first vowel */
	while (i < list_size && !test_phone)
	{
		tmp = SListNth(phoneList, i, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "Syllabify",
					  "Call to \"SListNth\" failed"))
			goto quit_error;

		/* 'phone_string' is analyzed */
		phone_string = SObjectGetString(tmp, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "Syllabify",
					  "Call to \"SObjectGetString\" failed"))
			goto quit_error;
			
		test_phone = phone_is_vowel(phoneset, phone_string, error);
		i++;
	}
	
	/* this loop starts after the first vowel of 'phoneList' */
	while (i < list_size)
	{
		tmp = SListNth(phoneList, i, error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "Syllabify",
					  "Call to \"SListNth\" failed"))
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
			/* get the level of the current vowel at i, and start
			   checking for direction from here */
			enum PhoneLevels phone_level = vowel;
			if (S_CHK_ERR(error, S_CONTERR,
						  "Syllabify",
					      "Call to \"phone_is_vowel\" failed"))
				goto quit_error;
			
			/* variables used for direction control */
			enum PhoneLevels phone_level_p = phone_level;
			enum PhoneLevels phone_level_pp = phone_level_p;
			/* set it to TRUE when phonemes' series changes direction */
			s_bool direc_changed = FALSE;
			
			sint32 j = i - 1;
			SObject *tmp_1 = SListNth(phoneList, j, error);
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
				SObject* obj_ind = SObjectSetInt(j + 1, error);
				SListAppend(indexes, obj_ind, error);
				if (S_CHK_ERR(error, S_CONTERR,
							  "Syllabify",
						      "Call to \"SListAppend\" failed"))	
	     			    goto quit_error;
			}
			
			while (!test_vowel && !direc_changed)
			{
				tmp_1 = SListNth(phoneList, j, error);
				if (S_CHK_ERR(error, S_CONTERR,
		     				  "Syllabify",
						  "Call to \"SListNth\" failed"))
		    		goto quit_error;
					
				phone_string_1 = SObjectGetString(tmp_1, error);
				if (S_CHK_ERR(error, S_CONTERR,
							  "Syllabify",
							  "Call to \"SObjectGetString\" failed"))
				    goto quit_error;
				
				test_vowel = phone_is_vowel(phoneset, phone_string_1, error);
				if (S_CHK_ERR(error, S_CONTERR,
						     "Syllabify",
						     "Call to \"SObjectGetString\" failed"))
					goto quit_error;
				
			    	phone_level_pp = phone_level_p;
			    	phone_level_p = phone_level;
				phone_level = phone_is_what(phoneset, phone_string_1, error);
				if (S_CHK_ERR(error, S_CONTERR,
					   		  "Syllabify",
							  "Call to \"phone_is_what\" failed"))
					goto quit_error;
				
				/* check for previous direction */
				s_bool is_dec = phone_level_p < phone_level_pp;
				/* to adjacent approximants */	
				if ((phone_level_p == phone_level_pp) && (phone_level_p == approximant))
				{
					is_dec = TRUE;
				}
				s_bool is_dec_c = phone_level > phone_level_p;
				/* to adjacent approximants */	
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
				if ((phone_level == lateral) && (phone_level_p == s))
				{
					is_dec_c = TRUE;
				}
				/* the direction check only works with at least two previous elements,
				 * so it begins this control from j < i - 1 (so we have i - 1 and i) */			
				if (j < i - 1)
				{
					if (is_dec)
					{
						if (is_dec_c)
						{
							SObject* obj_ind = SObjectSetInt(j + 1, error);
							SListAppend(indexes, obj_ind, error);
							if (S_CHK_ERR(error, S_CONTERR,
			  							  "Syllabify",
										"Call to \"SListAppend\" failed"))
							    goto quit_error;
							
						    direc_changed = TRUE;
						}
					}
				}	
				j--;
			}/*end of inner while*/
		}
		
		i++;
	}/* end of outer while */		
      	  
	/* PROCEDURE EXAMPLE: indexes = [0, 2, 4, 7]
	 * - loop from 0 to 1 and add to syl, then add syl to syllables
	 * - loop from 2 to 3 and add to syl, then add syl to syllables
	 * - etc...
	 * *
	 */
	/*LAST LOOP*/	
	
	int indexes_size = SListSize(indexes, error);
	uint32 j = 0;
	
	while (j < indexes_size)
	{
		SObject* kObj = SListNth(indexes, j, error);
		SObject* kObj_1;
		if (j < indexes_size - 1)
		{
			 kObj_1 = SListNth(indexes, j + 1, error);
		}
		
		sint32 k = SObjectGetInt(kObj, error);
		sint32 k1;
		
		if (j < indexes_size - 1)
		{
			k1 = SObjectGetInt(kObj_1, error);
		}
		else
		{
			/* k1 = list_size (phoneList's size) */
			k1 = list_size;
		}
		
		syl = S_LIST(S_NEW(SListList, error));
		if (S_CHK_ERR(error, S_CONTERR,
					  "Syllabify",
					  "Call to \"S_LIST\" failed"))
			goto quit_error;
			
		while (k < k1)
		{
		    /* get the k-th element in phoneList as a SObject* */
			SObject *tmp = SListNth(phoneList, k, error);
			if (S_CHK_ERR(error, S_CONTERR,
						  "Syllabify",
						  "Call to \"SListNth\" failed"))
			    goto quit_error;
			
			/* get the string from the k-th element in phoneList*/
			char *phone_string = SObjectGetString(tmp, error);
			if (S_CHK_ERR(error, S_CONTERR,
						  "Syllabify",
						  "Call to \"SObjectGetString\" failed"))
				goto quit_error;
		    
		    /* create a SObject* with the k-th phone in phoneList, then push
		     * this SObject* to 'syl' SList, which should contains all the phonemes 
		     * in the current syllable */
		    
		    SObject* segm_object = SObjectSetString(phone_string, error);		    
		    SListPush(syl, segm_object, error);
		    if (S_CHK_ERR(error, S_CONTERR,
						  "Syllabify",
						  "Call to \"SListPush/SObjectSetString\" failed"))
				goto quit_error;	
			k++;
		}  //end of inner while
		
		/* push on 'syllables' list */
		SListPush(syllables, S_OBJECT(syl), error);
		if (S_CHK_ERR(error, S_CONTERR,
					  "Syllabify",
					  "Call to \"SListPush\" failed"))
			goto quit_error;
		
		j++;
	} /* end of outer while */
    
    return syllables;

quit_error:
	if (syllables != NULL)
		S_DELETE(syllables, "Syllabify", error);

	if (phoneList != NULL)
		S_DELETE(phoneList, "Syllabify", error);

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

