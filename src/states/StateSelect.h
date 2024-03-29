#pragma once
#include <map>

#include "state.h"

class StateSelect : public State {
public:
	StateSelect(SCore* _score, Core* _core);
	~StateSelect();

	void init() override;
	void update() override;
	void exitState(StateType targetState) override;

	void handleHover(std::string name) override;
	void handleClick(std::string name, bool active) override;

private:
	int selectedGame;
	std::map<std::string, StateType> gameList;
};