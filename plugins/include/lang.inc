// vim: set ts=4 sw=4 tw=99 noet:
//
// AMX Mod X, based on AMX Mod by Aleksander Naszko ("OLO").
// Copyright (C) The AMX Mod X Development Team.
//
// This software is licensed under the GNU General Public License, version 3 or higher.
// Additional exceptions apply. For full license details, see LICENSE.txt or visit:
//     https://alliedmods.net/amxmodx-license

//
// Language Functions
//

#if defined _lang_included
  #endinput
#endif
#define _lang_included

//return the number of languages loaded
native get_langsnum();

//sets name to the two-letter name of a language returned by get_langsnum
//index starts at 0
native get_lang(id, name[3]);

//registers a dictionary file, making sure the words are in the dictionary
// the file should be in "addons/amxx/data/lang/", but only the name needs to be
// given.  (e.g. register_dictionary("file.txt") will be addons/amxx/data/file.txt).
native register_dictionary(const filename[]);

//returns 1 if the language is loaded, 0 otherwise.
native lang_exists(const name[]);

enum TransKey
{
	TransKey_Bad = -1,
};

/**
 * Adds or finds a translation key.
 */
native TransKey:CreateLangKey(const key[]);

/**
 * Finds a translation key id without adding on failure.
 * Returns -1 on not found.
 */
native TransKey:GetLangTransKey(const key[]);

/**
 * Adds a translation.
 */
native AddTranslation(const lang[3], TransKey:key, const phrase[]);

/**
 * Looks up the translation of the key for the given type
 * This does NOT format the output text.
 * eg: If the key includes %s, the outputted text will also contain %s.
 * NOTE: LANG_PLAYER is invalid in this, use a player index
 *       or LANG_SERVER
 */
native LookupLangKey(Output[], OutputSize, const Key[], &id);

/**
 * Sets the global language target.  
 *
 * @note This is useful for creating functions
 *       that will be compatible with the %l format specifier.  Note that invalid
 *       indexes can be specified but the error will occur during translation,
 *       not during this function call.
 *
 * @param client    Client index or LANG_SERVER
 * @noreturn
 */
native SetGlobalTransTarget(client);
