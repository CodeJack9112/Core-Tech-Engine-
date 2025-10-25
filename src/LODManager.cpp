#include "LODManager.h"
int LODManager::Select(float distance) const { for(size_t i=0;i<entries.size();++i) if(distance < entries[i].distanceThreshold) return entries[i].modelId; return entries.empty()? -1 : entries.back().modelId; }
