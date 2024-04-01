// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGen/DensityFunctionUtil.h"

float UDensityFunctionUtil::GetDensity(DFType dFType, FVector3f pos, int detail, bool debug)
{
    FVector3d posDouble = FVector3d(pos.X, pos.Y, pos.Z);
    //FVector3d posDouble = FVector3d((double)pos.X, (double)pos.Y, (double)pos.Z);
    //float d = 0.0f;
    //FVector3f center = FVector3f(3.0f, 3.0f, 3.0f);
    //float radius = 700;
    //d = (pos.X - center.X) * (pos.X - center.X)
    //    + (pos.Y - center.Y) * (pos.Y - center.Y)
    //    + (pos.Z - 3) * (pos.Z - center.Z) - radius * radius;
    //if (d == 0.0f)
    //d += .0001f;

    //return d;

    return GetDensity(dFType, posDouble, detail, debug);
}

float UDensityFunctionUtil::GetDensity(DFType dFType, FVector3d pos, int detail, bool debug)
{
    float d = 0.0f;
    if (dFType == DFType::simplex)
    {
        float total = 0.0f;
        int octaves = 1;// 5; // How many layers of noise, Highr number means more detail but takes more time
        if (detail == 1)
            octaves = 2;
        else if (detail >= 2)
            octaves = 3;

        float frequency = .00066f;//.0005f; // How Fast Noise Changes, Higher number means more changes more quickly
        float amplitude = 200.0f; // Strength of the Noise, Higher numbers means bigger features
        float amplitudeSum = 0.0f; // Tracks the total ampitude used to ensure we do not go over max
        float lacunarity = 2.0f; // How quickly the features get more detailed, Higher numbers means more detail more quickly, 2 or 4 are the reccomended values
        float gain = .25f; // How quickly ampitude decreases, lower numbers means more detail features are smaller
        float xRandShift = 0.0f;
        float yRandShift = 0.0f;
        float zRandShift = 0.0f;
        float xd = pos.X + xRandShift;
        float yd = pos.Y + yRandShift;
        float zd = pos.Z + zRandShift;

        if (debug)
            UE_LOG(LogTemp, Warning, TEXT("(%f,%f,%f)"), xd, yd, zd);

        for (int i = 0; i < octaves; ++i)
        {
            //float nNoise = Mathf.Lerp(0, 1, (float)sNoise.noise((float)xd * frequency, (float)zd * frequency));
            //float nNoise = ((float)sNoise.noise((float)xd * frequency, (float)zd * frequency) + 1.0f) / 2.0f;
            if (debug)
            {
                UE_LOG(LogTemp, Warning,
                    TEXT("::(%f,%f,%f)->frequency:%f->(%f,%f,%f)"),
                    xd, yd, zd, frequency,
                    xd * frequency, yd * frequency, zd);
            }
            float nNoise = SimplexNoise2D(xd * frequency, yd * frequency);
            total += nNoise * amplitude;
            if (debug)
            {
                UE_LOG(LogTemp, Warning,
                    TEXT("::::noiseValue:%f, nNoise:%f, amplitude:%f, total:%f"),
                    SimplexNoise2D(xd * frequency, yd * frequency), nNoise, amplitude, total);
            }
            //amplitudeSum += amplitude;
            frequency *= lacunarity;
            amplitude *= gain;
            if (debug)
            {
                UE_LOG(LogTemp, Warning, 
                    TEXT("::::lacunarity:%f, frequency:%f, gain:%f, amplitude:%f"),
                    lacunarity, frequency, gain, amplitude);
            }
        }
        //total = total / amplitudeSum;
        //float noise = total - pos.Z;
        float noise = total + 2000.f - pos.Z;
        //d = noise * -1 - 118f; //30 tall
        d = noise * -1.0f;// - 200f; //100 tall
        //d = noise * -1 - 400f; //100 tall
    }
    else if (dFType == DFType::sine)
    {
        d = pos.Y - (118.0f + 40.0f * UKismetMathLibrary::Sin(pos.X * .05f));
    }
    else if (dFType == DFType::yEx)
    {
        d = pos.Y - pos.X + .2f;
    }
    else if (dFType == DFType::flat)
    {
        //Z Crosss
        d = pos.Z - 50.0f; //Ground
        //d = 155.0f - pos.Z; //Roof

        //Y Crosss
        //d = pos.Y - 5.0f; //Wall
        //d = 155.0f - pos.Y; //Wall

        //X Crosss
        //d = pos.X - 5.0f; //Wall
        //d = 155.0f - pos.X; //Wall
    }
    else if (dFType == DFType::negative)
    {
        //d = pos.Z - 1500.0f; //Ground
        d = -1.f;
    }
    else if (dFType == DFType::wall)
    {
        if (pos.X <= 150.0f)
            d = 150.0f - pos.X;
        //else if (pos.x <= 160)
            //d = -1f;
        else if (pos.X <= 170.0f)
            d = pos.X - 170.0f;

        //d = pos.x - 270f;
        //d = 233f - pos.x;
    }
    else if (dFType == DFType::circle)
    {
        FVector3f center = FVector3f(3.0f, 3.0f, 3.0f);
        float radius = 700;
        d = (pos.X - center.X) * (pos.X - center.X) 
            + (pos.Y - center.Y) * (pos.Y - center.Y) 
            + (pos.Z - 3) * (pos.Z - center.Z) - radius * radius;
        //r^2 = (x - a)^2 + (y - b)^2 + (z - c)^2
        //d = (x - a)^2 + (y - b)^2 + (z - c)^2 - r^2
    }
    else if (dFType == DFType::parabaloid)
    {
        FVector3f base = FVector3f(11000.0f, 11000.0f, 0.0f);
        float height = 18000.f;
        FVector3f peak = FVector3f(base.X, base.Y, height);
        float xRadius = 7000.f;
        float yRadius = 8000.f;
        float downDirection = -1.f;
        
        //z = (x^2 / a^2) + (y^2 / b^2)
        //(z / steepness) - peak.z = ((x-peak.x)^2 / radiusX^2) + ((y-peak.y)^2 / radiusY^2)

        d = ((((pos.X - peak.X) * (pos.X - peak.X)) / (xRadius * xRadius))
            + (((pos.Y - peak.Y) * (pos.Y - peak.Y)) / (yRadius * yRadius))
            - (((pos.Z - peak.Z) / (height * downDirection))));
    }
    else if (dFType == DFType::parabaloidwithnoise)
    {
        FVector3f base = FVector3f(11000.0f, 11000.0f, 0.0f);
        float height = 4500.f; 
        FVector3f peak = FVector3f(base.X, base.Y, height);
        float xRadius = 7000.f;
        float yRadius = 8000.f;
        float downDirection = -1.f;

        float noiseFactor = MOSimplexNoise2D(pos, detail) * 15.f;// +pos.Z;

        //z = (x^2 / a^2) + (y^2 / b^2)
        //(z / steepness) - peak.z = ((x-peak.x)^2 / radiusX^2) + ((y-peak.y)^2 / radiusY^2)
        float d_parabola = ((((pos.X - peak.X) * (pos.X - peak.X)) / (xRadius * xRadius))
            + (((pos.Y - peak.Y) * (pos.Y - peak.Y)) / (yRadius * yRadius))
            - (((pos.Z + noiseFactor - peak.Z) / (height * downDirection))));

        d = d_parabola;
    }

    if (d == 0.0f)
        d += .0001f;

    return d;
}

//Simplex Noise Helper Data/Functions
static inline int32_t fastfloor(float fp) {
    int32_t i = static_cast<int32_t>(fp);
    return (fp < i) ? (i - 1) : (i);
}

static const uint8_t perm[256] = {
    151, 160, 137, 91, 90, 15,
    131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
    190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
    77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
    102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
    135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
    5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
    223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
    129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
    251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
    49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
    138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};

static inline uint8_t hash(int32_t i) {
    return perm[static_cast<uint8_t>(i)];
}

static float grad(int32_t hash, float x, float y) {
    const int32_t h = hash & 0x3F;  // Convert low 3 bits of hash code
    const float u = h < 4 ? x : y;  // into 8 simple gradient directions,
    const float v = h < 4 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v); // and compute the dot product with (x,y).
}

static float grad(int32_t hash, float x, float y, float z) {
    int h = hash & 15;     // Convert low 4 bits of hash code into 12 simple
    float u = h < 8 ? x : y; // gradient directions, and compute dot product.
    float v = h < 4 ? y : h == 12 || h == 14 ? x : z; // Fix repeats at h = 12 to 15
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

//Simplex Noise
float UDensityFunctionUtil::MOSimplexNoise2D(FVector3d pos, int detail)
{
    float total = 0.0f;
    int octaves = 1;// 5; // How many layers of noise, Highr number means more detail but takes more time
    if (detail == 1)
        octaves = 2;
    else if (detail >= 2)
        octaves = 3;

    float frequency = .00066f;//.0005f; // How Fast Noise Changes, Higher number means more changes more quickly
    float amplitude = 20.0f; // Strength of the Noise, Higher numbers means bigger features
    float amplitudeSum = 0.0f; // Tracks the total ampitude used to ensure we do not go over max
    float lacunarity = 2.0f; // How quickly the features get more detailed, Higher numbers means more detail more quickly, 2 or 4 are the reccomended values
    float gain = .25f; // How quickly ampitude decreases, lower numbers means more detail features are smaller
    float xRandShift = 0.0f;
    float yRandShift = 0.0f;
    float zRandShift = 0.0f;
    float xd = pos.X + xRandShift;
    float yd = pos.Y + yRandShift;
    float zd = pos.Z + zRandShift;

    for (int i = 0; i < octaves; ++i)
    {
        float nNoise = SimplexNoise2D(xd * frequency, yd * frequency);
        total += nNoise * amplitude;

        //amplitudeSum += amplitude;
        frequency *= lacunarity;
        amplitude *= gain;
    }
    //total = total / amplitudeSum;
    //float noise = total - pos.Z;
    float noise = total;
    return noise;
}

float UDensityFunctionUtil::SimplexNoise2D(float x, float y) {
    float n0, n1, n2;   // Noise contributions from the three corners

    // Skewing/Unskewing factors for 2D
    static const float F2 = 0.366025403f;  // F2 = (sqrt(3) - 1) / 2
    static const float G2 = 0.211324865f;  // G2 = (3 - sqrt(3)) / 6   = F2 / (1 + 2 * K)

    // Skew the input space to determine which simplex cell we're in
    const float s = (x + y) * F2;  // Hairy factor for 2D
    const float xs = x + s;
    const float ys = y + s;
    const int32_t i = fastfloor(xs);
    const int32_t j = fastfloor(ys);

    // Unskew the cell origin back to (x,y) space
    const float t = static_cast<float>(i + j) * G2;
    const float X0 = i - t;
    const float Y0 = j - t;
    const float x0 = x - X0;  // The x,y distances from the cell origin
    const float y0 = y - Y0;

    // For the 2D case, the simplex shape is an equilateral triangle.
    // Determine which simplex we are in.
    int32_t i1, j1;  // Offsets for second (middle) corner of simplex in (i,j) coords
    if (x0 > y0) {   // lower triangle, XY order: (0,0)->(1,0)->(1,1)
        i1 = 1;
        j1 = 0;
    }
    else {   // upper triangle, YX order: (0,0)->(0,1)->(1,1)
        i1 = 0;
        j1 = 1;
    }

    // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
    // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
    // c = (3-sqrt(3))/6

    const float x1 = x0 - i1 + G2;            // Offsets for middle corner in (x,y) unskewed coords
    const float y1 = y0 - j1 + G2;
    const float x2 = x0 - 1.0f + 2.0f * G2;   // Offsets for last corner in (x,y) unskewed coords
    const float y2 = y0 - 1.0f + 2.0f * G2;

    // Work out the hashed gradient indices of the three simplex corners
    const int gi0 = hash(i + hash(j));
    const int gi1 = hash(i + i1 + hash(j + j1));
    const int gi2 = hash(i + 1 + hash(j + 1));

    // Calculate the contribution from the first corner
    float t0 = 0.5f - x0 * x0 - y0 * y0;
    if (t0 < 0.0f) {
        n0 = 0.0f;
    }
    else {
        t0 *= t0;
        n0 = t0 * t0 * grad(gi0, x0, y0);
    }

    // Calculate the contribution from the second corner
    float t1 = 0.5f - x1 * x1 - y1 * y1;
    if (t1 < 0.0f) {
        n1 = 0.0f;
    }
    else {
        t1 *= t1;
        n1 = t1 * t1 * grad(gi1, x1, y1);
    }

    // Calculate the contribution from the third corner
    float t2 = 0.5f - x2 * x2 - y2 * y2;
    if (t2 < 0.0f) {
        n2 = 0.0f;
    }
    else {
        t2 *= t2;
        n2 = t2 * t2 * grad(gi2, x2, y2);
    }

    // Add contributions from each corner to get the final noise value.
    // The result is scaled to return values in the interval [-1,1].
    return 45.23065f * (n0 + n1 + n2);
}

float UDensityFunctionUtil::SimplexNoise3D(float x, float y, float z) {
    float n0, n1, n2, n3; // Noise contributions from the four corners

    // Skewing/Unskewing factors for 3D
    static const float F3 = 1.0f / 3.0f;
    static const float G3 = 1.0f / 6.0f;

    // Skew the input space to determine which simplex cell we're in
    float s = (x + y + z) * F3; // Very nice and simple skew factor for 3D
    int i = fastfloor(x + s);
    int j = fastfloor(y + s);
    int k = fastfloor(z + s);
    float t = (i + j + k) * G3;
    float X0 = i - t; // Unskew the cell origin back to (x,y,z) space
    float Y0 = j - t;
    float Z0 = k - t;
    float x0 = x - X0; // The x,y,z distances from the cell origin
    float y0 = y - Y0;
    float z0 = z - Z0;

    // For the 3D case, the simplex shape is a slightly irregular tetrahedron.
    // Determine which simplex we are in.
    int i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
    int i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords
    if (x0 >= y0) {
        if (y0 >= z0) {
            i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0; // X Y Z order
        }
        else if (x0 >= z0) {
            i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1; // X Z Y order
        }
        else {
            i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1; // Z X Y order
        }
    }
    else { // x0<y0
        if (y0 < z0) {
            i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1; // Z Y X order
        }
        else if (x0 < z0) {
            i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1; // Y Z X order
        }
        else {
            i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0; // Y X Z order
        }
    }

    // A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
    // a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
    // a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
    // c = 1/6.
    float x1 = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
    float y1 = y0 - j1 + G3;
    float z1 = z0 - k1 + G3;
    float x2 = x0 - i2 + 2.0f * G3; // Offsets for third corner in (x,y,z) coords
    float y2 = y0 - j2 + 2.0f * G3;
    float z2 = z0 - k2 + 2.0f * G3;
    float x3 = x0 - 1.0f + 3.0f * G3; // Offsets for last corner in (x,y,z) coords
    float y3 = y0 - 1.0f + 3.0f * G3;
    float z3 = z0 - 1.0f + 3.0f * G3;

    // Work out the hashed gradient indices of the four simplex corners
    int gi0 = hash(i + hash(j + hash(k)));
    int gi1 = hash(i + i1 + hash(j + j1 + hash(k + k1)));
    int gi2 = hash(i + i2 + hash(j + j2 + hash(k + k2)));
    int gi3 = hash(i + 1 + hash(j + 1 + hash(k + 1)));

    // Calculate the contribution from the four corners
    float t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0;
    if (t0 < 0) {
        n0 = 0.0;
    }
    else {
        t0 *= t0;
        n0 = t0 * t0 * grad(gi0, x0, y0, z0);
    }
    float t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1;
    if (t1 < 0) {
        n1 = 0.0;
    }
    else {
        t1 *= t1;
        n1 = t1 * t1 * grad(gi1, x1, y1, z1);
    }
    float t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2;
    if (t2 < 0) {
        n2 = 0.0;
    }
    else {
        t2 *= t2;
        n2 = t2 * t2 * grad(gi2, x2, y2, z2);
    }
    float t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3;
    if (t3 < 0) {
        n3 = 0.0;
    }
    else {
        t3 *= t3;
        n3 = t3 * t3 * grad(gi3, x3, y3, z3);
    }
    // Add contributions from each corner to get the final noise value.
    // The result is scaled to stay just inside [-1,1]
    return 32.0f * (n0 + n1 + n2 + n3);
}


