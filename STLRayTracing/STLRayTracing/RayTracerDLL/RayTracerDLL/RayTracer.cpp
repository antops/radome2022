#include <optix_prime.h>
#include <putil/Buffer.h>
#include <iostream>
using namespace std;

// �������߽ṹ��
struct RayIn {
	float origin[3];
	float direction[3];
};

// ������ײ����ṹ��
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

	float t;                        //�ɸ��ݹ�ʽ O+Dir*t �����㽻��
	int   triId;                    //�ཻ��������ID
	float u;                        //�����������꣬�ɸ��ݹ�ʽ  (1 - u - v)V0 + uV1 + vV2 �����㽻��
	float v;                        //������������
};

// ����������
#define CHK_PRIME(call) { const RTPresult res = (call); if (res != RTP_SUCCESS) { std::cerr << "OptiX Prime error: " << res << std::endl; exit(1); } }

// ����������׷�ٺ���
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
	// ���� OptiX Prime ������
	// OptixPrime��ʼ��
	RTPcontexttype contextType = RTP_CONTEXT_TYPE_CUDA; // ����������ΪCUDA
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

	// �����������ݻ�����
	RTPbufferdesc indicesDesc;
	CHK_PRIME(rtpBufferDescCreate(context, RTP_BUFFER_FORMAT_INDICES_INT3, RTP_BUFFER_TYPE_HOST, indices, &indicesDesc));
	CHK_PRIME(rtpBufferDescSetRange(indicesDesc, 0, numTriangles));
	std::cout << "indicesDesc Set" << endl;

	RTPbufferdesc verticesDesc;
	CHK_PRIME(rtpBufferDescCreate(context, RTP_BUFFER_FORMAT_VERTEX_FLOAT3, RTP_BUFFER_TYPE_HOST, vertices, &verticesDesc));
	CHK_PRIME(rtpBufferDescSetRange(verticesDesc, 0, numVertices));
	std::cout << "VerticesDesc Set" << endl;
	// ����ģ�Ͷ���
	RTPmodel model;
	CHK_PRIME(rtpModelCreate(context, &model));
	CHK_PRIME(rtpModelSetTriangles(model, indicesDesc, verticesDesc));
	CHK_PRIME(rtpModelUpdate(model, 0));
	std::cout << "Modle Buffer Set" << endl;

	// �������߻�����
	RTPbufferdesc raysDesc;
	Buffer<Ray> raysBuffer(0, bufferType, LOCKED);
	//��������
	raysBuffer.alloc(num_rays);
	Ray* rays = raysBuffer.ptr();
	//����������߷�ֵ��raysBuffer
	if (raysBuffer.type() == RTP_BUFFER_TYPE_HOST)
	{
		for (int cnt = 0; cnt < num_rays; cnt++) {
			rays[cnt] = { make_float3(raysIn[cnt].origin[0], raysIn[cnt].origin[1],  raysIn[cnt].origin[2]), 1e-5f, make_float3(raysIn[cnt].direction[0], raysIn[cnt].direction[1],  raysIn[cnt].direction[2]), 1e6f };
		}
	}
	
	CHK_PRIME(rtpBufferDescCreate(context, Ray::format, raysBuffer.type(), raysBuffer.ptr(), &raysDesc)); /*RTP_BUFFER_FORMAT_RAY_ORIGIN_TMIN_DIRECTION_TMAX*/
	CHK_PRIME(rtpBufferDescSetRange(raysDesc, 0, raysBuffer.count()));
	std::cout << "Ray Buffer Set" << endl;

	// ������ײ���������
	RTPbufferdesc hitsDesc;
	Buffer<Hit> hitsBuffer(raysBuffer.count(), bufferType, LOCKED);                                       // ��ײ������
	CHK_PRIME(rtpBufferDescCreate(context, Hit::format, hitsBuffer.type(), hitsBuffer.ptr(), &hitsDesc)); /*RTP_BUFFER_FORMAT_HIT_T_TRIID_U_V*/
	CHK_PRIME(rtpBufferDescSetRange(hitsDesc, 0, hitsBuffer.count()));
	std::cout << "Hit Buffer Set" << endl;

	// ������ѯ����
	//ע�⣬׷�ٲ�û���ڵ��жϣ����Լ�����
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
	
	// ������Դ
	CHK_PRIME(rtpQueryDestroy(query));
	CHK_PRIME(rtpModelDestroy(model));
	CHK_PRIME(rtpBufferDescDestroy(indicesDesc));
	CHK_PRIME(rtpBufferDescDestroy(verticesDesc));
	CHK_PRIME(rtpBufferDescDestroy(raysDesc));
	CHK_PRIME(rtpBufferDescDestroy(hitsDesc));
	CHK_PRIME(rtpContextDestroy(context));
}
