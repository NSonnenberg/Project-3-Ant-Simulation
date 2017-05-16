#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Field.h"
#include "Actor.h"
#include "Compiler.h"
#include <string>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	
	StudentWorld(std::string assetDir);

	~StudentWorld();

	virtual int init();

	virtual int move();

	virtual void cleanUp();

	bool isObstacleAt(int x, int y);
	bool isFoodAt(int x, int y);
	bool isEnemyAt(int x, int y, int colNum);
	bool isDangerAt(int x, int y, int colNum);
	bool isAnthillAt(int x, int y);
	bool isPheromoneAt(int x, int y, int colNum);

	int numInsectsAt(int x, int y, Actor *caller);

	void stunInsects(int x, int y, int amount);
	void poisonInsects(int x, int y);

	void moultGrasshopper(int x, int y);

	int eatFood(int x, int y, int amount);

	void biteInsect(int x, int y, int index, int dmg, Actor *caller);

	void createNewAnt(int x, int y, int colNum, Compiler* c);

	void emitPheromone(int x, int y, int colNum, int amount);

	void createFood(int x, int y, int amount);

	Actor* createObject(Field::FieldItem item, int x, int y);

private:

	std::list<Actor*> m_objects[VIEW_HEIGHT][VIEW_WIDTH];
	std::string m_fieldname;
	Field m_field;
	int m_tickCount;

	Compiler* m_c0;
	Compiler* m_c1;
	Compiler* m_c2;
	Compiler* m_c3;

	int m_ants0;
	int m_ants1;
	int m_ants2;
	int m_ants3;
};

#endif // STUDENTWORLD_H_
