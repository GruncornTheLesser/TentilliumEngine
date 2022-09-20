#version 460 core

layout(location = 5) in uvec2 lightArray;

out VERTEX_OUT {
	vec4 minPoint;
	vec4 maxPoint;
	uint isActive;
} vert_out;

struct AABB {		// axis aligned bounding box
	vec4 minPoint;	// + padding
	vec4 maxPoint;	// + padding
};

layout(std430, binding = 1) readonly buffer ClusterBuffer {
	AABB clusters[];
};

void main() {
	vert_out.minPoint = vec4(clusters[gl_VertexID].minPoint.xyz, 1);
	vert_out.maxPoint = vec4(clusters[gl_VertexID].maxPoint.xyz, 1);
	vert_out.isActive = lightArray.y - lightArray.x; // count
}