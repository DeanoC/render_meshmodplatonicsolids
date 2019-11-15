#include "al2o3_platform/platform.h"
#include "al2o3_memory/memory.h"
#include "render_meshmod/meshmod.h"
#include "render_meshmod/mesh.h"
#include "render_meshmod/vertex/position.h"
#include "render_meshmod/vertex/normal.h"
#include "render_meshmod/edge/halfedge.h"
#include "render_meshmod/polygon/tribrep.h"
#include "render_meshmod/polygon/quadbrep.h"
#include "render_meshmod/polygon/convexbrep.h"
#include "render_meshmod/polygon/basicdata.h"
#include "render_meshmodshapes/shapes.h"

static Math_Vec3F CalcNormal(Math_Vec3F const v0, Math_Vec3F v1, Math_Vec3F v2) {
	Math_Vec3F e0 = Math_SubVec3F(v1, v0);
	Math_Vec3F e1 = Math_SubVec3F(v2, v0);
	Math_Vec3F nn = Math_CrossVec3F(e0, e1);
	Math_Vec3F n = Math_NormaliseVec3F(nn);
	return n;
}

AL2O3_EXTERN_C MeshMod_MeshHandle MeshModShapes_TetrahedonCreate(MeshMod_RegistryHandle registry) {

	MeshMod_MeshHandle mesh = MeshMod_MeshCreate(registry, "Tetrahedron");

	static const uint32_t NumVertices = 4;
	static const uint32_t NumFaces = 4;
	float const pos[NumVertices * 3] = {
			-1, -1,  1,
			 1,  1,  1,
			 1, -1,  1,
			 1,  1, -1,
	};
	uint32_t const faces[NumFaces * 3] = {
			0, 1, 2,
			1, 3, 2,
			0, 2, 3,
			0, 3, 1,
	};
	MeshMod_MeshVertexTagEnsure(mesh, MeshMod_VertexPositionTag);
	MeshMod_MeshVertexTagEnsure(mesh, MeshMod_VertexNormalTag);
	MeshMod_MeshEdgeTagEnsure(mesh, MeshMod_EdgeHalfEdgeTag);
	MeshMod_MeshPolygonTagEnsure(mesh, MeshMod_PolygonTriBRepTag);
	MeshMod_MeshPolygonTagEnsure(mesh, MeshMod_PolygonIdTag);

	for (uint32_t faceIndex = 0u; faceIndex < NumFaces; ++faceIndex) {

		Math_Vec3F v[3];
		MeshMod_VertexHandle vh[3];
		MeshMod_EdgeHandle eh[3];

		for(uint32_t i = 0u; i < 3; ++i) {
			// allocate vertex and edges from mesh mod
			vh[i] = MeshMod_MeshVertexAlloc(mesh);
			eh[i] = MeshMod_MeshEdgeAlloc(mesh);

			// deindex and copy vertex data
			uint32_t const vertIndex = faces[(faceIndex*3) +i ];
			v[i] = Math_FromVec3F(pos + (vertIndex *3));
			v[i] = Math_ScalarMulVec3F(v[i], 0.5f);
		}
		// per face data
		Math_Vec3F normal = CalcNormal(v[0], v[1], v[2]);
		MeshMod_PolygonHandle triHandle = MeshMod_MeshPolygonAlloc(mesh);
		MeshMod_PolygonTriBRep* brep = MeshMod_MeshPolygonTriBRepTagHandleToPtr(mesh, triHandle, 0);
		*MeshMod_MeshPolygonU32TagHandleToPtr(mesh, triHandle, MeshMod_PolygonIdUserTag) = faceIndex;


		for(uint32_t i = 0u; i < 3; ++i) {
			Math_Vec3F* positionData = (Math_Vec3F*) MeshMod_MeshVertexPositionTagHandleToPtr(mesh,vh[i], 0);
			Math_Vec3F* normalData = (Math_Vec3F*) MeshMod_MeshVertexNormalTagHandleToPtr(mesh, vh[i], 0);
			memcpy( positionData, v + i, sizeof(Math_Vec3F));
			memcpy( normalData, &normal, sizeof(Math_Vec3F));

			MeshMod_EdgeHalfEdge* halfEdge = MeshMod_MeshEdgeHalfEdgeTagHandleToPtr(mesh, eh[i], 0);
			halfEdge->vertex = vh[i];
			halfEdge->polygon = triHandle;
			brep->edge[i] = eh[i];
		}

	}

	return mesh;
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
	MeshMod_MeshPolygonTagEnsure(mesh, MeshMod_PolygonIdTag);

	for (uint32_t faceIndex = 0u; faceIndex < NumFaces; ++faceIndex) {

		Math_Vec3F v[4];
		MeshMod_VertexHandle vh[4];
		MeshMod_EdgeHandle eh[4];

		for(uint32_t i = 0u; i < 4; ++i) {
			// allocate vertex and edges from mesh mod
			vh[i] = MeshMod_MeshVertexAlloc(mesh);
			eh[i] = MeshMod_MeshEdgeAlloc(mesh);

			// deindex and copy vertex data
			uint32_t const vertIndex = faces[(faceIndex*4) +i ];
			v[i] = Math_FromVec3F(pos + (vertIndex *3));
			v[i] = Math_ScalarMulVec3F(v[i], 0.5f);
		}
		// per face data
		Math_Vec3F normal = CalcNormal(v[0], v[1], v[2]);
		MeshMod_PolygonHandle quadHandle = MeshMod_MeshPolygonAlloc(mesh);
		MeshMod_PolygonQuadBRep* brep = MeshMod_MeshPolygonQuadBRepTagHandleToPtr(mesh, quadHandle, 0);
		*MeshMod_MeshPolygonU32TagHandleToPtr(mesh, quadHandle, MeshMod_PolygonIdUserTag) = faceIndex;

		for(uint32_t i = 0u; i < 4; ++i) {
			Math_Vec3F* positionData = (Math_Vec3F*) MeshMod_MeshVertexPositionTagHandleToPtr(mesh, vh[i], 0);
			Math_Vec3F* normalData = (Math_Vec3F*) MeshMod_MeshVertexNormalTagHandleToPtr(mesh, vh[i], 0);
			memcpy( positionData, v + i, sizeof(Math_Vec3F));
			memcpy( normalData, &normal, sizeof(Math_Vec3F));

			MeshMod_EdgeHalfEdge* halfEdge = MeshMod_MeshEdgeHalfEdgeTagHandleToPtr(mesh, eh[i], 0);
			halfEdge->vertex = vh[i];
			halfEdge->polygon = quadHandle;
			brep->edge[i] = eh[i];
		}

	}

	return mesh;
}

AL2O3_EXTERN_C MeshMod_MeshHandle MeshModShapes_OctahedronCreate(MeshMod_RegistryHandle registry) {

	MeshMod_MeshHandle mesh = MeshMod_MeshCreate(registry, "Octahedron");

	static const uint32_t NumVertices = 6;
	static const uint32_t NumFaces = 8;
	float const pos[NumVertices * 3] = {
			-1,  0,  0,
			1,  0,  0,
			0, -1,  0,
			0,  1,  0,
			0,  0, -1,
			0,  0,  1,
	};
	uint32_t const faces[NumFaces * 3] = {
			0, 3, 5,
			0, 5, 2,
			4, 3, 0,
			4, 0, 2,
			5, 3, 1,
			5, 1, 2,
			4, 1, 3,
			4, 2, 1,
	};
	MeshMod_MeshVertexTagEnsure(mesh, MeshMod_VertexPositionTag);
	MeshMod_MeshVertexTagEnsure(mesh, MeshMod_VertexNormalTag);
	MeshMod_MeshEdgeTagEnsure(mesh, MeshMod_EdgeHalfEdgeTag);
	MeshMod_MeshPolygonTagEnsure(mesh, MeshMod_PolygonTriBRepTag);
	MeshMod_MeshPolygonTagEnsure(mesh, MeshMod_PolygonIdTag);

	for (uint32_t faceIndex = 0u; faceIndex < NumFaces; ++faceIndex) {

		Math_Vec3F v[3];
		MeshMod_VertexHandle vh[3];
		MeshMod_EdgeHandle eh[3];

		for(uint32_t i = 0u; i < 3; ++i) {
			// allocate vertex and edges from mesh mod
			vh[i] = MeshMod_MeshVertexAlloc(mesh);
			eh[i] = MeshMod_MeshEdgeAlloc(mesh);

			// deindex and copy vertex data
			uint32_t const vertIndex = faces[(faceIndex*3) +i ];
			v[i] = Math_FromVec3F(pos + (vertIndex *3));
			v[i] = Math_ScalarMulVec3F(v[i], 0.5f);
		}
		// per face data
		Math_Vec3F normal = CalcNormal(v[0], v[1], v[2]);
		MeshMod_PolygonHandle triHandle = MeshMod_MeshPolygonAlloc(mesh);
		MeshMod_PolygonTriBRep* brep = MeshMod_MeshPolygonTriBRepTagHandleToPtr(mesh, triHandle, 0);
		*MeshMod_MeshPolygonU32TagHandleToPtr(mesh, triHandle, MeshMod_PolygonIdUserTag) = faceIndex;

		for(uint32_t i = 0u; i < 3; ++i) {
			Math_Vec3F* positionData = (Math_Vec3F*) MeshMod_MeshVertexPositionTagHandleToPtr(mesh,vh[i], 0);
			Math_Vec3F* normalData = (Math_Vec3F*) MeshMod_MeshVertexNormalTagHandleToPtr(mesh, vh[i], 0);
			memcpy( positionData, v + i, sizeof(Math_Vec3F));
			memcpy( normalData, &normal, sizeof(Math_Vec3F));

			MeshMod_EdgeHalfEdge* halfEdge = MeshMod_MeshEdgeHalfEdgeTagHandleToPtr(mesh, eh[i], 0);
			halfEdge->vertex = vh[i];
			halfEdge->polygon = triHandle;
			brep->edge[i] = eh[i];
		}

	}

	return mesh;
}

AL2O3_EXTERN_C MeshMod_MeshHandle MeshModShapes_IcosahedronCreate(MeshMod_RegistryHandle registry) {

	MeshMod_MeshHandle mesh = MeshMod_MeshCreate(registry, "Icosahedron");

	// Phi - the square root of 5 plus 1 divided by 2
	float const phi = 1.6810f;//(1.0 + sqrt(5.0)) * 0.5;
	float const p = 2.0f / (2.0f * phi);

	static const uint32_t NumVertices = 12;
	static const uint32_t NumFaces = 20;
	float const pos[NumVertices * 3] = {
			-p,  1,  0,
			p,  1,  0,
			0,  p, -1,
			0,  p,  1,
			-1,  0,  p,
			1,  0,  p,

			1,  0, -p,
			-1,  0, -p,
			0, -p,  1,
			0, -p, -1,
			p, -1,  0,
			-p, -1,  0
	};

	uint32_t const faces[NumFaces * 3] = {
			2, 1, 0,
			2, 0, 7,
			2, 9, 6,
			2, 6, 1,
			2, 7, 9,
			1, 5, 3,
			1, 6, 5,
			1, 3, 0,
			0, 3, 4,
			0, 4, 7,
			3, 8, 4,
			3, 5, 8,
			8, 5, 10,
			8, 10,11,
			4, 8, 11,
			4, 11,7,
			5, 6, 10,
			9, 7, 11,
			9, 10,6,
			9, 11,10,
	};

	MeshMod_MeshVertexTagEnsure(mesh, MeshMod_VertexPositionTag);
	MeshMod_MeshVertexTagEnsure(mesh, MeshMod_VertexNormalTag);
	MeshMod_MeshEdgeTagEnsure(mesh, MeshMod_EdgeHalfEdgeTag);
	MeshMod_MeshPolygonTagEnsure(mesh, MeshMod_PolygonTriBRepTag);
	MeshMod_MeshPolygonTagEnsure(mesh, MeshMod_PolygonIdTag);

	for (uint32_t faceIndex = 0u; faceIndex < NumFaces; ++faceIndex) {

		Math_Vec3F v[3];
		MeshMod_VertexHandle vh[3];
		MeshMod_EdgeHandle eh[3];

		for(uint32_t i = 0u; i < 3; ++i) {
			// allocate vertex and edges from mesh mod
			vh[i] = MeshMod_MeshVertexAlloc(mesh);
			eh[i] = MeshMod_MeshEdgeAlloc(mesh);

			// deindex and copy vertex data
			uint32_t const vertIndex = faces[(faceIndex*3) +i ];
			v[i] = Math_FromVec3F(pos + (vertIndex *3));
			v[i] = Math_ScalarMulVec3F(v[i], 0.5f);
		}
		// per face data
		Math_Vec3F normal = CalcNormal(v[0], v[1], v[2]);
		MeshMod_PolygonHandle triHandle = MeshMod_MeshPolygonAlloc(mesh);
		MeshMod_PolygonTriBRep* brep = MeshMod_MeshPolygonTriBRepTagHandleToPtr(mesh, triHandle, 0);
		*MeshMod_MeshPolygonU32TagHandleToPtr(mesh, triHandle, MeshMod_PolygonIdUserTag) = faceIndex;

		for(uint32_t i = 0u; i < 3; ++i) {
			Math_Vec3F* positionData = (Math_Vec3F*) MeshMod_MeshVertexPositionTagHandleToPtr(mesh,vh[i], 0);
			Math_Vec3F* normalData = (Math_Vec3F*) MeshMod_MeshVertexNormalTagHandleToPtr(mesh, vh[i], 0);
			memcpy( positionData, v + i, sizeof(Math_Vec3F));
			memcpy( normalData, &normal, sizeof(Math_Vec3F));

			MeshMod_EdgeHalfEdge* halfEdge = MeshMod_MeshEdgeHalfEdgeTagHandleToPtr(mesh, eh[i], 0);
			halfEdge->vertex = vh[i];
			halfEdge->polygon = triHandle;
			brep->edge[i] = eh[i];
		}

	}

	return mesh;
}

AL2O3_EXTERN_C MeshMod_MeshHandle MeshModShapes_DodecahedronCreate(MeshMod_RegistryHandle registry) {
	MeshMod_MeshHandle mesh = MeshMod_MeshCreate(registry, "Icosahedron");

	static const float a = sqrtf(2.0f / (3.0f + sqrtf(5.0f)));
	static const float b = 1.0f + sqrtf(6.0f / (3.0f + sqrtf(5.0f)) -
			2.0f + 2.0f * sqrtf(2.0f / (3.0f + sqrtf(5.0f))));

	static const uint32_t NumVertices = 20;
	static const uint32_t NumFaces = 12;
	static const float pos[NumVertices * 3] = {
			-a,  0,  b,
			 a,  0,  b,
			-1,  1, -1,
			-1,  1,  1,
			-1, -1, -1,
			-1, -1,  1,
			 1,  1, -1,
			 1,  1,  1,
			 1, -1, -1,
			 1, -1,  1,
			 b, -a,  0,
			 b,  a,  0,
			-b, -a,  0,
			-b,  a,  0,
			-a,  0, -b,
			 a,  0, -b,
			 0, -b,  a,
			 0, -b, -a,
			 0,  b,  a,
			 0,  b, -a,
	};

	static uint32_t const faces[NumFaces * 5] = {
			0, 1, 9, 16, 5,
			1, 0, 3, 18, 7,
			1, 7, 11, 10, 9,
			11, 7, 18, 19, 6,
			8, 17, 16, 9, 10,
			2, 14, 15, 6, 19,
			2, 13, 12, 4, 14,
			2, 19, 18, 3, 13,
			3, 0, 5, 12, 13,
			6, 15, 8, 10, 11,
			4, 17, 8, 15, 14,
			4, 12, 5, 16, 17,
	};

	MeshMod_MeshVertexTagEnsure(mesh, MeshMod_VertexPositionTag);
	MeshMod_MeshVertexTagEnsure(mesh, MeshMod_VertexNormalTag);
	MeshMod_MeshEdgeTagEnsure(mesh, MeshMod_EdgeHalfEdgeTag);
	MeshMod_MeshPolygonTagEnsure(mesh, MeshMod_PolygonConvexBRepTag);
	MeshMod_MeshPolygonTagEnsure(mesh, MeshMod_PolygonIdTag);

	for (uint32_t faceIndex = 0u; faceIndex < NumFaces; ++faceIndex) {

		Math_Vec3F v[5];
		MeshMod_VertexHandle vh[5];
		MeshMod_EdgeHandle eh[5];

		for(uint32_t i = 0u; i < 5; ++i) {
			// allocate vertex and edges from mesh mod
			vh[i] = MeshMod_MeshVertexAlloc(mesh);
			eh[i] = MeshMod_MeshEdgeAlloc(mesh);

			// deindex and copy vertex data
			uint32_t const vertIndex = faces[(faceIndex*5) +i ];
			v[i] = Math_FromVec3F(pos + (vertIndex *3));
			v[i] = Math_ScalarMulVec3F(v[i], 0.31f);
		}
		// per face data
		Math_Vec3F normal = CalcNormal(v[0], v[1], v[2]);
		MeshMod_PolygonHandle convexHandle = MeshMod_MeshPolygonAlloc(mesh);
		MeshMod_PolygonConvexBRep* brep = MeshMod_MeshPolygonConvexBRepTagHandleToPtr(mesh, convexHandle, 0);
		*MeshMod_MeshPolygonU32TagHandleToPtr(mesh, convexHandle, MeshMod_PolygonIdUserTag) = faceIndex;

		for(uint32_t i = 0u; i < 5; ++i) {
			Math_Vec3F* positionData = (Math_Vec3F*) MeshMod_MeshVertexPositionTagHandleToPtr(mesh,vh[i], 0);
			Math_Vec3F* normalData = (Math_Vec3F*) MeshMod_MeshVertexNormalTagHandleToPtr(mesh, vh[i], 0);
			memcpy( positionData, v + i, sizeof(Math_Vec3F));
			memcpy( normalData, &normal, sizeof(Math_Vec3F));

			MeshMod_EdgeHalfEdge* halfEdge = MeshMod_MeshEdgeHalfEdgeTagHandleToPtr(mesh, eh[i], 0);
			halfEdge->vertex = vh[i];
			halfEdge->polygon = convexHandle;
			brep->edge[i] = eh[i];
		}
		brep->numEdges = 5;

	}

	return mesh;
}
