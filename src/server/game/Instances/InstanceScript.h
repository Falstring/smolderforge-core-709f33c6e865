/*
 * Copyright (C) 2010-2012 Project SkyFire <http://www.projectskyfire.org/>
 * Copyright (C) 2010-2012 Oregon <http://www.oregoncore.com/>
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
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

#ifndef TRINITY_INSTANCE_DATA_H
#define TRINITY_INSTANCE_DATA_H

#include "ZoneScript.h"
//#include "GameObject.h"
//#include "Map.h"

class Map;
class Unit;
class Player;
class GameObject;
class Creature;

typedef std::set<GameObject*> DoorSet;
typedef std::set<Creature*> MinionSet;

enum EncounterState
{
    NOT_STARTED   = 0,
    IN_PROGRESS   = 1,
    FAIL          = 2,
    DONE          = 3,
    SPECIAL       = 4,
    TO_BE_DECIDED = 5,
};

enum DoorType
{
    DOOR_TYPE_ROOM = 0,
    DOOR_TYPE_PASSAGE,
    MAX_DOOR_TYPES,
};

struct DoorData
{
    uint32 entry, bossId;
    DoorType type;
};

struct MinionData
{
    uint32 entry, bossId;
};

struct BossInfo
{
    BossInfo() : state(TO_BE_DECIDED) {}
    EncounterState state;
    DoorSet door[MAX_DOOR_TYPES];
    MinionSet minion;
};

struct DoorInfo
{
    explicit DoorInfo(BossInfo *_bossInfo, DoorType _type)
        : bossInfo(_bossInfo), type(_type) {}
    BossInfo *bossInfo;
    DoorType type;
};

struct MinionInfo
{
    explicit MinionInfo(BossInfo *_bossInfo) : bossInfo(_bossInfo) {}
    BossInfo *bossInfo;
};

typedef std::multimap<uint32 /*entry*/, DoorInfo> DoorInfoMap;
typedef std::map<uint32 /*entry*/, MinionInfo> MinionInfoMap;

class InstanceScript : public ZoneScript
{
    public:

        explicit InstanceScript(Map *map) : instance(map) {}
        virtual ~InstanceScript() {}

        Map *instance;

        //On creation, NOT load.
        virtual void Initialize() {}

        //On load
        virtual void Load(const char * data) { LoadBossState(data); }

        //When save is needed, this function generates the data
        virtual std::string GetSaveData() { return GetBossSaveData(); }

        void SaveToDB();

        virtual void Update(uint32 /*diff*/) {}

        //Used by the map's CanEnter function.
        //This is to prevent players from entering during boss encounters.
        virtual bool IsEncounterInProgress() const;

        //Called when a player successfully enters the instance.
        virtual void OnPlayerEnter(Player *) {}

        //Handle open / close objects
        //use HandleGameObject(NULL, boolen, GO); in OnObjectCreate in instance scripts
        //use HandleGameObject(GUID, boolen, NULL); in any other script
        void HandleGameObject(uint64 GUID, bool open, GameObject *go = NULL);

        //change active state of doors or buttons
        void DoUseDoorOrButton(uint64 uiGuid, uint32 uiWithRestoreTime = 0, bool bUseAlternativeState = false);

        //Respawns a GO having negative spawntimesecs in gameobject-table
        void DoRespawnGameObject(uint64 uiGuid, uint32 uiTimeToDespawn = MINUTE);

        //sends world state update to all players in instance
        void DoUpdateWorldState(uint32 worldstateId, uint32 worldstateValue);

        virtual bool SetBossState(uint32 id, EncounterState state);
        EncounterState GetBossState(uint32 id) const { return id < bosses.size() ? bosses[id].state : TO_BE_DECIDED; }
    protected:
        void SetBossNumber(uint32 number) { bosses.resize(number); }
        void LoadDoorData(const DoorData *data);
        void LoadMinionData(const MinionData *data);

        void AddDoor(GameObject *door, bool add);
        void AddMinion(Creature *minion, bool add);

        void UpdateDoorState(GameObject *door);
        void UpdateMinionState(Creature *minion, EncounterState state);

        std::string LoadBossState(const char * data);
        std::string GetBossSaveData();
    private:
        std::vector<BossInfo> bosses;
        DoorInfoMap doors;
        MinionInfoMap minions;
};
#endif

