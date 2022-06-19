#pragma once
#include <array>
#include <unordered_set>
#include "Bitset.h"

// https://en.wikipedia.org/wiki/Unrolled_linked_list
// an unsorted unrolled linked list
// a more dynamic version that allows removing elements and being as memory efficient as possible
// this version also keeps all elements at the same place in memory
template<typename T, int NodeMaxElementCount>
class UnrolledLinkedList
{
	struct Node
	{
		std::array<T, NodeMaxElementCount> Elements;
		Bitset<NodeMaxElementCount> ElementsValidity;
		Node* Next = nullptr;
		Node* Previous = nullptr;
	};

	class Iterator
	{
		friend class UnrolledLinkedList;
	public:
		Iterator(Node* node, int index)
			: m_Node(node), m_Index(index)
		{
			while (m_Node != nullptr && !m_Node->ElementsValidity.Test(m_Index))
				Next();
		}

		T& operator*() const
		{
			return m_Node->Elements[m_Index];
		}

		bool operator==(const Iterator& other) const
		{
			return m_Node == other.m_Node && m_Index == other.m_Index;
		}

		bool operator!=(const Iterator& other) const
		{
			return m_Node != other.m_Node || m_Index != other.m_Index;
		}

		Iterator& operator++()
		{
			Increment();
			return *this;
		}

		Iterator operator++(int)
		{
			Iterator iterator = *this;
			Increment();
			return iterator;
		}
	private:
		void Increment()
		{
			Next();
			while (m_Node != nullptr && !m_Node->ElementsValidity.Test(m_Index))
				Next();
		}

		void Next()
		{
			if (m_Index >= NodeMaxElementCount - 1)
			{
				// moving to the next node
				m_Node = m_Node->Next;
				// reseting the index
				m_Index = 0;
			}
			else
			{
				m_Index++;
			}
		}
	private:
		Node* m_Node;
		int m_Index;
	};

public:
	UnrolledLinkedList()
		: m_First(new Node()), m_Last(m_First)
	{}

	~UnrolledLinkedList()
	{
		Node* currentNode = m_First;
		while (currentNode->Next)
		{
			Node* temp = currentNode->Next;
			delete currentNode;
			currentNode = temp;
		}
		delete currentNode;
	}

	void Insert(const T& element)
	{
		if (m_IncompleteNodes.empty())
		{
			if (IsNodeFull(m_Last))
				InsertNode();
			int index = m_Last->ElementsValidity.FindFirstZeroIndex();
			m_Last->Elements[index] = element;
			m_Last->ElementsValidity.Set(index, true);
		}
		else
		{
			auto first = m_IncompleteNodes.begin();
			Node* firstNode = *first;
			int index = firstNode->ElementsValidity.FindFirstZeroIndex();
			firstNode->Elements[index] = element;
			firstNode->ElementsValidity.Set(index, true);
			if (IsNodeFull(firstNode))
			{
				m_IncompleteNodes.erase(first);
			}
		}
	}

	Iterator Remove(Iterator iterator)
	{
		Node* node = iterator.m_Node;
		node->ElementsValidity.Set(iterator.m_Index, false);
		#ifdef DEBUG
		memset(&(node->Elements[iterator.m_Index]), 0, sizeof(T));
		#endif
		// if all of the elements are deleted then the node can be deleted as well
		if (IsNodeEmpty(node))
		{
			// updating the iterator for the next node at index 0
			iterator.m_Node = node->Next;
			iterator.m_Index = 0;
			// if there is more than 1 node we can delete a node
			// there should be always at least 1 node
			if (m_First != m_Last)
			{
				// dealing with different node locations is different so each function deals with a different location
				if(node == m_First)
					DeleteFirstNode();
				else if(node == m_Last)
					DeleteLastNode();
				else
					DeleteNode(node);
			}
		}
		else
		{
			// incrementing the iterator for the next index
			iterator++;
			// adding the node to the incomplete node list if it doesnt exist yet
			if (!m_IncompleteNodes.contains(node))
			{
				m_IncompleteNodes.insert(node);
			}
		}
		
		return iterator;
	}

	bool Empty() const
	{
		// checking if there is only 1 node
		// and checking if this node is empty
		return m_First == m_Last && IsNodeEmpty(m_First);
	}

	Iterator begin()
	{
		return Iterator(m_First, 0);
	}

	Iterator end()
	{
		return Iterator(nullptr, 0);
	}
private:
	bool IsNodeFull(Node* node) const
	{
		return node->ElementsValidity.All();
	}
	bool IsNodeEmpty(Node* node) const
	{
		return node->ElementsValidity.None();
	}
	void InsertNode()
	{
		Node* node = new Node();
		m_Last->Next = node;
		node->Previous = m_Last;
		m_Last = node;
	}
	void DeleteNode(Node* node)
	{
		node->Previous->Next = node->Next;
		node->Next->Previous = node->Previous;
		delete node;
	}

	void DeleteFirstNode()
	{
		m_First = m_First->Next;
		delete m_First->Previous;
		m_First->Previous = nullptr;
	}
	void DeleteLastNode()
	{
		m_Last = m_Last->Previous;
		delete m_Last->Next;
		m_Last->Next = nullptr;
	}

private:
	Node* m_First;
	Node* m_Last;
	std::unordered_set<Node*> m_IncompleteNodes;
};








// a less memory efficient unsorted unrolled linked list that performs faster
// intended to use when dont need a lot of element removals and when there is no need for the elements to stay in the same location
template<typename T, int NodeMaxElementCount>
class StaticUnrolledLinkedList
{
	struct Node
	{
		std::array<T, NodeMaxElementCount> Elements;
		int ElementCount = 0;
		Node* Next = nullptr;
		Node* Previous = nullptr;
	};

	class Iterator
	{
		friend class StaticUnrolledLinkedList;
	public:
		Iterator(Node* node, int index)
			: m_Node(node), m_Index(index)
		{}

		T& operator*() const
		{
			return m_Node->Elements[m_Index];
		}

		bool operator==(const Iterator& other) const
		{
			return m_Node == other.m_Node && m_Index == other.m_Index;
		}

		bool operator!=(const Iterator& other) const
		{
			return m_Node != other.m_Node || m_Index != other.m_Index;
		}

		Iterator& operator++()
		{
			Increment();
			return *this;
		}

		Iterator operator++(int)
		{
			Iterator iterator = *this;
			Increment();
			return iterator;
		}
	private:
		void Increment()
		{
			if (m_Index >= m_Node->ElementCount - 1)
			{
				m_Node = m_Node->Next;
				m_Index = 0;
			}
			else
			{
				m_Index++;
			}
		}
	private:
		Node* m_Node;
		int m_Index;
	};

public:
	StaticUnrolledLinkedList()
		: m_First(new Node()), m_Last(m_First)
	{}

	~StaticUnrolledLinkedList()
	{
		Node* currentNode = m_First;
		while (currentNode->Next)
		{

			Node* temp = currentNode->Next;
			delete currentNode;
			currentNode = temp;
		}
		delete currentNode;
	}

	void Insert(const T& element)
	{
		if (IsFull(m_Last))
			InsertNode();
		m_Last->Elements[m_Last->ElementCount] = element;
		m_Last->ElementCount++;
	}

	Iterator Remove(const Iterator& iterator)
	{
		Node* node = iterator.m_Node;
		Iterator resultIterator = iterator;
		// moving the last element of the node to the deleted element
		node->Elements[iterator.m_Index] = node->Elements[--node->ElementCount];
		if (node->ElementCount == 0)
		{
			resultIterator.m_Node = node->Next;
			resultIterator.m_Index = 0;
			if (!(node == m_First && node == m_Last))
			{
				if (node == m_First)
					DeleteFirstNode();
				else if (node == m_Last)
					DeleteLastNode();
				else
					DeleteNode(node);
			}
		}

		return resultIterator;
	}

	Iterator begin()
	{
		return Iterator(m_First, 0);
	}

	Iterator end()
	{
		return Iterator(nullptr, 0);
	}
private:
	bool IsFull(Node* node)
	{
		return node->ElementCount == NodeMaxElementCount;
	}
	void InsertNode()
	{
		Node* node = new Node();
		m_Last->Next = node;
		node->Previous = m_Last;
		m_Last = node;
	}
	void DeleteNode(Node* node)
	{
		node->Previous->Next = node->Next;
		node->Next->Previous = node->Previous;
		delete node;
	}

	void DeleteFirstNode()
	{
		m_First = m_First->Next;
		delete m_First->Previous;
		m_First->Previous = nullptr;
	}
	void DeleteLastNode()
	{
		m_Last = m_Last->Previous;
		delete m_Last->Next;
		m_Last->Next = nullptr;
	}
private:
	Node* m_First;
	Node* m_Last;
};