#version 400
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexVelocity;
layout (location = 2) in float VertexAge;

// Output to fragment shader
out float Transp;
out vec2 TexCoord;

// Output to transform feedback
out vec3 Position;
out vec3 Velocity;
out float Age;

uniform int Pass;

uniform float Time;                      // Simulation time
uniform float DeltaT;                    // Elapsed time between frames
uniform vec3 Accel;                      // Particle acceleration
uniform float ParticleLifetime;          // Max age for a particle
uniform float ParticleSize;              // Size of each particle.
uniform mat3 EmitterBasis;
uniform vec3 Emitter;

uniform sampler1D RandomTex;

// Transforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

vec3 randomInitialVelocity() {
    float velocity = mix(0.0, 10.0, texelFetch(RandomTex, 2 * gl_VertexID, 0).r );
    return EmitterBasis * vec3(0, velocity, 0);
}

vec3 randomInitialPosition() {
    float offsetX = mix(-50.0, 50.0, texelFetch(RandomTex, 2 * gl_VertexID + 0, 0).r);
    float offsetZ = mix(-50.0, 50.0, texelFetch(RandomTex, 2 * gl_VertexID + 1, 0).r);
	return Emitter + vec3(offsetX, 0, offsetZ);
}

// Offsets to the position in camera coordinates for each vertex of the particle's quad
const vec3 offsets[] = vec3[](vec3(-0.5,-0.5,0), vec3(0.5,-0.5,0), vec3(0.5,0.5,0),
                              vec3(-0.5,-0.5,0), vec3(0.5,0.5,0), vec3(-0.5,0.5,0) );

// Texture coordinates for each vertex of the particle's quad
const vec2 texCoords[] = vec2[](vec2(0,0), vec2(1,0), vec2(1,1), vec2(0,0), vec2(1,1), vec2(0,1));

void main()
{
	if(Pass == 1){
		Age = VertexAge + DeltaT;

	    if( VertexAge < 0 || VertexAge > ParticleLifetime ) {
	        // The particle is past it's lifetime (or not born yet)
	        Position = randomInitialPosition();
	        Velocity = randomInitialVelocity();
	        if(VertexAge > ParticleLifetime)
	        	Age = (VertexAge - ParticleLifetime) + DeltaT;
	    } else {
	        // The particle is alive, update.
	        Position = VertexPosition + VertexVelocity * DeltaT;
	        Velocity = VertexVelocity + Accel * DeltaT;
	    }
	}
	else{
		Transp = 0.0;
	    if(VertexAge >= 0.0) {
	        gl_Position = projection * vec4((view * model * vec4(VertexPosition,1)).xyz + offsets[gl_VertexID] * ParticleSize, 1.0);
	        if( VertexAge < 0 ) 
	        	Transp = 0.0;
	        else
	        	Transp = clamp(1.0 - VertexAge / ParticleLifetime, 0, 1);
	    }
	    TexCoord = texCoords[gl_VertexID];
	}
}