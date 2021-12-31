// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"
#include "RTree.hpp"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class MultiNoiseBiomeSource3d {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_MULTINOISEBIOMESOURCE3D
public:
    class MultiNoiseBiomeSource3d& operator=(class MultiNoiseBiomeSource3d const&) = delete;
    MultiNoiseBiomeSource3d(class MultiNoiseBiomeSource3d const&) = delete;
    MultiNoiseBiomeSource3d() = delete;
#endif

public:
    /*0*/ virtual ~MultiNoiseBiomeSource3d();
    /*1*/ virtual void fillBiomes(class LevelChunk&, class ChunkLocalNoiseCache const&) const;
    /*2*/ virtual class BiomeArea getBiomeArea(class BoundingBox const&, unsigned int) const;
    /*3*/ virtual bool containsOnly(int, int, int, int, class gsl::span<int const, -1>) const;
    /*4*/ virtual class Biome const* getBiome(int, int, int) const;

protected:

private:
    MCAPI class Biome* _selectBestFittingBiome(struct TargetPoint const&, struct RTree::Hint*) const;

};