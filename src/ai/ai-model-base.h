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

    int AgroRadius = 0;

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

    virtual void PrepareScript();

    void PrintBrains(Node* n, int indent);
};

#endif
