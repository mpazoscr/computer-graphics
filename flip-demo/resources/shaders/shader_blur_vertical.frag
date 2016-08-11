uniform sampler2D HorizBlurTexture;

const float blurSize = 1.0 / 200.0;

void main( void )
{
   vec4 sum = vec4( 0.0 );

   sum += texture2D( HorizBlurTexture, vec2( gl_TexCoord[0].s, gl_TexCoord[0].t - 4.0 * blurSize	)) * 0.05;
   sum += texture2D( HorizBlurTexture, vec2( gl_TexCoord[0].s, gl_TexCoord[0].t - 3.0 * blurSize	)) * 0.09;
   sum += texture2D( HorizBlurTexture, vec2( gl_TexCoord[0].s, gl_TexCoord[0].t - 2.0 * blurSize	)) * 0.12;
   sum += texture2D( HorizBlurTexture, vec2( gl_TexCoord[0].s, gl_TexCoord[0].t - blurSize			)) * 0.15;
   sum += texture2D( HorizBlurTexture, vec2( gl_TexCoord[0].s, gl_TexCoord[0].t						)) * 0.16;
   sum += texture2D( HorizBlurTexture, vec2( gl_TexCoord[0].s, gl_TexCoord[0].t + blurSize			)) * 0.15;
   sum += texture2D( HorizBlurTexture, vec2( gl_TexCoord[0].s, gl_TexCoord[0].t + 2.0 * blurSize	)) * 0.12;
   sum += texture2D( HorizBlurTexture, vec2( gl_TexCoord[0].s, gl_TexCoord[0].t + 3.0 * blurSize	)) * 0.09;
   sum += texture2D( HorizBlurTexture, vec2( gl_TexCoord[0].s, gl_TexCoord[0].t + 4.0 * blurSize	)) * 0.05;

   gl_FragColor = sum;
}
