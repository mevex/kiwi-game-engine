#pragma once

/*
NOTE: I realize that implementing a dynamic array from scratch
and trying to be smart about it is probably a very bad idea
in practical terms.
However, I’m convinced it will be a valuable learning experience
both for understanding the language better and from
an architectural perspective—even if it means rewriting it
many many times.
*/

#include "defines.h"
#include "core/kiwi_mem.h"
#include "core/logger.h"

#define KARRAY_RESIZE_FACTOR 2
#define KARRAY_DEFAULT_INIT_CAPACITY 4

// TODO: Do we need shrinking?
template <typename T>
class KIWI_API KArray
{
public:
	// NOTE: I want to manually controll the creation destruction of the elements
	void Create(u64 InitialCapacity = KARRAY_DEFAULT_INIT_CAPACITY, u64 InitialLength = 0)
	{
		if (Capacity > 0)
		{
			KDebugBreak();
			LogWarning("Attempting to create the KArray twice!");
		}

		Length = InitialLength;
		Capacity = InitialCapacity;
		Stride = sizeof(T);
		Elements = (T *)MemSystem::Allocate(Capacity * Stride, MemTag_KArray);
	}

	void Destroy()
	{
		MemSystem::Free(Elements, Capacity * Stride, MemTag_KArray);
		Length = 0;
		Capacity = 0;
		Stride = 0;
		Elements = nullptr;
	}

	void Resize(u64 NewSize)
	{
		if (NewSize <= Length)
		{
			KDebugBreak();
			LogWarning("Trying to resize the array to a smaller (or equal) value than the actual length.");
		}

		void *NewBlock = MemSystem::Allocate(NewSize * Stride, MemTag_KArray);
		MemSystem::Copy(NewBlock, Elements, Length * Stride);
		MemSystem::Free(Elements, Capacity * Stride, MemTag_KArray);

		Capacity = NewSize;
		Elements = (T *)NewBlock;
	}

	void Clear()
	{
		Length = 0;
	}

	void Push(T Element)
	{
		if (Capacity == 0)
		{
			Create();
		}

		if (Length == Capacity)
		{
			Resize(Capacity * KARRAY_RESIZE_FACTOR);
		}

		Elements[Length] = Element;
		++Length;
	}

	T *Pop()
	{
		if (Length <= 0)
		{
			LogWarning("Trying to pop from an empty array");
			return nullptr
		}

		T *Element = Elements + (Length - 1);
		--Length;

		return Element;
	}

	void InsertAt(T Element, u64 Index)
	{
		if (Capacity == 0)
		{
			Create();
		}

		if (Index < 0)
		{
			LogWarning("Negative index are not supported by KArray. InsertAt aborted");
			return;
		}
		if (Index > Length)
		{
			KDebugBreak();
			LogWarning("Index out of bounds in Array::InsertAt. Max idx: %d. Input idx: %d.",
					   Length - 1, Index);
			return;
		}

		if (Length == Capacity)
		{
			if (Capacity > 0)
				Resize(Capacity * KARRAY_RESIZE_FACTOR);
			else
				Create();
		}

		MemSystem::Copy(Elements + Index + 1, Elements + Index, (Length - Index) * Stride);
		Elements[Index] = Element;
		++Length;
	}

	T *RemoveAt(u64 Index)
	{
		if (Index < 0)
		{
			LogWarning("Negative index are not supported by KArray. RemoveAt aborted");
			return nullptr;
		}
		if (Index >= Length)
		{
			LogWarning("Index out of bounds in Array::RemoveAt. Max idx: %d. Input idx: %d.",
					   Length - 1, Index);
			return nullptr;
		}

		T *Element = Elements + Index;
		MemSystem::Copy(Elements + Index, Elements + Index + 1, (Length - Index - 1) * Stride);
		--Length;

		return Element;
	}

	T *Get(u64 Index)
	{
		if (Index < 0 || Index >= Length)
		{
			LogError("Index out of bounds in KArray [] operator");
			return nullptr;
		}

		return Elements + Index;
	}

	// NOTE: I want these values to be public fo easy access.
	// Anyone who messes with them better be really sure about
	// what they're doing.
	u64 Length = 0;
	u64 Capacity = 0;
	u32 Stride = 0;
	T *Elements = nullptr;

	// Operators overload
public:
	T &operator[](u64 Index)
	{
		if (Index < 0 || Index >= Length)
		{
			LogError("Index out of bounds in KArray [] operator");
			KDebugBreak();
		}

		return Elements[Index];
	}
};