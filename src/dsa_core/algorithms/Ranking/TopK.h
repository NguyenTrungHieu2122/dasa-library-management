#ifndef TOPK_H
#define TOPK_H

#include <vector>
#include "../../structures/MinHeap.h"

std::vector<HeapItem> getTopK(const MinHeap &heap, int k);

#endif