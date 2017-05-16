#include "Actor.h"
#include "StudentWorld.h"

using namespace std;


// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

///////////////////////
// Actor Implementation
///////////////////////

Actor::Actor(int imageID, int startX, int startY, StudentWorld* sw, Direction startDirection, int depth)
	:GraphObject(imageID, startX, startY, startDirection)
{
	m_studentWorld = sw;
	m_isInsect = false;
	m_isObstacle = false;
	m_isDead = false;
	m_stun = 0;
	m_canBeStunned = true;
	m_movedThisTurn = false;
	m_isFood = false;
	return;
}

void Actor::setDead()
{
	m_isDead = true;
}

bool Actor::isDead()
{
	return m_isDead;
}

void Actor::makeObstacle()
{
	m_isObstacle = true;
}

bool Actor::canBlock()
{
	return m_isObstacle;
}

bool Actor::isInsect()
{
	return m_isInsect;
}

void Actor::makeInsect()
{
	m_isInsect = true;
}

bool Actor::isFood()
{
	return m_isFood;
}

void Actor::makeFood()
{
	m_isFood = true;
}

void Actor::addFood(int amt)
{
	return;
}

int Actor::takeFood(int amt)
{
	return -1;
}

void Actor::setStudentWorld(StudentWorld* p)
{
	m_studentWorld = p;
}

int Actor::getHp() const
{
	return m_hp;
}

void Actor::setHp(int health)
{
	m_hp = health;
}

int Actor::getStun()
{
	return m_stun;
}

void Actor::addStun(int amount)
{
	m_stun += amount;
}

bool Actor::canBeStunned()
{
	return m_canBeStunned;
}

void Actor::setCanBeStunned(bool choice)
{
	m_canBeStunned = choice;
}

bool Actor::movedThisTurn()
{
	return m_movedThisTurn;
}

void Actor::setMovedThisTurnFalse()
{
	m_movedThisTurn = false;
}

void Actor::setMovedThisTurnTrue()
{
	m_movedThisTurn = true;
}

StudentWorld* Actor::getStudentWorld()
{
	return m_studentWorld;
}

////////////////////////
// Insect Implementation
////////////////////////

Insect::Insect(int imageID, int startX, int startY, StudentWorld* sw, int hp)
	:Actor(imageID, startX, startY, sw, none, 1)
{
	changeDirection();
	setHp(hp);
	makeInsect();
}

bool Insect::attemptMove(Direction d)
{
	StudentWorld* sw = getStudentWorld();
	switch (d)
	{
	case up:
		if (!(sw->isObstacleAt(getX(), getY() + 1)))
		{
			moveTo(getX(), getY() + 1);
			return true;
		}
		return false;
		break;
	case right:
		if (!sw->isObstacleAt(getX() + 1, getY()))
		{
			moveTo(getX() + 1, getY());
			return true;
		}
		return false;
		break;
	case down:
		if (!sw->isObstacleAt(getX(), getY() - 1))
		{
			moveTo(getX(), getY() - 1);
			return true;
		}
		return false;
		break;
	case left:
		if (!sw->isObstacleAt(getX() - 1, getY()))
		{
			moveTo(getX() - 1, getY());
			return true;
		}
		return false;
		break;
	}

	return false;
}

void Insect::changeDirection()
{
	int randDirec = randInt(1, 4);
	switch (randDirec)
	{
	case 1:
		setDirection(up);
		break;
	case 2:
		setDirection(right);
		break;
	case 3:
		setDirection(down);
		break;
	case 4:
		setDirection(left);
		break;
	}
}

/////////////////////
// Ant Implementation
/////////////////////
Ant::Ant(int imgID, int startX, int startY, StudentWorld* sw, Compiler *c, int colNum)
	:Insect(imgID, startX, startY, sw, 1500)
{
	m_instructCounter = 0;
	m_comp = c;
	m_lastNum = 0;
	m_colNum = colNum;
	m_heldFoodAmount = 0;
	m_wasBlocked = false;
	m_wasBitten = false;
}

void Ant::doSomething()
{
	setHp(getHp() - 1);

	if (getHp() <= 0)
	{
		setDead();
		return;
	}

	if (getStun() > 0)
	{
		addStun(-1);
		return;
	}

	if (!attemptMove(getDirection()))
		changeDirection();
}

bool Ant::performInstructions(int n)
{
	StudentWorld *sw = getStudentWorld();
	Compiler::Command cmd;

	for (int i = 0; i < n; i++)
	{
		if (!m_comp->getCommand(m_instructCounter, cmd))
			return true;

		switch (cmd.opcode)
		{
		case Compiler::moveForward:
			if (!attemptMove(getDirection()))
			{
				m_wasBlocked = true;
			}
			m_wasBlocked = false;
			m_wasBitten = false;
			break;
		case Compiler::eatFood:
			if (m_heldFoodAmount > 0)
			{
				if (m_heldFoodAmount <= 100)
				{
					setHp(getHp() + m_heldFoodAmount);
					m_heldFoodAmount = 0;
				}
				else
				{
					setHp(getHp() + 100);
					m_heldFoodAmount -= 100;
				}
			}
			break;
		case Compiler::dropFood:
			sw->createFood(getX(), getY(), m_heldFoodAmount);
			m_heldFoodAmount = 0;
			break;
		case Compiler::bite:
			sw->biteInsect(getX(), getY(), 0, 15, this);
			break;
		case Compiler::emitPheromone:
			break;

		}

		++m_instructCounter;
	}

	return true;
}

////////////////////////
// Pebble Implementation
////////////////////////
Pebble::Pebble(int startX, int startY, StudentWorld* sw)
	:Actor(IID_ROCK, startX, startY, sw, right, 1)
{
	makeObstacle();
	return;
}

void Pebble::doSomething()
{
	// pebbles do nothing lol
	return;
}

//////////////////////
// Food Implementation
//////////////////////

Food::Food(int startX, int startY, StudentWorld* sw, int amount)
	:Actor(IID_FOOD, startX, startY, sw, right, 2)
{
	m_amount = amount;
	makeFood();
}

void Food::doSomething()
{
	return;
}

int Food::takeFood(int amt)
{
	if (amt <= m_amount)
	{
		m_amount -= amt;
		if (m_amount == 0)
			setDead();
		return amt;
	}

	else if (m_amount == 0)
	{
		setDead();
		return -1;
	}

	else
	{
		int tempAmt = m_amount;
		m_amount = 0;
		setDead();
		return tempAmt;
	}


}

void Food::addFood(int amt)
{
	m_amount += amt;
}

///////////////////////////
// Pheromone Implementation
///////////////////////////

//TODO: FIGURE OUT A WAY TO DECLARE TYPE
Pheromone::Pheromone(int imageID, int startX, int startY, StudentWorld* sw)
	:Actor(imageID, startX, startY, sw, right, 2)
{
	setHp(256);
}

void Pheromone::doSomething()
{
	setHp(getHp() - 1);
	if (getHp() <= 0)
		setDead();
}

/////////////////////////
// Anthill Implementation
/////////////////////////

Anthill::Anthill(int startX, int startY, StudentWorld* sw, int colNumber, Compiler* c)
	:Actor(IID_ANT_HILL, startX, startY, sw, right, 2)
{
	m_colonyNumber = colNumber;
	m_c = c;
	setHp(8999);
}

void Anthill::doSomething()
{
	StudentWorld* sw = getStudentWorld();

	setHp(getHp() - 1);
	if (getHp() <= 0)
	{
		setDead();
		return;
	}

	int food = sw->eatFood(getX(), getY(), 10000);
	if (food >= 0)
	{
		setHp(getHp() + food);
		return;
	}

	//TODO: make new antsss

}

void Anthill::giveBirth()
{
	StudentWorld *sw = getStudentWorld();
	sw->createNewAnt(getX(), getY(), m_colonyNumber, m_c);
}

///////////////////////
// Water Implementation
///////////////////////

Water::Water(int startX, int startY, StudentWorld* sw)
	:Actor(IID_WATER_POOL, startX, startY, sw, right, 2)
{
}

void Water::doSomething()
{
	StudentWorld *sw = getStudentWorld();
	sw->stunInsects(getX(), getY(), 2);
}

////////////////////////
// Poison Implementation
////////////////////////

Poison::Poison(int startX, int startY, StudentWorld* sw)
	:Actor(IID_POISON, startX, startY, sw, right, 2)
{

}

void Poison::doSomething()
{
//	std::cout << "I did something" << std::endl;
	StudentWorld *sw = getStudentWorld();
	sw->poisonInsects(getX(), getY());
}

/////////////////////////////////
// BabyGrasshopper Implementation
/////////////////////////////////
BabyGrasshopper::BabyGrasshopper(int startX, int startY, StudentWorld* sw)
	:Insect(IID_BABY_GRASSHOPPER, startX, startY, sw, 500)
{
	int m_distance = randInt(2, 10);
}

void BabyGrasshopper::doSomething()
{
	StudentWorld *sw = getStudentWorld();

	// lose 1 hp
	setHp(getHp() - 1);
	//std::cout << "HP: " << getHp() << std::endl;

	if (getHp() <= 0)
	{
		setDead();
		return;
	}

	if (getStun() > 0)
	{
		addStun(-1);
		return;
	}

	if (getHp() >= 1600)
	{
		sw->moultGrasshopper(getX(), getY());
		setDead();
		return;
	}

	// try to eat food
	int food = sw->eatFood(getX(), getY(), 200);
	if (food >= 0)
	{
		setHp(getHp() + food);
		int sleepChance = randInt(1, 2);
		if (sleepChance == 1)
		{
			addStun(2);
			setMovedThisTurnTrue();
			setCanBeStunned(true);
			return;
		}
	}

	// change direction if finished current walking distance
	if (m_distance == 0)
	{
		changeDirection();
		m_distance = randInt(2, 10);
	}

	// try to move
	if (!attemptMove(getDirection()))
	{
		m_distance = 0;
		setMovedThisTurnTrue();
		setCanBeStunned(true);
		return;
	}

	m_distance--;
	setMovedThisTurnTrue();
	setCanBeStunned(true);

	if (getStun() == 0)
	{
		addStun(2);
	}
}

//////////////////////////////////
// AdultGrasshopper Implementation
//////////////////////////////////


// TODO: REMOVE SUSCEPTIBLITY TO WATER/POISON AND MAKE IT BITE BACK AFTER GETTING BITTEN
AdultGrasshopper::AdultGrasshopper(int startX, int startY, StudentWorld* sw)
	:Insect(IID_ADULT_GRASSHOPPER, startX, startY, sw, 1600)
{
	changeDirection();
	int m_distance = randInt(2, 10);
}

void AdultGrasshopper::doSomething()
{
	StudentWorld *sw = getStudentWorld();
	setHp(getHp() - 1);

	if (getHp() <= 0)
	{
		setDead();
		return;
	}

	if (getStun() > 0)
	{
		addStun(-1);
		return;
	}

	int biteChance = randInt(1, 3);
	if (biteChance == 1)
	{
		int numInsects = sw->numInsectsAt(getX(), getY(), this);
		if (numInsects > 0)
		{
			int insectNum = randInt(1, numInsects);
			sw->biteInsect(getX(), getY(), insectNum, 50, this);
			addStun(2);
			setMovedThisTurnTrue();
			setCanBeStunned(true);
			return;
		}
	}

	int jumpChance = randInt(1, 10);
	if (jumpChance == 1)
	{
		if (jump())
		{
			addStun(2);
			setMovedThisTurnTrue();
			setCanBeStunned(true);
			return;
		}
	}

	int food = sw->eatFood(getX(), getY(), 200);
	if (food >= 0)
	{
		setHp(getHp() + food);
		int sleepChance = randInt(1, 2);
		if (sleepChance == 1)
		{
			addStun(2);
			setMovedThisTurnTrue();
			setCanBeStunned(true);
			return;
		}
	}

	// change direction if finished current walking distance
	if (m_distance == 0)
	{
		changeDirection();
		m_distance = randInt(2, 10);
	}

	// try to move
	if (!attemptMove(getDirection()))
	{
		m_distance = 0;
		setMovedThisTurnTrue();
		setCanBeStunned(true);
		return;
	}

	m_distance--;
	setMovedThisTurnTrue();
	setCanBeStunned(true);

	if (getStun() == 0)
	{
		addStun(2);
	}
}

bool AdultGrasshopper::jump()
{
	vector<Coord> locations;
	int count = 0;
	StudentWorld *sw = getStudentWorld();

	for (int x = -10; x <= 10; x++)
	{
		for (int y = -10; y <= 10; y++)
		{
			int circ = (x ^ 2) + (y ^ 2);
			if (circ <= 100 && !sw->isObstacleAt(x + getX(), y + getY()))
			{

				Coord pos;
				pos.x = x + getX();
				pos.y = y + getY();
				locations.push_back(pos);
				count++;
			}

		}
	}

	if (count == 0)
		return false;

	int num = randInt(1, count);
	Coord end = locations[num - 1];
	moveTo(end.x, end.y);

	return true;
}