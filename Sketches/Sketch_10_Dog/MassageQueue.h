/**
 * @file MassageQueue.h
 * @author suhayl@freenove (support@freenove.com)
 * @brief Custom message queue.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */
// MassageQueue.h

#ifndef _MASSAGEQUEUE_h
#define _MASSAGEQUEUE_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// constexpr auto queue_size = 3;

//The head points to the first position in the queue, which is the position to be dequeued.
//The tail points to the last position +1 in the queue, here is a null value, the position to enqueue.
//When the queue is empty, head=tail.
//When the queue is full, head = tail + 1, leaving an empty position.
//The maximum storage number of the queue is SIZE-1, otherwise when empty and full, the head is equal to the tail.

template <typename T>
class DataQueue
{

private:
	// String values[queue_size];
	uint16_t queue_size;
	T *values;
	uint16_t head;
	uint16_t tail;
	T maxVal;
	T minVal;
	bool enableMaxMin;

	void calculateMaxMin();

public:
	// MassageQueue();
	DataQueue(int queueSize = 3, bool _enableMaxMin = false);
	~DataQueue();
	bool enter(T val);
	//Force to enqueue. When the queue is full, the dequeue will be executed first, and then normally enqueue.
	bool enterForced(T val);
	T out();
	bool isEmpty();
	bool isFull();
	uint16_t length();
	T getValue(uint16_t n);
	T getMax();
	T getMin();
	void setEnableMaxMin(bool _enableMaxMin = false);
};

template <typename T>
inline void DataQueue<T>::calculateMaxMin()
{
	if (enableMaxMin)
	{
		T val;
		maxVal = T('\0');
		minVal = T(0xFFFFFFFF);
		for (int i = 0; i < length(); i++)
		{
			val = values[(head + i) % queue_size];
			maxVal = val > maxVal ? val : maxVal;
			minVal = val < minVal ? val : minVal;
		}
	}
}

template <typename T>
DataQueue<T>::DataQueue(int queueSize, bool _enableMaxMin)
{
	enableMaxMin = _enableMaxMin;
	queue_size = queueSize;
	values = new T[queue_size];
	head = 0;
	tail = 0;
}
template <typename T>
inline DataQueue<T>::~DataQueue()
{
	delete values;
}
template <typename T>
bool DataQueue<T>::enter(T val)
{
	if (!isFull())
	{
		values[tail] = val;
		tail = (tail + 1) % queue_size;
		calculateMaxMin();
		return true;
	}
	return false;
}

template <typename T>
bool DataQueue<T>::enterForced(T val)
{
	if (!enter(val))
	{
		out();
		return enter(val);
	}
	else
	{
		return true;
	}
}

template <typename T>
inline T DataQueue<T>::out()
{
	T res = T(0);
	if (!isEmpty())
	{
		res = values[head];
		values[head] = T('\0');
		head = (head + 1) % queue_size;
		calculateMaxMin();
	}
	return res;
}

template <typename T>
bool DataQueue<T>::isEmpty()
{
	if (head == tail)
	{
		// Serial.println("MassageQueue is empty!");
		return true;
	}
	else
	{
		return false;
	}
}

template <typename T>
bool DataQueue<T>::isFull()
{
	if ((tail + 1) % queue_size == head)
	{
		// Serial.println("MassageQueue is full, can not enter more msg!");
		return true;
	}
	else
	{
		return false;
	}
}

template <typename T>
uint16_t DataQueue<T>::length()
{
	int len = tail - head;
	return len >= 0 ? len : len + queue_size;
}

template <typename T>
inline T DataQueue<T>::getValue(uint16_t n)
{
	if (n < length())
	{
		return values[(head + n) % queue_size];
	}
	else
	{
		return T('\0');
	}
}

template <typename T>
inline T DataQueue<T>::getMax()
{
	return maxVal;
}

template <typename T>
inline T DataQueue<T>::getMin()
{
	return minVal;
}

template <typename T>
inline void DataQueue<T>::setEnableMaxMin(bool _enableMaxMin)
{
	enableMaxMin = _enableMaxMin;
}

#endif
