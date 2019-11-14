#include "al2o3_platform/platform.h"
#include "al2o3_cmath/aabb.h"
#include "al2o3_memory/memory.h"
#include "render_meshmod/meshmod.h"
#include "render_meshmod/mesh.h"
#include "render_meshmod/vertex/position.h"
#include "render_meshmod/vertex/normal.h"
#include "render_meshmod/edge/halfedge.h"
#include "render_meshmod/polygon/tribrep.h"
#include "render_meshmod/polygon/quadbrep.h"
#include "render_meshmod/polygon/basicdata.h"
#include "render_meshmodshapes/shapes.h"

static Math_Vec3F CalcNormal(Math_Vec3F const v0, Math_Vec3F v1, Math_Vec3F v2) {
	Math_Vec3F e0 = Math_NormaliseVec3F(Math_SubVec3F(v1, v0));
	Math_Vec3F e1 = Math_NormaliseVec3F(Math_SubVec3F(v2, v0));
	Math_Vec3F n = Math_CrossVec3F(e0, e1);
	return n;
}
AL2O3_EXTERN_C MeshMod_MeshHandle MeshModShapes_DiamondCreate(MeshMod_RegistryHandle registry) {

	MeshMod_MeshHandle mesh = MeshMod_MeshCreate(registry, "Diamond");

	static const uint32_t NumVertices = 6;
	static const uint32_t NumFaces = 8;
	float const pos[NumVertices * 3] = {
			-0.5f, 0,  -0.5f,
			0.5f,  0,  -0.5f,
			0.5f,  0,  0.5f,
			-0.5f, 0,  0.5f,

			0,     1,  0,
			0,     -1, 0,
	};

	uint32_t const faces[NumFaces * 3] = {
			0, 4, 1,
			1, 4, 2,
			2, 4, 3,
			3, 4, 0,
			0, 1, 5,
			1, 2, 5,
			2, 3, 5,
			3, 0, 5,
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

/*
auto Shapes::CreateSphere(uint32_t subdivisionSteps_) -> std::unique_ptr<MeshMod::Mesh>
{
	using namespace MeshMod;
	using namespace Math;
	std::shared_ptr<Mesh> mesh = PlatonicSolids::CreateIcosahedron();

	for(auto i = 0u; i < subdivisionSteps_; ++i)
	{
		mesh = std::move(BasicMeshOps::tesselate4(std::move(mesh)));
	}

	BasicMeshOps::spherize(mesh, 1.0f);
	mesh->updateEditState(MeshMod::Mesh::PositionEdits);
	mesh->updateFromEdits();

	return mesh->clone();
}
*/

AL2O3_EXTERN_C MeshMod_MeshHandle MeshModShapes_AABB3FCreate(MeshMod_RegistryHandle registry, Math_Aabb3F aabb) {

	MeshMod_MeshHandle mesh = MeshMod_MeshCreate(registry, "AABB");

	Math_Vec3F const minBox = aabb.minExtent;
	Math_Vec3F const maxBox = aabb.maxExtent;

	static const uint32_t NumVertices = 8;
	static const uint32_t NumFaces = 6;
	float const pos[NumVertices * 3] = {
			minBox.x, maxBox.y, minBox.z,
			minBox.x, minBox.y, minBox.z,
			maxBox.x, minBox.y, minBox.z,
			maxBox.x, maxBox.y, minBox.z,

			minBox.x, maxBox.y, maxBox.z,
			minBox.x, minBox.y, maxBox.z,
			maxBox.x, minBox.y, maxBox.z,
			maxBox.x, maxBox.y, maxBox.z,
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

/*
auto Shapes::CreateSquare(Math::vec3 const& center_, Math::vec3 const& forward_) -> std::unique_ptr<MeshMod::Mesh>
{
	using namespace MeshMod;
	using namespace Math;
	auto mesh = std::make_unique<Mesh>("Square");

	vec3 up{0, 1, 0};
	float dot = Math::dot(forward_, up);
	if(ApproxEqual(dot, 1.0f))
	{
		up = {0, 0, 1};
	}
	vec3 forward = forward_ * 0.5f;
	vec3 right = cross(forward_, up) * 0.5f;
	up = up * 0.5f;
	vec3 const tl = center_ + -right + up;
	vec3 const tr = center_ + right + up;
	vec3 const ul = center_ + -right - up;
	vec3 const ur = center_ + right - up;

	vec3 const pos[]{ tl, tr, ur, ul };

	using vi = VertexIndex;
	VertexIndexContainer const faces[] = {
			{vi(0), vi(1), vi(2), vi(3)},
	};
	for(auto p : pos)
	{
		mesh->getVertices().add(p.x, p.y, p.z);
	}
	for(auto f : faces)
	{
		mesh->getPolygons().addPolygon(f);
	}
	mesh->updateEditState(MeshMod::Mesh::TopologyEdits);
	mesh->updateFromEdits();

	return std::move(mesh);
}
*/