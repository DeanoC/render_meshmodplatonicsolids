#pragma once

#include "al2o3_platform/platform.h"
#include "render_meshmod/meshmod.h"
#include "render_meshmod/registry.h"
#include "al2o3_cmath/aabb.h"

// all shapes have a polygon id element,
// this will survive triangulation for better original polygon colouring etc.

AL2O3_EXTERN_C MeshMod_MeshHandle MeshModShapes_TetrahedonCreate(MeshMod_RegistryHandle registry);
AL2O3_EXTERN_C MeshMod_MeshHandle MeshModShapes_CubeCreate(MeshMod_RegistryHandle registry);
AL2O3_EXTERN_C MeshMod_MeshHandle MeshModShapes_OctahedronCreate(MeshMod_RegistryHandle registry);
AL2O3_EXTERN_C MeshMod_MeshHandle MeshModShapes_IcosahedronCreate(MeshMod_RegistryHandle registry);
AL2O3_EXTERN_C MeshMod_MeshHandle MeshModShapes_DodecahedronCreate(MeshMod_RegistryHandle registry);

AL2O3_EXTERN_C MeshMod_MeshHandle MeshModShapes_DiamondCreate(MeshMod_RegistryHandle registry);
AL2O3_EXTERN_C MeshMod_MeshHandle MeshModShapes_AABB3FCreate(MeshMod_RegistryHandle registry, Math_Aabb3F aabb);
