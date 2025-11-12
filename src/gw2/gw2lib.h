/*
GW2LIB
static library
by rafi
Feel free to release compiled DLLs but please provide source and link to this thread
http://www.gamerevision.com/showthread.php?3691-Gw2lib&p=45709
*/

#ifndef GW2LIB_H
#define GW2LIB_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <rpc.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include <cstdint>

#include "enums.h"
#include "Gw2Hook.h"
namespace GameData {
    class PlayerData;
    class CharacterData;
    class AgentData;
    class GadgetData;
    class AttackTargetData;
    class CompassData;
    class ResourceNodeData;
    class BuffData;
    class ItemData;
    class EquipItemData;
}

namespace GW2LIB
{
    // This function is not defined. Define it yourself to provide an entry point for Gw2lib.
    void gw2lib_main();

    uintptr_t DataScan(const char *pattern, uintptr_t(*shiftCb)(uintptr_t offset) = nullptr, int32_t shift = 0, uint8_t size = 4, int instance = 0, bool isString = false, bool(*verify)(uintptr_t offset, uintptr_t value) = nullptr, bool cache = true);
    uintptr_t OffsetScan(const char *pattern, int32_t shift = 0, uint8_t size = 4, int instance = 0, bool isString = false, bool(*verify)(uintptr_t offset, uintptr_t value) = nullptr, bool cache = true);
    uintptr_t PointerScan(const char *pattern, uintptr_t(*cb)(uintptr_t offset), int instance = 0, bool isString = false, bool(*verify)(uintptr_t offset, uintptr_t value) = nullptr, bool cache = true);

    class Agent;
    class Character;
    class Player;
    class Compass;
    class Gadget;
    class AttackTarget;
    class ResourceNode;
    class Buff;
    class Item;
    class EquipItem;
    class Frame;

    class Vector3 {
    public:
        Vector3() : x(0.0f), y(0.0f), z(0.0f) { }
        Vector3(float x, float y, float z) : x(x), y(y), z(z) { }
        bool operator== (const Vector3 &vec){ return vec.x == x && vec.y == y && vec.z == z; }
        bool operator!= (const Vector3 &vec){ return vec.x != x || vec.y != y || vec.z != z; }
        float x, y, z;
    };
    class Vector4 {
    public:
        Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
        Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { }
        float x, y, z, w;
    };
    struct Matrix4x4 {
        float m[4][4];
    };

    namespace GW2
    {
        struct CharacterStats {
            int power = 0;
            int precision = 0;
            int toughness = 0;
            int vitality = 0;
            int ferocity = 0;
            int healing = 0;
            int condition = 0;
            int concentration = 0;
            int expertise = 0;
            int ar = 0;
            int unk1 = 0;
        };
    }

    //////////////////////////////////////////////////////////////////////////
    // # general functions
    //////////////////////////////////////////////////////////////////////////

    // registers a callback to be used for "ViewAdvanceDevice" (aka, game hook)
    void EnableGameHook(void (*)());

    void GameCtxCheck();

    float Dist3D(Vector3, Vector3);


    //////////////////////////////////////////////////////////////////////////
    // # game classes
    //////////////////////////////////////////////////////////////////////////
    // represents a general game object
    class Agent {
    public:
        Agent();
        Agent(const Agent &);
        Agent(uintptr_t*);
        Agent(uintptr_t);
        Agent &operator= (const Agent &);
        bool operator== (const Agent &);
        bool operator!= (const Agent &);

        bool IsValid() const;
        operator bool() const;

        bool BeNext();
        void BeSelf();

        Character GetCharacter() const;
        Player GetPlayer() const;
        Gadget GetGadget() const;
        AttackTarget GetAttackTarget() const;

        GW2::AgentCategory GetCategory() const;
        GW2::AgentType GetType() const;
        uint32_t GetAgentId() const;

        Vector3 GetPos() const;
        float GetRot() const;
        uint64_t GetToken() const;
        uint64_t GetSequence() const;
        float GetSpeed() const;
        float GetMaxSpeed() const;
        float GetRealSpeed() const;
        float GetCalcSpeed() const;
        bool IsSelectable() const;
        std::string GetName() const;
        void SetSequence(uint64_t state);
        void SetActiveSelection();
        bool LockCamera();

        GameData::AgentData *m_ptr = nullptr;
        size_t iterator = 0;
    };
    // represents advanced game objects like players and monsters
    class Character {
    public:
        Character();
        Character(const Character &);
        Character &operator= (const Character &);
        bool operator== (const Character &);

        bool IsValid() const;

        bool BeNext();
        void BeSelf();

        Agent GetAgent() const;

        uint16_t GetID() const;
        bool IsAlive() const;
        bool IsDowned() const;
        bool IsControlled() const;
        bool IsPlayer() const;
        bool IsInWater() const;
        bool IsMonster() const;
        bool IsClone() const;
        bool IsRangerPet() const;
        bool IsInCombat() const;
        bool IsWeaponDrawn() const;

        int GetLevel() const;
        int GetScaledLevel() const;
        GW2::CharacterGender GetGender() const;
        GW2::Race GetRace() const;
        GW2::CharacterStats GetStats() const;
        int GetWvwSupply() const;

        float GetCurrentEnergy() const;
        float GetMaxEnergy() const;
        float GetCurrentHealth() const;
        float GetMaxHealth() const;
        float GetCurrentBarrier() const;
        float GetCurrentEndurance() const;
        float GetMaxEndurance() const;
        float GetGliderPercent() const;
        float GetBreakbarPercent() const;

        bool SetSpeed(float speed);
        void TriggerSkill(GW2::SkillSlot);
        void TriggerSkillDown(GW2::SkillSlot);
        void TriggerSkillUp(GW2::SkillSlot);

        GW2::BreakbarState GetBreakbarState() const;
        GW2::Profession GetProfession() const;
        GW2::ProfessionStance GetStance() const;
        GW2::Attitude GetAttitude() const;

        std::string GetName() const;
        Buff GetBuffs() const;
        int32_t GetBuffStackCount(GW2::EffectType);
        int32_t GetBuffTimeLeft(GW2::EffectType);
        EquipItem GetEquipItems() const;
        EquipItem GetEquipItem(GW2::EquipmentSlot);
        GW2::CharacterRank GetRank();

        GameData::CharacterData *m_ptr = nullptr;
        size_t iterator = 0;
    };

    class Item {
    public:
        Item();
        Item(const Item &);
        Item &operator= (const Item &);
        bool operator== (const Item &);

        bool IsValid() const;

        uint32_t GetID();
        GW2::ItemRarity GetRarity();
        std::string GetName() const;

        GameData::ItemData *m_ptr = nullptr;
    };

    class EquipItem {
    public:
        EquipItem();
        EquipItem(const EquipItem &);
        EquipItem &operator= (const EquipItem &);
        bool operator== (const EquipItem &);

        bool IsValid() const;
        bool BeNext();

        Character GetSource();
        Item GetItem();
        GW2::EquipmentSlot GetEquipSlot();

        std::unordered_map<size_t, std::unique_ptr<GameData::EquipItemData>> *dataList = nullptr;
        std::unordered_map<size_t, std::unique_ptr<GameData::EquipItemData>>::iterator iterator;
        GameData::EquipItemData *m_ptr = nullptr;
    };

    class Buff {
    public:
        Buff();
        Buff(const Buff &);
        Buff &operator= (const Buff &);
        bool operator== (const Buff &);

        bool IsValid() const;
        bool BeNext();

        Agent GetSource();
        GW2::EffectType GetEffectType();
        int32_t GetDuration();
        int64_t GetApplyTime();
        GW2::BuffStackType GetStackType();

        std::unordered_map<size_t, std::unique_ptr<GameData::BuffData>> *buffDataList = nullptr;
        std::unordered_map<size_t, std::unique_ptr<GameData::BuffData>>::iterator iterator;
        GameData::BuffData *m_ptr = nullptr;
    };

    class Player {
    public:
        Player();
        Player(const Player &);
        Player &operator= (const Player &);
        bool operator== (const Player &);

        bool IsValid() const;

        bool BeNext();
        void BeSelf();

        Agent GetAgent() const;
        Character GetCharacter() const;

        int GetCurrency(GW2::CurrencyType type);
        int GetMasteryLevel() const;
        int GetAP() const;
        GW2::Specialization GetSpecType(GW2::SpecSlot slot);
        GW2::Trait GetTraitType(GW2::SpecSlot sslot, GW2::TraitSlot tslot);
        bool HasEliteSpec();
        bool HasPOFEliteSpec();
        bool HasEODEliteSpec();
        std::string GetName() const;
        UUID GetUUID() const;
        std::string GetUUID2Str() const;

        GameData::PlayerData *m_ptr = nullptr;
        size_t iterator = 0;
    };
    // represents a gadget
    class Gadget {
    public:
        Gadget();
        Gadget(const Gadget &);
        Gadget &operator= (const Gadget &);
        bool operator== (const Gadget &);
        Agent GetAgent() const;
        bool IsValid() const;
        float GetCurrentHealth() const;
        float GetMaxHealth() const;
        GW2::GadgetType GetType() const;
        ResourceNode GetResourceNode() const;
        int GetWvwTeamId() const;

        GameData::GadgetData *m_ptr = nullptr;
    };
    // represents a gadget attack target
    class AttackTarget {
    public:
        AttackTarget();
        AttackTarget(const AttackTarget &);
        AttackTarget &operator= (const AttackTarget &);
        bool operator== (const AttackTarget &);
        Agent GetAgent() const;
        bool IsValid() const;
        float GetCurrentHealth() const;
        float GetMaxHealth() const;

        GameData::AttackTargetData *m_ptr = nullptr;
    };
    // resource nodes
    class ResourceNode {
    public:
        ResourceNode();
        ResourceNode(const ResourceNode &);
        ResourceNode &operator= (const ResourceNode &);
        bool operator== (const ResourceNode &);
        Agent GetAgent() const;
        bool IsValid() const;
        GW2::ResourceNodeType GetType() const;
        bool IsGatherable() const;

        GameData::ResourceNodeData *m_ptr = nullptr;
    };
    // profession specific data
    class Profession {
    public:
        Profession();
    };
    // compass (minimap)
    class Compass {
    public:
        Compass();
        Compass(const Compass &);
        Compass &operator= (const Compass &);

        float GetMaxWidth() const;
        float GetMaxHeight() const;
        float GetWidth() const;
        float GetHeight() const;
        int   GetZoom() const;
        bool  GetRotation() const;
        bool  GetPosition() const;

        GameData::CompassData *m_ptr = nullptr;
    };

    class Memory {
    public:
        Memory();
        Memory(size_t cat);
        Memory &operator= (const size_t&);
        bool BeNext();
        bool SetCategory(size_t cat);
        uint32_t GetCategory();
        wchar_t* GetName();
        uint32_t GetSize();
        uint32_t GetCount();

        size_t iterator = 0;
        void *m_ptr = nullptr;
    };

    // frame api (aka, windows, tooltips, etc)
    class Frame {
    public:
        Frame();
        Frame& operator= (const Frame&);

        bool IsValid() const;
        operator bool() const;

        bool BeNext();
        uint32_t ListSize();

        uint32_t iterator = 0;
        void* m_ptr = nullptr;

        uint32_t frameId = 0;
    };


    //////////////////////////////////////////////////////////////////////////
    // # game functions
    //////////////////////////////////////////////////////////////////////////
    Character GetOwnCharacter();
    Agent GetOwnAgent();
    Agent GetAutoSelection();
    Agent GetHoverSelection();
    Agent GetLockedSelection();
    Vector3 GetMouseInWorld();
    int GetCurrentMapId();
    Compass GetCompass();
    GW2::UiIntefaceSize GetUiInterfaceSize();
    bool GetUiOptionFlag(GW2::UiOptionFlag opt);
    int GetPing();
    int GetFPS();
    bool IsInterfaceHidden();
    bool IsMapOpen();
    bool IsInCutscene();
    bool ActionCamOn();
    std::string GetIPAddress();
    void AddDrunkLevel(int lvl);
    uint32_t GetBuildId();

    //////////////////////////////////////////////////////////////////////////
    // # camera functions
    //////////////////////////////////////////////////////////////////////////
    Vector3 GetCameraPosition();
    Vector3 GetViewVector();
    float GetFieldOfViewY();
    float GetCamCurrentZoom();
    float GetCamMinZoom();
    float GetCamMaxZoom();
    void SetCamMinZoom(float);
    void SetCamMaxZoom(float);

    //////////////////////////////////////////////////////////////////////////
    // # advanced
    //////////////////////////////////////////////////////////////////////////

    void SetMems(const struct Mems& mems);
}

#endif // GW2LIB_H
