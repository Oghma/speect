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

#include "ita_it_mivoq.h"
#include "plugin_info.h"


/************************************************************************************/
/*                                                                                  */
/* Static function prototypes                                                       */
/*                                                                                  */
/************************************************************************************/

static void plugin_register_function(s_erc *error);

static void plugin_exit_function(s_erc *error);


/************************************************************************************/
/*                                                                                  */
/* Plug-in parameters                                                               */
/*                                                                                  */
/************************************************************************************/

/* The datas for this structure are defined in CMakeLists.txt */
static const s_plugin_params plugin_params =
{
	/* plug-in name, taken from CMakeLists.txt */
	SPCT_PLUGIN_NAME,

	/* description */
	SPCT_PLUGIN_DESCRIPTION,

	/* version */
	{
		SPCT_PLUGIN_VERSION_MAJOR,
		SPCT_PLUGIN_VERSION_MINOR
	},

	/* Speect ABI version (which plug-in was compiled with) */
	{
		S_MAJOR_VERSION,
		S_MINOR_VERSION
	},

	/* register function pointer */
	plugin_register_function,

	/* exit function pointer */
	plugin_exit_function
};


/************************************************************************************/
/*                                                                                  */
/* Function implementations                                                         */
/*                                                                                  */
/************************************************************************************/

S_PLUGIN_API const s_plugin_params *s_plugin_init(s_erc *error)
{
	S_CLR_ERR(error);

	if (!s_lib_version_ok(SPCT_MAJOR_VERSION_MIN, SPCT_MINOR_VERSION_MIN))
	{
		S_CTX_ERR(error, S_FAILURE,
				  SPCT_PLUGIN_INIT_STR,
				  "Incorrect Speect Engine version, require at least '%d.%d.x'",
				  SPCT_MAJOR_VERSION_MIN, SPCT_MINOR_VERSION_MIN);
		return NULL;
	}

	return &plugin_params;
}


/************************************************************************************/
/*                                                                                  */
/* Static function implementations                                                  */
/*                                                                                  */
/************************************************************************************/

/* plug-in register function */
static void plugin_register_function(s_erc *error)
{
	S_CLR_ERR(error);

	/* register plug-in classes here */
	_s_syllab_ita_it_mivoq_class_reg(error);
	S_CHK_ERR(error, S_CONTERR,
			  SPCT_PLUGIN_REG_STR,
			  SPCT_PLUGIN_REG_FAIL_STR);
}


/* plug-in exit function */
static void plugin_exit_function(s_erc *error)
{
	S_CLR_ERR(error);

	/* free plug-in classes here */
	_s_syllab_ita_it_mivoq_class_free(error);
	S_CHK_ERR(error, S_CONTERR,
			  SPCT_PLUGIN_EXIT_STR,
			  SPCT_PLUGIN_EXIT_FAIL_STR);
}
