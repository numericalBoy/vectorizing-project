#include "Vertex.h"

D3D11_INPUT_ELEMENT_DESC VertexDesc_PointVertex[] =
{
	{"POSITION",    0, DXGI_FORMAT_R32G32_FLOAT,        0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"SIZE",    0, DXGI_FORMAT_R32G32_FLOAT,        0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",   0, DXGI_FORMAT_R32G32B32_FLOAT,     0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ NULL, 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC VertexDesc_PICVertex[] =
{
	{"POSITION",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"SIZE",    0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ NULL, 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC VertexDesc_TRIVertex[] =
{
	{"POSA",     0, DXGI_FORMAT_R32G32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"POSB",     0, DXGI_FORMAT_R32G32_FLOAT,    0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"POSC",     0, DXGI_FORMAT_R32G32_FLOAT,    0,  16, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLORA",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLORB",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  36, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLORC",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  48, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ NULL, 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC VertexDesc_LineVertex[] =
{
	{"POSA",    0, DXGI_FORMAT_R32G32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"POSB",    0, DXGI_FORMAT_R32G32_FLOAT,    0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"POSC",    0, DXGI_FORMAT_R32G32_FLOAT,    0,  16, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"POSD",    0, DXGI_FORMAT_R32G32_FLOAT,    0,  24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"WA",      0, DXGI_FORMAT_R32G32_FLOAT,    0,  32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  40, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ NULL, 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC VertexDesc_LineVertex2w[] =
{
	{"POSA",    0, DXGI_FORMAT_R32G32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"POSB",    0, DXGI_FORMAT_R32G32_FLOAT,    0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"POSC",    0, DXGI_FORMAT_R32G32_FLOAT,    0,  16, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"POSD",    0, DXGI_FORMAT_R32G32_FLOAT,    0,  24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"WA",      0, DXGI_FORMAT_R32G32_FLOAT,    0,  32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"WB",      0, DXGI_FORMAT_R32G32_FLOAT,    0,  40, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  48, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ NULL, 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC VertexDesc_SkeletonLineVertex[] =
{
	{"POSA",    0, DXGI_FORMAT_R32G32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"POSB",    0, DXGI_FORMAT_R32G32_FLOAT,    0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  16, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ NULL, 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC VertexDesc_VSOVertex[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ NULL, 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC VertexDesc_CurveVertex[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXTURE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXTURE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXTURE", 2, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ NULL, 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

