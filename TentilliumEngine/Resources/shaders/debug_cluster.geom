#version 460 core

layout(lines) in; 
layout(line_strip, max_vertices = 256) out;

in VERTEX_OUT {
	vec4 minPoint;
	vec4 maxPoint;
	uint isActive;
} geom_in[];

uniform mat4 VP;

void main() {
	
	// for each box
	for (int i = 0; i < gl_in.length(); i++)
	{
		if (geom_in[i].isActive != 0) {

			vec3 cl_min = geom_in[i].minPoint.xyz;
			vec3 cl_max = geom_in[i].maxPoint.xyz;

			vec4 p1 = VP * vec4(cl_min.x, cl_min.y, cl_min.z, 1); // a
			vec4 p2 = VP * vec4(cl_min.x, cl_min.y, cl_max.z, 1); // b
			vec4 p3 = VP * vec4(cl_min.x, cl_max.y, cl_min.z, 1); // c
			vec4 p4 = VP * vec4(cl_min.x, cl_max.y, cl_max.z, 1); // d

			vec4 p5 = VP * vec4(cl_max.x, cl_min.y, cl_min.z, 1); // e
			vec4 p6 = VP * vec4(cl_max.x, cl_min.y, cl_max.z, 1); // f
			vec4 p7 = VP * vec4(cl_max.x, cl_max.y, cl_min.z, 1); // g
			vec4 p8 = VP * vec4(cl_max.x, cl_max.y, cl_max.z, 1); // h

			gl_Position = p1; EmitVertex();
			gl_Position = p2; EmitVertex();
			gl_Position = p4; EmitVertex();
			gl_Position = p3; EmitVertex();
			gl_Position = p1; EmitVertex();

			gl_Position = p5; EmitVertex();
			gl_Position = p6; EmitVertex();
			gl_Position = p8; EmitVertex();
			gl_Position = p7; EmitVertex();
			gl_Position = p5; EmitVertex();

			gl_Position = p6; EmitVertex();
			gl_Position = p2; EmitVertex();
			gl_Position = p4; EmitVertex();
			gl_Position = p8; EmitVertex();
			gl_Position = p7; EmitVertex();
			gl_Position = p3; EmitVertex();
		}

		
	}
}