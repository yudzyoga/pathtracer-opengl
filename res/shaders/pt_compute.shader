#shader compute
#version 460 core
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D screen;
uniform vec3 u_CameraPosition;

void main()
{
	vec4 pixel = vec4(0.075, 0.133, 0.173, 1.0);
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

	ivec2 dims = imageSize(screen);
	float x = -(float(pixel_coords.x * 2 - dims.x) / dims.x); // transforms to [-1.0, 1.0] 	
	float y = -(float(pixel_coords.y * 2 - dims.y) / dims.y); // transforms to [-1.0, 1.0]

	// float Epsilon = 0.0001;
	// float resolution = 500;
	// float fov = 30 * 0.017453292;
	// float focal_length = (0.5 * resolution) / (tan(fov * 0.5));
	// float xi = 0.5 * resolution * x;
	// float yi = 0.5 * resolution * y;

	// vec3 ray_o = u_CameraPosition;
	// vec3 ray_d = normalize(vec3(xi, yi, focal_length));
	// vec3 sphere_c = vec3(0.0, 0.0, 0.0);
	// float sphere_r = 1.0;

	// vec3 ray_origin_to_sphere_center = ray_o - sphere_c;
	// float A = dot(ray_d, ray_d);
    // float B = dot(ray_d, ray_origin_to_sphere_center);
    // float C = dot(ray_origin_to_sphere_center, ray_origin_to_sphere_center) - (sphere_r * sphere_r);
	// float D = B*B-A*C;

	// if(D > 0){
	// 	D = sqrt(D);
	// 	float t1 = (B - D) / A;
	// 	float t2 = (B + D) / A;

	// 	float distance;
	// 	if (t1 > Epsilon){
	// 		distance = t1;
	// 	} else if (t2 > Epsilon){
	// 		distance = t2;
	// 	} 

	// 	vec3 intersection = ray_o + ray_d * distance;
	// 	pixel = vec4((normalize(intersection - sphere_c) + 1.0) / 2.0, 1.0);
	// } else {
	// 	pixel = vec4(0.075, 0.133, 0.173, 1.0);
	// }

	pixel = vec4(0.5 * x + 0.5, 0.5 * y + 0.5, 0.0, 1.0);
	
	imageStore(screen, pixel_coords, pixel);
}