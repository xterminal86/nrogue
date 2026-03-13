#ifndef MESSAGEBOXSTATE_H
#define MESSAGEBOXSTATE_H

#include <string>
#include <vector>

#include "position.h"
#include "gamestate.h"

class GameObject;

class MessageBoxState : public GameState
{
  public:
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

    void SetMessage(MessageBoxType type,
                    const std::string& header,
                    const std::vector<std::string>& message,
                    const uint32_t& borderColor = Colors::ShadesOfGrey::Six,
                    const uint32_t& bgColor = Colors::ShadesOfGrey::Two);

    void SetActorStats(GameObject* actor);

  private:
    void DisplayText();
    void DisplayActorStats();
    void DrawStatsBlock(const Position& start);

    std::string _header;
    std::vector<std::string> _message;

    uint32_t _bgColor     = Colors::Black;
    uint32_t _borderColor = Colors::White;

    Position _leftCorner;
    Position _windowSize;

    MessageBoxType _type = MessageBoxType::ANY_KEY;

    enum class WindowToShow
    {
      TEXT = 0,
      ACTOR_STATS
    };

    WindowToShow _windowToShow = WindowToShow::TEXT;

    GameObject* _actorRef = nullptr;
};

#endif // MESSAGEBOXSTATE_H
