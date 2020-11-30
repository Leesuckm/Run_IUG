
attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec4 a_color;
					

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
								
void main()	
{							
	//cocos2d 에서는 CC_MVPMatrix를 사용하지 않는다.
    //gl_Position = CC_MVPMatrix * a_position;
	gl_Position = CC_PMatrix * a_position;
	v_fragmentColor = a_color;
	v_texCoord = a_texCoord;
}