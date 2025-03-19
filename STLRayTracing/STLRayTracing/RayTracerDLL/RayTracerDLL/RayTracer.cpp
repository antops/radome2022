#include <optix_prime.h>
#include <putil/Buffer.h>
#include <iostream>
using namespace std;

// 定义射线结构体
struct RayIn {
	float origin[3];
	float direction[3];
};

// 定义碰撞结果结构体
struct HitResult {
	bool hit;
	float hit_point[3];
	int triangle_index;
};

struct Ray
{
	static const RTPbufferformat format = RTP_BUFFER_FORMAT_RAY_ORIGIN_TMIN_DIRECTION_TMAX;

	float3 origin;
	float  tmin;
	float3 dir;
	float  tmax;
};

struct Hit
{
	static const RTPbufferformat format = RTP_BUFFER_FORMAT_HIT_T_TRIID_U_V;

	float t;                        //可根据公式 O+Dir*t 来计算交点
	int   triId;                    //相交的三角形ID
	float u;                        //交点纹理坐标，可根据公式  (1 - u - v)V0 + uV1 + vV2 来计算交点
	float v;                        //交点纹理坐标
};

// 定义错误处理宏
#define CHK_PRIME(call) { const RTPresult res = (call); if (res != RTP_SUCCESS) { std::cerr << "OptiX Prime error: " << res << std::endl; exit(1); } }

// 导出的射线追踪函数
extern "C" __declspec(dllexport) void TraceRays(
	RayIn* raysIn,
	int num_rays,
	float3* vertices,
	int3* indices,
	int numVertices,
	int numTriangles,
	HitResult* results
)
{
	// 创建 OptiX Prime 上下文
	// OptixPrime初始化
	RTPcontexttype contextType = RTP_CONTEXT_TYPE_CUDA; // 上下文类型为CUDA
	RTPbuffertype bufferType = RTP_BUFFER_TYPE_HOST;
	RTPcontext context;
	CHK_PRIME(rtpContextCreate(contextType, &context));
	if (contextType == RTP_CONTEXT_TYPE_CPU) { std::cerr << "Using cpu context\n"; }
	else {
		unsigned int device = 0;
		CHK_PRIME(rtpContextSetCudaDeviceNumbers(context, 1, &device));
		std::cerr << "Using cuda context\n";
	}

	std::cout << "There are " << num_rays << " rays" << endl;
	//std::cout << raysIn[num_rays - 1].origin[0] <<" " << endl;

	std::cout << "There are " << numVertices << " vertices" << endl;
	//std::cout << vertices[numVertices-1].x << " " << endl;

	std::cout << "There are " << numTriangles << " triangles" << endl;
	//std::cout << indices[numTriangles - 1].x << " " << endl;

	// 创建几何数据缓冲区
	RTPbufferdesc indicesDesc;
	CHK_PRIME(rtpBufferDescCreate(context, RTP_BUFFER_FORMAT_INDICES_INT3, RTP_BUFFER_TYPE_HOST, indices, &indicesDesc));
	CHK_PRIME(rtpBufferDescSetRange(indicesDesc, 0, numTriangles));
	std::cout << "indicesDesc Set" << endl;

	RTPbufferdesc verticesDesc;
	CHK_PRIME(rtpBufferDescCreate(context, RTP_BUFFER_FORMAT_VERTEX_FLOAT3, RTP_BUFFER_TYPE_HOST, vertices, &verticesDesc));
	CHK_PRIME(rtpBufferDescSetRange(verticesDesc, 0, numVertices));
	std::cout << "VerticesDesc Set" << endl;
	// 创建模型对象
	RTPmodel model;
	CHK_PRIME(rtpModelCreate(context, &model));
	CHK_PRIME(rtpModelSetTriangles(model, indicesDesc, verticesDesc));
	CHK_PRIME(rtpModelUpdate(model, 0));
	std::cout << "Modle Buffer Set" << endl;

	// 创建射线缓冲区
	RTPbufferdesc raysDesc;
	Buffer<Ray> raysBuffer(0, bufferType, LOCKED);
	//更新射线
	raysBuffer.alloc(num_rays);
	Ray* rays = raysBuffer.ptr();
	//将输入的射线幅值给raysBuffer
	if (raysBuffer.type() == RTP_BUFFER_TYPE_HOST)
	{
		for (int cnt = 0; cnt < num_rays; cnt++) {
			rays[cnt] = { make_float3(raysIn[cnt].origin[0], raysIn[cnt].origin[1],  raysIn[cnt].origin[2]), 1e-5f, make_float3(raysIn[cnt].direction[0], raysIn[cnt].direction[1],  raysIn[cnt].direction[2]), 1e6f };
		}
	}
	
	CHK_PRIME(rtpBufferDescCreate(context, Ray::format, raysBuffer.type(), raysBuffer.ptr(), &raysDesc)); /*RTP_BUFFER_FORMAT_RAY_ORIGIN_TMIN_DIRECTION_TMAX*/
	CHK_PRIME(rtpBufferDescSetRange(raysDesc, 0, raysBuffer.count()));
	std::cout << "Ray Buffer Set" << endl;

	// 创建碰撞结果缓冲区
	RTPbufferdesc hitsDesc;
	Buffer<Hit> hitsBuffer(raysBuffer.count(), bufferType, LOCKED);                                       // 碰撞缓冲区
	CHK_PRIME(rtpBufferDescCreate(context, Hit::format, hitsBuffer.type(), hitsBuffer.ptr(), &hitsDesc)); /*RTP_BUFFER_FORMAT_HIT_T_TRIID_U_V*/
	CHK_PRIME(rtpBufferDescSetRange(hitsDesc, 0, hitsBuffer.count()));
	std::cout << "Hit Buffer Set" << endl;

	// 创建查询对象
	//注意，追踪并没有遮挡判断，得自己处理。
	cout << "!!!!Note that: the Ray tracing does not include Occlusion Judgement!!!!" << endl;
	RTPquery query;
	CHK_PRIME(rtpQueryCreate(model, RTP_QUERY_TYPE_CLOSEST, &query));
	CHK_PRIME(rtpQuerySetRays(query, raysDesc));
	CHK_PRIME(rtpQuerySetHits(query, hitsDesc));
	CHK_PRIME(rtpQueryExecute(query, 0));
	cout << "raytracing done" << endl;

	cout << " Writing output for Python" << endl;
	const Hit* hits1 = hitsBuffer.hostPtr();
	float3 hitP;
	int count = 0;
	for (int cnt = 0; cnt < num_rays; cnt++) {
		if (hits1[cnt].triId >= 0 && hits1[cnt].t < 1e6)
			results[cnt].hit = true;
			results[cnt].triangle_index = hits1[cnt].triId;
			results[cnt].hit_point[0] = rays[cnt].origin.x + rays[cnt].dir.x * hits1[cnt].t;
			results[cnt].hit_point[1] = rays[cnt].origin.y + rays[cnt].dir.y * hits1[cnt].t;
			results[cnt].hit_point[2] = rays[cnt].origin.z + rays[cnt].dir.z * hits1[cnt].t;
			count++;
	}
	cout << count << " rays hit!" << endl;
	
	// 清理资源
	CHK_PRIME(rtpQueryDestroy(query));
	CHK_PRIME(rtpModelDestroy(model));
	CHK_PRIME(rtpBufferDescDestroy(indicesDesc));
	CHK_PRIME(rtpBufferDescDestroy(verticesDesc));
	CHK_PRIME(rtpBufferDescDestroy(raysDesc));
	CHK_PRIME(rtpBufferDescDestroy(hitsDesc));
	CHK_PRIME(rtpContextDestroy(context));
}
