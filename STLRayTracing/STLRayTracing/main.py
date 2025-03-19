import trimesh
import numpy as np
import ctypes
import time
import os

print("当前工作目录：", os.getcwd())

# 定义输入射线结构体
class RayIn(ctypes.Structure):
    _fields_ = [
        ("origin", ctypes.c_float * 3),
        ("direction", ctypes.c_float * 3)
    ]

# 定义碰撞结果结构体
class HitResult(ctypes.Structure):
    _fields_ = [
        ("hit", ctypes.c_bool),
        ("hit_point", ctypes.c_float * 3),
        ("triangle_index", ctypes.c_int)
    ]

# 定义 float3 结构体
class Float3(ctypes.Structure):
    _fields_ = [
        ("x", ctypes.c_float),
        ("y", ctypes.c_float),
        ("z", ctypes.c_float)
    ]

# 定义 int3 结构体
class Int3(ctypes.Structure):
    _fields_ = [
        ("x", ctypes.c_int),
        ("y", ctypes.c_int),
        ("z", ctypes.c_int)
    ]

# 加载 STL 文件
def load_stl(file_path):
    mesh = trimesh.load(file_path)
    if not isinstance(mesh, trimesh.Trimesh):
        raise ValueError("加载的文件不是有效的三角网格模型！")
    return mesh

# 计算三角网格中心点
def compute_triangle_centers(mesh):
    centers = []
    for face in mesh.faces:
        v1, v2, v3 = mesh.vertices[face]
        center = (v1 + v2 + v3) / 3
        centers.append(center)
    return np.array(centers)

# 生成射线
def generate_rays(observer_point, centers):
    rays = []
    for center in centers:
        direction = center - observer_point
        direction = direction / np.linalg.norm(direction)
        rays.append({
            "origin": observer_point,
            "direction": direction
        })
    return rays

# 准备射线数据传递给 DLL
def prepare_rays_for_dll(rays):
    rays_in = (RayIn * len(rays))()
    for i, ray in enumerate(rays):
        rays_in[i].origin[:] = ray["origin"]
        rays_in[i].direction[:] = ray["direction"]
    return rays_in

# 主程序
if __name__ == "__main__":
    # STL 文件路径
    stl_file_path = "M8d45.stl"

    # 加载 STL 文件
    mesh = load_stl(stl_file_path)
    print("三角网格加载完成，包含三角形数量：", len(mesh.faces))

    # 计算三角网格中心点
    triangle_centers = compute_triangle_centers(mesh)
    print("三角网格中心点数量：", len(triangle_centers))

    # 设置观察点
    observer_point = np.array([20.0, 0.0, 0.0])

    # 生成射线
    rays = generate_rays(observer_point, triangle_centers)
    print("生成射线数量：", len(rays))

    # 准备射线数据传递给 DLL
    rays_in = prepare_rays_for_dll(rays)

    # 准备顶点数据
    vertices = np.array(mesh.vertices, dtype=np.float32)
    num_vertices = len(vertices)

    # 将 vertices 转换为 Float3 数组
    vertices_for_dll = (Float3 * num_vertices)()
    for i in range(num_vertices):
        vertices_for_dll[i].x = vertices[i, 0]
        vertices_for_dll[i].y = vertices[i, 1]
        vertices_for_dll[i].z = vertices[i, 2]

    # 准备索引数据
    indices = np.array(mesh.faces, dtype=np.int32)
    num_triangles = len(indices)

    # 将 indices 转换为 Int3 数组
    indices_for_dll = (Int3 * num_triangles)()
    for i in range(num_triangles):
        indices_for_dll[i].x = indices[i, 0]
        indices_for_dll[i].y = indices[i, 1]
        indices_for_dll[i].z = indices[i, 2]

    # 加载 DLL
    ray_tracer_dll = ctypes.CDLL("./RayTracerDLL.dll")

    # 定义函数参数类型
    ray_tracer_dll.TraceRays.argtypes = [
        ctypes.POINTER(RayIn),
        ctypes.c_int,
        ctypes.POINTER(Float3),
        ctypes.POINTER(Int3),
        ctypes.c_int,
        ctypes.c_int,
        ctypes.POINTER(HitResult)
    ]

    # 定义函数返回类型
    ray_tracer_dll.TraceRays.restype = None

    # 准备结果数组
    num_rays = len(rays_in)
    results = (HitResult * num_rays)()

    # 调用 DLL 函数并计时
    start_time = time.time()
    ray_tracer_dll.TraceRays(
        rays_in,
        num_rays,
        vertices_for_dll,
        indices_for_dll,
        num_vertices,
        num_triangles,
        results
    )
    end_time = time.time()

    # 打印时间
    elapsed_time = end_time - start_time
    print(f"射线追踪耗时：{elapsed_time:.6f} 秒")

    # 打印结果
    #for i, result in enumerate(results):
    #    print(f"Ray {i}:")
    #    print(f"  Hit: {result.hit}")
    #    print(f"  Hit Point: {list(result.hit_point)}")
    #    print(f"  Triangle Index: {result.triangle_index}")
