 /*
  * Copyright (C) 2010-2012 Project SkyFire <http://www.projectskyfire.org/>
  * Copyright (C) 2010-2012 Oregon <http://www.oregoncore.com/>
  * Copyright (C) 2006-2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
  * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
  *
  * This program is free software; you can redistribute it and/or modify it
  * under the terms of the GNU General Public License as published by the
  * Free Software Foundation; either version 2 of the License, or (at your
  * option) any later version.
  *
  * This program is distributed in the hope that it will be useful, but WITHOUT
  * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
  * more details.
  *
  * You should have received a copy of the GNU General Public License along
  * with this program. If not, see <http://www.gnu.org/licenses/>.
  */

#include "ScriptPCH.h"
#include "blackfathom_deeps.h"

enum Spells
{
    SPELL_MIND_BLAST                                       = 15587,
    SPELL_SLEEP                                            = 8399,
};

//Id's from ACID
enum Yells
{
    SAY_AGGRO                                              = -1048002,
    SAY_SLEEP                                              = -1048001,
    SAY_DEATH                                              = -1048000
};

struct boss_kelrisAI : public ScriptedAI
{
    boss_kelrisAI(Creature *c) : ScriptedAI(c)
    {
        instance = c->GetInstanceScript();
    }

    uint32 uiMindBlastTimer;
    uint32 uiSleepTimer;

    ScriptedInstance *instance;

    void Reset()
    {
        uiMindBlastTimer = urand(2000, 5000);
        uiSleepTimer = urand(9000, 12000);
        if (instance)
            instance->SetData(TYPE_KELRIS, NOT_STARTED);
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(SAY_AGGRO, me);
        if (instance)
            instance->SetData(TYPE_KELRIS, IN_PROGRESS);
    }

    void JustDied(Unit* /*killer*/)
    {
        DoScriptText(SAY_DEATH, me);
        if (instance)
            instance->SetData(TYPE_KELRIS, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (uiMindBlastTimer < diff)
        {
            DoCastVictim(SPELL_MIND_BLAST);
            uiMindBlastTimer = urand(7000, 9000);
        } else uiMindBlastTimer -= diff;

        if (uiSleepTimer < diff)
        {
            if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
            {
                DoScriptText(SAY_SLEEP, me);
                DoCast(pTarget, SPELL_SLEEP);
            }
            uiSleepTimer = urand(15000, 20000);
        } else uiSleepTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_kelris(Creature* creature)
{
    return new boss_kelrisAI (creature);
}

void AddSC_boss_kelris()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_kelris";
    newscript->GetAI = &GetAI_boss_kelris;
    newscript->RegisterSelf();
}
