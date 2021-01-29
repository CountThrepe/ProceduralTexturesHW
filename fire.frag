//  Basic vertex shader
#version 120

uniform float time;
uniform vec3 origin;
uniform vec3 coeffs;

varying vec3 color;
varying float lightIntensity;
varying vec3 worldPos;

const float PI = 3.1415926535897932384626433832795;
const vec3 burnedColor = vec3(0.1, 0.1, 0.1);
const vec3 fireColor = vec3(1, 0.65, 0.15);
const float fireWidth = 0.06;
const float charWidth = 0.2;

/* This shader is meant to simulate burning, similar to when a piece of paper burns up. I made a function that describes the edge
 * of a complex 3d object, with lots of local noise provided by sin and cos functions. If a fragment is inside the edge, it is
 * discarded. If a fragment is outside but close to the edge, it is colored to create a burning effect. This edge grows with the
 * uniform time variable, so it consumes the object as time progresses. The origin of the burn is also passed, along with some
 * parts for the function that are calculated by the cpu for efficiency.
*/

void main() {
   if(time > 0) {
      // Get polar coordinates based around the origin of the fire
      vec3 pos = worldPos - origin;
      float r = length(pos);
      float theta = acos(pos.z / r);
      float phi = acos(pos.x / length(pos.xy));
      if(pos.y < 0) phi = 2 * PI - phi;

      // Determine the edge of the fire
      float phiInfluence = clamp((PI - theta - 0.4), 0, 1);
      vec3 vars = vec3(sin((11 * phi) + time) * phiInfluence, sin((6 * phi) + (2 * time)) * phiInfluence, sin(6 * theta));
      float edge = time + dot(coeffs, vars);

      // If we're inside the edge, discard. Otherwise, mix colors to create the burn effect.
      float distFromEdge = r - edge;
      if(distFromEdge < 0) discard;
      else {
         // Do lighting for base and char color but not for the fire itself, to give an appearance of producing it's own light.
         vec3 baseColor = color * lightIntensity;
         vec3 charColor = burnedColor * lightIntensity;
         gl_FragColor = vec4(mix(mix(fireColor, charColor, clamp(distFromEdge / fireWidth, 0, 1)), baseColor, clamp((distFromEdge - fireWidth) / charWidth, 0, 1)), 0);
      }
   } else {
      gl_FragColor = vec4(color * lightIntensity, 0);
   }
}
