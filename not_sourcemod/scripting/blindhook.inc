/**
 * =============================================================================
 * SourceMod Blind Hook Extension
 * Copyright (C) 2019 Maxim "Kailo" Telezhenko. All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 */

#if defined _blindhook_included
  #endinput
#endif
#define _blindhook_included

/**
 * Called when a flashbang grenade perform player blinding.
 * Return Plugin_Continue to ignore or return a higher
 * action to protect player from blinding.
 *
 * @param client		Client index which must be blinded
 * @param attacker		Entity index of the grenade thrower or -1 if not available
 * @param inflictor		Entity index of the grenade projectile
 */
forward Action CS_OnBlindPlayer(int client, int attacker, int inflictor);

public Extension __ext_blindhook = 
{
	name = "blindhook",
	file = "blindhook.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};
