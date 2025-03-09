#include <cmath>

namespace fluorite 
{
    struct IntVector {
        int x, y, z;
        IntVector(int v) : x(v), y(v), z(v) {}
        IntVector(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}

        IntVector div(int v) const {
            return IntVector(x/v, y/v, z/v);
        }

        IntVector smoothdiv(int v) const {
            auto signx = x < 0 ? -1 : 0;
            auto signy = y < 0 ? -1 : 0;
            auto signz = z < 0 ? -1 : 0;

            return IntVector(x/v + signx, y/v + signy, z/v + signz);
        }

        bool operator==(const IntVector& other) const
        {
            return x == other.x && y == other.y && z == other.z;
        }


        IntVector mul(int v) const {
            return IntVector(x*v, y*v, z*v);
        }


        IntVector sub(int v) const {
            return IntVector(x - v, y - v, z - v);
        }

        IntVector sub(IntVector v) const {
            return IntVector(x - v.x, y - v.y, z - v.z);
        }

        float length() const {
            return sqrt(x*x + y*y + z*z);
        }


        IntVector add(int v) const {
            return IntVector(x + v, y + v, z + v);
        }

        IntVector add(IntVector v) const {
            return IntVector(x + v.x, y + v.y, z + v.z);
        }

        IntVector mod(int v) const {
            return IntVector(x % v, y % v, z % v);
        }



    };
}