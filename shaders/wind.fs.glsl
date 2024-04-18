#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float darken_screen_factor;

in vec2 texcoord;

layout(location = 0) out vec4 color;

vec2 distort(vec2 uv) 
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE THE WIND DISTORTION HERE (you may want to try sin/cos)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    /*
	float distortionX = sin(time * 0.5) * 0.005; // sin works for horizontal oscillations, time allows for distinct distortions per iteration
    float distortionY = cos(time * 0.5) * 0.005; // cos works for vertical oscillations, ^^
    
    // apply distortion to x and y coordinates
    uv.x += distortionX;
    uv.y += distortionY;

    // scaled down to 90% along the x and y axis
    uv = uv * vec2(0.90, 0.90);

    // strictly keeps the coordinates within the specified 0,1 range
    uv = clamp(uv, 0.0, 1.0); 
    */


	return uv;
}

vec4 color_shift(vec4 in_color) 
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE THE COLOR SHIFTING HERE (you may want to make it blue-ish)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	in_color.r += 0.1; // red goes up
    in_color.g += 0.1; // green goes up
    // we don't change the blue since we want to stay away from it
    // restricts the color range from [0, 1] so we don't go entirely yellow
    in_color = clamp(in_color, 0.0, 1.0);

	return in_color;
}

vec4 fade_color(vec4 in_color) 
{
	if (darken_screen_factor > 0)
		in_color -= darken_screen_factor * vec4(0.8, 0.8, 0.8, 0);
	return in_color;
}

void main()
{
	vec2 coord = distort(texcoord);

    vec4 in_color = texture(screen_texture, coord);
    color = color_shift(in_color);
    color = fade_color(color);
}