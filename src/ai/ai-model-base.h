#ifndef AI_MODEL_BASE_H
#define AI_MODEL_BASE_H

#include <stack>
#include <memory>
#include <cstddef>
#include <ctime>

#include "behaviour-tree.h"
#include "bts-parser.h"

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
    size_t _hash = 0;
    Player* _playerRef = nullptr;

    std::unique_ptr<Root> _root;

    //
    // Behaviour tree script for AI management.
    //
    // It will be traversed starting from root node
    // every AIModel::Update() call.
    //
    std::string _scriptAsText;

    std::vector<uint8_t> _scriptCompiled;

    BTSParser _aiReader;

    std::unordered_map<std::string, ItemBonusType> _bonusTypeByDisplayName;

    Node* CreateNode(const ScriptNode* data);
    Node* CreateTask(const ScriptNode* data);
    Node* CreateConditionNode(const ScriptNode* data);

    std::function<BTResult()> GetConditionFunction(const ScriptNode* data);

    std::function<BTResult()> GetD100CF(const ScriptNode* data);
    std::function<BTResult()> GetIsPlayerVisibleCF();
    std::function<BTResult()> GetPlayerCanMoveCF();
    std::function<BTResult()> GetPlayerEnergyCF(const ScriptNode* data);
    std::function<BTResult()> GetPlayerNextTurnCF(const ScriptNode* data);
    std::function<BTResult()> GetPlayerInRangeCF(const ScriptNode* data);
    std::function<BTResult()> GetTurnsLeftCF(const ScriptNode* data);
    std::function<BTResult()> GetTurnsCheckCF(const ScriptNode* data);
    std::function<BTResult()> GetHasEffectCF(const ScriptNode* data);
    std::function<BTResult()> GetHPLowCF();
    std::function<BTResult()> GetHasEquippedCF(const ScriptNode* data);

    virtual void PrepareScript();

    void PrintBrains(Node* n, int indent);

    // -------------------------------------------------------------------------

    const std::unordered_map<ScriptParamNames, EquipmentCategory> _eqCats =
    {
      { ScriptParamNames::HEA, EquipmentCategory::HEAD   },
      { ScriptParamNames::NCK, EquipmentCategory::NECK   },
      { ScriptParamNames::TRS, EquipmentCategory::TORSO  },
      { ScriptParamNames::BTS, EquipmentCategory::BOOTS  },
      { ScriptParamNames::WPN, EquipmentCategory::WEAPON },
      { ScriptParamNames::SLD, EquipmentCategory::SHIELD },
      { ScriptParamNames::RNG, EquipmentCategory::RING   }
    };
};

#endif
