#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <vector>
#include <iostream>
#include "Compiler.h"

class StudentWorld;

struct Coord
{
	int x;
	int y;
};

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
public:
	Actor(int imageID, int startX, int startY, StudentWorld* sw, Direction startDirection = none, int depth = 0);
	virtual void doSomething() = 0;

	void setDead();
	bool isDead();

	void makeObstacle();
	bool canBlock();

	bool isInsect();
	void makeInsect();

	bool isFood();
	void makeFood();
	virtual void addFood(int amt);
	virtual int takeFood(int amt);

	void setStudentWorld(StudentWorld* p);
	StudentWorld* getStudentWorld();

	bool movedThisTurn();
	void setMovedThisTurnTrue();
	void setMovedThisTurnFalse();

	// hp access interface
	int getHp() const;
	void setHp(int health);

	// stun access interface
	int getStun();
	void addStun(int amount);
	bool canBeStunned();
	void setCanBeStunned(bool choice);

private:
	StudentWorld* m_studentWorld;
	bool m_isDead;
	int m_hp;
	int m_stun;
	bool m_canBeStunned;
	bool m_isInsect;
	bool m_isObstacle;
	bool m_movedThisTurn;
	bool m_isFood;
};

class Insect : public Actor
{
public:
	Insect(int imageID, int startX, int startY, StudentWorld* sw, int hp);
	virtual void doSomething() = 0;
	void changeDirection();
	bool attemptMove(Direction d);
private:
	int m_hp;
	int m_stun;
};

class Ant : public Insect
{
public:
	Ant(int imageID, int startX, int startY, StudentWorld* sw, Compiler *c, int colNum);
	virtual void doSomething();
	bool performInstructions(int n);
private:
	Compiler *m_comp;
	int m_instructCounter;
	int m_lastNum;
	int m_colNum;
	int m_heldFoodAmount;
	bool m_wasBlocked;
	bool m_wasBitten;
};

class Pebble : public Actor
{
public:
	Pebble(int startX, int startY, StudentWorld* sw);
	virtual void doSomething();

private:
};

class Food : public Actor
{
public:
	Food(int startX, int startY, StudentWorld* sw, int startAmount);
	virtual void doSomething();
	virtual int takeFood(int amt);
	virtual void addFood(int amt);
private:
	int m_amount;
};

class Pheromone : public Actor
{
public:
	Pheromone(int imageID, int startX, int startY, StudentWorld* sw);
	virtual void doSomething();
};

class Anthill : public Actor
{
public:
	Anthill(int startX, int startY, StudentWorld* sw, int colNumber, Compiler* c);
	virtual void doSomething();
	void giveBirth();
private:
	int m_colonyNumber;
	Compiler* m_c;
};

class Water : public Actor
{
public:
	Water(int startX, int startY, StudentWorld *sw);
	virtual void doSomething();
};

class Poison : public Actor
{
public:
	Poison(int startX, int startY, StudentWorld *sw);
	virtual void doSomething();
};

class BabyGrasshopper : public Insect
{
public:
	BabyGrasshopper(int startX, int startY, StudentWorld* sw);
	virtual void doSomething();
private:
	int m_distance;
};

class AdultGrasshopper : public Insect
{
public:
	AdultGrasshopper(int startX, int startY, StudentWorld* sw);
	virtual void doSomething();
	bool jump();
private:
	int m_distance;
};



#endif // ACTOR_H_
