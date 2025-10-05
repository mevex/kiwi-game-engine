#pragma once

#include "defines.h"
#include "core/kiwi_mem.h"
#include "core/logger.h"

template <typename T>
class KIWI_API KLinkedList
{
public:
	struct Node
	{
		T Element;
		struct Node *Next;
	};

	void Create(u8 Tag)
	{
		Arena = MemSystem::GetArena(Tag);
	}

	void Destroy()
	{
		Arena->Pop(sizeof(Node) * (NodeCount + FreeNodeCount));

		Arena = nullptr;
		FreeNode = nullptr;
		FirstNode = nullptr;
		NodeCount = 0;
	}

	void Clear()
	{
		Node *LastFreeNode = FreeNode;

		while (LastFreeNode && LastFreeNode->Next)
		{
			LastFreeNode = LastFreeNode->Next;
		}

		if (LastFreeNode)
		{
			LastFreeNode->Next = FirstNode;
		}
		else
		{
			FreeNode = FirstNode;
		}

		FirstNode = nullptr;
		FreeNodeCount += NodeCount;
		NodeCount = 0;
	}

	T *Add(T Element)
	{
		AssertMsg(Arena, "Linked List not initialized");

		Node *NewNode = nullptr;

		// Get memory for the new element either from the free list
		// or by pushing on the arena
		if (FreeNode)
		{
			NewNode = FreeNode;
			FreeNode = NewNode->Next;
			--FreeNodeCount;
		}
		else
		{
			NewNode = (Node *)Arena->Push(sizeof(Node));
		}

		NewNode->Element = Element;
		NewNode->Next = FirstNode;
		FirstNode = NewNode;

		++NodeCount;
		return &NewNode->Element;
	}

	void Remove(T *Element)
	{
		Node *ElementToRemove = FirstNode;
		Node *PreviousNode = nullptr;
		while (ElementToRemove != (Node *)Element && ElementToRemove)
		{
			PreviousNode = ElementToRemove;
			ElementToRemove = ElementToRemove->Next;
		}

		// Element not found or Element = nullptr
		if (!ElementToRemove)
		{
			return;
		}

		// NOTE: if PreviousNode is null it means that the element to be removed
		// is the first node;
		if (PreviousNode)
		{
			PreviousNode->Next = ElementToRemove->Next;
		}
		else
		{
			FirstNode = ElementToRemove->Next;
		}

		ElementToRemove->Next = FreeNode;
		FreeNode = ElementToRemove;
		++FreeNodeCount;
		--NodeCount;
	}

	MemArena *Arena = nullptr;

	Node *FirstNode = nullptr;
	Node *FreeNode = nullptr;

	u32 NodeCount = 0;
	u32 FreeNodeCount = 0;
};