#ifndef ENUMS_H
#define ENUMS_H

namespace GW2LIB {

    enum Gw2Hook {
        HOOK_AG_CAN_BE_SEL,
        HOOK_CHAT,
        HOOK_DAMAGE_LOG,
        HOOK_COMBAT_LOG,
        HOOK_ALLOCATOR,
        HOOK_LOGGER,
        HOOK_MSG_CONN,
        HOOK_SCREENSHOT,
        HOOK_WINDOW_PROC,
        HOOK_SEND_PACKET,
        HOOK_RECV_PACKET
    };

    namespace GW2 {

        enum SkillSlot {
            SKILL_SLOT_HEAL,
            SKILL_SLOT_UTILITY_1,
            SKILL_SLOT_UTILITY_2,
            SKILL_SLOT_UTILITY_3,
            SKILL_SLOT_ELITE,
            SKILL_SLOT_WEAPON_1,
            SKILL_SLOT_WEAPON_2,
            SKILL_SLOT_WEAPON_3,
            SKILL_SLOT_WEAPON_4,
            SKILL_SLOT_WEAPON_5,
            SKILL_SLOT_10,
            SKILL_SLOT_11,
            SKILL_SLOT_PROF_1,
            SKILL_SLOT_PROF_2,
            SKILL_SLOT_PROF_3,
            SKILL_SLOT_PROF_4,
            SKILL_SLOT_PROF_5,
            SKILL_SLOT_GATHERING,
            SKILL_SLOT_SPECIAL_ACTION,
            SKILL_SLOT_19,
            SKILL_SLOT_20,
            SKILL_SLOT_MOUNT,
            SKILL_SLOT_END
        };

        enum Profession {
            PROFESSION_NONE,
            PROFESSION_GUARDIAN,
            PROFESSION_WARRIOR,
            PROFESSION_ENGINEER,
            PROFESSION_RANGER,
            PROFESSION_THIEF,
            PROFESSION_ELEMENTALIST,
            PROFESSION_MESMER,
            PROFESSION_NECROMANCER,
            PROFESSION_REVENANT,
            PROFESSION_END
        };

        enum Race {
            RACE_ASURA,
            RACE_CHARR,
            RACE_HUMAN,
            RACE_NORN,
            RACE_SYLVARI,
            RACE_NONE
        };

        enum Attitude {
            ATTITUDE_FRIENDLY,
            ATTITUDE_HOSTILE,
            ATTITUDE_INDIFFERENT,
            ATTITUDE_NEUTRAL
        };

        enum AgentCategory {
            AGENT_CATEGORY_CHAR,
            AGENT_CATEGORY_DYNAMIC,
            AGENT_CATEGORY_KEYFRAMED
        };

        enum AgentType {
            AGENT_TYPE_CHAR = 0,
            AGENT_TYPE_GADGET = 10,
            AGENT_TYPE_GADGET_ATTACK_TARGET = 11,
            AGENT_TYPE_ITEM = 15
        };

        enum CharacterRank {
            CHAR_RANK_NORMAL,
            CHAR_RANK_AMBIENT,
            CHAR_RANK_VETERAN,
            CHAR_RANK_ELITE,
            CHAR_RANK_CHAMPION,
            CHAR_RANK_LEGENDARY,
            CHAR_RANK_END
        };

        enum BreakbarState {
            BREAKBAR_STATE_READY,
            BREAKBAR_STATE_RECOVER,
            BREAKBAR_STATE_IMMUNE,
            BREAKBAR_STATE_NONE
        };

        enum UiIntefaceSize {
            UI_IF_SMALL,
            UI_IF_NORMAL,
            UI_IF_LARGE,
            UI_IF_LARGER
        };

        enum UiOptionFlag {
            UI_OPT_AOE_LOOT = 1,
            UI_OPT_TGT_PCT = 31,
            UI_OPT_DBL_TAP = 36,
            UI_OPT_END = 70
        };

        enum ScreenshotMode {
            SCREENSHOT_NORMAL,
            SCREENSHOT_NOUI,
            SCREENSHOT_STEREOSCOPIC,
            SCREENSHOT_STEREOSCOPIC_NOUI,
            SCREENSHOT_HIGHRES,
            SCREENSHOT_HIGHRES_NOUI,
            SCREENSHOT_END
        };

        enum CombatLogType {
            CL_CONDI_DMG_IN,
            CL_CRIT_DMG_IN,
            CL_GLANCE_DMG_IN,
            CL_HEAL_IN,
            CL_BARRIER_IN,
            CL_PHYS_DMG_IN,
            CL_UNKNOWN_2,
            CL_UNKNOWN_3,
            CL_UNKNOWN_7,
            CL_CONDI_DMG_OUT,
            CL_CRIT_DMG_OUT,
            CL_GLANCE_DMG_OUT,
            CL_HEAL_OUT,
            CL_BARRIER_OUT,
            CL_PHYS_DMG_OUT,
            CL_UNKNOWN_5,
            CL_UNKNOWN_6,
            CL_UNKNOWN_8
        };

        enum GadgetType {
            GADGET_TYPE_DESTRUCTABLE = 1,// training dummy, wvw lobby siege practice target crate
            GADGET_TYPE_POINT,       // pvp control point, event spawn? (saw at VB Pale Reaver primary rally point)
            GADGET_TYPE_UNK_1,
            GADGET_TYPE_UNK_2,
            GADGET_TYPE_CRAFT,       // crafting station
            GADGET_TYPE_DOOR,        // pvp base gate, interactive door
            GADGET_TYPE_UNK_3,
            GADGET_TYPE_UNK_4,
            GADGET_TYPE_UNK_5,
            GADGET_TYPE_UNK_6,
            GADGET_TYPE_UNK_7,
            GADGET_TYPE_INTERACT,   // SW chest, AB exalted portal
            GADGET_TYPE_UNK_8,
            GADGET_TYPE_PLAYER_SPECIFIC, // LA karka hatchling, VB floating airship cargo
            GADGET_TYPE_UNK_9,
            GADGET_TYPE_ATK_TARGET,      // world boss, SW fort wall
            GADGET_TYPE_MAP_PORTAL,        // map border portal
            GADGET_TYPE_WAYPOINT,        // waypoint
            GADGET_TYPE_RESOURCE_NODE,   // gathering node, AB mask, strongbox, chest
            GADGET_TYPE_PROP,            // supply depot, anvil, jump pad, prop, LA marker/plaque, asura gate, mystic forge, bouncy shroom, book cart, mes cs rift
            GADGET_TYPE_UNK_10,
            GADGET_TYPE_UNK_11,
            GADGET_TYPE_PLAYER_CREATED, // turret, wvw siege, guild banner
            GADGET_TYPE_VISTA,           // vista
            GADGET_TYPE_BUILD_SITE,      // wvw siege build site
            GADGET_TYPE_NONE
        };

        enum ResourceNodeType {
            RNODE_TYPE_PLANT,
            RNODE_TYPE_TREE,
            RNODE_TYPE_ROCK,
            RNODE_TYPE_QUEST,
            RNODE_TYPE_NONE
        };

        enum ProfessionStance {
            STANCE_NONE,
            STANCE_ELE_FIRE,
            STANCE_ELE_WATER,
            STANCE_ELE_AIR,
            STANCE_ELE_EARTH,
            STANCE_UNKNOWN2,
            STANCE_NECRO_SHROUD,
            STANCE_WAR_ADREN1,
            STANCE_WAR_ADREN2,
            STANCE_WAR_ADREN3,
            STANCE_RANGER_DRUID,
            STANCE_RANGER_ASTRAL,
            STANCE_REV_DRAGON,
            STANCE_REV_ASSASSIN,
            STANCE_REV_DWARF,
            STANCE_REV_DEMON,
            STANCE_UNKNOWN1,
            STANCE_REV_CENTAUR,
            STANCE_UNKNOWN3,
            STANCE_UNKNOWN4,
            STANCE_REV_ALLIANCE
        };

        enum SpecSlot {
            SPEC_SLOT_1,
            SPEC_SLOT_2,
            SPEC_SLOT_3,
            SPEC_SLOT_END
        };

        enum TraitSlot {
            TRAIT_SLOT_1,
            TRAIT_SLOT_2,
            TRAIT_SLOT_3,
            TRAIT_SLOT_END
        };

        enum Specialization {
            SPEC_NONE,
            SPEC_MES_DEULING,
            SPEC_NECRO_DEATH_MAGIC,
            SPEC_REV_INVOCATION,
            SPEC_WAR_STRENGTH,
            SPEC_RANGER_DRUID,
            SPEC_ENGI_EXPLOSIVES,
            SPEC_THIEF_DAREDEVIL,
            SPEC_RANGER_MARKSMANSHIP,
            SPEC_REV_RETRIBUTION,
            SPEC_MES_DOMINAION,
            SPEC_WAR_TACTICS,
            SPEC_REV_SALVATION,
            SPEC_GUARD_VALOR,
            SPEC_REV_CORRUPTION,
            SPEC_REV_DEVASTATION,
            SPEC_GUARD_RADIANCE,
            SPEC_ELE_WATER,
            SPEC_WAR_BERSERKER,
            SPEC_NECRO_BLOOD_MAGIC,
            SPEC_THIEF_SHADOW_ARTS,
            SPEC_ENGI_TOOLS,
            SPEC_WAR_DEFENSE,
            SPEC_MES_INSPIRATION,
            SPEC_MES_ILLUSIONS,
            SPEC_RANGER_NATURE_MAGIC,
            SPEC_ELE_EARTH,
            SPEC_GUARD_DRAGONHUNTER,
            SPEC_THIEF_DEADLY_ARTS,
            SPEC_ENGI_ALCHEMY,
            SPEC_RANGER_SKIRMISHING,
            SPEC_ELE_FIRE,
            SPEC_RANGER_BEASTMASTERY,
            SPEC_RANGER_WILDERNESS_SURVIVAL,
            SPEC_NECRO_REAPER,
            SPEC_THIEF_CRITICAL_STRIKES,
            SPEC_WAR_ARMS,
            SPEC_ELE_ARCANE,
            SPEC_ENGI_FIREARMS,
            SPEC_NECRO_CURSES,
            SPEC_MES_CHRONOMANCER,
            SPEC_ELE_AIR,
            SPEC_GUARD_ZEAL,
            SPEC_ENGI_SCRAPPER,
            SPEC_THIEF_TRICKERY,
            SPEC_MES_CHAOS,
            SPEC_GUARD_VIRTUES,
            SPEC_ENGI_INVENTIONS,
            SPEC_ELE_TEMPEST,
            SPEC_GUARD_HONOR,
            SPEC_NECRO_SOUL_REAPING,
            SPEC_WAR_DISCIPLINE,
            SPEC_REV_HERALD,
            SPEC_NECRO_SPITE,
            SPEC_THIEF_ACROBATICS,
            SPEC_RANGER_SOULBEAST,
            SPEC_ELE_WEAVER,
            SPEC_ENGI_HOLOSMITH,
            SPEC_THIEF_DEADEYE,
            SPEC_MES_MIRAGE,
            SPEC_NECRO_SCOURGE,
            SPEC_WAR_SPELLBREAKER,
            SPEC_GUARD_FIREBRAND,
            SPEC_REV_RENEGADE,
            SPEC_NECRO_HARBINGER,
            SPEC_GUARD_WILLBENDER,
            SPEC_MES_VIRTUOSO,
            SPEC_ELE_CATALYST,
            SPEC_WAR_BLADESWORN,
            SPEC_REV_VINDICATOR,
            SPEC_ENGI_MECHANIST,
            SPEC_THIEF_SPECTER,
            SPEC_RANGER_UNTAMED,
            SPEC_END
        };

        enum Trait {
            TRAIT_NONE,
            TRAIT_MES_CONFOUNDING_SUGGESTIONS = 686,
            TRAIT_END
        };

        /*
        * type 1,17 = 0x2CDF07C8BA (CLOSED), 0x44D2C7B4A4 (OPEN)
        */
        enum AgentSequence {
            AGENT_SEQ_NONE,
            AGENT_SEQ_DOOR_OPEN = 0x7160F,
            AGENT_SEQ_DOOR_CLOSED = 0x59BD83,
            AGENT_SEQ_GADGET_READY = 0x817B0B2,
            AGENT_SEQ_GADGET_FIRING = 0x1037542C,
        };

        enum CharacterGender {
            CHAR_GENDER_MALE,
            CHAR_GENDER_FEMALE,
            CHAR_GENDER_NONE
        };

        enum CurrencyType {
            CURRENCY_NONE,
            CURRENCY_COIN,
            CURRENCY_KARMA,
            CURRENCY_LAURELS,
            CURRENCY_GEMS,
            CURRENCY_TEARS,
            CURRENCY_SHARDS,
            CURRENCY_RELICS,
            CURRENCY_SEALS = 9,
            CURRENCY_MANIFESTOS,
            CURRENCY_BLOOMS,
            CURRENCY_SYMBOLS,
            CURRENCY_CARVINGS,
            CURRENCY_CRYSTALS,
            CURRENCY_BADGES,
            CURRENCY_COMMENDATIONS,
            CURRENCY_TRANS_CHARGES = 18,
            CURRENCY_AIRSHIP_PARTS,
            CURRENCY_LEYLINE_CRYSTALS,
            CURRENCY_AURILLIUM = 22,
            CURRENCY_SPIRIT_SHARDS,
            CURRENCY_PRESTINE_RELICS,
            CURRENCY_GEODES,
            CURRENCY_CLAIM_TICKETS,
            CURRENCY_CRESTS,
            CURRENCY_MAGNETITE_SHARDS,
            CURRENCY_PROVISIONER_TOKENS,
            CURRENCY_PVP_TICKETS,
            CURRENCY_PROOF_HEROICS,
            CURRENCY_END
        };

        enum EquipmentSlot {
            EQUIP_SLOT_NONE = -1,
            EQUIP_SLOT_AQUATIC_HELM,
            EQUIP_SLOT_BACK,
            EQUIP_SLOT_CHEST,
            EQUIP_SLOT_BOOTS,
            EQUIP_SLOT_GLOVES,
            EQUIP_SLOT_HELM,
            EQUIP_SLOT_PANTS,
            EQUIP_SLOT_SHOULDERS,
            EQUIP_SLOT_TOWN_CHEST = 14,
            EQUIP_SLOT_TOWN_BOOTS,
            EQUIP_SLOT_TOWN_GLOVES,
            EQUIP_SLOT_TOWN_HELM,
            EQUIP_SLOT_TOWN_PANTS,
            EQUIP_SLOT_ACCESSORY1,
            EQUIP_SLOT_ACCESSORY2,
            EQUIP_SLOT_RING1,
            EQUIP_SLOT_RING2,
            EQUIP_SLOT_AMULET,
            EQUIP_SLOT_AQUATIC_WEAP1,
            EQUIP_SLOT_AQUATIC_WEAP2,
            EQUIP_SLOT_NOVELTY,
            EQUIP_SLOT_TRANSFORM_WEAP,
            EQUIP_SLOT_MAINHAND_WEAP1 = 29,
            EQUIP_SLOT_OFFHAND_WEAP1,
            EQUIP_SLOT_MAINHAND_WEAP2,
            EQUIP_SLOT_OFFHAND_WEAP2,
            EQUIP_SLOT_TOY,
            EQUIP_SLOT_FORAGING_TOOL,
            EQUIP_SLOT_LOGGING_TOOL,
            EQUIP_SLOT_MINING_TOOL,
            EQUIP_SLOT_PVP_AQUATIC_HELM = 40,
            EQUIP_SLOT_PVP_BACK,
            EQUIP_SLOT_PVP_CHEST,
            EQUIP_SLOT_PVP_BOOTS,
            EQUIP_SLOT_PVP_GLOVES,
            EQUIP_SLOT_PVP_HELM,
            EQUIP_SLOT_PVP_PANTS,
            EQUIP_SLOT_PVP_SHOULDERS,
            EQUIP_SLOT_PVP_AQUATIC_WEAP1,
            EQUIP_SLOT_PVP_AQUATIC_WEAP2,
            EQUIP_SLOT_PVP_MAINHAND_WEAP1,
            EQUIP_SLOT_PVP_OFFHAND_WEAP1,
            EQUIP_SLOT_PVP_MAINHAND_WEAP2,
            EQUIP_SLOT_PVP_OFFHAND_WEAP2,
            EQUIP_SLOT_PVP_AMULET = 58,
            EQUIP_SLOT_END = 60
        };

        enum ItemRarity {
            RARITY_NONE = -1,
            RARITY_JUNK,
            RARITY_COMMON,
            RARITY_FINE,
            RARITY_MASTERWORK,
            RARITY_RARE,
            RARITY_EXOTIC,
            RARITY_ASCENDED,
            RARITY_LEGENDARY,
            RARITY_END
        };

        enum BuffStackType {
            BUFF_STACK_TYPE_CUMULATIVE = 1,
            BUFF_STACK_TYPE_PROGRESSIVE,
            BUFF_STACK_TYPE_END
        };

        enum EffectType {
            EFFECT_NONE,
            EFFECT_PROTECTION = 717,
            EFFECT_REGENERATION = 718,
            EFFECT_SWIFTNESS = 719,
            EFFECT_BLIND = 720,
            EFFECT_CRIPPLED = 721,
            EFFECT_CHILLED = 722,
            EFFECT_POISON = 723,
            EFFECT_FURY = 725,
            EFFECT_VIGOR = 726,
            EFFECT_IMMOBILIZED = 727,
            EFFECT_BLEEDING = 736,
            EFFECT_BURNING = 737,
            EFFECT_VULNERABILITY = 738,
            EFFECT_SIGNET_OF_RESTORATION = 739,
            EFFECT_MIGHT = 740,
            EFFECT_WEAKNESS = 742,
            EFFECT_AEGIS = 743,
            EFFECT_ROCK_BARRIER_SHARD1 = 750,
            EFFECT_ROCK_BARRIER_SHARD2 = 751,
            EFFECT_ROCK_BARRIER_SHARD3 = 752,
            EFFECT_ROCK_BARRIER_SHARD4 = 753,
            EFFECT_ROCK_BARRIER_SHARD5 = 754,
            EFFECT_SHIELD_STANCE = 756, // shield 5
            EFFECT_WVW_DETERMINED = 762,
            EFFECT_DOWNED = 770,
            EFFECT_HEALING_SIGNET = 786,
            EFFECT_ENDURE_PAIN = 787,
            EFFECT_DEATH_SHROUD = 790,
            EFFECT_FEAR = 791,
            EFFECT_INVULNERABILITY = 848,
            EFFECT_CONFUSION = 861,
            EFFECT_STUN = 872,
            EFFECT_RETALIATION = 873,
            EFFECT_REVEALED = 890,
            EFFECT_STABILITY = 1122,
            EFFECT_QUICKNESS = 1187,
            EFFECT_MISTFORMBUFF1 = 3686, // also Ride the Lightning
            EFFECT_MISTFORMBUFF2 = 5543,
            EFFECT_ATTUNEMENT_WATER = 5568,
            EFFECT_ATTUNEMENT_AIR = 5575,
            EFFECT_SHOCKING_AURA = 5577,
            EFFECT_FROST_AURA = 5579,
            EFFECT_ATTUNEMENT_EARTH = 5580,
            EFFECT_GLYPH_ARCANE_POWER_STACK = 5582,
            EFFECT_TORNADO = 5583,
            EFFECT_ATTUNEMENT_FIRE = 5585,
            EFFECT_SOOTHING_MIST = 5587,
            EFFECT_UNDERWATER_ICE_GLOBE = 5612,
            EFFECT_UNDERWATER_ICE_WALL = 5613,
            EFFECT_ARCANE_SHIELD_STACK = 5640,
            EFFECT_UNDERWATER_AIR_POCKET = 5654,
            EFFECT_OBSIDIAN_FLESH = 5667,
            EFFECT_FIRE_SHIELD = 5677,
            EFFECT_MAGNETIC_AURA = 5684,
            EFFECT_CONJURE_FLAME_CHARGES = 5704,
            EFFECT_CONJURE_FROST_CHARGES = 5729,
            EFFECT_CONJURE_EARTH_CHARGES = 5730,
            EFFECT_GLYPH_ELEMENTAL_POWER_FIRE = 5739,
            EFFECT_GLYPH_ELEMENTAL_POWER_AIR = 5740,
            EFFECT_GLYPH_ELEMENTAL_POWER_WATER = 5741,
            EFFECT_GLYPH_ELEMENTAL_POWER_EARTH = 5742,
            EFFECT_CONJURE_FIRE_CHARGES = 5759,
            EFFECT_GLYPH_REVIVAL_OF_FIRE = 5764,
            EFFECT_GLYPH_REVIVAL_OF_AIR = 5765,
            EFFECT_ELIXIRS = 5863,
            EFFECT_SUPERSPEED = 5974,
            EFFECT_SIEGE_MANNED = 8726,
            EFFECT_RENEWED_FOCUS = 9255,
            EFFECT_CORRUPTION_SIGIL = 9374,
            EFFECT_SVANIRICEBLOCK = 9636,
            EFFECT_FOOD_100_POWER_70_CONDI_DMG = 9765, // plate of fire flank steak
            EFFECT_FOOD_80_POWER_60_PRECISION = 9768, // plate of steak and asparagus
            EFFECT_FOOD_100_POWER_70_PRECISION = 9769, // plate of truffle steak
            EFFECT_FOOD_80_PRECISION_60_FEROCITY = 9828, // bowl of butternut squash soup
            EFFECT_FOOD_100_PRECISION_70_FEROCITY = 9829, // bowl of curry butternut squash soup
            EFFECT_UTILITY_SHARPENING_STONE_ROUGH = 9958,
            EFFECT_UTILITY_SHARPENING_STONE_SIMPLE = 9959,
            EFFECT_UTILITY_SHARPENING_STONE_STANDARD = 9960,
            EFFECT_UTILITY_SHARPENING_STONE_QUALITY = 9961,
            EFFECT_UTILITY_SHARPENING_STONE_HARDENED = 9962,
            EFFECT_UTILITY_SHARPENING_STONE_SUPERIOR = 9963, // also includes the potent version
            EFFECT_UTILITY_TUNING_CRYSTAL_STANDARD = 9964,
            EFFECT_UTILITY_TUNING_CRYSTAL_ARTISAN = 9965,
            EFFECT_UTILITY_TUNING_CRYSTAL_QUALITY = 9966,
            EFFECT_UTILITY_TUNING_CRYSTAL_MASTER = 9967, // also includes the potent version
            EFFECT_UTILITY_MAINTENANCE_OIL_MASTER = 9968, // also includes the potent version
            EFFECT_UTILITY_MAINTENANCE_OIL_QUALITY = 9969,
            EFFECT_UTILITY_MAINTENANCE_OIL_ARTISAN = 9970,
            EFFECT_UTILITY_MAINTENANCE_OIL_STANDARD = 9971,
            EFFECT_FOOD_40_MF_WITH_BOON_20_BOON_DURATION = 9987, // chocolate omnomberry cream
            EFFECT_FOOD_4_MF = 9994, // onion ring
            EFFECT_FOOD_10_MF = 9996, // strawberries and biscuits
            EFFECT_FOOD_20_CONDI_DURATION_70_CONDI_DMG = 10009, // koi cake
            EFFECT_FOOD_50_SWIFTNESS_ON_KILL_8_MOVING_DMG = 10079, // bowl of asparagus and sage salad
            EFFECT_FOOD_60_SWIFTNESS_ON_KILL_10_MOVING_DMG = 10080, // bowl of seaweed salad
            EFFECT_UTILITY_MAINTENANCE_OIL_APPRENTICE = 10111,
            EFFECT_UTILITY_MAINTENANCE_OIL_JOURNEYMAN = 10112,
            EFFECT_UTILITY_TUNING_CRYSTAL_APPRENTICE = 10113,
            EFFECT_UTILITY_TUNING_CRYSTAL_JOURNEYMAN = 10114,
            EFFECT_FOOD_18_MF_40_POWER = 10134, // cherry tart
            EFFECT_FOOD_22_MF_50_POWER = 10135, // ginger pear tart
            EFFECT_FOOD_26_MF_60_POWER = 10136, // peach tart
            EFFECT_PORTAL_WEAVING = 10198,
            EFFECT_MES_SIGNET_DOM = 10231,
            EFFECT_BLUR = 10335,
            EFFECT_DISTORTION = 10371,
            EFFECT_BONE_MINION = 10539,
            EFFECT_BONE_FIEND = 10569,
            EFFECT_BLOOD_FIEND = 10575,
            EFFECT_PLAGUE_FORM = 10578,
            EFFECT_SPECTRAL_ARMOR = 10582,
            EFFECT_SHADOW_FIEND = 10591,
            EFFECT_FLESH_WURM = 10599,
            EFFECT_SIGNET_OF_UNDEATH = 10610,
            EFFECT_SIGNET_OF_THE_LOCUST = 10614,
            EFFECT_WELL_OF_POWER = 10615, // ticking condition conversion
            EFFECT_SIGNET_OF_SPITE = 10621,
            EFFECT_PLAGUE_SIGNET = 10630,
            EFFECT_LICH_FORM = 10631,
            EFFECT_FLESH_GOLEM = 10648,
            EFFECT_REAPER_SHROUD = 10668,
            EFFECT_SPECTRAL_WALK_GAIN_LF = 10686, // gain life force when walking in the darkness
            EFFECT_DETERMINED = 11641,
            EFFECT_ASURA_POWER_SUIT = 12326,
            EFFECT_QUICKENING_ZEPHYR = 12551,
            EFFECT_STEALTH = 13017,
            EFFECT_HASTE = 13067,
            EFFECT_TRAIT_ELEMENTALIST_EARTHS_EMBRACE = 13369,
            EFFECT_TRAIT_ELEMENTALIST_STRENGTH_OF_STONE = 13371,
            EFFECT_TRAIT_ELEMENTALIST_ELEMENTAL_SHIELDING = 13415,
            EFFECT_ARCANE_LIGHTNING = 13423,
            EFFECT_TRAIT_ELEMENTALIST_SERRATED_STONES = 13424,
            EFFECT_SPOTTER = 14055,
            EFFECT_BANNER_OF_POWER = 14417,
            EFFECT_SIGNET_OF_MIGHT = 14444,
            EFFECT_BANNER_OF_DISCIPLINE = 14449,
            EFFECT_BANNER_OF_TACTICS = 14450,
            EFFECT_BERSERKERS_STANCE = 14453,
            EFFECT_FRENZY = 14456,
            EFFECT_SIGNET_OF_THE_DOLYAK = 14458,
            EFFECT_SIGNET_OF_FURY = 14459,
            EFFECT_SIGNET_OF_STAMINA = 14478,
            EFFECT_RAMPAGE = 14484,
            EFFECT_SIGNET_OF_RAGE = 14496,
            EFFECT_BANNER_OF_DEFENSE = 14543,
            EFFECT_SIEGE_DEPLOYMENT_BLOCKED = 14712,
            EFFECT_SPECTRAL_WALK = 15083,
            EFFECT_PETRIFIED = 15090,
            EFFECT_FOOD_22_MF_50_POWER_15_EXP = 15255, // glazed pear tart
            EFFECT_FOOD_26_MF_60_POWER_15_EXP = 15258, // glazed peach tart
            EFFECT_CONJURE_EARTH_ATTRIBUTES = 15788,
            EFFECT_CONJURE_FLAME_ATTRIBUTES = 15789,
            EFFECT_CONJURE_FROST_ATTRIBUTES = 15790,
            EFFECT_CONJURE_FIRE_ATTRIBUTES = 15792,
            EFFECT_MAGNETIC_WAVE = 15794, // Scepter Earth 4
            EFFECT_FOOD_60_HEALING_50_CONDI_DMG = 15838, // orange passion fruit tart
            EFFECT_PORTAL = 16553,
            EFFECT_LAUREATE_COIN_BOOSTER = 16990,
            EFFECT_FOOD_100_POWER_70_FEROCITY = 17825, // bowl of sweet and spicy butternut squash soup
            EFFECT_TORMENT = 19426,
            EFFECT_TRAIT_ELEMENTALIST_ROCK_SOLID = 19522,
            EFFECT_GLYPH_ARCANE_POWER_EFFECT = 20449,
            EFFECT_ARCANE_SHIELD_EFFECT = 20450,
            EFFECT_SIGNET_OF_VAMPIRISM = 21761,
            EFFECT_DEFIANT_STANCE = 21816,
            EFFECT_UTILITY_SHARPENING_STONE_TOXIC = 21826,
            EFFECT_UTILITY_MAINTENANCE_OIL_TOXIC = 21827,
            EFFECT_UTILITY_FOCUSING_CRYSTAL_TOXIC = 21828,
            EFFECT_TRAIT_ELEMENTALIST_DIAMOND_SKIN = 22560,
            EFFECT_ROUSING_RESILIANCE = 24383,
            EFFECT_FOOD_70_TOUGHNESS_20_BOON_DURATION = 24797, // loaf of candy cactus cornbread
            EFFECT_FOOD_20_CONDI_DURATION_33_LIFESTEAL_ON_CRIT = 24800, // prickly pear pie
            EFFECT_GLYPH_FIRE_ELEMENTAL = 25475, // also Lesser Fire Elemental
            EFFECT_GLYPH_EARTH_ELEMENTAL = 25477,
            EFFECT_GLYPH_LESSER_AIR_ELEMENTAL = 25481,
            EFFECT_GLYPH_LESSER_ICE_ELEMENTAL = 25483,
            EFFECT_GLYPH_LESSER_EARTH_ELEMENTAL = 25484,
            EFFECT_GLYPH_ICE_ELEMENTAL = 25493,
            EFFECT_GLYPH_AIR_ELEMENTAL = 25496,
            EFFECT_BURNING_RETREAT = 25502,
            EFFECT_BURNING_SPEED = 25503,
            EFFECT_RAMS_REWARD_BOOSTER = 25627,
            EFFECT_UTILITY_TUNING_CRYSTAL_BOUNTIFUL = 25877,
            EFFECT_UTILITY_TUNING_CRYSTAL_FURIOUS = 25878,
            EFFECT_UTILITY_MAINTENANCE_OIL_BOUNTIFUL = 25879,
            EFFECT_UTILITY_SHARPENING_STONE_BOUNTIFUL = 25880,
            EFFECT_UTILITY_MAINTENANCE_OIL_FURIOUS = 25881,
            EFFECT_UTILITY_SHARPENING_STONE_FURIOUS = 25882,
            EFFECT_FOOD_20_BOON_DURATION_25_MF_DURING_LNY_33_MIGHT_ON_CRIT = 26530, // fried golden dumpling
            EFFECT_FOOD_20_BOON_DURATION_25_MF_DURING_LNY_100_SWIFTNESS_ON_KILL = 26534, // spring roll
            EFFECT_SLOW = 26766,
            EFFECT_RESISTANCE = 26980,
            EFFECT_BLOOD_RECKONING = 29466,
            EFFECT_BERSERK = 29502,
            EFFECT_QUICK_DRAW = 29703,
            EFFECT_INFUSING_TERROR = 30129, // reaper shroud 3
            EFFECT_VAMPIRIC_AURA = 30285,
            EFFECT_ALACRITY = 30328,
            EFFECT_NATURAL_MENDER = 30449,
            EFFECT_ATTUNEMENT_OVERLOADING = 30608,
            EFFECT_GLIDING = 30955,
            EFFECT_EXPERIENCE_BOOSTER = 31078,
            EFFECT_ITEM_BOOSTER = 31080,
            EFFECT_REBOUND = 31337,
            EFFECT_STATIC_CHARGE = 31487, // From overload air
            EFFECT_CELESTIAL_AVATAR = 31704,
            EFFECT_FLAMES_OF_WAR = 31708, // berserker torch 5
            EFFECT_GUILD_KARMA_BOOST_10 = 32103,
            EFFECT_GUILD_WVW_EXPERIENCE_BOOST_10 = 32665,
            EFFECT_LEADER_OF_THE_PACT_2 = 32675,
            EFFECT_GUILD_EXPERIENCE_BOOST_10 = 33106,
            EFFECT_GUILD_PVP_REWARD_TRACK_BOOST_10 = 33362,
            EFFECT_LEADER_OF_THE_PACT_3 = 33611,
            EFFECT_MAP_BONUS_BOOST_10 = 33772,
            EFFECT_GUILD_ITEM_RESEARCH_10 = 33833,
            EFFECT_GUILD_CRAFTING_BOOST_20 = 33969,
            EFFECT_GUILD_GATHERING_BOOST_20 = 33984,
            EFFECT_GRACE_OF_THE_LAND = 34062,
            EFFECT_ALWAYS_ANGRY = 34099, // berserker trait
            EFFECT_ROCK_GUARD = 34256, // berserker rage skill
            EFFECT_ROCK_BARRIER_EFFECT = 34633,
            EFFECT_STATIC_FIELD = 34641,
            EFFECT_UNSTEADY_GROUND = 34706,
            EFFECT_SPECTRAL_WALL = 34726,
            EFFECT_BALANCED_STANCE = 34778,
            EFFECT_GUILD_WVW_REWARD_TRACKB_BOOST_10 = 35126,
            EFFECT_END
        };



    }; // namespace GW2
}; // namespace GW2LIB

#endif

