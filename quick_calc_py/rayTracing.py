'''
读取STL模型使用的库为：numpy-stl，现已改为vtk
加速需要安装：numba
'''

from numba import cuda
import numpy as np
import time

@cuda.jit(device=True)
def cross(r1,r2):
    return r1[1]*r2[2]-r1[2]*r2[1],r1[2]*r2[0]-r1[0]*r2[2],r1[0]*r2[1]-r1[1]*r2[0]

@cuda.jit(device=True)
def vec_sub(r1,r2):
    return r1[0]-r2[0],r1[1]-r2[1],r1[2]-r2[2]

@cuda.jit(device=True)
def vec_dot(r1,r2):
    return r1[0]*r2[0]+r1[1]*r2[1]+r1[2]*r2[2]

@cuda.jit
def gpu_intersect(ray_orig,D,Points,lim,n,out,cell_index,ray_index):
    idx = cuda.threadIdx.x + cuda.blockDim.x * cuda.blockIdx.x
    # idy = cuda.threadIdx.y + cuda.blockDim.y * cuda.blockIdx.y
    # print(ray_index)
    if(idx < n):
        # if(idy<m):
        # or_to_ptsx = Points[idx][0] - ray_orig[0]
        # or_to_ptsy = Points[idx][1] - ray_orig[1]
        # or_to_ptsz = Points[idx][2] - ray_orig[2]
        #
        # cross_x = or_to_ptsy * D[2] - or_to_ptsz * D[1]
        # cross_y = or_to_ptsz * D[0] - or_to_ptsx * D[2]
        # cross_z = or_to_ptsx * D[1] - or_to_ptsy * D[0]
        # d_ptsToRay = math.sqrt(cross_x**2 + cross_y**2 + cross_z**2)

        or_to_pts = vec_sub(Points[idx,0:3],ray_orig[ray_index])
        x,y,z = cross(or_to_pts,D[ray_index])
        d_ptsToRay = cuda.libdevice.sqrtf(x**2+y**2+z**2)

        if(d_ptsToRay<lim):
            E1 = vec_sub(Points[idx, 3:6],Points[idx,0:3])
            E2 = vec_sub(Points[idx, 6:9],Points[idx,0:3])
            T = vec_sub(ray_orig[ray_index],Points[idx,0:3])
            P = cross(D[ray_index], E2)
            Q = cross(T, E1)
            det = vec_dot(P,E1)
            t = vec_dot(Q,E2)/det
            u = vec_dot(P,T)/det
            v = vec_dot(Q,D[ray_index])/det
            if((t >= 0) * (u >= 0) * (v >= 0) * (u + v <= 1)):
                if(t<out[ray_index]):
                    out[ray_index] = t
                    cell_index[ray_index] = idx
                    print(ray_index)

@cuda.jit
def gpu_intersect_new(ray_orig,D,Points,lim,n,out,cell_index,ray_num):
    idx = cuda.threadIdx.x + cuda.blockDim.x * cuda.blockIdx.x
    if(idx < n):
        for i in range(ray_num):
            or_to_pts = vec_sub(Points[idx,0:3],ray_orig[i])
            x,y,z = cross(or_to_pts,D[i])
            d_ptsToRay = cuda.libdevice.sqrtf(x**2+y**2+z**2)
            if(d_ptsToRay<lim):
                E1 = vec_sub(Points[idx, 3:6],Points[idx,0:3])
                E2 = vec_sub(Points[idx, 6:9],Points[idx,0:3])
                T = vec_sub(ray_orig[i],Points[idx,0:3])
                P = cross(D[i], E2)
                Q = cross(T, E1)
                det = vec_dot(P,E1)
                t = vec_dot(Q,E2)/det
                u = vec_dot(P,T)/det
                v = vec_dot(Q,D[i])/det
                if((t >= 0) * (u >= 0) * (v >= 0) * (u + v <= 1)):
                    if(t<out[i] and t>1e-4):
                        out[i] = t
                        cell_index[i] = idx
                        # print(i)

def ray_tracing(ray_start_points,ray_directions,triangle_points, threads_per_block):
    """
    注：输入数据格式为 np.float32
    :param ray_start_points: 射线起点，数组形式为（n，3）
    :param ray_directions: 射线方向，数组形式为（n，3）
    :param triangle_points: 三角形点坐标，数组形式为（n，9），即【x1,y1,z1,x2,y2,z2,x3,y3,z3】
    :return:tri_intersect_index:相交面元的索引 , ray_index:相交射线的索引 , ray_intersect_points：射线交点
    """
    num_of_tri = len(triangle_points)
    num_of_rays = len(ray_start_points)
    # print('三角形数量：', num_of_tri)
    # print('射线数量：', num_of_rays)
    #   取一个判断的标准长度，以第一个三角形的边的5倍为准
    tem_lim = 2 * np.linalg.norm(triangle_points[0, 3:6] - triangle_points[0, 0:3])

    # Threads_per_block = 512
    Block_per_gridx = int(np.ceil(num_of_tri/ threads_per_block))

    tem_INF = np.float32(np.inf)
    Out_result = tem_INF * np.ones([num_of_rays], dtype=np.float32)
    Out_result_device = cuda.to_device(Out_result)
    Orig_device = cuda.to_device(ray_start_points)
    Dir_device = cuda.to_device(ray_directions)
    Tri_points_device = cuda.to_device(triangle_points)
    Lim_device = np.float32(tem_lim)

    tri_index = np.zeros_like(Out_result, dtype=np.float32)
    tri_index_device = cuda.to_device(tri_index)
    START_time = time.time()
    cuda.synchronize()
    # for ii in range(num_of_rays):
    #     gpu_intersect[Block_per_gridx, Threads_per_block](Orig_device, Dir_device, Tri_points_device, Lim_device, num_of_tri,Out_result_device,tri_index_device, ii)
    #     # cuda.synchronize()
    gpu_intersect_new[Block_per_gridx, threads_per_block](Orig_device, Dir_device, Tri_points_device, Lim_device,
                                                      num_of_tri, Out_result_device, tri_index_device, num_of_rays)
    cuda.synchronize()

    END_time = time.time()
    # print('GPU_{}_intersect_judge_time:'.format(num_of_rays),END_time-START_time)
    Out_result = Out_result_device.copy_to_host()
    tri_index = np.int32(tri_index_device.copy_to_host())

    ray_index = np.argwhere(Out_result<tem_INF)
    ray_index = np.reshape(ray_index,(len(ray_index)))
    T = Out_result[ray_index]
    ##################################
    ray_intersect_points = np.zeros((len(ray_index),3))
    for j in range(len(ray_index)):
        ray_intersect_points[j] = ray_start_points[ray_index[j]] + Out_result[ray_index[j]]*ray_directions[ray_index[j]]
    # ray_intersect_points = ray_start_points[ray_index] + Out_result[ray_index]*ray_directions[ray_index]
    tri_intersect_index = tri_index[ray_index]
    return tri_intersect_index,ray_index,ray_intersect_points,T


# myMesh = mesh.Mesh.from_file('STL模型/球1m3G.stl')
# pts_tri = myMesh.points
# #   射线起点方向
# orig = np.array([-1.0,-1.0,-1.0],dtype=np.float32)
# dir = np.array([1,1,1],dtype=np.float32)
# dir = dir/np.linalg.norm(dir)
# Points = np.array(pts_tri,dtype=np.float32)
#
# num_of_ray = 100000
# orig = np.tile(orig,(num_of_ray,1))
# for i in range(int(num_of_ray/3)):
#     orig[2*i]=-1*orig[2*i]
# dir = np.tile(dir,(num_of_ray,1))
#
# tri_INDEX,ray_INDEX,intersection,T = ray_tracing(orig,dir,Points)
# print('射线交点：',intersection)
# print('相交三角面元id：',tri_INDEX,' 数量：',len(tri_INDEX))
# print('相交射线id：',ray_INDEX)


