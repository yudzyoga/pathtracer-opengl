#shader compute
#version 460 core
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D screen;
uniform vec3 u_CameraPosition;
uniform vec3 u_CameraRotationEuler;
uniform int u_FrameIndex;
uniform int u_SampleOffset;
uniform int u_MaxFrameIndex;

const float Epsilon = 0.0001;
const float resolution = 500;
const float fov = 60 * 0.017453292;
const float focal_length = (0.5 * resolution) / (tan(fov * 0.5));
const int MAX_BOUNCES = 32;


mat3 rotationMatrixFromEuler(vec3 degrees) {
    vec3 r = radians(degrees);   // convert to radians
    float cx = cos(r.x), sx = sin(r.x); // pitch
    float cy = cos(r.y), sy = sin(r.y); // yaw
    float cz = cos(r.z), sz = sin(r.z); // roll

    mat3 Rx = mat3(1, 0, 0,
                   0, cx, -sx,
                   0, sx,  cx);

    mat3 Ry = mat3(cy, 0, sy,
                   0,  1, 0,
                  -sy, 0, cy);

    mat3 Rz = mat3(cz, -sz, 0,
                   sz,  cz, 0,
                   0,   0,  1);

    return Rz * Ry * Rx;
}

struct Ray3f {
	vec3 origin;
	vec3 direction;
	vec3 throughput;
	vec3 illumination;
	float tmin;
	float tmax;
};

struct Sphere {
	vec3 center;
	float radius;
	vec3 color;
	bool is_emitter;
};

struct Plane {
	float y_pos;
	vec3 color;
};

struct Intersection {
	bool is_valid;
	float t;
	vec3 position;
	vec3 normal;
	vec3 bsdf_emit_eval;
	bool is_hit_emitter;
};

Intersection intersect_sphere(Ray3f ray, Sphere sphere){
	Intersection its;
	its.is_valid = false;
	its.t = 100000.0;
	its.position = vec3(0.0, 0.0, 0.0);
	its.normal = vec3(0.0, 1.0, 0.0);
	its.bsdf_emit_eval = vec3(1.0, 1.0, 1.0); // envmap
	its.is_hit_emitter = false;

	vec3 ray_origin_to_sphere_center = sphere.center - ray.origin;
	float A = dot(ray.direction, ray.direction);
    float B = dot(ray.direction, ray_origin_to_sphere_center);
    float C = dot(ray_origin_to_sphere_center, ray_origin_to_sphere_center) - (sphere.radius * sphere.radius);
	float D = B*B-A*C;

	if(D > 0){
		D = sqrt(D);
		float t1 = (B - D) / A;
		float t2 = (B + D) / A;

		float distance;
		if (t1 > Epsilon){
			distance = t1;
		} else if (t2 > Epsilon){
			distance = t2;
		} 

		its.is_valid = true;
		its.t = distance;
		its.position = ray.origin + ray.direction * distance;
		its.normal = -normalize(its.position - sphere.center);
		its.bsdf_emit_eval = sphere.color;
		
	}

	return its;
}

Intersection intersect_infplane(Ray3f ray, Plane plane){
	Intersection its;
	its.is_valid = false;
	its.t = 100000.0;
	its.position = vec3(0.0, 0.0, 0.0);
	its.normal = vec3(0.0, 1.0, 0.0);
	its.bsdf_emit_eval = vec3(0.9, 0.9, 0.9); // envmap
	its.is_hit_emitter = false;

	// if(dot(ray.direction, vec3(0.0, 1.0, 0.0)) > 0.0) {
	// 	return its;
	// };

	float denom = ray.direction.y;
	if (abs(denom) > 0.001) {
		float t = (plane.y_pos - ray.origin.y) / denom;
		if ((t > ray.tmin) && (t < ray.tmax)) {
			vec3 its_position = ray.origin + t * ray.direction;
			its.is_valid = true;
			its.t = t;
			its.position = its_position;
			its.normal = vec3(0.0, 1.0, 0.0); // explicit normal
			its.bsdf_emit_eval = plane.color;
		}
	}

	return its;
}

Intersection intersect(Ray3f ray){
	Plane plane = Plane(0, vec3(0.8, 0.8, 0.8));
	Sphere sphs[4] = Sphere[4](
		Sphere(vec3(5.0, 2.5, 5.0), 2.5, vec3(10.0, 10.0, 5.0), true),
		Sphere(vec3(-1.0, 0.5, -1.0), 0.5, vec3(1.0, 0.0, 0.0), false),
		Sphere(vec3(1.5, 0.5, -1.0), 0.5, vec3(0.0, 1.0, 0.0), false),
		Sphere(vec3(-1.0, 0.5, 1.5), 0.5, vec3(0.0, 0.0, 1.0), false)
	);

	Intersection its, cur_its;
	its.is_valid = false;
	its.t = 1000000.0;

	// * intersect sphere
	for (int i = 0; i < sphs.length() + 1; i++) {
		if(i < sphs.length()){
			Sphere s = sphs[i];
			cur_its = intersect_sphere(ray, s);
			cur_its.is_hit_emitter = s.is_emitter;
		} 
		else {
			cur_its = intersect_infplane(ray, plane);
		}

		if(cur_its.is_valid){
			if(!its.is_valid){
				its = cur_its;
			} else {
				if(cur_its.t < its.t){
					its = cur_its;
				}
			}
		}
    }

	return its;
}
struct LCGRand {
    uint state;
};

// MurmurHash3-like mixing (matches C++ version exactly)
uint murmur_hash3_mix(uint hash, uint k) {
    const uint c1 = 0xcc9e2d51u;
    const uint c2 = 0x1b873593u;
    const uint r1 = 15u;
    const uint r2 = 13u;
    const uint m  = 5u;
    const uint n  = 0xe6546b64u;

    k *= c1;
    k = (k << r1) | (k >> (32u - r1));
    k *= c2;

    hash ^= k;
    hash = ((hash << r2) | (hash >> (32u - r2))) * m + n;

    return hash;
}

uint murmur_hash3_finalize(uint hash) {
    hash ^= (hash >> 16u);
    hash *= 0x85ebca6bu;
    hash ^= (hash >> 13u);
    hash *= 0xc2b2ae35u;
    hash ^= (hash >> 16u);
    return hash;
}

// Linear Congruential Generator
uint lcg_random(inout LCGRand rng) {
    const uint m = 1664525u;
    const uint n = 1013904223u;
    rng.state = rng.state * m + n;
    return rng.state;
}

// Uniform float in [0,1)
float lcg_randomf(inout LCGRand rng) {
    uint r = lcg_random(rng);
    // Mask to 23 mantissa bits to avoid NaNs
    uint bits = 0x3f800000u | (r >> 9);
    return uintBitsToFloat(bits) - 1.0;
}

// 2D random vector
vec2 lcg_random2f(inout LCGRand rng) {
    return vec2(lcg_randomf(rng), lcg_randomf(rng));
}

// 3D random vector
vec3 lcg_random3f(inout LCGRand rng) {
    return vec3(lcg_randomf(rng), lcg_randomf(rng), lcg_randomf(rng));
}

// Initialize RNG with hashed seed
LCGRand get_lcg_rng(uint index, uint frame, uint linear) {
    LCGRand rng;
    rng.state = murmur_hash3_mix(0u, linear);
    rng.state = murmur_hash3_mix(rng.state, 512u * frame + index);
    rng.state = murmur_hash3_finalize(rng.state);
    return rng;
}

vec3 envColor(vec3 dir) {
    float t = 0.5 * (dir.y + 1.0);
    return mix(vec3(1.0, 1.0, 1.0), vec3(0.5, 0.7, 1.0), t);
}

vec3 randomHemisphereDirection(vec3 normal, vec2 xi) {
    // xi = random numbers in [0,1)
    float phi = 2.0 * 3.14159265 * xi.x;
    float cosTheta = sqrt(1.0 - xi.y);
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    // local-space direction
    vec3 localDir = vec3(cos(phi)*sinTheta, sin(phi)*sinTheta, cosTheta);

    // build tangent space
    vec3 tangent, bitangent;
    if (abs(normal.x) > 0.1)
        tangent = normalize(cross(vec3(0,1,0), normal));
    else
        tangent = normalize(cross(vec3(1,0,0), normal));
    bitangent = cross(normal, tangent);

    // convert to world-space
    return localDir.x * tangent + localDir.y * bitangent + localDir.z * normal;
}

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	ivec2 dims = imageSize(screen);
	int i = int(dims.x * pixel_coords.y + pixel_coords.x);
	LCGRand rng = get_lcg_rng(u_FrameIndex, u_SampleOffset, i);	
	float x = (float(pixel_coords.x * 2 - dims.x + lcg_randomf(rng)) / dims.x); // transforms to [-1.0, 1.0] 	
	float y = (float(pixel_coords.y * 2 - dims.y + lcg_randomf(rng)) / dims.y); // transforms to [-1.0, 1.0]
	float xi = 0.5 * resolution * x;
	float yi = 0.5 * resolution * y;

	mat3 R = rotationMatrixFromEuler(u_CameraRotationEuler);
	Ray3f ray = Ray3f(
		u_CameraPosition,
		normalize(R * vec3(xi, yi, focal_length)),
		vec3(1.0, 1.0, 1.0),
		vec3(0.0, 0.0, 0.0),
		1e-4, 
		200.0
	);

	for (int bounce = 0; bounce < MAX_BOUNCES; bounce++) {
		Intersection its = intersect(ray);
		if(!its.is_valid){
			ray.illumination += ray.throughput * envColor(ray.direction); // should be envmap
			break;
		}

		if(its.is_hit_emitter){
			ray.illumination += ray.throughput * its.bsdf_emit_eval;
			break;
		}

		// * sample new ray
		ray.origin = its.position;
		ray.direction = normalize(randomHemisphereDirection(normalize(its.normal), lcg_random2f(rng)));
		ray.throughput *= its.bsdf_emit_eval * max(dot(its.normal, ray.direction), 0.0);
	}

	// read previous accumulation
	if (u_FrameIndex == 0) {
        // overwrite on first frame
        imageStore(screen, pixel_coords, vec4(ray.illumination, 1.0));
    } else if (u_FrameIndex < u_MaxFrameIndex){
        // running average: (prev * n + cur) / (n+1)
        float n = float(u_FrameIndex);
	    vec4 prev = imageLoad(screen, pixel_coords);
        vec3 accum = (prev.rgb * n + ray.illumination) / (n + 1.0);
        imageStore(screen, pixel_coords, vec4(accum, 1.0));
    }
}