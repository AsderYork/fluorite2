#include <cstdint>
#include <vector>
#include <memory>
#include <cmath>
#include <map>

#include <misc/intVector.hpp>


namespace fluorite 
{   
    class TerrainMap
    {
        public:
            class Chunk;

            class SubChunkData {};

            struct SubChunkAddress {

                public:
                    IntVector pos = IntVector(0);
                    int size = 0;

                    SubChunkAddress(IntVector _pos, int _size) : pos(_pos), size(_size) {}

                    auto operator<=>(const SubChunkAddress& other) const = default;

                    IntVector center() const {
                        return pos.add(size/2);
                    }

                    float distanceToPoint(const IntVector point) const {
                        return center().sub(point).length();
                    }

                    SubChunkAddress createdShiftedSubAddress(const IntVector shift, const int subchunkSize) const {
                        return SubChunkAddress(pos.add(shift.mul(subchunkSize)), subchunkSize);
                    }
                    

            };
            
            /**
             * Subchunks for LoD
             */
            class SubChunk {

                private:
                    friend class Chunk;
                
                public:
                    IntVector pos;
                    int size;
                    int lod;
                    bool inUse = true;
                    bool isInitialized = false;
                    std::vector<std::shared_ptr<SubChunkData>> subchunkData;

                    SubChunk(const SubChunkAddress addr) : pos(addr.pos), size(addr.size), lod(0) {} 


                    SubChunk(IntVector _pos, int _lod, int _size) : pos(_pos), lod(_lod), size(_size) {}

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

                


            };


        private:
            int datachunkSize;
            int minChunkSize;
            std::map<IntVector, Chunk> chunks;
            std::map<SubChunkAddress, SubChunk> subChunks;


          
        public:
        
            TerrainMap(int _datachunkSize = 256, int _minChunkSize = 16) : datachunkSize(_datachunkSize), minChunkSize(_minChunkSize)
            {}
        
            void resetInUseFlags() {
                for(auto& chunk : chunks) {
                    chunk.second.setInUse(false);
                }

                for(auto& subchunk : subChunks) {
                    subchunk.second.setInUse(false);
                }

            }

            
            void generateSubchunkAddressesInPlace(SubChunkAddress addr, IntVector viewpoint, int chunkMinimumSize = 16) {

                //If it's minimal subchunk, just add it
                if(addr.size <= chunkMinimumSize) {
                    auto newsubchunk = subChunks.find(addr);
                    if(newsubchunk == subChunks.end()) {
                        subChunks.emplace(std::make_pair(addr, SubChunk(addr)));
                    } else {
                        newsubchunk->second.setInUse();
                    }
                    return;
                }
                auto len = addr.distanceToPoint(viewpoint);
                    /**
                 * Let's assume that minimal chunk is 16x16x16. And on every level there must be about 2 chunks
                 * Then 16*2 for the first level, 32*2 for the second and size*2 for every next one
                 * luckly we do keep size with us
                 */

                    //If distance is too great there can be no subchunks. So will just add this one
                if(len >= addr.size*3) {
                    auto newsubchunk = subChunks.find(addr);
                    if(newsubchunk == subChunks.end()) {
                        subChunks.emplace(std::make_pair(addr, SubChunk(addr)));
                    } else {
                        newsubchunk->second.setInUse();
                    }

                } else {
                    int subChunkSize = addr.size/2;
                    for(int x = 0; x < 2; x++) {
                        for(int y = 0; y < 2; y++) {
                            for(int z = 0; z < 2; z++) {
                                auto subchunkAddr = addr.createdShiftedSubAddress(IntVector(x,y,z), subChunkSize);
                                generateSubchunkAddressesInPlace(subchunkAddr, viewpoint, chunkMinimumSize);
                            }
                        }
                    }
                } 
            }
            
            
            void createChunksForAViewpoint(IntVector pos, int radius) {
                auto mapBlockMin = pos.sub(1).sub(radius).smoothdiv(datachunkSize).mul(datachunkSize);
                auto mapBlockMax = pos.add(1).add(radius).smoothdiv(datachunkSize).mul(datachunkSize);

                std::vector<Chunk> chunksOfViewpoint;

                for(int x = mapBlockMin.x; x <= mapBlockMax.x; x += datachunkSize) {
                    for(int y = mapBlockMin.y; y <= mapBlockMax.y; y += datachunkSize) {
                        for(int z = mapBlockMin.z; z <= mapBlockMax.z; z += datachunkSize) { 
                            auto chunkPos = IntVector(x,y,z);

                            auto newchunk = chunks.find(chunkPos);
                            if(newchunk == chunks.end()) {
                                chunks.emplace(std::make_pair(chunkPos, Chunk(chunkPos, datachunkSize)));
                            } else {
                                newchunk->second.setInUse();
                            }

                            auto& chunk = chunksOfViewpoint.emplace_back(Chunk(chunkPos, datachunkSize));
                            generateSubchunkAddressesInPlace(SubChunkAddress(chunkPos, datachunkSize), pos, minChunkSize);
                        }
                    }
                }

            }

            void clearUnusedChunks() {
                std::erase_if(subChunks, [](auto x) { return !x.second.isInUse(); } );
                std::erase_if(chunks, [](auto x) { return !x.second.isInUse(); } );
            }

            void initializeSubchunks(std::function<void(SubChunk*)> initializator) {
                for(auto& subchunk : subChunks) {
                    if(!subchunk.second.isInitialized) {
                        initializator(&subchunk.second);
                        subchunk.second.setInInitialized();
                    }
                }
            }

            int mapsize() {
                return subChunks.size();
            }
            int datachunksSize() {
                return chunks.size();
            }

    };
    
    

}