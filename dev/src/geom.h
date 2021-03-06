// Copyright 2014 Wouter van Oortmerssen. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#define PI 3.14159265f
#define RAD (PI/180.0f)

// vec: supports 2..4 components of any numerical type.

// Compile time unrolled loops for 2..4 components.
#define DOVEC(F)  { const int i = 0; F; \
                  { const int i = 1; F; if (N > 2) \
                  { const int i = 2; F; if (N > 3) \
                  { const int i = 3; F; } } } }

#define DOVECR(F) { vec<T,N> _t; DOVEC(_t.set(i, F)); return _t; }

union int2float { int i; float f; };
inline void default_debug_value(float  &a) { int2float nan; nan.i = 0x7F800001; a = nan.f; }
inline void default_debug_value(double &a) { int2float nan; nan.i = 0x7F800001; a = nan.f; }
inline void default_debug_value(int    &a) { a = 0x1BADCAFE; }
inline void default_debug_value(short  &a) { a = 0x1BAD; }
inline void default_debug_value(uchar  &a) { a = 0x1B; }

template<typename T, int C, int R> class matrix;

template<typename T, int N> class vec {
    protected:
    T c[N];

    public:
    enum { NUM_ELEMENTS = N };
    typedef T CTYPE;

    vec() {
        #ifdef _DEBUG
        DOVEC(default_debug_value(c[i]));
        #endif
    }

    explicit vec(T e)         { DOVEC(c[i] = e); }
    explicit vec(const T *v)  { DOVEC(c[i] = v[i]); }

    template<typename U> explicit vec(const vec<U,N> &v) { DOVEC(c[i] = (T)v[i]); }

    vec(T x, T y, T z, T w) { assert(N == 4); c[0] = x; c[1] = y; c[2] = z; c[3] = w; }
    vec(T x, T y, T z)      { assert(N == 3); c[0] = x; c[1] = y; c[2] = z; }
    vec(T x, T y)           { assert(N == 2); c[0] = x; c[1] = y; }

    const T *data()  const { return c; }
    const T *begin() const { return c; }
    const T *end()   const { return c + N; }

    T x() const {                return c[0]; }
    T y() const {                return c[1]; }
    T z() const { assert(N > 2); return c[2]; }
    T w() const { assert(N > 3); return c[3]; }

    T operator[](int i) const { return c[i]; }

    // No operator for this on purpose, you shouldn't be using this outside of this header
    // (trying to encourage a functional style).
    void set(int i, const T a) { c[i] = a; }
    vec &add(int i, const T a) { c[i] += a; return *this; }

    // Nasty name to discourage use :)
    T &x_mut() {                return c[0]; }
    T &y_mut() {                return c[1]; }
    T &z_mut() { assert(N > 2); return c[2]; }
    T &w_mut() { assert(N > 3); return c[3]; }

    vec(const vec<T,3> &v, T e) { DOVEC(c[i] = i < 3 ? v[i] : e); }
    vec(const vec<T,2> &v, T e) { DOVEC(c[i] = i < 2 ? v[i] : e); }

    vec<T,3> xyz() const { assert(N == 4); return vec<T,3>(c); }
    vec<T,2> xy()  const { assert(N == 3); return vec<T,2>(c); }

    vec operator+(const vec &v) const { DOVECR(c[i] + v[i]); }
    vec operator-(const vec &v) const { DOVECR(c[i] - v[i]); }
    vec operator*(const vec &v) const { DOVECR(c[i] * v[i]); }
    vec operator/(const vec &v) const { DOVECR(c[i] / v[i]); }

    vec operator+(T e) const { DOVECR(c[i] + e); }
    vec operator-(T e) const { DOVECR(c[i] - e); }
    vec operator*(T e) const { DOVECR(c[i] * e); }
    vec operator/(T e) const { DOVECR(c[i] / e); }
    vec operator%(T e) const { DOVECR(c[i] % e); }

    vec operator-() const { DOVECR(-c[i]); }

    vec &operator+=(const vec &v) { DOVEC(c[i] += v[i]); return *this; }
    vec &operator-=(const vec &v) { DOVEC(c[i] -= v[i]); return *this; }
    vec &operator*=(const vec &v) { DOVEC(c[i] *= v[i]); return *this; }
    vec &operator/=(const vec &v) { DOVEC(c[i] /= v[i]); return *this; }

    vec &operator+=(T e) { DOVEC(c[i] += e); return *this; }
    vec &operator-=(T e) { DOVEC(c[i] -= e); return *this; }
    vec &operator*=(T e) { DOVEC(c[i] *= e); return *this; }
    vec &operator/=(T e) { DOVEC(c[i] /= e); return *this; }

    bool operator<=(const vec &v) const {
        bool all = true; DOVEC(all = all && c[i] <= v[i]); return all;
    }
    bool operator< (const vec &v) const {
        bool all = true; DOVEC(all = all && c[i] <  v[i]); return all;
    }
    bool operator>=(const vec &v) const {
        bool all = true; DOVEC(all = all && c[i] >= v[i]); return all;
    }
    bool operator> (const vec &v) const {
        bool all = true; DOVEC(all = all && c[i] >  v[i]); return all;
    }
    bool operator==(const vec &v) const {
        bool all = true;  DOVEC(all = all && c[i] == v[i]); return all;
    }
    bool operator!=(const vec &v) const {
        bool any = false; DOVEC(any = any || c[i] != v[i]); return any;
    }

    bool operator<=(T e) const { bool all = true; DOVEC(all = all && c[i] <= e); return all; }
    bool operator< (T e) const { bool all = true; DOVEC(all = all && c[i] <  e); return all; }
    bool operator>=(T e) const { bool all = true; DOVEC(all = all && c[i] >= e); return all; }
    bool operator> (T e) const { bool all = true; DOVEC(all = all && c[i] >  e); return all; }

    bool operator==(T e) const { bool all = true;  DOVEC(all = all && c[i] == e); return all; }
    bool operator!=(T e) const { bool any = false; DOVEC(any = any || c[i] != e); return any; }

    vec iflt(T e, const vec &a, const vec &b) const { DOVECR(c[i] < e ? a[i] : b[i]); }

    string to_string() {
        string s = "(";
        DOVEC(if (i) s += ", "; s += std::to_string(c[i]));
        return s + ")";
    }

    template<typename T2, int C, int R> friend class matrix;
};


template<typename T> inline T mix(T a, T b, float f) { return a * (1 - f) + b * f; }

// Rational replacement for powf (when t = 0..1), due to
// "Ratioquadrics: An Alternative Model for Superquadrics"
//inline float rpowf(float t, float e)
//{
//    assert(t >= 0 && t <= 1); e = 1 / e + 1.85f/* wtf */; return t / (e + (1 - e) * t);
//}
inline float rpowf(float t, float e) { return expf(e * logf(t)); }

template<typename T, int N> inline vec<T,N> operator+(T f, const vec<T,N> &v) { DOVECR(f + v[i]); }
template<typename T, int N> inline vec<T,N> operator-(T f, const vec<T,N> &v) { DOVECR(f - v[i]); }
template<typename T, int N> inline vec<T,N> operator*(T f, const vec<T,N> &v) { DOVECR(f * v[i]); }
template<typename T, int N> inline vec<T,N> operator/(T f, const vec<T,N> &v) { DOVECR(f / v[i]); }

template<typename T, int N> inline T dot(const vec<T,N> &a, const vec<T,N> &b) {
    T t = 0; DOVEC(t += a[i] * b[i]);
    return t;
}
template<typename T, int N> inline T squaredlength(const vec<T,N> &v) { return dot(v, v); }
template<typename T, int N> inline T length(const vec<T,N> &v) { return sqrtf(squaredlength(v)); }
template<typename T, int N> inline vec<T,N> normalize(const vec<T,N> &v) { return v / length(v); }
template<typename T, int N> inline vec<T,N> abs(const vec<T,N> &v) { DOVECR(fabsf(v[i])); }
template<typename T, int N> inline vec<T,N> sign(const vec<T,N> &v) {
    DOVECR((T)(v[i] >= 0 ? 1 : -1));
}
template<typename T, int N> inline vec<T,N> mix(const vec<T,N> &a, const vec<T,N> &b, float f) {
    return a * (1 - f) + b * f;
}
template<typename T, int N> inline vec<T,N> min(const vec<T,N> &a, const vec<T,N> &b) {
    DOVECR(min(a[i], b[i]));
}
template<typename T, int N> inline vec<T,N> max(const vec<T,N> &a, const vec<T,N> &b) {
    DOVECR(max(a[i], b[i]));
}
template<typename T, int N> inline vec<T,N> pow(const vec<T,N> &a, const vec<T,N> &b) {
    DOVECR(powf(a[i], b[i]));
}
template<typename T, int N> inline vec<T,N> rpow(const vec<T,N> &a, const vec<T,N> &b) {
    DOVECR(rpowf(a[i], b[i]));
}

template<typename T, int N> inline vec<T,N> ceilf(const vec<T,N> &v) { DOVECR(ceilf(v[i])); }
template<typename T, int N> inline vec<T,N> floorf(const vec<T,N> &v) { DOVECR(floorf(v[i])); }

#undef DOVEC
#undef DOVECR

typedef vec<float,2> float2;
typedef vec<float,3> float3;
typedef vec<float,4> float4;

typedef vec<int,2> int2;
typedef vec<int,3> int3;
typedef vec<int,4> int4;

typedef vec<uchar,4> byte4;

const float4 float4_0 = float4(0.0f);
const float4 float4_1 = float4(1.0f);

const float3 float3_0 = float3(0.0f);
const float3 float3_1 = float3(1.0f);
const float3 float3_x = float3(1, 0, 0);
const float3 float3_y = float3(0, 1, 0);
const float3 float3_z = float3(0, 0, 1);

const float2 float2_0 = float2(0.0f);
const float2 float2_1 = float2(1.0f);

const int2 int2_0 = int2(0);
const int2 int2_1 = int2(1);

const int3 int3_0 = int3(0);
const int3 int3_1 = int3(1);

const byte4 byte4_0   = byte4((uchar)0);
const byte4 byte4_255 = byte4((uchar)255);

inline float3 cross(const float3 &a, const float3 &b)		 {
    return float3(a.y()*b.z()-a.z()*b.y(), a.z()*b.x()-a.x()*b.z(), a.x()*b.y()-a.y()*b.x());
}

template<typename T> inline float3 random_point_in_sphere(RandomNumberGenerator<T> &r) {
    for (;;) {
        const float3 p(r.rndfloatsigned(), r.rndfloatsigned(), r.rndfloatsigned());
        if (dot(p, p) < 1.f)
            return p;
    }
}

inline float3 rotateX(const float3 &v, const float2 &a) {
    return float3(v.x(), v.y() * a.x() - v.z() * a.y(), v.y() * a.y() + v.z() * a.x());
}
inline float3 rotateY(const float3 &v, const float2 &a) {
    return float3(v.x() * a.x() + v.z() * a.y(), v.y(), v.z() * a.x() - v.x() * a.y());
}
inline float3 rotateZ(const float3 &v, const float2 &a) {
    return float3(v.x() * a.x() - v.y() * a.y(), v.x() * a.y() + v.y() * a.x(), v.z());
}

inline float3 rotateX(const float3 &v, float a) { return rotateX(v, float2(cosf(a), sinf(a))); }
inline float3 rotateY(const float3 &v, float a) { return rotateY(v, float2(cosf(a), sinf(a))); }
inline float3 rotateZ(const float3 &v, float a) { return rotateZ(v, float2(cosf(a), sinf(a))); }

struct quat : float4 {
    quat() {}
    quat(float x, float y, float z, float w) : float4(x, y, z, w) {}
    quat(const float3 &v, float w)           : float4(v, w) {}
    quat(const float4 &v)                    : float4(v) {}
    quat(float angle, const float3 &axis) {
        float s = sinf(0.5f*angle);
        *this = quat(s * axis, cosf(0.5f * angle));
    }
    explicit quat(const float3 &v) : float4(v, -sqrtf(max(1.0f - squaredlength(v), 0.0f))) {}
    explicit quat(const float *v) : float4(v[0], v[1], v[2], v[3]) {}

    quat operator*(const quat &o) const {
        return quat(w()*o.x() + x()*o.w() + y()*o.z() - z()*o.y(),
                    w()*o.y() - x()*o.z() + y()*o.w() + z()*o.x(),
                    w()*o.z() + x()*o.y() - y()*o.x() + z()*o.w(),
                    w()*o.w() - x()*o.x() - y()*o.y() - z()*o.z());
    }

    quat operator-() const { return quat(-xyz(), w()); }

    void flip() { *this = quat(-(float4)*this); }

    float3 transform(const float3 &p) const {
        return p + cross(xyz(), cross(xyz(), p) + p * w()) * 2.0f;
    }
};

template<typename T, int C, int R> class matrix {
    typedef vec<T,R> V;
    V m[C];

    public:
    matrix() {}

    explicit matrix(T e) {
        for (int x = 0; x < C; x++)
            for (int y = 0; y < R; y++)
                m[x].c[y] = e * (T)(x == y);
    }

    explicit matrix(const V &v) {
        for (int x = 0; x < C; x++)
            for (int y = 0; y < R; y++)
                m[x].c[y] = x == y ? v[x] : 0;
    }

    explicit matrix(const T *mat_data) {
        memcpy(this, mat_data, sizeof(T) * R * C);
    }

    matrix(V x, V y, V z, V w) { assert(C == 4); m[0] = x; m[1] = y; m[2] = z; m[3] = w; }
    matrix(V x, V y, V z)      { assert(C == 3); m[0] = x; m[1] = y; m[2] = z; }
    matrix(V x, V y)           { assert(C == 2); m[0] = x; m[1] = y; }

    matrix(float a, const float3 &v) {
        assert(C >= 3);
        assert(R >= 3);

        *this = matrix(1);

        float s = sinf(a);
        float c = cosf(a);

        const float	t = 1.f - c;
        const float3 n = normalize(v);
        const float	x = n.x();
        const float	y = n.y();
        const float	z = n.z();

        m[0].c[0] = t*x*x + c;
        m[0].c[1] = t*x*y + z*s;
        m[0].c[2] = t*x*z - y*s;
        m[1].c[0] = t*x*y - z*s;
        m[1].c[1] = t*y*y + c;
        m[1].c[2] = t*y*z + x*s;
        m[2].c[0] = t*x*z + y*s;
        m[2].c[1] = t*y*z - x*s;
        m[2].c[2] = t*z*z + c;
    }

    const T *data()  const { return m[0].c; }
          T *data_mut()    { return m[0].c; }
    const T *begin() const { return m[0].c; }
    const T *end()   const { return m[C].c; }

    const V &operator[](int i) const { return m[i]; }

    operator const T *() const { return m[0].c; }

    // not an operator on purpose, don't use outside this header
    void set(int i, const V &v) { m[i] = v; }

    vec<T,C> row(int i) const {
        if (C == 2) return vec<T,C>(m[0][i], m[1][i]);
        if (C == 3) return vec<T,C>(m[0][i], m[1][i], m[2][i]);
        if (C == 4) return vec<T,C>(m[0][i], m[1][i], m[2][i], m[3][i]);
    }

    matrix<T,R,C> transpose() const {
        matrix<T,R,C> res;
        for (int y = 0; y < R; y++) res.set(y, row(y));
        return res;
    }

    V operator*(const vec<T,C> &v) const {
        V res(0.0f);
        for (int i = 0; i < C; i++) res += m[i] * v[i];
        return res;
    }

    matrix &operator*=(const matrix &o) { return *this = *this * o; }

    matrix operator*(const matrix<T,R,C> &o) const {
        matrix<T,R,C> t = transpose();
        matrix<T,R,R> res;
        for (int x = 0; x < R; x++)
            for (int y = 0; y < R; y++)
                res.m[x].c[y] = dot(t[y], o[x]);
        return res;
    }

    matrix operator*(T f) const {
        matrix res;
        for (int x = 0; x < C; x++)
            res.m[x] = m[x] * f;
        return res;
    }

    matrix operator+(const matrix &o) const {
        matrix res;
        for (int x = 0; x < C; x++)
            res.m[x] = m[x] + o.m[x];
        return res;
    }
};

template<typename T, int C, int R> inline vec<T,R> operator*(const vec<T,R> &v,
                                                             const matrix<T,C,R> &m) {
    vec<T,R> t;
    for (int i = 0; i < R; i++) t.set(i, dot(v, m[i]));
    return t;
}

typedef matrix<float,4,4> float4x4;
typedef matrix<float,3,3> float3x3;
typedef matrix<float,3,4> float3x4;
typedef matrix<float,4,3> float4x3;

const float4x4 float4x4_1 = float4x4(1);
const float3x3 float3x3_1 = float3x3(1);

inline float3x4 operator*(const float3x4 &m, const float3x4 &o) {  // FIXME: clean this up
    return float3x4(
        (o[0]*m[0].x() + o[1]*m[0].y() + o[2]*m[0].z()).add(3, m[0].w()),
        (o[0]*m[1].x() + o[1]*m[1].y() + o[2]*m[1].z()).add(3, m[1].w()),
        (o[0]*m[2].x() + o[1]*m[2].y() + o[2]*m[2].z()).add(3, m[2].w()));
}

inline float4x4 translation(const float3 &t) {
    return float4x4(
        float4(1, 0, 0, 0),
        float4(0, 1, 0, 0),
        float4(0, 0, 1, 0),
        float4(t, 1));
}

inline float4x4 scaling(float s) {
    return float4x4(
        float4(s, 0, 0, 0),
        float4(0, s, 0, 0),
        float4(0, 0, s, 0),
        float4(0, 0, 0, 1));
}

inline float4x4 scaling(const float3 &s) {
    return float4x4(
        float4(s.x(), 0, 0, 0),
        float4(0, s.y(), 0, 0),
        float4(0, 0, s.z(), 0),
        float4(0, 0, 0, 1));
}

inline float4x4 rotationX(const float2 &v) {
    return float4x4(
        float4(1, 0,     0,     0),
        float4(0, v.x(), v.y(), 0),
        float4(0,-v.y(), v.x(), 0),
        float4(0, 0,     0,     1));
}

inline float4x4 rotationY(const float2 &v) {
    return float4x4(
        float4(v.x(), 0,-v.y(), 0),
        float4(0,     1, 0,     0),
        float4(v.y(), 0, v.x(), 0),
        float4(0,     0, 0,     1));
}

inline float4x4 rotationZ(const float2 &v) {
    return float4x4(
        float4( v.x(), v.y(), 0, 0),
        float4(-v.y(), v.x(), 0, 0),
        float4( 0,     0,     1, 0),
        float4( 0,     0,     0, 1));
}

inline float4x4 rotation3D(const float3 &v) {
    return float4x4(
        float4( 0,     -v.z(),  v.y(), 0),
        float4( v.z(),  0,     -v.x(), 0),
        float4(-v.y(),  v.x(),  0,     0),
        float4( 0,      0,      0,     1));
}

inline float4x4 rotationX(float a) { return rotationX(float2(cosf(a), sinf(a))); }
inline float4x4 rotationY(float a) { return rotationY(float2(cosf(a), sinf(a))); }
inline float4x4 rotationZ(float a) { return rotationZ(float2(cosf(a), sinf(a))); }

inline quat quatfromtwovectors(const float3 &u, const float3 &v) {
    float norm_u_norm_v = sqrt(dot(u, u) * dot(v, v));
    float real_part = norm_u_norm_v + dot(u, v);
    float3 w;
    if (real_part < 1.e-6f * norm_u_norm_v) {
        // If u and v are exactly opposite, rotate 180 degrees
        // around an arbitrary orthogonal axis. Axis normalisation
        // can happen later, when we normalise the quaternion.
        real_part = 0.0f;
        w = fabsf(u.x()) > fabsf(u.z()) ? float3(-u.y(), u.x(), 0.f)
                                        : float3(0.f,   -u.z(), u.y());
    } else {
        // Otherwise, build quaternion the standard way.
        w = cross(u, v);
    }
    return normalize(quat(w, real_part));
}

inline float3x3 rotation(const quat &q) {
    float x = q.x(), y = q.y(), z = q.z(), w = q.w(),
              tx = 2*x, ty = 2*y, tz = 2*z,
              txx = tx*x, tyy = ty*y, tzz = tz*z,
              txy = tx*y, txz = tx*z, tyz = ty*z,
              twx = w*tx, twy = w*ty, twz = w*tz;
    return float3x3(float3(1 - (tyy + tzz), txy - twz, txz + twy),
                    float3(txy + twz, 1 - (txx + tzz), tyz - twx),
                    float3(txz - twy, tyz + twx, 1 - (txx + tyy)));
}

// FIXME: This is not generic, here because of IQM.
inline float3x4 rotationscaletrans(const quat &q, const float3 &s, const float3 &t) {
    float3x3 m = rotation(q);
    for (int i = 0; i < 3; i++) m.set(i, m[i] * s);
    return float3x4(float4(m[0], t.x()),
                    float4(m[1], t.y()),
                    float4(m[2], t.z()));
}

inline float4x4 float3x3to4x4(const float3x3 &m) {
    return float4x4(float4(m[0], 0),
                    float4(m[1], 0),
                    float4(m[2], 0),
                    float4(0, 0, 0, 1));
}

// FIXME: This is not generic, here because of IQM.
inline float3x4 invertortho(const float3x4 &o) {
    float4x3 inv = o.transpose();
    for (int i = 0; i < 3; i++) inv.set(i, inv[i] / squaredlength(inv[i]));
    return float3x4(float4(inv[0], -dot(inv[0], inv[3])),
                    float4(inv[1], -dot(inv[1], inv[3])),
                    float4(inv[2], -dot(inv[2], inv[3])));
}

inline float4x4 invert(const float4x4 &mat) {
    auto m = mat.data();
    float4x4 dest;
    auto inv = dest.data_mut();

    inv[0] =   m[5]  * m[10] * m[15] -
               m[5]  * m[11] * m[14] -
               m[9]  * m[6]  * m[15] +
               m[9]  * m[7]  * m[14] +
               m[13] * m[6]  * m[11] -
               m[13] * m[7]  * m[10];

    inv[4] =  -m[4]  * m[10] * m[15] +
               m[4]  * m[11] * m[14] +
               m[8]  * m[6]  * m[15] -
               m[8]  * m[7]  * m[14] -
               m[12] * m[6]  * m[11] +
               m[12] * m[7]  * m[10];

    inv[8] =   m[4]  * m[9]  * m[15] -
               m[4]  * m[11] * m[13] -
               m[8]  * m[5]  * m[15] +
               m[8]  * m[7]  * m[13] +
               m[12] * m[5]  * m[11] -
               m[12] * m[7]  * m[9];

    inv[12] = -m[4]  * m[9]  * m[14] +
               m[4]  * m[10] * m[13] +
               m[8]  * m[5]  * m[14] -
               m[8]  * m[6]  * m[13] -
               m[12] * m[5]  * m[10] +
               m[12] * m[6]  * m[9];

    inv[1] =  -m[1]  * m[10] * m[15] +
               m[1]  * m[11] * m[14] +
               m[9]  * m[2]  * m[15] -
               m[9]  * m[3]  * m[14] -
               m[13] * m[2]  * m[11] +
               m[13] * m[3]  * m[10];

    inv[5] =   m[0]  * m[10] * m[15] -
               m[0]  * m[11] * m[14] -
               m[8]  * m[2]  * m[15] +
               m[8]  * m[3]  * m[14] +
               m[12] * m[2]  * m[11] -
               m[12] * m[3]  * m[10];

    inv[9] =  -m[0]  * m[9]  * m[15] +
               m[0]  * m[11] * m[13] +
               m[8]  * m[1]  * m[15] -
               m[8]  * m[3]  * m[13] -
               m[12] * m[1]  * m[11] +
               m[12] * m[3]  * m[9];

    inv[13] =  m[0]  * m[9]  * m[14] -
               m[0]  * m[10] * m[13] -
               m[8]  * m[1]  * m[14] +
               m[8]  * m[2]  * m[13] +
               m[12] * m[1]  * m[10] -
               m[12] * m[2]  * m[9];

    inv[2] =   m[1]  * m[6]  * m[15] -
               m[1]  * m[7]  * m[14] -
               m[5]  * m[2]  * m[15] +
               m[5]  * m[3]  * m[14] +
               m[13] * m[2]  * m[7]  -
               m[13] * m[3]  * m[6];

    inv[6] =  -m[0]  * m[6]  * m[15] +
               m[0]  * m[7]  * m[14] +
               m[4]  * m[2]  * m[15] -
               m[4]  * m[3]  * m[14] -
               m[12] * m[2]  * m[7]  +
               m[12] * m[3]  * m[6];

    inv[10] =  m[0]  * m[5]  * m[15] -
               m[0]  * m[7]  * m[13] -
               m[4]  * m[1]  * m[15] +
               m[4]  * m[3]  * m[13] +
               m[12] * m[1]  * m[7]  -
               m[12] * m[3]  * m[5];

    inv[14] = -m[0]  * m[5]  * m[14] +
               m[0]  * m[6]  * m[13] +
               m[4]  * m[1]  * m[14] -
               m[4]  * m[2]  * m[13] -
               m[12] * m[1]  * m[6]  +
               m[12] * m[2]  * m[5];

    inv[3] =  -m[1]  * m[6]  * m[11] +
               m[1]  * m[7]  * m[10] +
               m[5]  * m[2]  * m[11] -
               m[5]  * m[3]  * m[10] -
               m[9]  * m[2]  * m[7]  +
               m[9]  * m[3]  * m[6];

    inv[7] =   m[0]  * m[6]  * m[11] -
               m[0]  * m[7]  * m[10] -
               m[4]  * m[2]  * m[11] +
               m[4]  * m[3]  * m[10] +
               m[8]  * m[2]  * m[7]  -
               m[8]  * m[3]  * m[6];

    inv[11] = -m[0]  * m[5]  * m[11] +
               m[0]  * m[7]  * m[9]  +
               m[4]  * m[1]  * m[11] -
               m[4]  * m[3]  * m[9] -
               m[8]  * m[1]  * m[7]  +
               m[8]  * m[3]  * m[5];

    inv[15] =  m[0]  * m[5]  * m[10] -
               m[0]  * m[6]  * m[9]  -
               m[4]  * m[1]  * m[10] +
               m[4]  * m[2]  * m[9]  +
               m[8]  * m[1]  * m[6]  -
               m[8]  * m[2]  * m[5];

    float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
    if (det == 0) {
        assert(false);
        return float4x4_1;
    }
    det = 1.0f / det;
    for (int i = 0; i < 16; i++)
        inv[i] = inv[i] * det;
    return dest;
}

// Handedness: 1.f for RH, -1.f for LH.
inline float4x4 perspective( float fovy, float aspect, float znear, float zfar, float handedness) {
    const float y = 1 / tanf(fovy * .5f);
    const float x = y / aspect;
    const float zdist = (znear - zfar) * handedness;
    const float zfar_per_zdist = zfar / zdist;
    return float4x4(
        float4(x, 0, 0,					   		             0),
        float4(0, y, 0,					   		             0),
        float4(0, 0, zfar_per_zdist,    	   	             -1.f * handedness),
        float4(0, 0, 2 * znear * zfar_per_zdist * handedness, 0));
}

inline float4x4 ortho(float left, float right, float bottom, float top, float znear, float zfar) {
    return float4x4(
        float4(2.0f / (right - left), 0, 0, 0),
        float4(0, 2.0f / (top - bottom), 0, 0),
        float4(0, 0, -2.0f / (zfar - znear), 0),
        float4(-(right + left) / (right - left),
               -(top + bottom) / (top - bottom),
               -(zfar + znear) / (zfar - znear),
               1.0f)
    );
}

inline byte4 quantizec(const float3 &v) { return byte4(float4(v, 1) * 255); }
inline byte4 quantizec(const float4 &v) { return byte4(v            * 255); }

inline float4 color2vec(byte4 &col) { return float4(col) / 255; }

// Spline interpolation.
inline float3 cardinalspline(const float3 &z, const float3 &a, const float3 &b, const float3 &c,
                             float s, float tension = 0.5) {
    float s2 = s*s;
    float s3 = s*s2;
    return a                 * ( 2*s3 - 3*s2 + 1) +
           b                 * (-2*s3 + 3*s2    ) +
           (b - z) * tension * (   s3 - 2*s2 + s) +
           (c - a) * tension * (   s3 -   s2    );
}

inline bool line_intersect(const float2 &l1a, const float2 &l1b, const float2 &l2a,
                           const float2 &l2b, float2 *out = nullptr) {
    float2 a(l1b - l1a);
    float2 b(l2b - l2a);
    float2 aperp(-a.y(), a.x());
    auto f = dot(aperp, b);
    if (!f) return false;     // Parallel.
    float2 c(l2b - l1b);
    float2 bperp(-b.y(), b.x());
    auto aa = dot(aperp, c);
    auto bb = dot(bperp, c);
    if(f < 0) {
        if(aa > 0 || bb > 0 || aa < f || bb < f)     return false;
    } else {
        if(aa < 0 || bb < 0 || aa > f || bb > f)     return false;
    }
    if(out) {
        auto lerp = 1.0f - (aa / f);
        *out = ((l2b - l2a) * lerp) + l2a;
    }
    return true;
}

inline void normalize_mesh(int *idxs, size_t idxlen, void *verts, size_t vertlen, size_t vsize,
                           size_t normaloffset, bool ignore_bad_tris = true) {
    for (size_t i = 0; i < vertlen; i++) {
        *(float3 *)((uchar *)verts + i * vsize + normaloffset) = float3_0;
    }
    for (size_t t = 0; t < idxlen; t += 3) {
        int v1i = idxs[t + 0];
        int v2i = idxs[t + 1];
        int v3i = idxs[t + 2];
        float3 &v1p = *(float3 *)((uchar *)verts + v1i * vsize);
        float3 &v2p = *(float3 *)((uchar *)verts + v2i * vsize);
        float3 &v3p = *(float3 *)((uchar *)verts + v3i * vsize);
        float3 &v1n = *(float3 *)((uchar *)verts + v1i * vsize + normaloffset);
        float3 &v2n = *(float3 *)((uchar *)verts + v2i * vsize + normaloffset);
        float3 &v3n = *(float3 *)((uchar *)verts + v3i * vsize + normaloffset);
        if (v1p != v2p && v1p != v3p && v2p != v3p) {
            float3 v12 = normalize(v2p - v1p);
            float3 v13 = normalize(v3p - v1p);
            float3 v23 = normalize(v3p - v2p);
            float3 d3  = normalize(cross(v13, v12));
            v1n += d3 * (1 - dot( v12, v13));
            v2n += d3 * (1 - dot(-v12, v23));
            v3n += d3 * (1 - dot(-v23,-v13));
        } else {
            assert(ignore_bad_tris);
            (void)ignore_bad_tris;
        }
    }
    for (size_t i = 0; i < vertlen; i++) {
        float3 &norm = *(float3 *)((uchar *)verts + i * vsize + normaloffset);
        if (norm != float3_0)
            norm = normalize(norm);
    }
}
