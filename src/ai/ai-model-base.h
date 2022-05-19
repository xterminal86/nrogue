#ifndef AI_MODEL_BASE_H
#define AI_MODEL_BASE_H

#include <stack>
#include <memory>
#include <cstddef>
#include <ctime>

#include "behaviour-tree.h"
#include "bts-parser.h"

enum class BTNodeType
{
  NONE = -1,
  TREE,
  SEQ,
  SEL,
  SUCC,
  FAIL,
  COND,
  TASK
};

enum class AITasks
{
  NONE = -1,
  IDLE,
  MOVE_RND,
  MOVE_SMART,
  MOVE_AWAY,
  ATTACK,
  ATTACK_RANGED,
  BREAK_STUFF,
  PICK_ITEMS,
  CHASE_PLAYER,
  SAVE_PLAYER_POS,
  GOTO_LAST_PLAYER_POS,
  GOTO_LAST_MINED_POS,
  MINE_TUNNEL,
  MINE_BLOCK,
  APPLY_EFFECT,
  DRINK_POTION,
  PRINT_MESSAGE,
  END
};

enum class AIConditionFunctions
{
  NONE = -1,
  D100,
  PLAYER_VISIBLE,
  PLAYER_CAN_MOVE,
  PLAYER_IN_RANGE,
  PLAYER_ENERGY,
  PLAYER_NEXT_TURN,
  HAS_EQUIPPED,
  TURNS_LEFT,
  HAS_EFFECT,
  HP_LOW
};

class AIComponent;
class Player;

class AIModelBase
{
  public:
    AIModelBase();
    virtual ~AIModelBase() = default;

    virtual void Update();

    void ConstructAI();

    AIComponent* AIComponentRef = nullptr;

    bool IsAgressive = false;

  protected:
    size_t _hash;
    Player* _playerRef = nullptr;

    std::unique_ptr<Root> _root;

    // Behaviour tree script for AI management.
    //
    // It will be traversed starting from root node
    // every AIModel::Update() call.
    std::string _script;

    BTSParser _aiReader;

    std::map<std::string, ItemBonusType> _bonusTypeByDisplayName;

    Node* CreateNode(const ScriptNode* data);
    Node* CreateTask(const ScriptNode* data);
    Node* CreateConditionNode(const ScriptNode* data);

    std::function<BTResult()> GetConditionFunction(const ScriptNode* data);

    std::function<BTResult()> GetD100CF(const ScriptNode* data);
    std::function<BTResult()> GetIsPlayerVisibleCF(const ScriptNode* data);
    std::function<BTResult()> GetPlayerCanMoveCF(const ScriptNode* data);
    std::function<BTResult()> GetPlayerEnergyCF(const ScriptNode* data);
    std::function<BTResult()> GetPlayerNextTurnCF(const ScriptNode* data);
    std::function<BTResult()> GetInRangeCF(const ScriptNode* data);
    std::function<BTResult()> GetTurnsLeftCF(const ScriptNode* data);
    std::function<BTResult()> GetHasEffectCF(const ScriptNode* data);
    std::function<BTResult()> GetHPLowCF(const ScriptNode* data);
    std::function<BTResult()> GetHasEquippedCF(const ScriptNode* data);

    virtual void PrepareScript();

    void PrintBrains(Node* n, int indent);

    const std::map<std::string, BTNodeType> _btNodeTypeByName =
    {
      { "TREE", BTNodeType::TREE },
      { "SEQ",  BTNodeType::SEQ  },
      { "SEL",  BTNodeType::SEL  },
      { "SUCC", BTNodeType::SUCC },
      { "FAIL", BTNodeType::FAIL },
      { "COND", BTNodeType::COND },
      { "TASK", BTNodeType::TASK },
    };

    const std::map<std::string, AITasks> _aiTasksByName =
    {
      { "idle",                 AITasks::IDLE                 },
      { "move_rnd",             AITasks::MOVE_RND             },
      { "move_smart",           AITasks::MOVE_SMART           },
      { "move_away",            AITasks::MOVE_AWAY            },
      { "attack",               AITasks::ATTACK               },
      { "attack_ranged",        AITasks::ATTACK_RANGED        },
      { "break_stuff",          AITasks::BREAK_STUFF          },
      { "pick_items",           AITasks::PICK_ITEMS           },
      { "chase_player",         AITasks::CHASE_PLAYER         },
      { "save_player_pos",      AITasks::SAVE_PLAYER_POS      },
      { "goto_last_player_pos", AITasks::GOTO_LAST_PLAYER_POS },
      { "goto_last_mined_pos",  AITasks::GOTO_LAST_MINED_POS  },
      { "mine_tunnel",          AITasks::MINE_TUNNEL          },
      { "mine_block",           AITasks::MINE_BLOCK           },
      { "apply_effect",         AITasks::APPLY_EFFECT         },
      { "drink_potion",         AITasks::DRINK_POTION         },
      { "print_message",        AITasks::PRINT_MESSAGE        },
      { "end",                  AITasks::END                  }
    };

    const std::map<std::string, AIConditionFunctions> _cfByName =
    {
      { "d100",             AIConditionFunctions::D100             },
      { "player_visible",   AIConditionFunctions::PLAYER_VISIBLE   },
      { "player_can_move",  AIConditionFunctions::PLAYER_CAN_MOVE  },
      { "player_in_range",  AIConditionFunctions::PLAYER_IN_RANGE  },
      { "player_energy",    AIConditionFunctions::PLAYER_ENERGY    },
      { "player_next_turn", AIConditionFunctions::PLAYER_NEXT_TURN },
      { "turns_left",       AIConditionFunctions::TURNS_LEFT       },
      { "has_effect",       AIConditionFunctions::HAS_EFFECT       },
      { "has_equipped",     AIConditionFunctions::HAS_EQUIPPED     },
      { "hp_low",           AIConditionFunctions::HP_LOW           }
    };

    const std::map<std::string, EquipmentCategory> _eqCategoryByName =
    {
      { "HEA", EquipmentCategory::HEAD   },
      { "NCK", EquipmentCategory::NECK   },
      { "TRS", EquipmentCategory::TORSO  },
      { "BTS", EquipmentCategory::BOOTS  },
      { "WPN", EquipmentCategory::WEAPON },
      { "SLD", EquipmentCategory::SHIELD },
      { "RNG", EquipmentCategory::RING   }
    };
};

#endif
