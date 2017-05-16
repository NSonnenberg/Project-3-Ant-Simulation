#include "StudentWorld.h"
#include <string>
#include <Windows.h>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(string assetDir)
	: GameWorld(assetDir)
{

	m_tickCount = 0;

	m_c0 = new Compiler;
	m_c1 = new Compiler;
	m_c2 = new Compiler;
	m_c3 = new Compiler;

	m_ants0 = 0;
	m_ants1 = 0;
	m_ants2 = 0;
	m_ants3 = 0;
}

StudentWorld::~StudentWorld()
{
	cleanUp();
}

int StudentWorld::init()
{
	m_fieldname = getFieldFilename();
	m_fieldname = "field.txt";
	if (m_field.loadField(m_fieldname))
		return false;

	cout << "initializing" << endl;

	for (int i = 0; i < VIEW_HEIGHT; i++)
	{
		for (int j = 0; j < VIEW_WIDTH; j++)
		{
			Field::FieldItem item = m_field.getContentsOf(i, j);
			Actor *p = createObject(item, j, 63 - i);
			if (p != nullptr)
			{
				m_objects[63 - i][j].push_back(p);
			}
		}
	}

	cout << "done initializing" << endl;
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	m_tickCount++;

	// let all actors do something
	for (int i = 0; i < VIEW_HEIGHT; i++)
	{
		for (int j = 0; j < VIEW_WIDTH; j++)
		{
			if (!m_objects[i][j].empty())
			{
				list<Actor*>::iterator it;
				it = m_objects[i][j].begin();
				while (it != m_objects[i][j].end())
				{
					if (!(*it)->movedThisTurn())
						(*it)->doSomething();
					int t_x = (*it)->getX();
					int t_y = (*it)->getY();
					if (t_x != j || t_y != i)
					{
						m_objects[t_y][t_x].push_back(*it);
						it = m_objects[i][j].erase(it);
					}
					else if ((*it)->isDead())
					{
						if ((*it)->isInsect())
						{
							createFood(j, i, 100);
						}
						//cout << "Tick :" << m_tickCount << endl;
						delete (*it);
						it = m_objects[i][j].erase(it);
					}
					else
						it++;
				}
				//cout << "Tick : " << m_tickCount << endl;
			}
		}
	}

	for (int i = 0; i < VIEW_HEIGHT; i++)
	{
		for (int j = 0; j < VIEW_WIDTH; j++)
		{
			if (!m_objects[i][j].empty())
			{
				list<Actor*>::iterator it;
				it = m_objects[i][j].begin();
				while (it != m_objects[i][j].end())
				{
					(*it)->setMovedThisTurnFalse();
					it++;
				}
			}
		}
	}

	string ticks = "Tick: ";
	string temp;
	temp = to_string(m_tickCount);
	//cout << "tick " << m_tickCount << endl;
	ticks += temp;
	setGameStatText(ticks);

	// TODO:GET RID OF THIS LINE
	//Sleep(1000);

	// if 2000 ticks has been reached, end the game and announce a winner
	if (m_tickCount >= 2000)
	{
		return GWSTATUS_NO_WINNER;
	}
	// This code is here merely to allow the game to build, run, and terminate after you hit enter.
	// Notice that the return value GWSTATUS_NO_WINNER will cause our framework to end the simulation.
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	for (int i = 0; i < VIEW_HEIGHT; i++)
	{
		for (int j = 0; j < VIEW_WIDTH; j++)
		{
			if (!m_objects[i][j].empty())
			{
				list<Actor*>::iterator it;
				it = m_objects[i][j].begin();

				while (it != m_objects[i][j].end())
				{
					Actor *p = (*it);
					delete p;
					it = m_objects[i][j].erase(it);
				}
			}
		}
	}

	delete m_c0;
	delete m_c1;
	delete m_c2;
	delete m_c3;
}

Actor* StudentWorld::createObject(Field::FieldItem item, int x, int y)
{
	vector<string> fileNames = getFilenamesOfAntPrograms();
	string err;

	switch (item)
	{
	case Field::rock:
		return new Pebble(x, y, this);
		break;
	case Field::grasshopper:
		return new BabyGrasshopper(x, y, this);
		break;
	case Field::food:
		//TODO: MAKE FOOD GREAT AGAIN
		return new Food(x, y, this, 6000);
		break;
	case Field::anthill0:
		if (!m_c0->compile(fileNames[0], err))
		{
			setError(err);
			return nullptr;
		}
		return new Anthill(x, y, this, 0, m_c0);
		break;
	case Field::anthill1:
		if (!m_c1->compile(fileNames[1], err))
		{
			setError(err);
			return nullptr;
		}
		return new Anthill(x, y, this, 1, m_c1);
		break;
	case Field::anthill2:
		if (!m_c2->compile(fileNames[2], err))
		{
			setError(err);
			return nullptr;
		}
		return new Anthill(x, y, this, 2, m_c2);
		break;
	case Field::anthill3:
		if (!m_c3->compile(fileNames[3], err))
		{
			setError(err);
			return nullptr;
		}
		return new Anthill(x, y, this, 3, m_c3);
		break;
	case Field::water:
		return new Water(x, y, this);
		break;
	case Field::poison:
		return new Poison(x, y, this);
		break;
	default:
		return nullptr;
	}
}

void StudentWorld::createFood(int x, int y, int amount)
{
	if (!m_objects[y][x].empty())
	{
		list<Actor*>::iterator it;
		it = m_objects[y][x].begin();
		while (it != m_objects[y][x].end())
		{
			if ((*it)->isFood())
			{
				(*it)->addFood(amount);
			}

			it++;
		}
	}
	Actor *p = new Food(x, y, this, amount);
	m_objects[y][x].push_back(p);
}

bool StudentWorld::isObstacleAt(int x, int y)
{
	if (x < 0 || y < 0 || x > 63 || y > 63)
		return true;

	if (!m_objects[y][x].empty())
	{
		list<Actor*>::iterator it;
		it = m_objects[y][x].begin();

		while (it != m_objects[y][x].end())
		{
			if ((*it)->canBlock())
				return true;

			it++;
		}

	}

	return false;
}

bool StudentWorld::isFoodAt(int x, int y)
{
	if (!m_objects[y][x].empty())
	{
		list<Actor*>::iterator it;
		it = m_objects[y][x].begin();

		while (it != m_objects[y][x].end())
		{
			if ((*it)->isFood() && !(*it)->isDead())
				return true;

			it++;
		}

	}

	return false;
}

bool StudentWorld::isEnemyAt(int x, int y, int colNum)
{
	if (!m_objects[y][x].empty())
	{
		list<Actor*>::iterator it;
		it = m_objects[y][x].begin();

		while (it != m_objects[y][x].end())
		{
			if ((*it)->isEnemy(colNum) && !(*it)->isDead())
				return true;

			it++;
		}

	}

	return false;
}

bool StudentWorld::isDangerAt(int x, int y, int colNum)
{
	if (!m_objects[y][x].empty())
	{
		list<Actor*>::iterator it;
		it = m_objects[y][x].begin();

		while (it != m_objects[y][x].end())
		{
			if ((*it)->isDangerous(colNum) && !(*it)->isDead())
				return true;

			it++;
		}

	}

	return false;
}

bool StudentWorld::isAnthillAt(int x, int y)
{
	if (!m_objects[y][x].empty())
	{
		list<Actor*>::iterator it;
		it = m_objects[y][x].begin();

		while (it != m_objects[y][x].end())
		{
			if ((*it)->isAnthill() && !(*it)->isDead())
				return true;

			it++;
		}

	}
}

bool StudentWorld::isPheromoneAt(int x, int y, int colNum)
{
	if (!m_objects[y][x].empty())
	{
		list<Actor*>::iterator it;
		it = m_objects[y][x].begin();

		while (it != m_objects[y][x].end())
		{
			if ((*it)->isPheromone(colNum) && !(*it)->isDead())
				return true;

			it++;
		}

	}
}

int StudentWorld::numInsectsAt(int x, int y, Actor *caller)
{
	int count = 0;

	if (!m_objects[y][x].empty())
	{
		list<Actor*>::iterator it;
		it = m_objects[y][x].begin();

		while (it != m_objects[y][x].end())
		{
			if ((*it)->isInsect() && caller != (*it))
				count++;

			it++;
		}
	}

	return count;
}


void StudentWorld::stunInsects(int x, int y, int amount)
{
	if (!m_objects[y][x].empty())
	{
		list<Actor*>::iterator it;
		it = m_objects[y][x].begin();

		while (it != m_objects[y][x].end())
		{
			if ((*it)->isInsect() && (*it)->canBeStunned())
			{
				(*it)->addStun(amount);
				(*it)->setCanBeStunned(false);
			}

			it++;
		}
	}
}


void StudentWorld::poisonInsects(int x, int y)
{
	if (!m_objects[y][x].empty())
	{
		list<Actor*>::iterator it;
		it = m_objects[y][x].begin();

		while (it != m_objects[y][x].end())
		{
			if ((*it)->isInsect())
			{
				(*it)->setHp((*it)->getHp() - 150);
			}

			it++;
		}
	}
}

int StudentWorld::eatFood(int x, int y, int amount)
{
	if (!m_objects[y][x].empty())
	{
		list<Actor*>::iterator it;
		it = m_objects[y][x].begin();

		while (it != m_objects[y][x].end())
		{
			if ((*it)->isFood())
			{
				int amt = (*it)->takeFood(amount);
				if (amt >= 0)
					return amt;
				else
					return -1;
			}

			it++;
		}

	}

	return -1;
}

void StudentWorld::biteInsect(int x, int y, int index, int dmg, Actor *caller)
{
	int i = 0;

	if (!m_objects[y][x].empty())
	{
		list<Actor*>::iterator it;
		it = m_objects[y][x].begin();

		while (it != m_objects[y][x].end())
		{
			if ((*it)->isInsect() && caller != (*it))
				i++;

			if (i == index)
			{
				(*it)->getBitten(caller, dmg);
				cout << "bit an insect" << endl;
				return;
			}

			it++;
		}
	}
}

void StudentWorld::moultGrasshopper(int x, int y)
{
	Actor *p = new AdultGrasshopper(x, y, this);
	m_objects[y][x].push_back(p);
}

void StudentWorld::createNewAnt(int x, int y, int colNum, Compiler *c)
{
	Actor *p;

	switch (colNum)
	{
	case 0:
		p = new Ant(IID_ANT_TYPE0, x, y, this, c, colNum);
		m_objects[y][x].push_back(p);
		m_ants0++;
		break;
	case 1:
		p = new Ant(IID_ANT_TYPE1, x, y, this, c, colNum);
		m_objects[y][x].push_back(p);
		m_ants1++;
		break;
	case 2:
		p = new Ant(IID_ANT_TYPE2, x, y, this, c, colNum);
		m_objects[y][x].push_back(p);
		m_ants2++;
		break;
	case 3:
		p = new Ant(IID_ANT_TYPE3, x, y, this, c, colNum);
		m_objects[y][x].push_back(p);
		m_ants3++;
		break;
	}
}

void StudentWorld::emitPheromone(int x, int y, int colNum, int amount)
{
	if (!m_objects[y][x].empty())
	{
		list<Actor*>::iterator it;
		it = m_objects[y][x].begin();

		while (it != m_objects[y][x].end())
		{
			if ((*it)->isPheromone(colNum))
			{
				int currStrength = (*it)->getHp();
				currStrength += amount;
				if (currStrength > 768)
				{
					int subAmt = currStrength - 768;
					currStrength -= subAmt;
					(*it)->setHp(currStrength);
				}
				else
					(*it)->setHp(currStrength);
				return;
			}

			it++;
		}
	}
	Actor *p;
	switch (colNum)
	{
	case 0:
		p = new Pheromone(IID_PHEROMONE_TYPE0, x, y, this, colNum);
		m_objects[y][x].push_back(p);
		break;
	case 1:
		p = new Pheromone(IID_PHEROMONE_TYPE1, x, y, this, colNum);
		m_objects[y][x].push_back(p);
		break;
	case 2:
		p = new Pheromone(IID_PHEROMONE_TYPE2, x, y, this, colNum);
		m_objects[y][x].push_back(p);
		break;
	case 3:
		p = new Pheromone(IID_PHEROMONE_TYPE3, x, y, this, colNum);
		m_objects[y][x].push_back(p);
		break;
	}

}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
