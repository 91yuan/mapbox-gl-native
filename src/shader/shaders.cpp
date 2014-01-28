// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
#include <llmr/shader/shaders.hpp>

using namespace llmr;

const shader_source llmr::shaders[SHADER_COUNT] = {
   [FILL_SHADER] = {
       .vertex = "attribute vec2 a_pos;\n\nuniform mat4 u_matrix;\n\nvoid main() {\n    gl_Position = u_matrix * vec4(a_pos, 0, 1);\n}\n",
       .fragment = "uniform vec4 u_color;\n\nvoid main() {\n    gl_FragColor = u_color;\n}\n",
   },
   [LINE_SHADER] = {
       .vertex = "// these are the shaders for rendering antialiased lines\n\n// floor(127 / 2) == 63.0\n// the maximum allowed miter limit is 2.0 at the moment. the extrude normal is\n// stored in a byte (-128..127). we scale regular normals up to length 63, but\n// there are also \"special\" normals that have a bigger length (of up to 126 in\n// this case).\n#define scale 63.0\n\nattribute vec2 a_pos;\nattribute vec2 a_extrude;\nattribute float a_linesofar;\n\n// matrix is for the vertex position, exmatrix is for rotating and projecting\n// the extrusion vector.\nuniform mat4 u_matrix;\nuniform mat4 u_exmatrix;\n\n\nuniform float u_debug;\n\n// shared\nuniform float u_ratio;\nuniform vec2 u_linewidth;\nuniform vec4 u_color;\nuniform float u_point;\n\nvarying vec2 v_normal;\nvarying float v_linesofar;\n\nvoid main() {\n    // We store the texture normals in the most insignificant bit\n    // transform y so that 0 => -1 and 1 => 1\n    // In the texture normal, x is 0 if the normal points straight up/down and 1 if it's a round cap\n    // y is 1 if the normal points up, and -1 if it points down\n    vec2 normal = mod(a_pos, 2.0);\n    normal.y = sign(normal.y - 0.5);\n    v_normal = normal;\n\n    // Scale the extrusion vector down to a normal and then up by the line width\n    // of this vertex.\n    vec2 extrude = a_extrude / scale;\n    vec2 dist = u_linewidth.s * extrude * (1.0 - u_point);\n\n    // If the x coordinate is the maximum integer, we move the z coordinates out\n    // of the view plane so that the triangle gets clipped. This makes it easier\n    // for us to create degenerate triangle strips.\n    float z = step(32767.0, a_pos.x);\n\n    // When drawing points, skip every other vertex\n    z += u_point * step(1.0, v_normal.y);\n\n    // Remove the texture normal bit of the position before scaling it with the\n    // model/view matrix. Add the extrusion vector *after* the model/view matrix\n    // because we're extruding the line in pixel space, regardless of the current\n    // tile's zoom level.\n    gl_Position = u_matrix * vec4(floor(a_pos / 2.0), 0.0, 1.0) + u_exmatrix * vec4(dist, z, 0.0);\n    v_linesofar = a_linesofar * u_ratio;\n\n\n    gl_PointSize = 2.0 * u_linewidth.s - 1.0;\n}\n",
       .fragment = "#version 120\n// shared\nuniform float u_debug;\nuniform vec2 u_linewidth;\nuniform vec4 u_color;\nuniform float u_point;\nuniform float u_gamma;\n\nuniform vec2 u_dasharray;\n\nvarying vec2 v_normal;\nvarying float v_linesofar;\n\nvoid main() {\n    // Calculate the distance of the pixel from the line in pixels.\n    float dist = length(v_normal) * (1.0 - u_point) + u_point * length(gl_PointCoord * 2.0 - 1.0);\n\n    dist *= u_linewidth.s;\n\n    // Calculate the antialiasing fade factor. This is either when fading in\n    // the line in case of an offset line (v_linewidth.t) or when fading out\n    // (v_linewidth.s)\n    float alpha = clamp(min(dist - (u_linewidth.t - 1.0), u_linewidth.s - dist) * u_gamma, 0.0, 1.0);\n\n    // Calculate the antialiasing fade factor based on distance to the dash.\n    // Only affects alpha when line is dashed\n    float pos = mod(v_linesofar, u_dasharray.x + u_dasharray.y);\n    alpha *= max(step(0.0, -u_dasharray.y), clamp(min(pos, u_dasharray.x - pos), 0.0, 1.0));\n\n    gl_FragColor = u_color * alpha;\n\n    if (u_debug > 0.0) {\n        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n    }\n}\n",
   },
   [OUTLINE_SHADER] = {
       .vertex = "attribute vec2 a_pos;\nuniform mat4 u_matrix;\nuniform vec2 u_world;\n\nvarying vec2 v_pos;\n\nvoid main() {\n    // If the x coordinate is the maximum integer, we move the z coordinates out\n    // of the view plane so that the triangle gets clipped. This makes it easier\n    // for us to create degenerate triangle strips.\n    float z = step(32767.0, a_pos.x) * 2.0;\n    gl_Position = u_matrix * vec4(a_pos, z, 1);\n\n    v_pos = (gl_Position.xy + 1.0) / 2.0 * u_world;\n}\n",
       .fragment = "uniform vec4 u_color;\n\nvarying vec2 v_pos;\n\nvoid main() {\n    float dist = length(v_pos - gl_FragCoord.xy);\n    float alpha = smoothstep(1.0, 0.0, dist);\n    gl_FragColor = u_color * alpha;\n}\n",
   },
   [PLAIN_SHADER] = {
       .vertex = "attribute vec2 a_pos;\n\nuniform mat4 u_matrix;\n\nvoid main() {\n    float z = step(32767.0, a_pos.x) * 2.0;\n    gl_Position = u_matrix * vec4(a_pos, z, 1);\n}\n",
       .fragment = "uniform vec4 u_color;\n\nvoid main() {\n    gl_FragColor = u_color;\n}\n",
   }
};
