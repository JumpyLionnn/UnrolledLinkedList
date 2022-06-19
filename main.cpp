#include "Timer.h"

#include <list>
#include <vector>
#include "UnrolledLinkedList.h"


struct Player
{

	float x = 0;
	float y = 0;
	float width = 0;
	float height = 0;
	float hunger = 0;
	int health = 0;

	Player(float health)
		: health(health)
	{}

	Player() = default;
};

#define DISABLE_OTHER 0

static const int elementCount = 100000;

void Test()
{
	float listSum = 0;
	float vectorSum = 0;
	float unrolledListSum = 0;
	float staticUnrolledListSum = 0;
		
	#if !DISABLE_OTHER
	std::list<Player> list;
	{
		Timer timer("list-insert");
		for (int i = 0; i < elementCount; i++)
		{
			//list.emplace_front(0.25f * (float)i);
			list.emplace_back((float)i);
		}
	}


	std::vector<Player> vector;
	{
		Timer timer("vector-insert");
		for (int i = 0; i < elementCount; i++)
		{
			//vector.emplace(vector.begin(), 0.12f * (float)i);
			vector.emplace_back((float)i);
		}
	}
	#endif

	static const int NodeCapacity = 500;
	UnrolledLinkedList<Player, NodeCapacity> unrolledList;
	{
		Timer timer("unrolledLinkedList-insert");
		for (int i = 0; i < elementCount; i++)
		{
			unrolledList.Insert(Player((float)i));
		}
	}

	StaticUnrolledLinkedList<Player, NodeCapacity> staticUnrolledList;
	{
		Timer timer("staticUnrolledLinkedList-insert");
		for (int i = 0; i < elementCount; i++)
		{
			staticUnrolledList.Insert(Player((float)i));
		}
	}

	std::cout << std::endl;

	
	#if !DISABLE_OTHER 
	{
		int index = 0;
		Timer timer("list-delete");
		for (std::list<Player>::iterator it = list.begin(); it != list.end();)
		{
			if (index % 2 == 0)
				it = list.erase(it);
			else
				it++;
			index++;
		}
	}

	
	{
		int index = 0;
		Timer timer("vector-delete");
		for (std::vector<Player>::iterator it = vector.begin(); it != vector.end();)
		{
			if (index % 2 == 0)
				it = vector.erase(it);
			else
				it++;
			index++;
		}
	}
	
	
	#endif

	{
		int index = 0;
		Timer timer("unrolledLinkedList-delete");
		for (auto it = unrolledList.begin(); it != unrolledList.end();)
		{
			if (index % 2 == 0)
				it = unrolledList.Remove(it);
			else
				it++;
			index++;
		}
	}
	

	std::cout << std::endl;
#if !DISABLE_OTHER
	{
		Timer timer("list-insert2");
		for (int i = 0; i < elementCount / 2; i++)
		{
			//list.emplace_front(0.25f * (float)i);
			list.emplace_back((float)i);
		}
	}

	{
		Timer timer("vector-insert2");
		for (int i = 0; i < elementCount / 2; i++)
		{
			//vector.emplace(vector.begin(), 0.12f * (float)i);
			vector.emplace_back((float)i);
		}
	}
#endif

	{
		Timer timer("unrolledLinkedList-insert2");
		for (int i = 0; i < elementCount / 2; i++)
		{
			unrolledList.Insert(Player((float)i));
		}
	}
	

	std::cout << std::endl;

	#if !DISABLE_OTHER
	{
		Timer timer("list-iterate");
		for (Player& player : list)
		{
			listSum += player.health;
		}
	}

	{
		Timer timer("vector-iterate");
		for (Player& player : vector)
		{
			vectorSum += player.health;
		}
	}
	#endif

	{
		Timer timer("unrolledLinkedList-iterate");
		for (Player& player : unrolledList)
		{
			unrolledListSum += player.health;
		}
	}

	{
		Timer timer("staticUnrolledLinkedList-iterate");
		for (Player& player : staticUnrolledList)
		{
			staticUnrolledListSum += player.health;
		}
	}

	std::cout << std::endl;

	std::cout << "list result: " << listSum << std::endl;
	std::cout << "vector result: " << vectorSum << std::endl;
	std::cout << "unrolled list result: " << unrolledListSum << std::endl;
	std::cout << "static unrolled list result: " << staticUnrolledListSum << std::endl;

	// testing node deletetion if empty in the dynamic unrolled linked list
	int count = 0;
	for (auto it = unrolledList.begin(); it != unrolledList.end() && count < NodeCapacity;)
	{
		it = unrolledList.Remove(it);
		count++;
	}

	return;

}


int main()
{
	Test();

	std::cin.get();
}