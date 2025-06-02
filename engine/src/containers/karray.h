#pragma once

#include "defines.h"
#include "core/kiwi_mem.h"
#include "core/logger.h"

#define KARRAY_RESIZE_FACTOR 2

// TODO: Do we need shrinking?
template <typename T>
class KIWI_API KArray
{
public:
	KArray() {}

	~KArray()
	{
		if (Elements)
			Destroy();
	}

	// NOTE: I want to be able to manually controll the creation
	// destruction of the Elements array if I want/need to
	void Create(u64 InitialCapacity = 1)
	{
		Length = 0;
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
		if (NewSize > Length)
		{
			void *NewBlock = MemSystem::Allocate(NewSize * Stride, MemTag_KArray);
			MemSystem::Copy(NewBlock, Elements, Length * Stride);
			MemSystem::Free(Elements, Capacity * Stride, MemTag_KArray);

			Capacity = NewSize;
			Elements = (T *)NewBlock;
		}
		else
		{
			KDebugBreak();
			LogError("Trying to resize the array to a smaller (or equal) value than the actual length.");
		}
	}

	void Clear()
	{
		Length = 0;
	}

	void Push(T Element)
	{
		if (!Elements)
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
		if (!Elements)
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
			LogWarning("Index out of bounds in Array::InsertAt. Max idx: %d. Input idx: %d.",
					   Length - 1, Index);
			return nullptr;
		}

		if (Length == Capacity)
		{
			Resize(Capacity * KARRAY_RESIZE_FACTOR);
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

	T *GetRawData()
	{
		return Elements;
	}

	// NOTE: I want these values to be public fo easy access.
	// Anyone who messes with them better be really sure about
	// what they're doing.
	u64 Length = 0;
	u64 Capacity = 0;

private:
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