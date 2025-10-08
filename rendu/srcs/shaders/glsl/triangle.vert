#version 450
layout(row_major) uniform;
layout(row_major) buffer;

#line 27 0
void _S1(out vec2  _S2[3])
{

#line 5
    const vec2 _S3 = vec2(0.5, 0.5);
    const vec2 _S4 = vec2(-0.5, 0.5);

#line 6
    _S2[0] = vec2(0.0, -0.5);

#line 6
    _S2[1] = _S3;

#line 6
    _S2[2] = _S4;

#line 6
    return;
}


#line 3
vec2  positions_0[3];


#line 28
void _S5(out vec3  _S6[3])
{

#line 11
    const vec3 _S7 = vec3(0.0, 1.0, 0.0);
    const vec3 _S8 = vec3(0.0, 0.0, 1.0);

#line 12
    _S6[0] = vec3(1.0, 0.0, 0.0);

#line 12
    _S6[1] = _S7;

#line 12
    _S6[2] = _S8;

#line 12
    return;
}


#line 9
vec3  colors_0[3];


#line 15
layout(location = 0)
out vec3 entryPointParam_vertMain_color_0;


#line 15
struct VertexOutput_0
{
    vec4 sv_position_0;
    vec3 color_0;
};



void main()
{

#line 23
    vec2  _S9[3];

#line 23
    _S1(_S9);

#line 23
    positions_0 = _S9;

#line 23
    vec3  _S10[3];

#line 23
    _S5(_S10);

#line 23
    colors_0 = _S10;

#line 23
    uint _S11 = uint(gl_VertexIndex);

    VertexOutput_0 output_0;

    output_0.sv_position_0 = vec4(positions_0[_S11], 0.0, 1.0);
    output_0.color_0 = colors_0[_S11];
    VertexOutput_0 _S12 = output_0;

#line 29
    gl_Position = output_0.sv_position_0;

#line 29
    entryPointParam_vertMain_color_0 = _S12.color_0;

#line 29
    return;
}

