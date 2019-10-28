#include "al2o3_platform/platform.h"
#include "al2o3_memory/memory.h"
#include "render_meshmod/meshmod.h"
#include "render_meshmod/mesh.h"
#include "render_meshmod/vertex/position.h"
#include "render_meshmod/vertex/normal.h"
#include "render_meshmod/edge/halfedge.h"
#include "render_meshmod/polygon/tribrep.h"
#include "render_meshmod/polygon/quadbrep.h"
#include "render_meshmodshapes/shapes.h"

static Math_Vec3F CalcNormal(Math_Vec3F const v0, Math_Vec3F v1, Math_Vec3F v2) {
	Math_Vec3F e0 = Math_NormaliseVec3F(Math_SubVec3F(v1, v0));
	Math_Vec3F e1 = Math_NormaliseVec3F(Math_SubVec3F(v2, v0));
	Math_Vec3F n = Math_CrossVec3F(e0, e1);
	return n;
}

AL2O3_EXTERN_C MeshMod_MeshHandle MeshModShapes_CubeCreate(MeshMod_RegistryHandle registry) {

	MeshMod_MeshHandle mesh = MeshMod_MeshCreate(registry, "Cube");

	static const uint32_t NumVertices = 8;
	static const uint32_t NumFaces = 6;
	static const float pos[NumVertices * 3] = {
			-1,  1, -1,
			-1, -1, -1,
			1, -1, -1,
			1,  1, -1,

			-1,  1,  1,
			-1, -1,  1,
			1, -1,  1,
			1,  1,  1,
	};

	uint32_t const faces[NumFaces * 4] = {
			0, 1, 2, 3,
			7, 6, 5, 4,
			4, 0, 3, 7,
			5, 6, 2, 1,
			5, 1, 0, 4,
			2, 6, 7, 3
	};

	MeshMod_MeshVertexTagEnsure(mesh, MeshMod_VertexPositionTag);
	MeshMod_MeshVertexTagEnsure(mesh, MeshMod_VertexNormalTag);
	MeshMod_MeshEdgeTagEnsure(mesh, MeshMod_EdgeHalfEdgeTag);
	MeshMod_MeshPolygonTagEnsure(mesh, MeshMod_PolygonQuadBRepTag);

	for (uint32_t faceIndex = 0u; faceIndex < NumFaces; ++faceIndex) {

		Math_Vec3F v[4];
		MeshMod_VertexHandle vh[4];
		MeshMod_EdgeHandle eh[4];

		for(uint32_t i = 0u; i < 4; ++i) {
			// allocate vertex and edges from mesh mod
			vh[i] = MeshMod_MeshVertexAlloc(mesh);
			eh[i] = MeshMod_MeshEdgeAlloc(mesh);

			// deindex and copy vertex data
			uint32_t const vertIndex = (faceIndex*4) + i;
			v[i] = Math_FromVec3F(pos + (vertIndex *3));
		}
		// per face data
		Math_Vec3F normal = CalcNormal(v[0], v[1], v[2]);
		MeshMod_PolygonHandle quadHandle = MeshMod_MeshPolygonAlloc(mesh);
		MeshMod_PolygonQuadBRep* brep = (MeshMod_PolygonQuadBRep*) MeshMod_MeshPolygonTagHandleToPtr(mesh,
																																																 MeshMod_PolygonQuadBRepTag,
																																																 quadHandle);

		for(uint32_t i = 0u; i < 4; ++i) {
			Math_Vec3F* positionData = (Math_Vec3F*) MeshMod_MeshVertexTagHandleToPtr(mesh, MeshMod_VertexPositionTag, vh[i]);
			Math_Vec3F* normalData = (Math_Vec3F*) MeshMod_MeshVertexTagHandleToPtr(mesh, MeshMod_VertexNormalTag, vh[i]);
			memcpy( positionData, v + i, sizeof(Math_Vec3F));
			memcpy( normalData, &normal, sizeof(Math_Vec3F));

			MeshMod_EdgeHalfEdge* halfEdge = (MeshMod_EdgeHalfEdge*) MeshMod_MeshEdgeTagHandleToPtr(mesh,
																																															MeshMod_EdgeHalfEdgeTag,
																																															eh[i]);

			halfEdge->vertex = vh[i];
			halfEdge->polygon = quadHandle;
			brep->edge[i] = eh[i];
		}

	}

	return mesh;
}
