/*
  ==============================================================================

    LockFreeQueue.h
    Created: 12 Jun 2020 10:39:06pm
    Author:  Joe

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

//==============================================================================

class LockFreeQueue
{

public:

    ScopedPointer<AbstractFifo> lockFreeFifo;
    Array<float> data;
    int lastReadPos = 0;

    LockFreeQueue()
    {
        lockFreeFifo = new AbstractFifo(512);

        //clear
        data.ensureStorageAllocated(512);
        FloatVectorOperations::clear(data.getRawDataPointer(), 512);

        while (data.size() < 512)
        {
            data.add(0.f);
        }
    }

    void setTotalSize(int newSize)
    {
        lockFreeFifo->setTotalSize(newSize);

        // clear
        data.ensureStorageAllocated(newSize);
        FloatVectorOperations::clear(data.getRawDataPointer(), newSize);

        while (data.size() < newSize)
        {
            data.add(0.f);
        }
    }

    void writeTo(const float* writeData, int numToWrite)
    {
        int start1, start2, blockSize1, blockSize2;

        lockFreeFifo->prepareToWrite(numToWrite, start1, blockSize1, start2, blockSize2);

        if (blockSize1 > 0) FloatVectorOperations::copy(data.getRawDataPointer() + start1, writeData, blockSize1);
        if (blockSize2 > 0) FloatVectorOperations::copy(data.getRawDataPointer() + start2, writeData + blockSize1, blockSize2);

        // MOVE the FIFO write head...
        lockFreeFifo->finishedWrite(numToWrite);

    }

    void readFrom(float* readData, int numToRead)
    {
        int start1, blockSize1, start2, blockSize2;
        lockFreeFifo->prepareToRead(numToRead, start1, blockSize1, start2, blockSize2);

        if (blockSize1 > 0)
        {
            FloatVectorOperations::copy(readData, data.getRawDataPointer() + start1, blockSize1);
            lastReadPos = start1 + blockSize1;
        }

        if (blockSize2 > 0)
        {
            FloatVectorOperations::copy(readData + blockSize1, data.getRawDataPointer() + start2, blockSize2);
            lastReadPos = start2 + blockSize2;
        }

        lockFreeFifo->finishedRead(blockSize1 + blockSize2);
    }

    void readMostRecent(float* readData, int numToRead)
    {

    }

    int writeToArray(Array<float>* dest, int destPos)
    {
        // Append any new data to a circular array

        // DRAIN the excess
        while (getNumReady() > dest->size())
        {
            lockFreeFifo->finishedRead(getNumReady() - dest->size());
        }

        // READ latest data from the LFQ
        const int numToAppend = getNumReady();

        // ADD the tail (one buffer's worth) to the output
        if (destPos + numToAppend < dest->size())
        {
            readFrom(&dest->getRawDataPointer()[destPos], numToAppend);
        }
        else
        {
            int toTheEnd = dest->size() - destPos;
            readFrom(&dest->getRawDataPointer()[destPos], toTheEnd);
            readFrom(&dest->getRawDataPointer()[0], numToAppend - toTheEnd);
        }

        return numToAppend;
    }

    int getNumReady()
    {
        return lockFreeFifo->getNumReady();
    }

};
