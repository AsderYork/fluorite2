#include <cstdint>
#include <vector>
#include <memory>
#include <cmath>

#include <misc/intVector.hpp>


namespace fluorite 
{   
    class TerrainMap
    {
        public:
            class Chunk;

            class SubChunkData {};
            
            /**
             * Subchunks for LoD
             */
            class SubChunk {

                private:
                    friend class Chunk;

                    IntVector center() {
                        return pos.add(size/2);
                    }
    
                    /**
                     * Checks if a chunk is too close to a viewpoint. If it is so, a chunk will be subdivided into 8 subchunks
                     * They too will be checked and potentially subdivided
                     * This method will return only chunks that where not subdivided further, itself excluded
                     */
                    std::vector<SubChunk> generateTerminalChunks(IntVector viewPoint, int chunkMinimumSize = 16) {
        
                        std::vector<SubChunk> result;
                        if(size <= chunkMinimumSize) {
                            return result;
                        }
        
        
                        auto chunkCenter = center();
                        auto len = chunkCenter.sub(viewPoint).length();
        
                        /**
                         * Let's assume that minimal chunk is 16x16x16. And on every level there must be about 2 chunks
                         * Then 16*2 for the first level, 32*2 for the second and size*2 for every next one
                         * luckly we do keep size with us
                         */
        
                        if(len < size*2) {
                            int subChunkSize = size/2;
                            for(int x = 0; x < 2; x++) {
                                for(int y = 0; y < 2; y++) {
                                    for(int z = 0; z < 2; z++) {
                                        auto subchunk = SubChunk(pos.add(IntVector(x,y,z).mul(subChunkSize)), lod+1, subChunkSize);
                                        auto subChunkTerminals = subchunk.generateTerminalChunks(viewPoint, chunkMinimumSize);
                                        if(subChunkTerminals.size() > 0){
                                            result.insert(result.begin(), subChunkTerminals.begin(), subChunkTerminals.end()); 
                                        } else {
                                            result.push_back(subchunk);
                                        }
                                    }
                                }
                            }
                        }
                        return result;
        
                    }
        
                    void appendToGenerateSubchunks(std::vector<SubChunk> &subchunks, IntVector viewPoint, int chunkMinimumSize = 16) {
    
                        if(size <= chunkMinimumSize) {
                            return;
                        }
        
        
                        auto chunkCenter = center();
                        auto len = chunkCenter.sub(viewPoint).length();
        
                        /**
                         * Let's assume that minimal chunk is 16x16x16. And on every level there must be about 2 chunks
                         * Then 16*2 for the first level, 32*2 for the second and size*2 for every next one
                         * luckly we do keep size with us
                         */
        
                        if(len < size*2) {
                            int subChunkSize = size/2;
                            for(int x = 0; x < 2; x++) {
                                for(int y = 0; y < 2; y++) {
                                    for(int z = 0; z < 2; z++) {
                                        auto subchunk = SubChunk(pos.add(IntVector(x,y,z).mul(subChunkSize)), lod+1, subChunkSize);
                                        int lastSubchunksSize = subchunks.size();
                                        subchunk.appendToGenerateSubchunks(subchunks, viewPoint, chunkMinimumSize);
                                        if(subchunks.size() == lastSubchunksSize) {
                                            subchunks.push_back(subchunk);
                                        }
                                    }
                                }
                            }
                        }
                    }
        

                
                public:
                    IntVector pos;
                    int size;
                    int lod;
                    bool inUse;
                    bool isInitialized = false;
                    std::vector<std::shared_ptr<SubChunkData>> subchunkData;


                    SubChunk(IntVector _pos, int _lod, int _size) : pos(_pos), lod(_lod), size(_size) {}

                    bool operator==(const SubChunk& other) const
                    {
                        return pos == other.pos && lod == other.lod;
                    }

                    void setInUse(bool _inUse = true) {
                        inUse = _inUse;
                    }

                    bool isInUse() const {
                        return inUse;
                    }

                    void setInInitialized(bool initialized = true) {
                        isInitialized = initialized;
                    }
                  
                    

            };


            /**
             * Main Chunk structure. Holds data, controls it's usage and manages subchunks
             */
            class Chunk {

                private:
                    IntVector pos;
                    int size;

                    int inUse = true;

                    //Chunk doesn't actually store it's subchunks. Their lifetime is controlled in the Map.
                    //Chunk is responsible only for creating them and determining if they are visible or not
                    std::vector<std::shared_ptr<SubChunk>> terminalSubchunks;

                public:

                    Chunk(IntVector _pos, int _size) : pos(_pos), size(_size) {}

                    bool operator==(const Chunk& other) const
                    {
                        return pos == other.pos;
                    }

                    void setInUse(bool _inUse = true) {
                        inUse = _inUse;
                    }

                    bool isInUse() {
                        return inUse;
                    }

                    /**
                     * Generates an array of subchunks, that are supposed to exist for a viewpoint
                     */
                    std::vector<SubChunk> generateSubchunks(IntVector viewpoint, int chunkMinimumSize = 16) {
                        auto mainSubchunk = SubChunk(pos, 1, size);
                        std::vector<SubChunk> result = mainSubchunk.generateTerminalChunks(viewpoint, chunkMinimumSize);
                        if(result.size() == 0) {
                            result.push_back(mainSubchunk);
                        }
                        return result;

                    }

                    void appendToGenerateSubchunks(std::vector<SubChunk> &subchunks, IntVector viewpoint, int chunkMinimumSize = 16) {
                        int initialSize = subchunks.size();
                        auto mainSubchunk = SubChunk(pos, 1, size);
                        mainSubchunk.appendToGenerateSubchunks(subchunks, viewpoint, chunkMinimumSize);
                        if(subchunks.size() == initialSize) {
                            subchunks.push_back(mainSubchunk);
                        }

                    }

                


            };


        private:
            int datachunkSize;
            int minChunkSize;
            std::vector<Chunk> chunks;
            std::vector<SubChunk> subChunks;

          
        public:

            TerrainMap(int _datachunkSize = 256, int _minChunkSize = 16) : datachunkSize(_datachunkSize), minChunkSize(_minChunkSize)
            {}
        
            void resetInUseFlags() {
                for(auto& chunk : chunks) {
                    chunk.setInUse(false);
                }

                for(auto& subchunk : subChunks) {
                    subchunk.setInUse(false);
                }

            }

            void createChunksForAViewpoint(IntVector pos, int radius) {
                auto mapBlockMin = pos.sub(1).sub(radius).smoothdiv(datachunkSize).mul(datachunkSize);
                auto mapBlockMax = pos.add(1).add(radius).smoothdiv(datachunkSize).mul(datachunkSize);

                int approxSize = (radius/minChunkSize);
                approxSize = approxSize*approxSize;

                std::vector<Chunk> chunksOfViewpoint;
                std::vector<SubChunk> subchunksOfViewpoint;
                subchunksOfViewpoint.reserve(approxSize);

                for(int x = mapBlockMin.x; x <= mapBlockMax.x; x += datachunkSize) {
                    for(int y = mapBlockMin.y; y <= mapBlockMax.y; y += datachunkSize) {
                        for(int z = mapBlockMin.z; z <= mapBlockMax.z; z += datachunkSize) {
                            auto& chunk = chunksOfViewpoint.emplace_back(Chunk({x,y,z}, datachunkSize));
                            chunk.appendToGenerateSubchunks(subchunksOfViewpoint, pos, minChunkSize);

                        }
                    }
                }

                std::vector<Chunk> chunksToAdd;
                for(auto& chunk : chunksOfViewpoint) {
                    auto fundChunkIter = std::find(chunks.begin(), chunks.end(), chunk);
                    if(fundChunkIter != chunks.end()) {
                        fundChunkIter->setInUse(true);
                    } else {
                        chunksToAdd.push_back(chunk);
                    }
                }
                chunks.insert(chunks.begin(), chunksToAdd.begin(), chunksToAdd.end()); 

                std::vector<SubChunk> subChunksToAdd;
                for(auto& subChunk : subchunksOfViewpoint) {
                    auto fundSubChunkIter = std::find(subChunks.begin(), subChunks.end(), subChunk);
                    if(fundSubChunkIter != subChunks.end()) {
                        fundSubChunkIter->setInUse(true);
                    } else {
                        subChunksToAdd.push_back(subChunk);
                    }
                }
                subChunks.insert(subChunks.begin(), subChunksToAdd.begin(), subChunksToAdd.end());        

            }

            void clearUnusedChunks() {
                std::erase_if(subChunks, [](SubChunk x) { return !x.isInUse(); } );
                std::erase_if(chunks, [](Chunk x) { return !x.isInUse(); } );
            }

            void initializeSubchunks(std::function<void(SubChunk*)> initializator) {
                for(auto& subchunk : subChunks) {
                    if(!subchunk.isInitialized) {
                        initializator(&subchunk);
                        subchunk.setInInitialized();
                    }
                }
            }

    };
    
    

}