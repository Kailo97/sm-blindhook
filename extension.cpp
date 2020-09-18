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

#include "extension.h"
#include "subhook/subhook.h"

/**
 * @file extension.cpp
 * @brief Implement extension code here.
 */

BlindHook g_BlindHook;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_BlindHook);


using namespace subhook;

IGameConfig *g_pGameConf;
Hook *g_HBlindPlayer;
void *g_addr_continue;
void *g_addr_skip;
IForward *g_pBlindForward = NULL;

bool __stdcall BlindHookHandler(CBaseEntity *pEntity, CBaseEntity *pevInflictor, CBaseEntity *pevAttacker)
{
	cell_t result = Pl_Continue;

	g_pBlindForward->PushCell(gamehelpers->EntityToBCompatRef(pEntity));
	g_pBlindForward->PushCell(pevAttacker != pevInflictor ? gamehelpers->EntityToBCompatRef(pevAttacker) : -1);
	g_pBlindForward->PushCell(gamehelpers->EntityToBCompatRef(pevInflictor));
	g_pBlindForward->Execute(&result);

	return result != Pl_Continue;
}

__declspec(naked) void blindhook()
{
#if defined(WIN32)
	__asm push [esp+0x10]		// pevAttacker
	__asm push [esp+0x4+0x14]	// pevInflictor
	__asm push esi				// pEntity
#else
	__asm push [ebp+0x18]		// pevAttacker
	__asm push [ebp+0x14]		// pevInflictor
	__asm push ebx				// pEntity
#endif
	__asm call BlindHookHandler;

	__asm test al, al;
	__asm jz Trampoline
	// skip
	__asm mov ecx, g_addr_skip
	__asm jmp ecx

	// Trampoline back
	__asm Trampoline:
#if defined(WIN32)
	__asm _emit 0x8B
	__asm _emit 0x06
	__asm _emit 0x8D
	__asm _emit 0x8C
	__asm _emit 0x24
	__asm _emit 0xB0
	__asm _emit 0x00
	__asm _emit 0x00
	__asm _emit 0x00
	__asm _emit 0x51
#else
	__asm _emit 0x8B
	__asm _emit 0x03
	__asm _emit 0x8D
	__asm _emit 0x55
	__asm _emit 0xD0
#endif

	__asm mov ecx, g_addr_continue
	__asm jmp ecx
}

bool BlindHook::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	if (!gameconfs->LoadGameConfigFile("blindhook.games", &g_pGameConf, error, maxlength))
		return false;

	void *addr;
	if (!g_pGameConf->GetMemSig("RadiusFlash", &addr) || !addr)
	{
		snprintf(error, maxlength, "Failed to lookup RadiusFlash signature.");
		return false;
	}

	gameconfs->CloseGameConfigFile(g_pGameConf);

	void *addr_hook;

#if defined(WIN32)
	addr_hook = (void*)((uintptr_t)addr + 0x1DD);
	g_addr_continue = (void*)((uintptr_t)addr + 0x1E7);
	g_addr_skip = (void*)((uintptr_t)addr + 0x6E3);
#else
	addr_hook = (void*)((uintptr_t)addr + 0xD6);
	g_addr_continue = (void*)((uintptr_t)addr + 0xDB);
	g_addr_skip = (void*)((uintptr_t)addr + 0x60);
#endif

	sharesys->RegisterLibrary(myself, "blindhook");
	plsys->AddPluginsListener(this);

	g_HBlindPlayer = new Hook(addr_hook, (void *)blindhook);

	m_BlindPlayerHookInstalled = false;

	g_pBlindForward = forwards->CreateForward("CS_OnBlindPlayer", ET_Hook, 3, NULL, Param_Cell, Param_Cell, Param_Cell);

	return true;
}

void BlindHook::SDK_OnUnload()
{
	plsys->RemovePluginsListener(this);
	forwards->ReleaseForward(g_pBlindForward);
	delete g_HBlindPlayer;
}

void BlindHook::OnPluginLoaded(IPlugin *plugin)
{
	if (!m_BlindPlayerHookInstalled && g_pBlindForward->GetFunctionCount())
	{
		g_HBlindPlayer->Install();
		m_BlindPlayerHookInstalled = true;
	}
}

void BlindHook::OnPluginUnloaded(IPlugin *plugin)
{
	if (m_BlindPlayerHookInstalled && !g_pBlindForward->GetFunctionCount())
	{
		g_HBlindPlayer->Remove();
		m_BlindPlayerHookInstalled = false;
	}
}
