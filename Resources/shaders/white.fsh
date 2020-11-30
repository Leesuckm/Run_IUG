
varying vec4 v_fragmentColor;	
varying vec2 v_texCoord;	
		
void main()			
{
	vec4 v_orColor = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);
	float gray = dot(v_orColor.rgb, vec3(1.0, 1.0, 1.0));
	gl_FragColor = vec4(gray, gray, gray, v_orColor.a);
}				
