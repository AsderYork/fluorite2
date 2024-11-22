#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <optional>
#include <memory>
#include <algorithm>
#include <functional>
#include <limits>
#include <Terrain/TransvoxelTables/TransvoxelTables.h>
#include <Ogre.h>
#include <iostream>
#include <string>

namespace fluorite
{
    int div_floor(int x, int y);


    template<class T> 
    struct Vec3Type {
        T x,y,z;

        Vec3Type(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
        Vec3Type(T uniform = 0) : x(uniform), y(uniform), z(uniform) {}
        Vec3Type<T> align(int val) { return Vec3Type<T>(div_floor(x, val) * val, div_floor(y, val) * val, div_floor(z, val) * val);  }
        T& operator[](T index) {
            switch(index) {
                case 0: return x;
                case 1: return y;
                case 2: return z;
            }
            return x;
        }
        Vec3Type<T> operator+(const Vec3Type<T> other) {
            return Vec3Type<T>(x + other.x, y + other.y, z + other.z);
        }
        Vec3Type<T> operator-(const Vec3Type<T> other) {
            return Vec3Type<T>(x - other.x, y - other.y, z - other.z);
        }
        Vec3Type<T> operator*(const Vec3Type<T> other) {
            return Vec3Type<T>(x * other.x, y * other.y, z * other.z);
        }
         Vec3Type<T> operator/(const Vec3Type<T> other) {
            return Vec3Type<T>(x / other.x, y / other.y, z / other.z);
        }


        T distanceSquared(Vec3Type<T> other) {
            return (x - other.x) * (x - other.x) +
            (y - other.y) * (y - other.y) +
            (z - other.x) * (z - other.z);
        }

        static Vec3Type<T> unitX() { return Vec3Type<T>(1, 0, 0); }
        static Vec3Type<T> unitY() { return Vec3Type<T>(0, 1, 0); }
        static Vec3Type<T> unitZ() { return Vec3Type<T>(0, 0, 1); }

        Vec3Type<T> substract(const Vec3Type<T> other) { return Vec3Type<T>(x - other.x, y - other.y, z - other.z);  }
        Vec3Type<T> add(Vec3Type<T> other) {  return Vec3Type<T>(x + other.x, y + other.y, z + other.z); }
        Vec3Type<T> mul(Vec3Type<T> other) { return Vec3Type<T>(x * other.x, y * other.y, z * other.z); }


        bool lowerThen(Vec3Type<T> other) const {
            return x < other.x || y < other.y || z < other.z;
        }
        bool greaterThen(Vec3Type<T> other) const {
            return x > other.x || y > other.y || z > other.z;
        }
        bool greaterOrEqualThen(Vec3Type<T> other) const {
            return x >= other.x || y >= other.y || z >= other.z;
        }
        bool operator<(const Vec3Type<T>& rhs) const  {
            return std::make_tuple(x, y, z) < std::make_tuple(rhs.x, rhs.y, rhs.z);
        }
        bool isBetween(Vec3Type<T> lo, Vec3Type<T> hi) const {
            return this->lowerThen(lo) || this->greaterOrEqualThen(hi);
        }

        std::string to_string() {
            return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
        }
    };

    /*template<template<typename>class ContainerType, class T>
    class Container {
        ContainerType<T>& cnt;
        Container(ContainerType<T>& v) cnt(v) {};
        template<class RES, class Fn>
        Container<RES> map(Fn fn) {
                RES storage;
                std::transform(cnr.cbegin(), cnt.cend(), std::back_inserter(storage),  Fn);
                return Container(storage);
        }
    };*/

    typedef Vec3Type<int> Vec3Int;
    typedef Vec3Type<int> Vec3Float;

    template <typename T> int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }

    template <typename T> int sgnp(T val) {
        return (T)(val >= T(0));
    }

    struct Matrix3X3
    {
       float M11;
       float M12;
       float M13;
       float M21;
       float M22;
       float M23;
       float M31;
       float M32;
       float M33;

        Matrix3X3(Vec3Float col1, Vec3Float col2, Vec3Float col3)
        {
            M11 = col1.x;
            M12 = col2.x;
            M13 = col3.x;
            M21 = col1.y;
            M22 = col2.y;
            M23 = col3.y;
            M31 = col1.z;
            M32 = col2.z;
            M33 = col3.z;
        }

        Matrix3X3(float m11,float m12,float m13,
                         float m21,float m22,float m23,
                         float m31,float m32,float m33)
        {
            M11 = m11;
            M12 = m12;
            M13 = m13;
            M21 = m21;
            M22 = m22;
            M23 = m23;
            M31 = m31;
            M32 = m32;
            M33 = m33;
        }

        Matrix3X3()
        {
            M11 = 1; M12 = 0; M13 = 0;
            M21 = 0; M22 = 1; M23 = 0;
            M31 = 0; M32 = 0; M33 = 1;
        }

        Vec3Float Col1Get() {return Vec3Float(M11, M21, M31);}
        void Col1Set(Vec3Float value) {M11 = value.x; M21 = value.y; M31 = value.z;}

        Vec3Float Col2Get() {return Vec3Float(M12, M22, M32);}
        void Col2Set(Vec3Float value) {M12 = value.x; M22 = value.y; M32 = value.z;}
        
        Vec3Float Col3Get() {return Vec3Float(M13, M23, M33);}
        void Col3Set(Vec3Float value) {M13 = value.x; M23 = value.y; M33 = value.z;}

        Vec3Float operator*(Vec3Float v)
        {
            return Vec3Float(
                M11 * v.x + M12 * v.y + M13 * v.z,
                M21 * v.x + M22 * v.y + M23 * v.z,
                M31 * v.x + M32 * v.y + M33 * v.z);
        }

        Matrix3X3 operator*(const Matrix3X3 m)
        {
            return Matrix3X3(
                M11*m.M11 + M12*m.M21 + M13*m.M31, M11*m.M12 + M12*m.M22 + M13*m.M32, M11*m.M13 + M12*m.M23 + M13*m.M33,
                M21*m.M11 + M22*m.M21 + M23*m.M31, M21*m.M12 + M22*m.M22 + M23*m.M32, M21*m.M13 + M22*m.M23 + M23*m.M33,
                M31*m.M11 + M32*m.M21 + M33*m.M31, M31*m.M12 + M32*m.M22 + M33*m.M32, M31*m.M13 + M32*m.M23 + M33*m.M33
            );
        }

        Matrix3X3 operator+(const Matrix3X3 m)
        {
            return Matrix3X3(
                M11 + m.M11, M12 + m.M12, M13 + m.M13,
                M21 + m.M21, M22 + m.M22, M23 + m.M23,
                M31 + m.M31, M32 + m.M32, M33 + m.M33
            );
        }
    
          Matrix3X3 rotate(Vec3Float u, float angle) {
            auto cos = std::cos(angle);
            auto sin = std::sin(angle);

            if(std::abs(cos) <= std::numeric_limits<float>::epsilon()) {cos = 0;}
            if(std::abs(sin) <= std::numeric_limits<float>::epsilon()) {sin = 0;}

            auto ncos = (1 - cos);

            auto rot = Matrix3X3(
                cos + u.x*u.x*ncos      , u.x*u.y*ncos - u.z*sin, u.x*u.z*ncos + u.y*sin,
                u.y*u.x*ncos + u.z*sin  , cos + u.y*u.y*ncos    , u.y*u.z*ncos - u.x*sin,
                u.z*u.x*ncos - u.y*sin  , u.z*u.y*ncos + u.x*sin, cos + u.z*u.z*ncos
            );
            return rot;

        }
    
    };


    template<class T>
    class EventDelegate {
        private:
            std::vector<std::function<void(T)>> m_events;
        public:
            void add(std::function<void(T)> event) {
                m_events.push_back(event);
            }
            void call(T data) {
                std::for_each(m_events.begin(), m_events.end(), [&](auto& function){function(data);});
            }
    };

    
    inline Vec3Int V3IFromOgreVec(Ogre::Vector3 vec) {
        return Vec3Int(vec.x, vec.y, vec.z);
    };

    inline Ogre::Vector3 OgreVecFromV3I(Vec3Int vec) {
        return Ogre::Vector3(vec.x, vec.y, vec.z);
    };

    class TerrainChunk;

    class TerrainChunkRenderableInterface {
        public:
        virtual std::string getName() {return "UNKNOWN";}
        virtual void init(TerrainChunk*) = 0;
        virtual ~TerrainChunkRenderableInterface(){};
    };

    class TerrainChunk {
        public:
            static int chunkSize;
            static int getLevelChunkSize(int level);
            static std::vector<Vec3Int> neghboursShifts;
        private:
            Vec3Int m_pos;
            int m_lod;
            int m_framesSinceLastUse = 0;
            std::vector<std::unique_ptr<TerrainChunkRenderableInterface>> m_renderables;
            std::vector<int> m_neghbourLod;

        public:
            static Vec3Int gridAlign(Vec3Int pos, int level);
            TerrainChunk(Vec3Int pos, int lod);

            void setNeghbouringLods(std::vector<int> lods) {
                m_neghbourLod = lods;
            }

            std::vector<int> getNeighbouringLods() {
                return m_neghbourLod;
            }

            void addRenderable(std::unique_ptr<TerrainChunkRenderableInterface> renderable);
            Vec3Int getPos();
            int getLoD();
            int getFramesSinceLastUse();
            void resetFramesSinceLastUse();
            void incrementFramesSiceLastUse();
            void initRenderables();

    };



    class TerrainBlocksStorageInterface {
        public:
            virtual std::shared_ptr<TerrainChunk> requestBlock(Vec3Int pos, int level) = 0;
            virtual std::shared_ptr<TerrainChunk> getBlockIfExists(Vec3Int currBlock, int currBlockLevel) = 0;
            virtual void clearUnusedBlocks() = 0;
    };

    class TerrainBlocksStorageDummy : public TerrainBlocksStorageInterface {
        public:
            std::shared_ptr<TerrainChunk> requestBlock(Vec3Int pos, int level);
            std::shared_ptr<TerrainChunk> getBlockIfExists(Vec3Int currBlock, int currBlockLevel);
            void clearUnusedBlocks();
    };

    class TerrainBlocksProvidePersistant : public TerrainBlocksStorageInterface {

        private:
            struct StorageKey {
                Vec3Int m_pos;
                int level;
                  bool operator <(const StorageKey& rhs) const  {
                        return std::make_tuple(level, m_pos.x, m_pos.y, m_pos.z) < std::make_tuple(rhs.level, rhs.m_pos.x, rhs.m_pos.y, rhs.m_pos.z);
                    }
            };
           
            std::function<void(TerrainChunk*)> m_onChunkCreated;
            std::map<StorageKey, std::shared_ptr<TerrainChunk>> m_chunkStorage;
            int m_removeAfterUnused = 10;

        public:
            std::shared_ptr<TerrainChunk> requestBlock(Vec3Int pos, int level);
            std::shared_ptr<TerrainChunk> getBlockIfExists(Vec3Int currBlock, int currBlockLevel);
            std::shared_ptr<TerrainChunk> getNeghbour(Vec3Int currBlock, int currBlockLevel, Vec3Int shift);
            void clearUnusedBlocks();
            void onChunkCreated(std::function<void(TerrainChunk*)>);
            void clearStorage();
    };

    class TerrainChunkFinder {

        private:
        TerrainBlocksStorageInterface* m_storage;

        public:
            TerrainChunkFinder(TerrainBlocksStorageInterface* provider);

            static int getLODSpacing(int level);
            std::vector<std::shared_ptr<TerrainChunk>> findBlocks(Vec3Int center, int radius, int startLevel = 1);

    };



    struct TerrainDataBlockNode {
        uint16_t mat;
        int8_t val;
        TerrainDataBlockNode(uint16_t _val = std::numeric_limits<uint16_t>::min(), int8_t _mat = std::numeric_limits<int8_t>::min()) : val(_val), mat(_mat) {} 
    };


    class FrameCounter {
        private:
            int m_framesSinceLastRequest = 0;
        public:
            int getFramesSinceLastRequest() {
                return m_framesSinceLastRequest;
            }
            void resetFramesSinceLastRequest() {
                m_framesSinceLastRequest = 0;
            }
            void incrementFrameSinceLastRequestCounter() {
                m_framesSinceLastRequest++;
            }

    };


    class TerrainDataBlock : public FrameCounter {
        private:
            static int blockSize;

            std::vector<TerrainDataBlockNode> m_vec;
            Vec3Int m_pos;

            int getNodeIndex(Vec3Int pos) {
                return pos.x + blockSize * pos.y + blockSize*blockSize * pos.z;
            }
            int getNodeIndex(int x, int y, int z) {
                return x + blockSize * y + blockSize*blockSize * z;
            }

        public:
            void reset() {
                m_vec.clear();
                m_vec.resize(blockSize*blockSize*blockSize);
            }

            TerrainDataBlock(Vec3Int pos) : m_pos(pos) {reset();}

            TerrainDataBlockNode& getNode(Vec3Int pos) {return m_vec[getNodeIndex(pos)];}

            Vec3Int getPos() {
                return m_pos;
            }

    };




    class TerrainDataBlockStorage {
        private:
            std::map<Vec3Int, std::shared_ptr<TerrainDataBlock>> m_storage;
            EventDelegate<std::shared_ptr<TerrainDataBlock>> m_onBlockCreated;
            int m_removeBlocksAfterFrames = 10;

        public:

            void onBlockCreated(std::function<void(std::shared_ptr<TerrainDataBlock>)> block) {
                m_onBlockCreated.add(block);
            }
            
            std::shared_ptr<TerrainDataBlock> requestBlock(Vec3Int pos) {
                auto block = m_storage.find(pos);
                if(block != m_storage.end()) {
                    block->second->resetFramesSinceLastRequest();
                    return block->second;
                }

                auto newBlock = std::make_shared<TerrainDataBlock>(pos);
                m_storage.insert({pos, newBlock});
                m_onBlockCreated.call(newBlock);
                return newBlock;
            }
            
            void removeOldBlocks() {
                 for (auto it = m_storage.begin(); it != m_storage.end();) {
                    it->second->incrementFrameSinceLastRequestCounter();
                    if (it->second->getFramesSinceLastRequest() > m_removeBlocksAfterFrames)  {
                        m_storage.erase(it++);
                    } else {
                        ++it;
                    }
                }
            }

    };















struct ReuseCell
{
    std::vector<int> verts;
    uint16_t caseIndex;
    ReuseCell(int size = 4) {
        verts.resize(size);
        for (int i = 0; i < size; i++) {
            verts[i] = -1;
        }
    }
};

class RegularCellCache
{
    private:
        ReuseCell _cache[2][16 * 16];
    public:
        ReuseCell GetReusedIndex(Vec3Int pos, uint8_t rDir)
        {
            int rx = rDir & 0x01;
            int rz = (rDir >> 1) & 0x01;
            int ry = (rDir >> 2) & 0x01;

            int dx = pos.x - rx;
            int dy = pos.y - ry;
            int dz = pos.z - rz;

            return _cache[dx & 1][dy * 16 + dz];
        }

        void SetReusableIndex(Vec3Int pos, uint8_t reuseIndex, int p) {
            _cache[pos.x & 1][pos.y * 16 + pos.z].verts[reuseIndex] = p;
        }
    };

class TransitionCache
{
    private:
        ReuseCell _cache[2 * 17 * 17];

    public:
        TransitionCache() {
        const int cacheSize =  2 * 17 * 17;

        for (int i = 0; i < cacheSize; i++)
        {
            _cache[i] = ReuseCell(12);
        }
    }

    ReuseCell& getCell(int x, int y) {
        return _cache[x + (y & 1) * 17]; 
    }

    void setCell(int x, int y, ReuseCell val) {
        _cache[x + (y & 1) * 16] = val;
    }
};



    struct Vertex
    {
        Ogre::Vector3 Primary;
        Ogre::Vector3 Secondary;
        Ogre::Vector3 Normal;
        uint8_t Near;
    };



    struct TransvoxelPolygonizatorVertex {
        Ogre::Vector3 pos;
        Ogre::Vector3 normal;
        TransvoxelPolygonizatorVertex(Ogre::Vector3 _pos, Ogre::Vector3 _normal) : pos(_pos), normal(_normal) {}
    };


    class TransvoxelPolygonizator {

        private:
            
            static Ogre::Vector3 generateVertexPos(Vec3Int offsetPos, Vec3Int pos, int lod, long t, uint8_t v0, uint8_t v1)
            {
                auto p0 = OgreVecFromV3I(offsetPos.add(getCornerByIndex(v0)).mul(lod));
                auto p1 = OgreVecFromV3I(offsetPos.add(getCornerByIndex(v1)).mul(lod));
                return interpolateVoxelVector(t, p0, p1);
            }

            static Ogre::Vector3 interpolateVoxelVector(long t, Ogre::Vector3 P0, Ogre::Vector3 P1)
            {
                long u = 0x0100 - t; //256 - t
                float s = 1.0f / 256.0f;
                Ogre::Vector3 Q = P0 * t + P1 * u; //Density Interpolation
                Q *= s; // shift to shader ! 
                return Q;
            }

            static Vec3Int getCornerByIndex(int index) {
                return Vec3Int(index & 0x01, (index >> 2) && 0x01, (index >> 1) & 0x01);
            }

            static uint8_t HiNibble(uint8_t b)
            {
                return (uint8_t)(((b) >> 4) & 0x0F);
            }

            static uint8_t LoNibble(uint8_t b)
            {
                return (uint8_t)(b & 0x0F);
            }


            float S = 1.0f / 256.0f;

            Ogre::Vector3 Interp(Vec3Int v0, Vec3Int v1, Vec3Int p0, Vec3Int p1, uint8_t lodIndex = 0)
            {
                return Interp(OgreVecFromV3I(v0), OgreVecFromV3I(v1), p0, p1, lodIndex);
            }

            Ogre::Vector3 Interp(Ogre::Vector3 v0, Ogre::Vector3 v1, Vec3Int p0, Vec3Int p1, uint8_t lodIndex = 0)
            {
                auto s0 = getNode(p0).val;
                auto s1 = getNode(p1).val;

                int t = (s1 << 8) / (s1 - s0);
                int u = 0x0100 - t;

                if ((t & 0x00ff) == 0)
                {
                    // The generated vertex lies at one of the corners so there 
                    // is no need to subdivide the interval.
                    if (t == 0)
                    {
                        return v1;
                    }
                    return v0;
                }
                else
                {
                    for (int i = 0; i < lodIndex; ++i)
                    {
                        Ogre::Vector3 vm = (v0 + v1) / 2;
                        Vec3Int pm = (p0 + p1) / 2;

                        auto sm = getNode(pm).val;

                        // Determine which of the sub-intervals that contain 
                        // the intersection with the isosurface.
                        if (sgn(s0) != sgn(sm))
                        {
                            v1 = vm;
                            p1 = pm;
                            s1 = sm;
                        }
                        else
                        {
                            v0 = vm;
                            p0 = pm;
                            s0 = sm;
                        }
                    }
                    t = (s1 << 8) / (s1 - s0);
                    u = 0x0100 - t;

                    return v0 * t * S + v1 * u * S;
                }
            }

            static Ogre::Vector3 ComputeDelta(Ogre::Vector3 v, int k, int s)
            {
                float p2k = k * k; // 1 << k would do the job much more efficient
                float wk = std::pow(2, k - 2);
                auto delta = Ogre::Vector3::ZERO;

                if (k < 1)
                {
                    return delta;
                }

                // x
                float p = v.x;
                float p2mk = std::pow(2.0, -k);
                if (p < p2k)
                {
                    // The vertex is inside the minimum cell.
                    delta.x = (1.0f - p2mk * p) * wk;
                }
                else if (p > (p2k * (s - 1)))
                {
                    // The vertex is inside the maximum cell.
                    delta.x = ((p2k * s) - 1.0f - p) * wk;
                }

                // y
                p = v.y;
                if (p < p2k)
                {
                    // The vertex is inside the minimum cell.
                    delta.y = (1.0f - p2mk * p) * wk;
                }
                else if (p > (p2k * (s - 1)))
                {
                    // The vertex is inside the maximum cell.
                    delta.y = ((p2k * s) - 1.0f - p) * wk;
                }

                // z
                p = v.z;
                if (p < p2k)
                {
                    // The vertex is inside the minimum cell.
                    delta.z = (1.0f - p2mk * p) * wk;
                }
                else if (p > (p2k * (s - 1)))
                {
                    // The vertex is inside the maximum cell.
                    delta.z = ((p2k * s) - 1.0f - p) * wk;
                }

                return delta;
            }

            static Ogre::Vector3 ProjectNormal(Ogre::Vector3 n, Ogre::Vector3 delta)
            {
                return n.crossProduct(delta);
            }

            Vec3Int pos;

            TerrainDataBlockNode getNode(Vec3Int shift) {
                auto specPos = pos.add(shift);
                float val = 0;// + sin(specPos.x * 0.3) * 4 + -sin(specPos.z * 0.07) * 12;
                return TerrainDataBlockNode(std::clamp((int)(val - specPos.y ) * 4, -127, 127));
                /*if(specPos.y < 9) {
                    return TerrainDataBlockNode(127);
                }
                return TerrainDataBlockNode(-127);*/
            }
            
            uint8_t getCaseCode(std::vector<TerrainDataBlockNode> corners) {
                uint8_t caseCode = 
                    ((corners[0].val >> 7) & 0x01)
                    | ((corners[1].val >> 6) & 0x02)
                    | ((corners[2].val >> 5) & 0x04)
                    | ((corners[3].val >> 4) & 0x08)
                    | ((corners[4].val >> 3) & 0x10)
                    | ((corners[5].val >> 2) & 0x20)
                    | ((corners[6].val >> 1) & 0x40)
                    |  (corners[7].val & 0x80); 

                    return caseCode;
            }

        
            int PolygonizeTransitionCell(Vec3Int offset, Vec3Int origin, int lodIndex, int axis)
            {
                int lodStep = 1 << lodIndex;
                int sampleStep = 1 << (lodIndex - 1);
                int lodScale = 1 << lodIndex;
                int last = 16 * lodScale;

            uint8_t directionMask = 2;


                /**
                unsigned char nearq = 0;

                // Compute which of the six faces of the block that the vertex 
                // is near. (near is defined as being in boundary cell.)
                for (int i = 0; i < 3; i++)
                {
                    // Vertex close to negative face.
                    if (origin[i] == 0) { nearq |= (1 << (i * 2 + 0)); }
                    // Vertex close to positive face.
                    if (origin[i] == last) { nearq |= (1 << (i * 2 + 1)); }
                }
                Doesnt make any sence to calculate this. We are allways at the maximal face of the block
                Calling this method for any other places is prohibited
                */

                /**
                 * 
                 * 
                 *                  <>--------<>
                 *                 /|         /|
                 *                / |        / |
                 *               /  |       /  |
                 *              <>---<>---<>   |
                 *              |   | |   |    |
                 *              |   <>----|---<>
                 *              <>---<>---<>  /
                 *     z        | /   |   |  /
                 *    /|\       |/    |   | /
                 *     | _      <>---<>---<>
                 *     | /|z
                 *     |/____\ x
                 *           /
                 *    
                 *
                 *  This is a 9-4 block template where low-z values are more detailed and high-z values are less detailed
                 */
            Vec3Int coords[] = {
                    Vec3Int(0,0,0), Vec3Int(1,0,0), Vec3Int(2,0,0), // High-res lower row
                    Vec3Int(0,1,0), Vec3Int(1,1,0), Vec3Int(2,1,0), // High-res middle row
                    Vec3Int(0,2,0), Vec3Int(1,2,0), Vec3Int(2,2,0), // High-res upper row
                    Vec3Int(0,0,2), Vec3Int(2,0,2), // Low-res lower row
                    Vec3Int(0,2,2), Vec3Int(2,2,2)  // Low-res upper row
                };

            auto mx = Vec3Float( 1,  0, -1) * sampleStep;
            auto my = Vec3Float( 0,  1,  0) * sampleStep;
            auto mz = Vec3Float( 1,  0,  1) * sampleStep;

            auto basis = Matrix3X3().rotate({0,1,0},  Ogre::Math::DegreesToRadians(90));

            std::vector<Vec3Int> pos;
            for(int i = 0; i < 13; i++) {
                auto currVec = (basis * coords[i]);
                currVec.x = std::round(currVec.x);
                currVec.y = std::round(currVec.y);
                currVec.z = std::round(currVec.z);
                pos.push_back(offset + currVec);
            }

            auto basis2 = Matrix3X3().rotate({0,1,0},  Ogre::Math::DegreesToRadians(0));

                std::vector<Vec3Int> pos2;
                for(int i = 0; i < 13; i++) {
                    pos2.push_back(offset + (basis2 * coords[i]));
                }


            auto normals = std::vector<Ogre::Vector3>(13);

            for (int i = 0; i < 9; i++)
            {
                auto p = pos[i];
                float nx = (getNode(origin + p + Vec3Int::unitX()).val - getNode(origin + p - Vec3Int::unitX()).val) * 0.5f;
                float ny = (getNode(origin + p + Vec3Int::unitY()).val - getNode(origin + p - Vec3Int::unitY()).val) * 0.5f;
                float nz = (getNode(origin + p + Vec3Int::unitZ()).val - getNode(origin + p - Vec3Int::unitZ()).val) * 0.5f;
                normals[i] = Ogre::Vector3(nx, ny, nz).normalisedCopy();
            }

            normals[0x9] = normals[0];
            normals[0xA] = normals[2];
            normals[0xB] = normals[6];
            normals[0xC] = normals[8];

                Vec3Int samplePos[] = {
                    pos[0], pos[1], pos[2], 
                    pos[3], pos[4], pos[5], 
                    pos[6], pos[7], pos[8],
                    pos[0], pos[2],
                    pos[6], pos[8]
                };

                uint32_t caseCode = (sgnp(getNode(origin + pos[0]).val) * 0x001 |
                                     sgnp(getNode(origin + pos[1]).val) * 0x002 |
                                     sgnp(getNode(origin + pos[2]).val) * 0x004 |
                                     sgnp(getNode(origin + pos[5]).val) * 0x008 |
                                     sgnp(getNode(origin + pos[8]).val) * 0x010 |
                                     sgnp(getNode(origin + pos[7]).val) * 0x020 |
                                     sgnp(getNode(origin + pos[6]).val) * 0x040 |
                                     sgnp(getNode(origin + pos[3]).val) * 0x080 |
                                     sgnp(getNode(origin + pos[4]).val) * 0x100);

                if (caseCode == 0 || caseCode == 511)
                    return 0;

                tcahe.getCell(offset.x, offset.y).caseIndex = caseCode;

                auto classIndex = transitionCellClass[caseCode]; // Equivalence class index.
                auto data = transitionCellData[classIndex & 0x7F];
                bool inverse = (classIndex & 128) != 0;
                std::vector<int> localVertexMapping(12);

                int nv = (int)data.GetVertexCount();
                int nt = (int)data.GetTriangleCount();

                auto vert = Vertex();

                for (int i = 0; i < nv; i++)
                {
                    auto edgeCode = transitionVertexData[caseCode][i];
                    
                    auto v0 = (edgeCode >> 4) & 0x0F; //First Corner Index
                    auto v1 = (edgeCode) & 0x0F; //Second Corner Index

                    bool lowside = (v0 > 8) && (v1 > 8);

                    int d0 = getNode(samplePos[v0]).val;
                    int d1 = getNode(samplePos[v1]).val;


                    int t = (d1 << 8) / (d1 - d0);
                    int u = 0x0100 - t;
                    float t0 = t * S;
                    float t1 = u * S;

                    Ogre::Vector3 n0 = normals[v0];
                    Ogre::Vector3 n1 = normals[v1];

                    //vert.Near = nearq;
                    vert.Normal = n0 * t0 + n1 * t1;


                    if ((t & 0x00ff) != 0)
                    {
                        // Use the reuse information in transitionVertexData
                        auto dir = HiNibble(edgeCode >> 8);
                        auto idx = LoNibble(edgeCode >> 8);

                        bool present = (dir & directionMask) == dir;

                        if (present)
                        {
                            // The previous cell is available. Retrieve the cached cell 
                            // from which to retrieve the reused vertex index from.
                            auto prev = tcahe.getCell(offset.x - (dir & 1), offset.y - ((dir >> 1) & 1));

                            if (prev.caseIndex == 0 || prev.caseIndex == 511)
                            {
                                // Previous cell does not contain any geometry.
                                localVertexMapping[i] = -1;
                            }
                            else
                            {
                                // Reuse the vertex index from the previous cell.
                                localVertexMapping[i] = prev.verts[idx];
                            }
                        }
                        if (!present || localVertexMapping[i] < 0)
                        {
                            
                            auto p0 = OgreVecFromV3I(pos[v0]);
                            auto p1 = OgreVecFromV3I(pos[v1]);

                            auto pi = Interp(p0, p1, samplePos[v0], samplePos[v1], lowside ? lodIndex : (lodIndex - 1));

                            auto vecPos = Ogre::Vector3();
                            if (lowside)
                            {
                                switch (axis)
                                {
                                    case 0:
                                        pi.x = (float)origin.x;
                                        break;
                                    case 1:
                                        pi.y = (float)origin.y;
                                        break;
                                    case 2:
                                        pi.z = (float)origin.z;
                                        break;
                                }

                                Ogre::Vector3 delta = ComputeDelta(pi, lodIndex, 16);
                                auto proj = ProjectNormal(vert.Normal, delta);

                                //vert.Primary = Unused;
                                vert.Secondary = pi;// + proj;
                                vecPos = vert.Secondary;
                            }
                            else
                            {
                                vert.Near = 0;
                                vert.Primary = pi;
                                vecPos = vert.Primary;
                            }

                            localVertexMapping[i] = vertices.size();
                            vertices.push_back(TransvoxelPolygonizatorVertex(vecPos, Ogre::Vector3::UNIT_X));

                            if ((dir & 8) != 0)
                            {
                                // The vertex can be reused.
                                tcahe.getCell(offset.x, offset.y).verts[idx] = localVertexMapping[i];
                            }
                        }
                    }
                    else
                    {
                        // Try to reuse corner vertex from a preceding cell.
                        // Use the reuse information in transitionCornerData.
                        auto v = t == 0 ? v1 : v0;
                        auto cornerData = transitionCornerData[v];

                        auto dir = HiNibble(cornerData); // High nibble contains direction code.
                        auto idx = LoNibble((cornerData)); // Low nibble contains storage slot for vertex.
                        bool present = (dir & directionMask) == dir;

                        if (present)
                        {
                            // The previous cell is available. Retrieve the cached cell 
                            // from which to retrieve the reused vertex index from.
                            auto prev = tcahe.getCell(offset.x - (dir & 1), offset.y - ((dir >> 1) & 1));

                            if (prev.caseIndex == 0 || prev.caseIndex == 511)
                            {
                                // Previous cell does not contain any geometry.
                                localVertexMapping[i] = -1;
                            }
                            else
                            {
                                // Reuse the vertex index from the previous cell.
                                localVertexMapping[i] = prev.verts[idx];
                            }
                        }

                        Ogre::Vector3 vecPos;
                        if (!present || localVertexMapping[i] < 0)
                        {
                            // A vertex has to be created.
                            auto pi = OgreVecFromV3I(pos[v]);

                            if (v > 8)
                            {
                                // On low-resolution side.
                                // Necessary to translate the intersection point to the 
                                // high-res side so that it is transformed the same way 
                                // as the vertices in the regular cell.
                                switch (axis)
                                {
                                    case 0:
                                        pi.x = (float)origin.x;
                                        break;
                                    case 1:
                                        pi.y = (float)origin.y;
                                        break;
                                    case 2:
                                        pi.z = (float)origin.z;
                                        break;
                                }

                                auto delta = ComputeDelta(pi, lodIndex, 16);
                                auto proj = ProjectNormal(vert.Normal, delta);

                                vert.Secondary = pi + proj;
                                vecPos = vert.Secondary;
                            }
                            else
                            {
                                // On high-resolution side.
                                vert.Near = 0; // Vertices on high-res side are never moved.
                                vert.Primary = pi;
                                vecPos = vert.Primary;
                            }
                            std::cout << vecPos.x;
                            localVertexMapping[i] = vertices.size();
                            tcahe.getCell(offset.x, offset.y).verts[idx] = localVertexMapping[i];
                            vertices.push_back(TransvoxelPolygonizatorVertex(vecPos, Ogre::Vector3::UNIT_X));
                        }
                    }
                }

                for (long t = 0; t < nt; ++t)
                {
                    if (inverse)
                    {
                        indices.push_back(localVertexMapping[data.vertexIndex[t * 3 + 2]]);
                        indices.push_back(localVertexMapping[data.vertexIndex[t * 3 + 1]]);
                        indices.push_back(localVertexMapping[data.vertexIndex[t * 3 + 0]]);
                        //indices.push_back(localVertexMapping[ptr[2]]);
                        //indices.push_back(localVertexMapping[ptr[1]]);
                        //indices.push_back(localVertexMapping[ptr[0]]);
                    }
                    else
                    {
                        indices.push_back(localVertexMapping[data.vertexIndex[t * 3 + 0]]);
                        indices.push_back(localVertexMapping[data.vertexIndex[t * 3 + 1]]);
                        indices.push_back(localVertexMapping[data.vertexIndex[t * 3 + 2]]);
                        // indices.push_back(localVertexMapping[ptr[0]]);
                        // indices.push_back(localVertexMapping[ptr[1]]);
                        // indices.push_back(localVertexMapping[ptr[2]]);
                    }
                }

                return nt;
            }
        


            void polygonizeCell(TerrainDataBlock* dataBlock, Vec3Int offsetPos, int lod) {

                uint8_t directionMask = (offsetPos.x > 0 ? 1 : 0) | ((offsetPos.z > 0 ? 1 : 0) << 1) | ((offsetPos.y > 0 ? 1 : 0) << 2);

                std::vector<TerrainDataBlockNode> corners = {
                    getNode(offsetPos.add(getCornerByIndex(0)) * lod),
                    getNode(offsetPos.add(getCornerByIndex(1)) * lod),
                    getNode(offsetPos.add(getCornerByIndex(2)) * lod),
                    getNode(offsetPos.add(getCornerByIndex(3)) * lod),
                    getNode(offsetPos.add(getCornerByIndex(4)) * lod),
                    getNode(offsetPos.add(getCornerByIndex(5)) * lod),
                    getNode(offsetPos.add(getCornerByIndex(6)) * lod),
                    getNode(offsetPos.add(getCornerByIndex(7)) * lod),
                };

                auto caseCode = getCaseCode(corners);

                if ((caseCode ^ ((corners[7].val >> 7) & 0xFF)) == 0) {
                    return;
                }

                auto cornerNormals = new Ogre::Vector3[8];
                for (int i = 0; i < 8; i++)
                {
                    auto p = offsetPos.add(getCornerByIndex(i).mul(lod));
                    float nx = (getNode(p.add({1,0,0})).val - getNode(p.add({-1, 0, 0})).val) * 0.5f;
                    float ny = (getNode(p.add({0,1,0})).val - getNode(p.add({ 0,-1, 0})).val) * 0.5f;
                    float nz = (getNode(p.add({0,0,1})).val - getNode(p.add({ 0, 0,-1})).val) * 0.5f;
                    cornerNormals[i] = Ogre::Vector3(nx, ny, nz).normalisedCopy();
                }

                auto classVal = regularCellClass[caseCode];
                auto vertexLocations = regularVertexData[caseCode];
                auto cellData = regularCellData[classVal];
                auto vertexCount = cellData.GetVertexCount();
                auto triangleCount = cellData.GetTriangleCount();
                auto indexOffset = cellData.vertexIndex;
                int mappedIndizes[16]; //array with real indizes for current cell

                for (int i = 0; i < vertexCount; i++)
                {
                    auto edge = vertexLocations[i] >> 8;
                    auto reuseIndex = edge & 0xF; //Vertex id which should be created or reused 1,2 or 3
                    auto rDir = edge >> 4; //the direction to go to reach a previous cell for reusing 

                    auto v0 = (vertexLocations[i] >> 4) & 0x0F; //First Corner Index
                    auto v1 = (vertexLocations[i]) & 0x0F; //Second Corner Index

                    auto d0 = corners[v0].val;
                    auto d1 = corners[v1].val;

                    Ogre::Vector3 n0 = cornerNormals[v0];
                    Ogre::Vector3 n1 = cornerNormals[v1];

                    int t = (d1 << 8) / (d1 - d0);
                    int u = 0x0100 - t;
                    float t0 = t / 256.0f;
                    float t1 = u / 256.0f;

                    int index = -1;

                    if (v1 != 7 && (rDir & directionMask) == rDir)
                    {
                        ReuseCell cell = cache.GetReusedIndex(offsetPos, rDir);
                        index = cell.verts[reuseIndex];
                    }

                    if (index == -1)
                    {
                        auto normal = cornerNormals[v0] * t0 + cornerNormals[v1] * t1;
                        auto vertex = generateVertexPos(offsetPos, dataBlock->getPos(), lod, t, v0, v1);
                        vertices.push_back(TransvoxelPolygonizatorVertex(vertex, normal));
                        index = vertices.size() - 1;
                    }

                    if ((rDir & 8) != 0)
                    {
                        cache.SetReusableIndex(offsetPos, reuseIndex, vertices.size() - 1);
                    }

                    mappedIndizes[i] = index;
                }

                for (int t = 0; t < triangleCount; t++)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        indices.push_back(mappedIndizes[cellData.vertexIndex[t * 3 + i]]);
                    }
                }

            }

            static std::vector<Vec3Int> getTransitionCellNodes(Vec3Int OpposingChunkPosition) {
                
                std::vector<Vec3Int> result;
                result.push_back(Vec3Int(0));
                result.push_back(Vec3Int(0));
                

            }

            RegularCellCache cache;
            TransitionCache tcahe;
            std::vector<TransvoxelPolygonizatorVertex> vertices;
            std::vector<int> indices;

        public:
            void PolygonizeSingleBlock(TerrainDataBlock* dataBlock, int lod) {
                pos = dataBlock->getPos();

                if(pos.x == 16 && pos.y == 0 && pos.z == 0) {
                    PolygonizeTransitionCell({14,0,6}, pos, 2, 1);
                    //PolygonizeTransitionCell({15,0,0}, pos, 2, 1);
                } else {
                    //if(lod == 1) {return;}
                    for (int x = 0; x < 16 ; x++) {
                        for (int y = 0; y < 16; y ++) {
                            for (int z = 0; z < 16; z ++) {
                                auto position = Vec3Int(x, y, z);
                                polygonizeCell(dataBlock, position, lod);
                            }
                        }
                    } 
                }
            }

            std::vector<TransvoxelPolygonizatorVertex> getVertices() {
                return vertices;
            }

            std::vector<Ogre::Vector3> getVerticesPositions() {
                std::vector<Ogre::Vector3> result;
                std::transform(vertices.cbegin(), vertices.cend(), std::back_inserter(result),  [](auto& x) { return x.pos; });
                return result;
            }

            std::vector<int> getIndices() {
                return indices;
            }


       
    };

}