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
/* AUTHOR  : Simone Daminato                                                        */
/* DATE    : January 2016                                                           */
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* Generate HTS Labels for italian voices.                                          */
/*                                                                                  */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* Modules used                                                                     */
/*                                                                                  */
/************************************************************************************/

#include "hts_label_generator.h"
#include "hts_data_collector.h"


/************************************************************************************/
/*                                                                                  */
/* Defines                                                                          */
/*                                                                                  */
/************************************************************************************/

#define MAX_PHONE_NAME_LENGTH 8

/**
 * @hideinitializer
 * @def EXTRACTFEATURE
 * Extract from @a data the feature with key @a featureName.
 * The feature then is unboxed using the function @a unboxingFunction,
 * a string is generated as defined by @a outputString, and appended to
 * @a appendTo.
 * If the feature @a featureName is not present, @a defaultOutput is appended
 * to @a appendTo instead.
 *
 * @param data Data container (type #SHTSLabelsDataCollectorFeatProc*).
 * @param featureName Key of the feature (type #char*).
 * @param unboxingFunction Function used to get data from the resulting SObject (type #void*).
 * @param outputString String to format the feature (type #char*).
 * @param defaultOutput Default string output, used if there isn't the given feature (type #char*).
 * @param appendTo String on which we append the output (type #char*).
 * @param ERROR Pointer to error code variable to set (type #s_erc*).
 */
#define EXTRACTFEATURE(data, featureName, unboxingFunction, outputString, appendTo, error) 			\
do {														\
	char *tmp = NULL;											\
	do {													\
		const SObject *feature;										\
		char *featureStr = featureName;									\
		char *outputStringLoc = outputString;								\
		s_bool isPresent;										\
		S_CLR_ERR(error);										\
														\
		isPresent= S_HTSLABELSDATACOLLECTOR_CALL(data, has_feature)(data, featureName, error);		\
		if (S_CHK_ERR(error, S_CONTERR,									\
					"EXTRACTFEATURE",							\
					"Call to \"SHTSLabelDataCollectorFeatureIsPresent\" failed"))		\
			break;											\
		if (isPresent)											\
		{												\
			feature = S_HTSLABELSDATACOLLECTOR_CALL(data, get_feature)(data, featureName, error);	\
			if (S_CHK_ERR(error, S_CONTERR,								\
					"EXTRACTFEATURE",							\
					"Call to \"SHTSLabelDataCollectorGetFeature\" failed"))			\
				break;										\
			CheckFeature(self, &featureStr, error);							\
			/* now featureStr contains the hts code */     						\
			s_asprintf(&outputStringLoc, error, featureStr, unboxingFunction(feature, error));	\
			if (S_CHK_ERR(error, S_CONTERR,	       							\
					"EXTRACTFEATURE",	       						\
					"Call to \"CheckFeature\" failed"))					\
					break;									\
			s_asprintf(&tmp, error, outputStringLoc/* , unboxingFunction(feature, error) */);	\
		}												\
		s_sappend(&appendTo, tmp, error);								\
		if (S_CHK_ERR(error, S_CONTERR,									\
					"EXTRACTFEATURE",							\
					"Call to \"s_sappend\" failed"))					\
			break;											\
	} while(0);												\
	if (tmp != NULL) S_FREE(tmp);										\
} while(0);


/**
 * @hideinitializer
 * @def EXTRACTPHONEME
 * Extract from @a data the phoneme with key @a featureName.
 * The feature then is unboxed, fixed using @c CheckPhoneme, and a string is generated
 * as defined by @a outputString, and appended to @a appendTo.
 * If the feature @a featureName is not present,  @a nullPhoneme (taken from @c self)
 * is used instead.
 *
 * @param self self pointer (type #SHTSLabelsGeneratorItFeatProc*)
 * @param data Data container (type #SHTSLabelsDataCollectorFeatProc*).
 * @param featureName Key of the feature (type #char*).
 * @param outputString String to format the feature (type #char*).
 * @param appendTo String on which we append the output (type #char*).
 * @param ERROR Pointer to error code variable to set (type #s_erc*).
 */
#define EXTRACTPHONEME(self, data, featureName, outputString, appendTo, error)					\
do {														\
	char *tmp = NULL;											\
	do {													\
		const SObject *feature;										\
		char *phone;											\
		s_bool isPresent;										\
		S_CLR_ERR(error);										\
														\
		isPresent= S_HTSLABELSDATACOLLECTOR_CALL(data, has_feature)(data, featureName, error);		\
		if (S_CHK_ERR(error, S_CONTERR,									\
					"EXTRACTPHONEME",							\
					"Call to \"SHTSLabelDataCollectorFeatureIsPresent\" failed"))		\
			break;											\
		if (isPresent)											\
		{												\
			feature = S_HTSLABELSDATACOLLECTOR_CALL(data, get_feature)(data, featureName, error);	\
			if (S_CHK_ERR(error, S_CONTERR,								\
					"EXTRACTPHONEME",							\
					"Call to \"SHTSLabelDataCollectorGetFeature\" failed"))			\
				break;										\
			phone = (char*) SObjectGetString(feature, error);					\
			if (S_CHK_ERR(error, S_CONTERR,								\
					"EXTRACTPHONEME",							\
					"Call to \"SObjectGetString\" failed"))					\
				break;										\
			CheckPhoneme(self, &phone, error);							\
			if (S_CHK_ERR(error, S_CONTERR,								\
					"EXTRACTPHONEME",							\
					"Call to \"CheckPhoneme\" failed"))					\
				break;										\
			s_asprintf(&tmp, error, outputString, phone);						\
		}												\
		else												\
		{												\
			s_asprintf(&tmp, error, outputString, self->nullPhoneme);				\
			if (S_CHK_ERR(error, S_CONTERR,								\
					"EXTRACTPHONEME",							\
					"Call to \"s_asprintf\" failed"))					\
			break;											\
		}												\
		s_sappend(&appendTo, tmp, error);								\
		if (S_CHK_ERR(error, S_CONTERR,									\
					"EXTRACTPHONEME",							\
					"Call to \"s_sappend\" failed"))					\
			break;											\
	} while(0);												\
	if (tmp != NULL) S_FREE(tmp);										\
} while(0);


/************************************************************************************/
/*                                                                                  */
/* Static variables                                                                 */
/*                                                                                  */
/************************************************************************************/

/* SHTSLabelsConfigurableFeatProc class declaration. */
static SHTSLabelsGeneratorItFeatProcClass HTSLabelsGeneratorItFeatProcClass;


/************************************************************************************/
/*                                                                                  */
/* Static function prototypes                                                       */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* Function implementations                                                         */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* Plug-in class registration/free                                                  */
/*                                                                                  */
/************************************************************************************/


/* local functions to register and free classes */
S_LOCAL void _s_hts_labels_generator_class_reg(s_erc *error)
{
	S_CLR_ERR(error);
	s_class_reg(S_OBJECTCLASS(&HTSLabelsGeneratorItFeatProcClass), error);
	S_CHK_ERR(error, S_CONTERR,
			  "_s_hts_labels_generator_class_reg",
			  "Failed to register SHTSLabelsGeneratorItFeatProcClass");
}


S_LOCAL void _s_hts_labels_generator_class_free(s_erc *error)
{
	S_CLR_ERR(error);
	s_class_free(S_OBJECTCLASS(&HTSLabelsGeneratorItFeatProcClass), error);
	S_CHK_ERR(error, S_CONTERR,
			  "_s_hts_labels_configurable_class_free",
			  "Failed to free SHTSLabelsGeneratorItFeatProcClass");
}


/************************************************************************************/
/*                                                                                  */
/* Static function implementations                                                  */
/*                                                                                  */
/************************************************************************************/


/************************************************************************************/
/*                                                                                  */
/* Static class function implementations                                            */
/*                                                                                  */
/************************************************************************************/
static void LoadConfiguration(SHTSLabelsGeneratorItFeatProc *self, const SItem *item, s_erc *error)
{
	const SVoice *voice;
	const SMap *labelsGenerator_data;

	S_CLR_ERR(error);

	voice = SItemVoice(item, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "LoadConfiguration",
				  "Call to \"SItemVoice\" failed"))
		return;

	if (voice == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "LoadConfiguration",
				  "Item voice is NULL, voice is required to get phoneset");
		return;
	}

	/* get the hunpos data map */
	labelsGenerator_data = S_MAP(SVoiceGetFeature(voice, "labels generator data", error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "LoadConfiguration",
				  "Call to \"SVoiceGetFeature\" failed"))
		return;

	if (labelsGenerator_data == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "LoadConfiguration",
				  "Failed to get \"labels generator data\" map from voice features");
		return;
	}

	self->specialPhones = S_MAP(SMapGetObject(labelsGenerator_data, "phones_map", error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "LoadConfiguration",
				  "Call to \"SMapGetObject\" failed"))
		return;
	if (self->specialPhones == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "LoadConfiguration",
				  "Failed to get \"phones_map\" map from voice features");
		return;
	}

	self->specialFeatures = S_MAP(SMapGetObject(labelsGenerator_data, "features_map_int", error));
	if (S_CHK_ERR(error, S_CONTERR,
				  "LoadConfiguration",
				  "Call to \"SMapGetObject\" failed"))
		return;

	if (self->specialFeatures == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "LoadConfiguration",
				  "Failed to get \"features_map\" map from voice features");
		return;
	}

	self->nullPhoneme = SMapGetString(labelsGenerator_data, "null_phone", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "LoadConfiguration",
				  "Call to \"SMapGetString\" failed"))
		return;
	if (self->nullPhoneme == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "LoadConfiguration",
				  "Failed to get \"null_phone\" string from voice features");
		return;
	}

}

static void CheckPhoneme(SHTSLabelsGeneratorItFeatProc *self, char **phone, s_erc *error)
{
	S_CLR_ERR(error);

	*phone = (char*) SMapGetStringDef(self->specialPhones, *phone, *phone, error);
}

static void CheckFeature(SHTSLabelsGeneratorItFeatProc *self, char **feature, s_erc *error)
{
	S_CLR_ERR(error);

	*feature = (char*) SMapGetStringDef(self->specialFeatures, *feature, *feature, error);
}

static void Init(void *obj, s_erc *error)
{
	S_CLR_ERR(error);

	SHTSLabelsGeneratorItFeatProc* self = (SHTSLabelsGeneratorItFeatProc*) obj;
	self->specialPhones = NULL;
	self->nullPhoneme = NULL;
}

static void Dispose(void *obj, s_erc *error)
{
	S_CLR_ERR(error);

	SObjectDecRef(obj);
}

static void Destroy(void *obj, s_erc *error)
{
	S_CLR_ERR(error);

	SHTSLabelsGeneratorItFeatProc* self = (SHTSLabelsGeneratorItFeatProc*) obj;
	self->specialPhones = NULL;
	self->nullPhoneme = NULL;
}

static SObject *Run(const SFeatProcessor *self, const SItem *item,
					s_erc *error)
{
	SHTSLabelsGeneratorItFeatProc* labelGenerator = (SHTSLabelsGeneratorItFeatProc*) self;
	const SHTSLabelsDataCollectorFeatProc *data;
	char *resultString = NULL;
	char *tmp = NULL;

	S_CLR_ERR(error);

	/* load configuration if it's the first run */
	if (labelGenerator->specialPhones == NULL || labelGenerator->nullPhoneme == NULL)
	{
		LoadConfiguration(labelGenerator, item, error);
		if (labelGenerator->specialPhones == NULL)
		{
			S_CTX_ERR(error, S_FAILURE,
				  "Run",
				  "Failed to load special phones list.");
			goto quit_error;
		}
		if (labelGenerator->nullPhoneme == NULL)
		{
			S_CTX_ERR(error, S_FAILURE,
				  "Run",
				  "Failed to load null phone string.");
			goto quit_error;
		}
	}

	SItem* segItem = SItemAs(item, "Segment", error);

	data = (SHTSLabelsDataCollectorFeatProc *) SItemPathToFeatProc(segItem, "hts_label_data_collector", error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Call to \"SItemPathToFeatProc\" failed"))
		goto quit_error;

	if (data == NULL)
	{
		S_CTX_ERR(error, S_FAILURE,
				  "Run",
				  "data is NULL, HTS label data collector has encountered an error.");
		goto quit_error;
	}

	if (! S_HTSLABELSDATACOLLECTOR_METH_VALID(data, has_feature))
	{
		S_CTX_ERR(error, S_FAILURE,
				  "Run",
				  "labelsGenerator's has_feature method is NULL.");
		goto quit_error;
	}

	if (! S_HTSLABELSDATACOLLECTOR_METH_VALID(data, get_feature))
	{
		S_CTX_ERR(error, S_FAILURE,
				  "Run",
				  "labelsGenerator's get_feature method is NULL.");
		goto quit_error;
	}

	/**** phone context ****/
	/* p1 */
	EXTRACTPHONEME(labelGenerator, data, "p.p.phone", "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTPHONEME\" failed"))
		goto quit_error;

	/* p2 */
	EXTRACTPHONEME(labelGenerator, data, "p.phone", "^%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTPHONEME\" failed"))
		goto quit_error;

	/* p3 */
	EXTRACTPHONEME(labelGenerator, data, "phone", "-%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTPHONEME\" failed"))
		goto quit_error;

	/* p4 */
	EXTRACTPHONEME(labelGenerator, data, "n.phone", "+%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTPHONEME\" failed"))
		goto quit_error;

	/* p5 */
	EXTRACTPHONEME(labelGenerator, data, "n.n.phone", "=%s||", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTPHONEME\" failed"))
		goto quit_error;

	/* word.phones.num */
	EXTRACTFEATURE(data, "word.phones.num", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\" failed"))
		goto quit_error;

	/**** syllable context ****/
	/* phones.from.syl.start */
	EXTRACTFEATURE(data, "phones.from.syl.start", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
			      "Run",
				  "Execution of \"EXTRACTFEATURE\" failed"))
		goto quit_error;

	/* phones.from.syl.end */
	EXTRACTFEATURE(data, "phones.from.syl.end", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\", failed"))
		goto quit_error;

	/* syl.phones.num */
	EXTRACTFEATURE(data, "syl.phones.num", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\", failed"))
		goto quit_error;

	/**** word context ****/
	/* syls.from.word.start */
	EXTRACTFEATURE(data, "syl.from.word.start", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\", failed"))
		goto quit_error;

	/* syls.from.word.end */
	EXTRACTFEATURE(data, "syl.from.word.end", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\", failed"))
		goto quit_error;

	/* word.syls.num */
	EXTRACTFEATURE(data, "word.syls.num", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\", failed"))
		goto quit_error;

	/* word.phones.num */
	EXTRACTFEATURE(data, "word.phones.num", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\", failed"))
		goto quit_error;


	/* f?: word.gpos
	   cannot find the hts f# code for this feature */

	/**** phrase context ****/
	/* syls.from.phrase.start */
	EXTRACTFEATURE(data, "syls.from.phrase.start", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\", failed"))
		goto quit_error;

	/* syls.from.phrase.end */
	EXTRACTFEATURE(data, "syls.from.phrase.end", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\", failed"))
		goto quit_error;

	/* phrase.syls.num */
	EXTRACTFEATURE(data, "phrase.syls.num", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\", failed"))
		goto quit_error;

	/* phrase.words.num*/
	EXTRACTFEATURE(data, "phrase.words.num", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\", failed"))
		goto quit_error;

	/**** stress context ****/
	/* stress */
	EXTRACTFEATURE(data, "stress", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\", failed"))
		goto quit_error;

	/* stressed.syls.before */
	/* EXTRACTFEATURE(data, "stressed.syls.before", SObjectGetInt, "f44=%d|", "f44=0|", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\", failed"))
		goto quit_error;
	*/

	/* stressed.syls.after */
	EXTRACTFEATURE(data, "stressed.syls.after", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\", failed"))
		goto quit_error;

	/* syls.from.prev.stress */
	EXTRACTFEATURE(data, "syls.from.prev.stress", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\", failed"))
		goto quit_error;

	/* syls.to.next.stress */
	EXTRACTFEATURE(data, "syls.to.next.stress", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\", failed"))
		goto quit_error;

	/**** utterance context ****/
	/* phrases.from.utt.start */
	EXTRACTFEATURE(data, "phrases.from.utt.start", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\", failed"))
		goto quit_error;

	/* phrases.from.utt.end */
	EXTRACTFEATURE(data, "phrases.from.utt.end", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\", failed"))
		goto quit_error;


	/* utterance.words.num */
	EXTRACTFEATURE(data, "utterance.words.num", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\", failed"))
		goto quit_error;

	/* utterance.phrases.num */
	EXTRACTFEATURE(data, "utterance.phrases.num", SObjectGetInt, "%s", resultString, error);
	if (S_CHK_ERR(error, S_CONTERR,
				  "Run",
				  "Execution of \"EXTRACTFEATURE\", failed"))
		goto quit_error;

	s_sappend(&resultString, "|", error);
	return SObjectSetString(resultString, error);

	/* error cleanup */
quit_error:
	S_DELETE(data, "Run", error);
	S_FREE(tmp);
	S_FREE(resultString);
	return NULL;

	S_UNUSED(self);
}


/************************************************************************************/
/*                                                                                  */
/* SHTSLabelsConfigurableFeatProc class initialization                              */
/*                                                                                  */
/************************************************************************************/

static SHTSLabelsGeneratorItFeatProcClass HTSLabelsGeneratorItFeatProcClass =
{
	/* SObjectClass */
	{
		"SFeatProcessor:SHTSLabelsGeneratorItFeatProc",
		sizeof(SHTSLabelsGeneratorItFeatProc),
		{ 0, 1},
		Init,            /* init    */
		Destroy,         /* destroy */
		Dispose,         /* dispose */
		NULL,            /* compare */
		NULL,            /* print   */
		NULL,            /* copy    */
	},
	/* SFeatProcessorClass */
	Run                  /* run     */
};
