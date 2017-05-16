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
	m_isDead = false;
	m_stun = 0;
	m_canBeStunned = true;
	m_movedThisTurn = false;
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

bool Actor::canBlock()
{
	return false;
}

bool Actor::isInsect()
{
	return false;
}

bool Actor::isFood()
{
	return false;
}

bool Actor::isPheromone(int colNum)
{
	return false;
}

bool Actor::isEnemy(int colNum)
{
	return false;
}

bool Actor::isDangerous(int colNum)
{
	return false;
}

bool Actor::isEnergyHolder()
{
	return false;
}

bool Actor::isAnthill()
{
	return false;
}

void Actor::getBitten(Actor *biter, int dmg)
{
	return;
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
	return 0;
}

void Actor::setHp(int health)
{
	 return;
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

//////////////////////////////
// EnergyHolder Implementation
//////////////////////////////

EnergyHolder::EnergyHolder(int imageID, int startX, int startY, StudentWorld* sw, Direction startDirection, int hp, int depth)
	:Actor(imageID, startX, startY, sw, none, depth)
{
	m_hp = hp;
}

bool EnergyHolder::isEnergyHolder()
{
	return true;
}

int EnergyHolder::getHp()
{
	return m_hp;
}

void EnergyHolder::setHp(int health)
{
	m_hp = health;
}

void EnergyHolder::getBitten(Actor *biter, int dmg)
{
	m_hp -= dmg;
}

////////////////////////
// Insect Implementation
////////////////////////

Insect::Insect(int imageID, int startX, int startY, StudentWorld* sw, int hp)
	:EnergyHolder(imageID, startX, startY, sw, none, hp, 1)
{
	changeDirection();
}

bool Insect::isInsect()
{
	return true;
}

bool Insect::attemptMove(Direction d, int distance)
{
	StudentWorld* sw = getStudentWorld();
	switch (d)
	{
	case up:
		if (!(sw->isObstacleAt(getX(), getY() + distance)))
		{
			moveTo(getX(), getY() + distance);
			return true;
		}
		return false;
		break;
	case right:
		if (!sw->isObstacleAt(getX() + distance, getY()))
		{
			moveTo(getX() + distance, getY());
			return true;
		}
		return false;
		break;
	case down:
		if (!sw->isObstacleAt(getX(), getY() - distance))
		{
			moveTo(getX(), getY() - distance);
			return true;
		}
		return false;
		break;
	case left:
		if (!sw->isObstacleAt(getX() - distance, getY()))
		{
			moveTo(getX() - distance, getY());
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

	int numInstructs = randInt(1, 10);
	performInstructions(numInstructs);
}

bool Ant::isEnemy(int colNum)
{
	return (m_colNum != colNum);
}

bool Ant::isDangerous(int colNum)
{
	if (m_colNum != colNum)
		return true;
	else
		return false;
}

void Ant::performInstructions(int n)
{
	StudentWorld *sw = getStudentWorld();
	Compiler::Command cmd;

	for (int i = 0; i < n; i++)
	{
		if (!m_comp->getCommand(m_instructCounter, cmd))
			return;

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
		case Compiler::pickupFood: //TODO: IMPLEMENT 1800 FOOD LIMIT
			m_heldFoodAmount = sw->eatFood(getX(), getY(), 400);
		case Compiler::emitPheromone:
			sw->emitPheromone(getX(), getY(), m_colNum, 256);
			break;
		case Compiler::faceRandomDirection:
			changeDirection();
			break;
		case Compiler::generateRandomNumber:
			m_lastNum = randInt(1, (stoi(cmd.operand1) - 1));
			break;
		case Compiler::goto_command:
			m_instructCounter = stoi(cmd.operand1);
			break;
		case Compiler::if_command:
			evaluateIf(stoi(cmd.operand1), stoi(cmd.operand2));
			break;
		}

		++m_instructCounter;
	}
}

bool Ant::evaluateIf(int cmd, int instruct)
{
	StudentWorld *sw = getStudentWorld();
	switch (cmd)
	{
	case 1: // last_random_number_was_zero
		if (m_lastNum == 0)
			m_instructCounter = instruct;
		break;
	case 2: // I_am_carrying_food
		if (m_heldFoodAmount > 0)
			m_instructCounter = instruct;
		break;
	case 3: // I_am_hungry
		if (getHp() <= 25)
			m_instructCounter = instruct;
		break;
	case 4: // I_am_standing_with_an_enemy
		if (sw->isEnemyAt(getX(), getY(), m_colNum))
			m_instructCounter = instruct;
		break;
	case 5: // I_am_standing_on_food
		if (sw->isFoodAt(getX(), getY()))
			m_instructCounter = instruct;
		break;
	case 6: // I_am_standing_on_my_anthill
		if (sw->isAnthillAt(getX(), getY()))
			m_instructCounter = instruct;
		break;
	case 7: // I_smell_pheromone_in_front_of_me 
		if (smellPheromoneAhead())
			m_instructCounter = instruct;
		break;
	case 8: // I_smell_danger_in_front_of_me 
		if (smellDangerAhead())
			m_instructCounter = instruct;
		break;
	case 9: // I_was_bit
		if (m_wasBitten)
			m_instructCounter = instruct;
		break;
	case 10: // I_was_blocked_from_moving
		if (m_wasBlocked)
			m_instructCounter = instruct;
		break;


	}
	return true;
}

bool Ant::smellDangerAhead()
{
	StudentWorld* sw = getStudentWorld();
	switch (getDirection())
	{
	case up:
		return (sw->isDangerAt(getX(), getY() + 1, m_colNum));
		break;
	case right:
		return (sw->isDangerAt(getX() + 1, getY(), m_colNum));
		return false;
		break;
	case down:
		return (sw->isDangerAt(getX(), getY() - 1, m_colNum));
		return false;
		break;
	case left:
		return (sw->isDangerAt(getX() - 1, getY(), m_colNum));
		return false;
		break;
	}

	return false;
}

bool Ant::smellPheromoneAhead()
{
	StudentWorld* sw = getStudentWorld();
	switch (getDirection())
	{
	case up:
		return (sw->isPheromoneAt(getX(), getY() + 1, m_colNum));
		break;
	case right:
		return (sw->isPheromoneAt(getX() + 1, getY(), m_colNum));
		return false;
		break;
	case down:
		return (sw->isPheromoneAt(getX(), getY() - 1, m_colNum));
		return false;
		break;
	case left:
		return (sw->isPheromoneAt(getX() - 1, getY(), m_colNum));
		return false;
		break;
	}

	return false;
}

////////////////////////
// Pebble Implementation
////////////////////////
Pebble::Pebble(int startX, int startY, StudentWorld* sw)
	:Actor(IID_ROCK, startX, startY, sw, right, 1)
{
	return;
}

void Pebble::doSomething()
{
	// pebbles do nothing lol
	return;
}

bool Pebble::canBlock()
{
	return true;
}

//////////////////////
// Food Implementation
//////////////////////

Food::Food(int startX, int startY, StudentWorld* sw, int amount)
	:EnergyHolder(IID_FOOD, startX, startY, sw, right, 0, 2)
{
	m_amount = amount;
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

bool Food::isFood()
{
	return true;
}

///////////////////////////
// Pheromone Implementation
///////////////////////////

Pheromone::Pheromone(int imageID, int startX, int startY, StudentWorld* sw, int colNum)
	:EnergyHolder(imageID, startX, startY, sw, right, 256, 2)
{
	m_colonyNumber = colNum;
}

void Pheromone::doSomething()
{
	setHp(getHp() - 1);
	if (getHp() <= 0)
		setDead();
}

bool Pheromone::isPheromone(int colNum)
{
	return (m_colonyNumber == colNum);
}

/////////////////////////
// Anthill Implementation
/////////////////////////

Anthill::Anthill(int startX, int startY, StudentWorld* sw, int colNumber, Compiler* c)
	:EnergyHolder(IID_ANT_HILL, startX, startY, sw, right, 8900, 2)
{
	m_colonyNumber = colNumber;
	m_c = c;
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

	//TODO: make new antss
	if (getHp() >= 2000)
	{
		sw->createNewAnt(getX(), getY(), m_colonyNumber, m_c);
		setHp(getHp() - 1500);
	}
}

bool Anthill::isAnthill()
{
	return true;
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

bool Poison::isDangerous(int colNum)
{
	return true;
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


// TODO: REMOVE SUSCEPTIBLITY TO WATER/POISON
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

bool AdultGrasshopper::isEnemy(int colNum)
{
	return true;
}

bool AdultGrasshopper::isDangerous(int colNum)
{
	return true;
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

void AdultGrasshopper::getBitten(Actor *biter, int dmg)
{
	EnergyHolder::getBitten(biter, dmg);
	if (getHp() <= 0)
	{
		setDead();
		return;
	}

	int chance = randInt(0, 1);
	if (chance == 0)
	{
		biter->getBitten(this, 150);
	}
}