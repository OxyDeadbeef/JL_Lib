/* Lib Aldaron --- Copyright (c) 2016 Jeron A. Lau */
/* This file must be distributed with the GNU LESSER GENERAL PUBLIC LICENSE. */
/* DO NOT REMOVE THIS NOTICE */

#include <la_config.h>
#ifdef LA_FEATURE_DISPLAY

#include <la_draw.h>
#include "la_opengl__.h"

#include "la_memory.h"

#include <la_effect.h>
#include <la_ro.h>
#include <la_llgraphics.h>

extern float la_banner_size;

const char *LA_EFFECT_SHADOW = 
	GLSL_HEAD
	"uniform sampler2D texture;\n"
	"\n"
	"varying vec2 texcoord;\n"
	"\n"
	"void main() {\n"
	"	gl_FragColor = vec4(0., 0., 0., texture2D(texture, texcoord).a);\n"
	"}";

const char* LA_EFFECT_ALPHA = 
	GLSL_HEAD
	"uniform sampler2D texture;\n"
	"uniform float multiply_alpha;\n"
	"\n"
	"varying vec2 texcoord;\n"
	"\n"
	"void main() {\n"
	"	vec4 vcolor = texture2D(texture, texcoord);\n"
	"	gl_FragColor = vec4(vcolor.rgb, vcolor.a * multiply_alpha);\n"
	"}";

const char* LA_EFFECT_HUE = 
	GLSL_HEAD
	"uniform sampler2D texture;\n"
	"uniform vec4 new_color;\n"
	"\n"
	"varying vec2 texcoord;\n"
	"\n"
	"void main() {\n"
	"	vec4 vcolor = texture2D(texture, texcoord);\n"
	"	mediump float grayscale = vcolor.r + vcolor.g + vcolor.b;\n"
	"	grayscale = grayscale / 3.0;\n"
	"	gl_FragColor = \n"
	"		vec4(new_color.r * grayscale, new_color.g * grayscale,"
	"		new_color.b * grayscale, new_color.a * vcolor.a);\n"
	"}";

// Simple Point Light.
const char* LA_EFFECT_LIGHT_AA =
	GLSL_HEAD
	"#define NUM_LIGHTS %d\n"
	"\n"
	"uniform sampler2D texture;\n"
	"varying vec2 texcoord;\n"
	"varying vec3 fragpos;\n"
	"\n"
	// Material
	"uniform vec3 brightness;\n"
	"\n"
	// Light
	"uniform vec3 color[NUM_LIGHTS];\n"
	"uniform vec3 where[NUM_LIGHTS];\n"
	"uniform float power[NUM_LIGHTS];\n"
	"\n"
	"void main() {\n"
	"	vec3 light = vec3(0.0, 0.0, 0.0);\n"
	"	for(int i = 0; i < NUM_LIGHTS; i++) light += vec3(min(pow(power[i] * length(where[i] - fragpos), -2.0), 1.0) * brightness * color[i]);\n"
	"	gl_FragColor = vec4(light, 1.0) * texture2D(texture, texcoord);\n"
	"}";

const char* LA_EFFECT_LIGHTV =
	GLSL_HEAD
	"uniform mat4 scale_object;\n"
	"uniform mat4 rotate_object;\n"
	"uniform mat4 translate_object;\n"
	"uniform mat4 rotate_camera;\n"
	"uniform mat4 project_scene;\n"
	"uniform vec3 offset;\n"
	"\n"
	"attribute vec4 position;\n"
	"attribute vec2 texpos;\n"
	"\n"
	"varying vec2 texcoord;\n"
	"varying vec3 fragpos;\n"
	"\n"
	"void main() {\n"
	"	texcoord = texpos;\n"
	"	vec4 pos = translate_object * rotate_object * scale_object *\n"
	"		position;\n"
	"	fragpos = offset + position.xyz;\n"
	"	gl_Position = project_scene * rotate_camera * pos;\n"
	"}";

static inline void la_effect_format__(char* rtn, const char* format, ...) {
	va_list arglist;

	va_start( arglist, format );
	vsnprintf( rtn, strlen(format) + 128, format, arglist );
	va_end( arglist );
}

void la_effect_fade(la_ro_t* ro, float a) {
	la_window_t* window = ro->window;

	if(a < 0.f) a = 0.f;
	if(a > 1.f) a = 1.f;

	// Bind shader
	la_llgraphics_shader_bind(&window->effect.alpha);
	// Set Alpha Value In Shader
	la_llgraphics_uniformf(window, &window->effect.alpha, &a, 1,
		"multiply_alpha", -1);
	// Draw on screen
	la_ro_draw_shader(ro, &window->effect.alpha);
}

void la_effect_hue(la_ro_t* ro, float c[]) {
	la_window_t* window = ro->window;

	// Bind shader
	la_llgraphics_shader_bind(&window->effect.hue);
	// Set Hue Value In Shader
	la_llgraphics_uniformf(window, &window->effect.hue, c, 4,
		"new_color", -1);
	// Draw on screen
	la_ro_draw_shader(ro, &window->effect.hue);
}

void la_effect_light(la_ro_t* ro, la_light_t* lights, uint8_t light_count,
	la_v3_t material_brightness)
{
	la_window_t* window = ro->window;
	int i;
	la_shader_t* shader = &window->effect.shader_laa[light_count];

	// Create a shader if doesn't exist.
	if(!window->effect.shader_laa_init[light_count]) {
		if(light_count) {
			char frag_code[strlen(LA_EFFECT_LIGHT_AA) + 128];

			la_effect_format__(frag_code, LA_EFFECT_LIGHT_AA, light_count);
			la_llgraphics_shader_make(shader, LA_EFFECT_LIGHTV,
				frag_code, 1);
		}else{
			la_llgraphics_shader_make(shader, NULL,
				LA_EFFECT_SHADOW, 1);
		}
		window->effect.shader_laa_init[light_count] = 1;
	}
	// Bind shader
	la_llgraphics_shader_bind(shader);
	// Update uniforms for material.
	for(i = 0; i < light_count; i++) {
		float power = 1.f / lights[i].power;
		// Push the uniform
		la_llgraphics_uniformf(window, shader, (float*)&lights[i].position,
			3, "where", i);
		la_llgraphics_uniformf(window, shader, (float*)&lights[i].color,
			3, "color", i);
		la_llgraphics_uniformf(window, shader, (float*)&power,
			1, "power", i);
	}
	if(light_count) {
		la_llgraphics_uniformf(window, shader,
			(float*)&material_brightness, 3, "brightness", -1);
		la_llgraphics_uniformf(window, shader, (float*)&ro->cb.pos, 3,
			"offset", -1);
	}
	// Draw on screen
	la_ro_draw_shader(ro, shader);
}

void la_effects_init__(la_window_t* window) {
	la_print("MAKING EFFECT: ALPHA");
	la_llgraphics_shader_make(&window->effect.alpha, NULL, LA_EFFECT_ALPHA, 1);
	la_print("MAKING EFFECT: HUE");
	la_llgraphics_shader_make(&window->effect.hue, NULL, LA_EFFECT_HUE, 1);
	la_print("MAKING EFFECT: SHADOW");
	la_llgraphics_shader_make(&window->effect.shadow, NULL, LA_EFFECT_SHADOW, 1);
	la_print("MADE EFFECTS!");
}

#endif
