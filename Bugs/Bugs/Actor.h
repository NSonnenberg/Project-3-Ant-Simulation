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

	virtual bool canBlock();
	virtual bool isInsect();
	virtual bool isFood();
	virtual bool isPheromone(int colNum);
	virtual bool isEnemy(int colNum);
	virtual bool isDangerous(int colNum);
	virtual bool isEnergyHolder();
	virtual bool isAnthill();

	virtual void getBitten(Actor *biter, int dmg);

	virtual void addFood(int amt);
	virtual int takeFood(int amt);

	void setStudentWorld(StudentWorld* p);
	StudentWorld* getStudentWorld();

	bool movedThisTurn();
	void setMovedThisTurnTrue();
	void setMovedThisTurnFalse();

	// hp access interface
	virtual int getHp() const;
	virtual void setHp(int health);

	// stun access interface
	int getStun();
	void addStun(int amount);
	bool canBeStunned();
	void setCanBeStunned(bool choice);

private:
	StudentWorld* m_studentWorld;
	bool m_isDead;
	int m_stun;
	bool m_canBeStunned;
	bool m_movedThisTurn;
};

class EnergyHolder : public Actor
{
public:
	EnergyHolder(int imageID, int startX, int startY, StudentWorld* sw, Direction startDirection, int hp, int depth);
	virtual void doSomething() = 0;
	virtual bool isEnergyHolder();
	virtual int getHp();
	virtual void setHp(int health);
	virtual void getBitten(Actor *biter, int dmg);

private:
	int m_hp;

};

class Insect : public EnergyHolder
{
public:
	Insect(int imageID, int startX, int startY, StudentWorld* sw, int hp);
	virtual void doSomething() = 0;
	virtual bool isInsect();
	void changeDirection();
	bool attemptMove(Direction d, int distance = 1);
private:
	int m_hp;
	int m_stun;
};

class Ant : public Insect
{
public:
	Ant(int imageID, int startX, int startY, StudentWorld* sw, Compiler *c, int colNum);
	virtual void doSomething();
	virtual bool isEnemy(int colNum);
	virtual bool isDangerous(int colNum);
	void performInstructions(int n);
	bool evaluateIf(int cmd, int instruct);
	bool smellDangerAhead();
	bool smellPheromoneAhead();
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
	virtual bool canBlock();

private:
};

class Food : public EnergyHolder
{
public:
	Food(int startX, int startY, StudentWorld* sw, int startAmount);
	virtual void doSomething();
	virtual int takeFood(int amt);
	virtual void addFood(int amt);
	virtual bool isFood();
private:
	int m_amount;
};

class Pheromone : public EnergyHolder
{
public:
	Pheromone(int imageID, int startX, int startY, StudentWorld* sw, int colNum);
	virtual void doSomething();
	virtual bool isPheromone(int colNum);
private:
	int m_colonyNumber;
};

class Anthill : public EnergyHolder
{
public:
	Anthill(int startX, int startY, StudentWorld* sw, int colNumber, Compiler* c);
	virtual void doSomething();
	virtual bool isAnthill();
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
	virtual bool isDangerous(int colNum = -1);
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
	virtual bool isEnemy(int colNum);
	virtual bool isDangerous(int colNum);
	virtual void getBitten(Actor *biter, int dmg);
	bool jump();
private:
	int m_distance;
};



#endif // ACTOR_H_
