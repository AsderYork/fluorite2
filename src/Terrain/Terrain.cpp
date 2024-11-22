#include <Terrain/Terrain.hpp>

#include <iostream>

namespace fluorite
{

    int div_floor(int x, int y) {
        int q = x/y;
        int r = x%y;
        if ((r!=0) && ((r<0) != (y<0))) --q;
        return q;
    }
    
    std::shared_ptr<TerrainChunk> TerrainBlocksStorageDummy::requestBlock(Vec3Int pos, int level) {
        return std::make_shared<TerrainChunk>(pos, level);
    }
    void TerrainBlocksStorageDummy::clearUnusedBlocks() {}
    std::shared_ptr<TerrainChunk> TerrainBlocksStorageDummy::getBlockIfExists(Vec3Int currBlock, int currBlockLevel) {
        return nullptr;
    }



    TerrainChunkFinder::TerrainChunkFinder(TerrainBlocksStorageInterface* provider) : m_storage(provider) {}

    int TerrainChunk::chunkSize = 16;
    std::vector<Vec3Int> TerrainChunk::neghboursShifts = {{1,0,0}, {0,1,0}, {1,1,0}, {0,0,1}, {0,1,1}, {1,0,1}, {1,1,1} };
    int TerrainChunk::getLevelChunkSize(int level) {  return chunkSize * (1 << (level - 1)); }
    Vec3Int TerrainChunk::gridAlign(Vec3Int pos, int level) {  return pos.align(TerrainChunk::getLevelChunkSize(level)); }
    TerrainChunk::TerrainChunk(Vec3Int pos, int lod) : m_pos(pos), m_lod(lod) {}
    int TerrainChunk::getFramesSinceLastUse() {
        return m_framesSinceLastUse;
    }
    void TerrainChunk::resetFramesSinceLastUse() {
        m_framesSinceLastUse = 0;
    }
    void TerrainChunk::incrementFramesSiceLastUse() {
        m_framesSinceLastUse++;
    }
    void TerrainChunk::addRenderable(std::unique_ptr<TerrainChunkRenderableInterface> renderable){
        m_renderables.emplace_back(std::move(renderable));
    };
    void TerrainChunk::initRenderables(){
        std::for_each(m_renderables.begin(), m_renderables.end(), [this](auto& renderable){renderable->init(this);});
    };
    
    Vec3Int TerrainChunk::getPos() {
        return m_pos;
    }
    int TerrainChunk::getLoD() {
        return m_lod;
    }

    int TerrainChunkFinder::getLODSpacing(int level) {
        return 1;
    }
    std::vector<std::shared_ptr<TerrainChunk>> TerrainChunkFinder::findBlocks(Vec3Int center, int radius, int startLevel) {

        std::vector<std::shared_ptr<TerrainChunk>> result;

        int currLevel = startLevel;
        auto alignBlock = center.align(currLevel);

        int currRadius = 0;
        int prevRadius = 0;
        auto prevLoBorder = Vec3Int(0);
        auto prevHiBorder = Vec3Int(0);
        do {
            auto currBlockSize = TerrainChunk::getLevelChunkSize(currLevel);
            auto currLevelSpacing = getLODSpacing(currLevel);
            currRadius = currBlockSize * currLevelSpacing + prevRadius;

            auto loBorder = TerrainChunk::gridAlign(center.substract(currRadius), currLevel + 1);
            auto hiBorder = TerrainChunk::gridAlign(center.add(currRadius), currLevel + 1).add(TerrainChunk::getLevelChunkSize(currLevel + 1));

            for(int x = loBorder.x; x < hiBorder.x; x+=currBlockSize) {
                for(int y = loBorder.y; y < hiBorder.y; y+=currBlockSize) {
                    for(int z = loBorder.z; z < hiBorder.z; z+=currBlockSize) {
                        auto newBlock = Vec3Int(x, y, z);
                        if(prevRadius == 0 || newBlock.isBetween(prevLoBorder, prevHiBorder)) {
                            auto requestedBlock = m_storage->requestBlock(newBlock, currLevel);
                           
                           //Calculating neghbouringLods
                            auto neghbourLods = std::vector<int>();
                            for(auto neghbour : TerrainChunk::neghboursShifts) {
                                auto neghbourPos = neghbour * currBlockSize;
                                if(neghbourPos.isBetween(prevLoBorder, prevHiBorder)) {
                                    neghbourLods.push_back(prevRadius == 0 ? 1 : (currLevel - 1));
                                } else if(neghbourPos.isBetween(loBorder, hiBorder)) {
                                    neghbourLods.push_back(currLevel);
                                } else {
                                    neghbourLods.push_back(currLevel + 1);
                                }
                            }
                            requestedBlock->setNeghbouringLods(neghbourLods);


                            result.push_back(requestedBlock);
                        }
                    }
                }
            }

            prevRadius = currRadius;
            prevLoBorder = loBorder;
            prevHiBorder = hiBorder;
            currLevel++;

        } while(currRadius < radius);

        return result;

    }



    std::shared_ptr<TerrainChunk> TerrainBlocksProvidePersistant::requestBlock(Vec3Int pos, int level) {

        auto key = StorageKey({pos, level});
        auto block = m_chunkStorage.find(key);
        if(block != m_chunkStorage.end()) {
            block->second->resetFramesSinceLastUse();
            return block->second;
        }

        auto newBlock = std::make_shared<TerrainChunk>(pos, level);
        m_chunkStorage.insert({key, newBlock});
        if(m_onChunkCreated) {
            m_onChunkCreated(&(*newBlock));
        }
        return newBlock;


    }
    std::shared_ptr<TerrainChunk> TerrainBlocksProvidePersistant::getBlockIfExists(Vec3Int currBlock, int currBlockLevel) {

        auto key = StorageKey({currBlock, currBlockLevel});
        auto block = m_chunkStorage.find(key);
        if(block != m_chunkStorage.end()) {
            return block->second;
        }
        return nullptr;             
    }

    std::shared_ptr<TerrainChunk> TerrainBlocksProvidePersistant::getNeghbour(Vec3Int currBlock, int currBlockLevel, Vec3Int shift) {
        int minPossibleLevel = currBlockLevel > 1 ? currBlockLevel - 1 : 1;
        int maxPossibleLevel = currBlockLevel + 1;

        for(int neghbourLevel = minPossibleLevel; neghbourLevel <= maxPossibleLevel; neghbourLevel++) {
            auto neghbourPos = currBlock.add(Vec3Int(TerrainChunk::getLevelChunkSize(neghbourLevel)).mul(shift));
            auto neghbour = getBlockIfExists(neghbourPos, currBlockLevel);
            if(neghbour) {
                return neghbour;
            }
        }
        return nullptr;
    }

    void TerrainBlocksProvidePersistant::clearUnusedBlocks() {

        for (auto it = m_chunkStorage.begin(); it != m_chunkStorage.end();) {
            it->second->incrementFramesSiceLastUse();
            if (it->second->getFramesSinceLastUse() > m_removeAfterUnused)  {
                m_chunkStorage.erase(it++);
            } else {
                ++it;
            }
        }

    }
    void TerrainBlocksProvidePersistant::onChunkCreated(std::function<void(TerrainChunk*)> onChunkCreated) {
        m_onChunkCreated = onChunkCreated;
    }
    void TerrainBlocksProvidePersistant::clearStorage() {
        m_chunkStorage.clear();
    }


    int TerrainDataBlock::blockSize = 16;

}

    

