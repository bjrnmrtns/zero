#ifndef GAMESTATE__
#define GAMESTATE__

#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/mpl/list.hpp>
#include <iostream>

namespace sc = boost::statechart;

struct EvToMenu : sc::event<EvToMenu>
{
	EvToMenu(int someNumber)
	{
		std::cout << someNumber << std::endl;
	}
};


struct EvToGame : sc::event<EvToGame>
{
	EvToGame(int someNumber)
	{
		std::cout << someNumber << std::endl;
	}
};

struct InMenu;
struct InGame;
struct GameState : sc::state_machine<GameState, InMenu> {};
struct InGame : sc::simple_state<InGame, GameState>
{
	typedef sc::transition<EvToMenu, InMenu> reactions;
	InGame()
	{
		std::cout << "InGame State Hello" << std::endl;
	}
	~InGame()
	{
		std::cout << "InGame State Bye" << std::endl;
	}
};
struct InMenu : sc::simple_state<InMenu, GameState>
{
	typedef sc::transition<EvToGame, InGame> reactions;
	InMenu()
	{
		std::cout << "InMenu State Hello" << std::endl;
	}
	~InMenu()
	{
		std::cout << "InMenu State Bye" << std::endl;
	}
};

#endif
