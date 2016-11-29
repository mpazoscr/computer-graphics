uniform sampler2D SceneTexture;

const float blurSize = 1.0 / 200.0;

void main( void )
{
   vec4 sum = vec4( 0.0 );

   sum += texture2D( SceneTexture, vec2( gl_TexCoord[0].s - 4.0 * blurSize	, gl_TexCoord[0].t )) * 0.05;
   sum += texture2D( SceneTexture, vec2( gl_TexCoord[0].s - 3.0 * blurSize	, gl_TexCoord[0].t )) * 0.09;
   sum += texture2D( SceneTexture, vec2( gl_TexCoord[0].s - 2.0 * blurSize	, gl_TexCoord[0].t )) * 0.12;
   sum += texture2D( SceneTexture, vec2( gl_TexCoord[0].s - blurSize		, gl_TexCoord[0].t )) * 0.15;
   sum += texture2D( SceneTexture, vec2( gl_TexCoord[0].s					, gl_TexCoord[0].t )) * 0.16;
   sum += texture2D( SceneTexture, vec2( gl_TexCoord[0].s + blurSize		, gl_TexCoord[0].t )) * 0.15;
   sum += texture2D( SceneTexture, vec2( gl_TexCoord[0].s + 2.0 * blurSize	, gl_TexCoord[0].t )) * 0.12;
   sum += texture2D( SceneTexture, vec2( gl_TexCoord[0].s + 3.0 * blurSize	, gl_TexCoord[0].t )) * 0.09;
   sum += texture2D( SceneTexture, vec2( gl_TexCoord[0].s + 4.0 * blurSize	, gl_TexCoord[0].t )) * 0.05;

   gl_FragColor = sum;
}
