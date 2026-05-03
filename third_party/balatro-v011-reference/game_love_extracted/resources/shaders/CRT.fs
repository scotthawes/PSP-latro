vec4 effect(vec4 color, Image tex, vec2 tc, vec2 pc) {
    return vec4(0.0);
}

#ifdef VERTEX
vec4 position(mat4 transform_projection, vec4 vertex_position) {
    return transform_projection * vertex_position;
}
#endif