#include <cstdint>
#include <vector>
#include <memory>
#include <cmath>

#include <misc/intVector.hpp>

#define TERRAIN_CHUNKSIZE 16
#define MAX_LOD 6



namespace fluorite 
{   
    class TerrainMap
    {
        public:
            /**
             * Subchunks for LoD
             */
            class SubChunk {
                
                public:
                    IntVector pos;
                    int size;
                    int lod;
                    bool inUse;
                    SubChunk(IntVector _pos, int _lod, int _size) : pos(_pos), lod(_lod), size(_size) {}

                    bool operator==(const SubChunk& other) const
                    {
                        return pos == other.pos && lod == other.lod;
                    }

                    void setInUse(bool _inUse = true) {
                        inUse = _inUse;
                    }

                    bool isInUse() {
                        return inUse;
                    }



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

                


            };


        private:
            int datachunkSize = 256;
            int minChunkSize = 16;
            std::vector<Chunk> chunks;
            std::vector<SubChunk> subChunks;

          
        public:
        
            class GraphicalSubchunk {
                std::vector<GraphicalSubchunk> subchunks;

            public:
            IntVector pos;
            int size;
            int lod;

                GraphicalSubchunk(IntVector _pos, int _lod, int _size) : pos(_pos), lod(_lod), size(_size) {}

                IntVector center() {
                    return pos.add(size/2);
                }


                /**
                 * Returns all terminal chunks. Meaning chunks that have no subchunks. The smallest ones
                 */
                std::vector<GraphicalSubchunk*> getTerminalChunks() {
                    std::vector<GraphicalSubchunk*> result;
                    if(subchunks.size() == 0) {
                        result.emplace_back(this);
                    } else {
                        for(auto& subchunk : subchunks) {
                            auto subchunkTerminals = subchunk.getTerminalChunks();
                            result.insert(result.begin(), subchunkTerminals.begin(), subchunkTerminals.end()); 
                        }
                    }
                    return result;
                }



                /**
                 * Checks if a chunk is too close to a viewpoint. If it is so, a chunk will be subdivided into 8 subchunks
                 * They too will be checked and potentially subdivided
                 */
                void subdivideIfNeeded(IntVector viewPoint) {

                    //16 is minimum chunk size;
                    if(size <= 16) {
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
                                    auto& subchunk = subchunks.emplace_back(GraphicalSubchunk(pos.add(IntVector(x,y,z).mul(subChunkSize)), lod+1, subChunkSize));
                                    subchunk.subdivideIfNeeded(viewPoint);
                                }
                            }
                        }
                    }

                }


            };
            
            std::vector<GraphicalSubchunk> graphicsChunks;

            std::vector<GraphicalSubchunk*> getTerminalChunks() {
                std::vector<GraphicalSubchunk*> result;
                for(auto& subchunk : graphicsChunks) {
                    auto subchunkTerminals = subchunk.getTerminalChunks();
                    result.insert(result.begin(), subchunkTerminals.begin(), subchunkTerminals.end()); 
                }
                return result;
            }

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

                std::vector<Chunk> chunksOfViewpoint;
                std::vector<SubChunk> subchunksOfViewpoint;

                for(int x = mapBlockMin.x; x <= mapBlockMax.x; x += datachunkSize) {
                    for(int y = mapBlockMin.y; y <= mapBlockMax.y; y += datachunkSize) {
                        for(int z = mapBlockMin.z; z <= mapBlockMax.z; z += datachunkSize) {
                            auto& chunk = chunksOfViewpoint.emplace_back(Chunk({x,y,z}, datachunkSize));
                            auto subchunks = chunk.generateSubchunks(pos, minChunkSize);
                            subchunksOfViewpoint.insert(subchunksOfViewpoint.begin(), subchunks.begin(), subchunks.end()); 

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




            void loadForAPoint(IntVector pos, int radius) {
                /**
                 * Chunks sizes are powers of two
                 * Chunk's coordinates are it's minimal cords
                 * Chunks are alligned to coordinates (Meaning p = size*a where a - integer) 
                 * First let's find biggest chunks that would fit our FOV completely
                 */

                auto mapBlockMin = pos.sub(1).sub(radius).smoothdiv(datachunkSize).mul(datachunkSize);
                auto mapBlockMax = pos.add(1).add(radius).smoothdiv(datachunkSize).mul(datachunkSize);

                for(int x = mapBlockMin.x; x <= mapBlockMax.x; x += datachunkSize) {
                    for(int y = mapBlockMin.y; y <= mapBlockMax.y; y += datachunkSize) {
                        for(int z = mapBlockMin.z; z <= mapBlockMax.z; z += datachunkSize) {
                            auto& graphicChunk = graphicsChunks.emplace_back(GraphicalSubchunk({x,y,z}, 1, datachunkSize));
                            graphicChunk.subdivideIfNeeded(pos);

                        }
                    }
                }


            }

    };
    
    

}