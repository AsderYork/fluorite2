#include <cstdint>
#include <vector>
#include <memory>
#include <cmath>

#include <misc/intVector.hpp>

#define TERRAIN_CHUNKSIZE 16
#define MAX_LOD 6



namespace fluorite 
{
    struct TerrainNode {
        int8_t val;
    };

    struct TerrainChunk
    {
        //Coordinates are absolute. So they can only be multiples of TERRAIN_CHUNKSIZE. Minimal corner
        int x, y, z;
        TerrainNode nodes[TERRAIN_CHUNKSIZE][TERRAIN_CHUNKSIZE][TERRAIN_CHUNKSIZE];
    };

    class TerrainLODChunk
    {
    private:
        //Absolute coordinates of the minimal corner
        int x,y,z;
        int lodLevel;
        int chunkSize;
        std::vector<TerrainLODChunk> subChunks;

    public:

        bool isPointInChunk(int px, int py, int pz) {
            return (px >= x && px < x + chunkSize) &&
            (py >= y && py < y + chunkSize) &&
            (pz >= z && pz < z + chunkSize);
        }

        TerrainLODChunk(int _x, int _y, int _z, int _lodLevel) : x(_x), y(_y), z(_z), lodLevel(_lodLevel) {
            chunkSize = TERRAIN_CHUNKSIZE * (2 << (lodLevel - 1)) / 2;
        }        

    };


    

    class TerrainMap
    {
    private:
        int datachunkSize = 256;

        class TerrainMapChunk {
            IntVector pos;
            public:
                TerrainMapChunk(IntVector _pos) : pos(_pos) {}

        };

        
        //Store raw data. No info about LOD-levels
        std::vector<TerrainMapChunk> dataChunks;



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
                        dataChunks.emplace_back(TerrainMapChunk({x,y,z}));
                        auto& graphicChunk = graphicsChunks.emplace_back(GraphicalSubchunk({x,y,z}, 1, datachunkSize));
                        graphicChunk.subdivideIfNeeded(pos);

                    }
                }
             }


        }

    };
    
    


    

    



    class Terrain2
    {
    private:

    public:
        Terrain2(/* args */);
        ~Terrain2();
    };
    
    Terrain2::Terrain2(/* args */)
    {
    }
    
    Terrain2::~Terrain2()
    {
    }
    

}