#include "stdafx.h"

void Sonic_CheckActionWindow_asm();
static Trampoline trampoline(0x007230E0, 0x007230E5, Sonic_CheckActionWindow_asm);

extern "C"
{
	EXPORT ModInfo SA2ModInfo = { ModLoaderVer, nullptr, nullptr, 0, nullptr, 0, nullptr, 0, nullptr, 0 };
}

static Sint32 __cdecl Sonic_CheckActionWindow_orig(CharObj1* data1, void* entity2, CharObj2* data2, SonicCharObj2* sonicdata)
{
	auto target = trampoline.Target();
	Sint32 result;
	__asm
	{
		push [sonicdata]
		mov ecx, [data2]
		mov edx, [entity2]
		mov eax, [data1]
		call target
		add esp, 4
		mov result, eax
	}
	return result;
}

// Sonic_PerformLightDash(SonicCharObj2 *sonicdata@<eax>, CharObj2Base *basedata@<edx>, EntityData1 *entity@<ecx>)
// Sets action, speed, probably some timers
void __stdcall Sonic_PerformLightDash(SonicCharObj2* sonicdata, CharObj2Base* basedata, CharObj1* entity)
{
	__asm
	{
		mov esi, 00724010h
		mov eax, [sonicdata]
		mov edx, [basedata]
		mov ecx, [entity]
		call esi
	}
}

static Sint32 __cdecl Sonic_CheckActionWindow(CharObj1* data1, void* entity2, CharObj2* data2, SonicCharObj2* sonicdata)
{
	// This code is based on the pseudocode of the original function
	int  pnum   = data2->PlayerNum;
	char action = data2->field_D[0];
	int  count  = data2->ActionWindowItemCount;
	int  i      = 0;

	if (count)
	{
		if (count > 0)
		{
			do
			{
				if (data2->field_D[0] == data2->ActionWindowItems[i])
				{
					break;
				}
				++i;
			} while (i < count);
			action = data2->field_D[0];
		}

		if (count == i)
		{
			action = data2->ActionWindowItems[0];
		}

		// Action check for light dash.
		if (action == Action_LightDash)
		{
			data2->field_D[1] = action;

			// Just nope right out of here if Y isn't pressed.
			if (!(Controllers[pnum].PressedButtons & Buttons_Y))
			{
				return 0;
			}

			Sonic_PerformLightDash(sonicdata, data2, data1);
			return 1; // Original function returns this value on light dash.
		}
	}

	// If all those conditions fail, just call the original and let it handle it.
	return Sonic_CheckActionWindow_orig(data1, entity2, data2, sonicdata);
}

static void __declspec(naked) Sonic_CheckActionWindow_asm()
{
	__asm
	{
		push [esp + 4]
		push ecx
		push edx
		push eax
		call Sonic_CheckActionWindow
		add esp, 4 // eax
		pop edx
		pop ecx
		add esp, 4
		retn
	}
}
