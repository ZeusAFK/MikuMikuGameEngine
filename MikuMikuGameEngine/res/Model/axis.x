xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 196;
 2.47875;0.02523;-0.02600;,
 2.47875;0.00000;-0.03676;,
 0.02785;0.00000;-0.03676;,
 0.02785;0.02523;-0.02600;,
 2.47875;0.03568;0.00000;,
 0.02785;0.03568;0.00000;,
 2.47875;0.02523;0.02600;,
 0.02785;0.02523;0.02600;,
 2.47875;0.00000;0.03676;,
 0.02785;0.00000;0.03676;,
 2.47875;-0.02523;0.02600;,
 0.02785;-0.02523;0.02600;,
 2.47875;-0.03568;-0.00000;,
 0.02785;-0.03568;-0.00000;,
 2.47875;-0.02523;-0.02600;,
 0.02785;-0.02523;-0.02600;,
 2.47875;0.00000;-0.03676;,
 0.02785;0.00000;-0.03676;,
 2.54600;0.00000;0.00000;,
 2.54600;0.00000;0.00000;,
 2.54600;0.00000;0.00000;,
 2.54600;0.00000;0.00000;,
 2.54600;0.00000;0.00000;,
 2.54600;0.00000;0.00000;,
 2.54600;0.00000;0.00000;,
 2.54600;0.00000;0.00000;,
 -0.02600;0.02523;-2.47875;,
 -0.03676;0.00000;-2.47875;,
 -0.03676;0.00000;-0.02785;,
 -0.02600;0.02523;-0.02785;,
 -0.00000;0.03568;-2.47875;,
 -0.00000;0.03568;-0.02785;,
 0.02600;0.02523;-2.47875;,
 0.02600;0.02523;-0.02785;,
 0.03676;0.00000;-2.47875;,
 0.03676;0.00000;-0.02785;,
 0.02600;-0.02523;-2.47875;,
 0.02600;-0.02523;-0.02785;,
 -0.00000;-0.03568;-2.47875;,
 -0.00000;-0.03568;-0.02785;,
 -0.02600;-0.02523;-2.47875;,
 -0.02600;-0.02523;-0.02785;,
 -0.03676;0.00000;-2.47875;,
 -0.03676;0.00000;-0.02785;,
 -0.00000;0.00000;-2.54600;,
 -0.00000;0.00000;-2.54600;,
 -0.00000;0.00000;-2.54600;,
 -0.00000;0.00000;-2.54600;,
 -0.00000;0.00000;-2.54600;,
 -0.00000;0.00000;-2.54600;,
 -0.00000;0.00000;-2.54600;,
 -0.00000;0.00000;-2.54600;,
 -0.02523;2.47875;-0.02600;,
 -0.00000;2.47875;-0.03676;,
 -0.00000;0.02785;-0.03676;,
 -0.02523;0.02785;-0.02600;,
 -0.03568;2.47875;0.00000;,
 -0.03568;0.02785;0.00000;,
 -0.02523;2.47875;0.02600;,
 -0.02523;0.02785;0.02600;,
 -0.00000;2.47875;0.03676;,
 -0.00000;0.02785;0.03676;,
 0.02523;2.47875;0.02600;,
 0.02523;0.02785;0.02600;,
 0.03568;2.47875;0.00000;,
 0.03568;0.02785;0.00000;,
 0.02523;2.47875;-0.02600;,
 0.02523;0.02785;-0.02600;,
 -0.00000;2.47875;-0.03676;,
 -0.00000;0.02785;-0.03676;,
 -0.00000;2.54600;0.00000;,
 -0.00000;2.54600;0.00000;,
 -0.00000;2.54600;0.00000;,
 -0.00000;2.54600;0.00000;,
 -0.00000;2.54600;0.00000;,
 -0.00000;2.54600;0.00000;,
 -0.00000;2.54600;0.00000;,
 -0.00000;2.54600;0.00000;,
 -0.11770;0.11770;-0.11653;,
 0.11770;0.11770;-0.11653;,
 0.11770;-0.11770;-0.11653;,
 -0.11770;-0.11770;-0.11653;,
 0.11770;0.11770;-0.11653;,
 0.11770;0.11770;0.11653;,
 0.11770;-0.11770;0.11653;,
 0.11770;-0.11770;-0.11653;,
 0.11770;0.11770;0.11653;,
 -0.11770;0.11770;0.11653;,
 -0.11770;-0.11770;0.11653;,
 0.11770;-0.11770;0.11653;,
 -0.11770;0.11770;0.11653;,
 -0.11770;0.11770;-0.11653;,
 -0.11770;-0.11770;-0.11653;,
 -0.11770;-0.11770;0.11653;,
 0.11770;0.11770;-0.11653;,
 -0.11770;0.11770;-0.11653;,
 -0.11770;-0.11770;-0.11653;,
 0.11770;-0.11770;-0.11653;,
 -0.13669;0.13669;-0.13533;,
 -0.13669;-0.13669;-0.13533;,
 0.13669;-0.13669;-0.13533;,
 0.13669;0.13669;-0.13533;,
 0.13669;0.13669;-0.13533;,
 0.13669;-0.13669;-0.13533;,
 0.13669;-0.13669;0.13533;,
 0.13669;0.13669;0.13533;,
 0.13669;0.13669;0.13533;,
 0.13669;-0.13669;0.13533;,
 -0.13669;-0.13669;0.13533;,
 -0.13669;0.13669;0.13533;,
 -0.13669;0.13669;0.13533;,
 -0.13669;-0.13669;0.13533;,
 -0.13669;-0.13669;-0.13533;,
 -0.13669;0.13669;-0.13533;,
 -0.13669;0.13669;-0.13533;,
 0.13669;0.13669;-0.13533;,
 -0.13669;-0.13669;-0.13533;,
 0.13669;-0.13669;-0.13533;,
 2.50885;0.03932;-0.04052;,
 -0.00394;0.03932;-0.04052;,
 -0.00394;0.00000;-0.05731;,
 2.50885;0.00000;-0.05731;,
 2.50885;0.05561;0.00000;,
 -0.00394;0.05561;0.00000;,
 2.50885;0.03932;0.04052;,
 -0.00394;0.03932;0.04052;,
 2.50885;0.00000;0.05731;,
 -0.00394;0.00000;0.05731;,
 2.50885;-0.03932;0.04052;,
 -0.00394;-0.03932;0.04052;,
 2.50885;-0.05561;-0.00000;,
 -0.00394;-0.05561;0.00000;,
 2.50885;-0.03932;-0.04052;,
 -0.00394;-0.03932;-0.04052;,
 2.50885;0.00000;-0.05731;,
 -0.00394;0.00000;-0.05731;,
 2.57779;0.00000;0.00000;,
 2.57779;0.00000;0.00000;,
 2.57779;0.00000;0.00000;,
 2.57779;0.00000;0.00000;,
 2.57779;0.00000;0.00000;,
 2.57779;0.00000;0.00000;,
 2.57779;0.00000;0.00000;,
 2.57779;0.00000;0.00000;,
 -0.04052;0.03932;-2.50885;,
 -0.04052;0.03932;0.00394;,
 -0.05731;0.00000;0.00394;,
 -0.05731;0.00000;-2.50885;,
 -0.00000;0.05561;-2.50885;,
 0.00000;0.05561;0.00394;,
 0.04052;0.03932;-2.50885;,
 0.04052;0.03932;0.00394;,
 0.05731;0.00000;-2.50885;,
 0.05731;0.00000;0.00394;,
 0.04052;-0.03932;-2.50885;,
 0.04052;-0.03932;0.00394;,
 -0.00000;-0.05561;-2.50885;,
 0.00000;-0.05561;0.00394;,
 -0.04052;-0.03932;-2.50885;,
 -0.04052;-0.03932;0.00394;,
 -0.05731;0.00000;-2.50885;,
 -0.05731;0.00000;0.00394;,
 -0.00000;0.00000;-2.57779;,
 -0.00000;0.00000;-2.57779;,
 -0.00000;0.00000;-2.57779;,
 -0.00000;0.00000;-2.57779;,
 -0.00000;0.00000;-2.57779;,
 -0.00000;0.00000;-2.57779;,
 -0.00000;0.00000;-2.57779;,
 -0.00000;0.00000;-2.57779;,
 -0.03932;2.50885;-0.04052;,
 -0.03932;-0.00394;-0.04052;,
 0.00000;-0.00394;-0.05731;,
 -0.00000;2.50885;-0.05731;,
 -0.05561;2.50885;0.00000;,
 -0.05561;-0.00394;0.00000;,
 -0.03932;2.50885;0.04052;,
 -0.03932;-0.00394;0.04052;,
 -0.00000;2.50885;0.05731;,
 0.00000;-0.00394;0.05731;,
 0.03932;2.50885;0.04052;,
 0.03932;-0.00394;0.04052;,
 0.05561;2.50885;0.00000;,
 0.05561;-0.00394;-0.00000;,
 0.03932;2.50885;-0.04052;,
 0.03932;-0.00394;-0.04052;,
 -0.00000;2.50885;-0.05731;,
 0.00000;-0.00394;-0.05731;,
 -0.00000;2.57779;0.00000;,
 -0.00000;2.57779;0.00000;,
 -0.00000;2.57779;0.00000;,
 -0.00000;2.57779;0.00000;,
 -0.00000;2.57779;0.00000;,
 -0.00000;2.57779;0.00000;,
 -0.00000;2.57779;0.00000;,
 -0.00000;2.57779;0.00000;;
 
 108;
 4;0,1,2,3;,
 4;4,0,3,5;,
 4;6,4,5,7;,
 4;8,6,7,9;,
 4;10,8,9,11;,
 4;12,10,11,13;,
 4;14,12,13,15;,
 4;16,14,15,17;,
 3;18,0,4;,
 3;19,4,6;,
 3;20,6,8;,
 3;21,8,10;,
 3;22,10,12;,
 3;23,12,14;,
 3;24,1,0;,
 3;25,14,16;,
 4;26,27,28,29;,
 4;30,26,29,31;,
 4;32,30,31,33;,
 4;34,32,33,35;,
 4;36,34,35,37;,
 4;38,36,37,39;,
 4;40,38,39,41;,
 4;42,40,41,43;,
 3;44,26,30;,
 3;45,30,32;,
 3;46,32,34;,
 3;47,34,36;,
 3;48,36,38;,
 3;49,38,40;,
 3;50,27,26;,
 3;51,40,42;,
 4;52,53,54,55;,
 4;56,52,55,57;,
 4;58,56,57,59;,
 4;60,58,59,61;,
 4;62,60,61,63;,
 4;64,62,63,65;,
 4;66,64,65,67;,
 4;68,66,67,69;,
 3;70,52,56;,
 3;71,56,58;,
 3;72,58,60;,
 3;73,60,62;,
 3;74,62,64;,
 3;75,64,66;,
 3;76,53,52;,
 3;77,66,68;,
 4;78,79,80,81;,
 4;82,83,84,85;,
 4;86,87,88,89;,
 4;90,91,92,93;,
 4;90,83,94,95;,
 4;96,97,84,93;,
 4;98,99,100,101;,
 4;102,103,104,105;,
 4;106,107,108,109;,
 4;110,111,112,113;,
 4;110,114,115,105;,
 4;116,111,104,117;,
 4;118,119,120,121;,
 4;122,123,119,118;,
 4;124,125,123,122;,
 4;126,127,125,124;,
 4;128,129,127,126;,
 4;130,131,129,128;,
 4;132,133,131,130;,
 4;134,135,133,132;,
 3;136,122,118;,
 3;137,124,122;,
 3;138,126,124;,
 3;139,128,126;,
 3;140,130,128;,
 3;141,132,130;,
 3;142,118,121;,
 3;143,134,132;,
 4;144,145,146,147;,
 4;148,149,145,144;,
 4;150,151,149,148;,
 4;152,153,151,150;,
 4;154,155,153,152;,
 4;156,157,155,154;,
 4;158,159,157,156;,
 4;160,161,159,158;,
 3;162,148,144;,
 3;163,150,148;,
 3;164,152,150;,
 3;165,154,152;,
 3;166,156,154;,
 3;167,158,156;,
 3;168,144,147;,
 3;169,160,158;,
 4;170,171,172,173;,
 4;174,175,171,170;,
 4;176,177,175,174;,
 4;178,179,177,176;,
 4;180,181,179,178;,
 4;182,183,181,180;,
 4;184,185,183,182;,
 4;186,187,185,184;,
 3;188,174,170;,
 3;189,176,174;,
 3;190,178,176;,
 3;191,180,178;,
 3;192,182,180;,
 3;193,184,182;,
 3;194,170,173;,
 3;195,186,184;;
 
 MeshMaterialList {
  6;
  108;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  3,
  3,
  3,
  3,
  3,
  3,
  4,
  4,
  4,
  4,
  4,
  4,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5,
  5;;
  Material {
   1.000000;0.000000;0.035294;1.000000;;
   5.230000;
   0.000000;0.000000;0.000000;;
   1.000000;0.000000;0.035294;;
  }
  Material {
   0.000000;0.043137;1.000000;1.000000;;
   5.230000;
   0.000000;0.000000;0.000000;;
   0.000000;0.043137;1.000000;;
  }
  Material {
   0.039216;1.000000;0.000000;1.000000;;
   5.230000;
   0.000000;0.000000;0.000000;;
   0.039216;1.000000;0.000000;;
  }
  Material {
   0.996078;1.000000;0.000000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.996078;1.000000;0.000000;;
  }
  Material {
   0.000000;0.000000;0.000000;1.000000;;
   0.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
  Material {
   0.000000;0.000000;0.000000;1.000000;;
   0.000000;
   0.000000;0.000000;0.000000;;
   1.000000;1.000000;1.000000;;
  }
 }
 MeshTextureCoords {
  196;
  0.125000;0.927060;,
  0.000000;0.927060;,
  0.000000;0.000000;,
  0.125000;0.000000;,
  0.250000;0.927060;,
  0.250000;0.000000;,
  0.375000;0.927060;,
  0.375000;0.000000;,
  0.500000;0.927060;,
  0.500000;0.000000;,
  0.625000;0.927060;,
  0.625000;0.000000;,
  0.750000;0.927060;,
  0.750000;0.000000;,
  0.875000;0.927060;,
  0.875000;0.000000;,
  1.000000;0.927060;,
  1.000000;0.000000;,
  0.187500;1.000000;,
  0.312500;1.000000;,
  0.437500;1.000000;,
  0.562500;1.000000;,
  0.687500;1.000000;,
  0.812500;1.000000;,
  0.062500;1.000000;,
  0.937500;1.000000;,
  0.125000;0.927060;,
  0.000000;0.927060;,
  0.000000;0.000000;,
  0.125000;0.000000;,
  0.250000;0.927060;,
  0.250000;0.000000;,
  0.375000;0.927060;,
  0.375000;0.000000;,
  0.500000;0.927060;,
  0.500000;0.000000;,
  0.625000;0.927060;,
  0.625000;0.000000;,
  0.750000;0.927060;,
  0.750000;0.000000;,
  0.875000;0.927060;,
  0.875000;0.000000;,
  1.000000;0.927060;,
  1.000000;0.000000;,
  0.187500;1.000000;,
  0.312500;1.000000;,
  0.437500;1.000000;,
  0.562500;1.000000;,
  0.687500;1.000000;,
  0.812500;1.000000;,
  0.062500;1.000000;,
  0.937500;1.000000;,
  0.125000;0.927060;,
  0.000000;0.927060;,
  0.000000;0.000000;,
  0.125000;0.000000;,
  0.250000;0.927060;,
  0.250000;0.000000;,
  0.375000;0.927060;,
  0.375000;0.000000;,
  0.500000;0.927060;,
  0.500000;0.000000;,
  0.625000;0.927060;,
  0.625000;0.000000;,
  0.750000;0.927060;,
  0.750000;0.000000;,
  0.875000;0.927060;,
  0.875000;0.000000;,
  1.000000;0.927060;,
  1.000000;0.000000;,
  0.187500;1.000000;,
  0.312500;1.000000;,
  0.437500;1.000000;,
  0.562500;1.000000;,
  0.687500;1.000000;,
  0.812500;1.000000;,
  0.062500;1.000000;,
  0.937500;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.125000;0.927060;,
  0.125000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.927060;,
  0.250000;0.927060;,
  0.250000;0.000000;,
  0.375000;0.927060;,
  0.375000;0.000000;,
  0.500000;0.927060;,
  0.500000;0.000000;,
  0.625000;0.927060;,
  0.625000;0.000000;,
  0.750000;0.927060;,
  0.750000;0.000000;,
  0.875000;0.927060;,
  0.875000;0.000000;,
  1.000000;0.927060;,
  1.000000;0.000000;,
  0.187500;1.000000;,
  0.312500;1.000000;,
  0.437500;1.000000;,
  0.562500;1.000000;,
  0.687500;1.000000;,
  0.812500;1.000000;,
  0.062500;1.000000;,
  0.937500;1.000000;,
  0.125000;0.927060;,
  0.125000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.927060;,
  0.250000;0.927060;,
  0.250000;0.000000;,
  0.375000;0.927060;,
  0.375000;0.000000;,
  0.500000;0.927060;,
  0.500000;0.000000;,
  0.625000;0.927060;,
  0.625000;0.000000;,
  0.750000;0.927060;,
  0.750000;0.000000;,
  0.875000;0.927060;,
  0.875000;0.000000;,
  1.000000;0.927060;,
  1.000000;0.000000;,
  0.187500;1.000000;,
  0.312500;1.000000;,
  0.437500;1.000000;,
  0.562500;1.000000;,
  0.687500;1.000000;,
  0.812500;1.000000;,
  0.062500;1.000000;,
  0.937500;1.000000;,
  0.125000;0.927060;,
  0.125000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.927060;,
  0.250000;0.927060;,
  0.250000;0.000000;,
  0.375000;0.927060;,
  0.375000;0.000000;,
  0.500000;0.927060;,
  0.500000;0.000000;,
  0.625000;0.927060;,
  0.625000;0.000000;,
  0.750000;0.927060;,
  0.750000;0.000000;,
  0.875000;0.927060;,
  0.875000;0.000000;,
  1.000000;0.927060;,
  1.000000;0.000000;,
  0.187500;1.000000;,
  0.312500;1.000000;,
  0.437500;1.000000;,
  0.562500;1.000000;,
  0.687500;1.000000;,
  0.812500;1.000000;,
  0.062500;1.000000;,
  0.937500;1.000000;;
 }
 MeshVertexColors {
  196;
  0;1.000000;1.000000;1.000000;1.000000;,
  1;1.000000;1.000000;1.000000;1.000000;,
  2;1.000000;1.000000;1.000000;1.000000;,
  3;1.000000;1.000000;1.000000;1.000000;,
  4;1.000000;1.000000;1.000000;1.000000;,
  5;1.000000;1.000000;1.000000;1.000000;,
  6;1.000000;1.000000;1.000000;1.000000;,
  7;1.000000;1.000000;1.000000;1.000000;,
  8;1.000000;1.000000;1.000000;1.000000;,
  9;1.000000;1.000000;1.000000;1.000000;,
  10;1.000000;1.000000;1.000000;1.000000;,
  11;1.000000;1.000000;1.000000;1.000000;,
  12;1.000000;1.000000;1.000000;1.000000;,
  13;1.000000;1.000000;1.000000;1.000000;,
  14;1.000000;1.000000;1.000000;1.000000;,
  15;1.000000;1.000000;1.000000;1.000000;,
  16;1.000000;1.000000;1.000000;1.000000;,
  17;1.000000;1.000000;1.000000;1.000000;,
  18;1.000000;1.000000;1.000000;1.000000;,
  19;1.000000;1.000000;1.000000;1.000000;,
  20;1.000000;1.000000;1.000000;1.000000;,
  21;1.000000;1.000000;1.000000;1.000000;,
  22;1.000000;1.000000;1.000000;1.000000;,
  23;1.000000;1.000000;1.000000;1.000000;,
  24;1.000000;1.000000;1.000000;1.000000;,
  25;1.000000;1.000000;1.000000;1.000000;,
  26;1.000000;1.000000;1.000000;1.000000;,
  27;1.000000;1.000000;1.000000;1.000000;,
  28;1.000000;1.000000;1.000000;1.000000;,
  29;1.000000;1.000000;1.000000;1.000000;,
  30;1.000000;1.000000;1.000000;1.000000;,
  31;1.000000;1.000000;1.000000;1.000000;,
  32;1.000000;1.000000;1.000000;1.000000;,
  33;1.000000;1.000000;1.000000;1.000000;,
  34;1.000000;1.000000;1.000000;1.000000;,
  35;1.000000;1.000000;1.000000;1.000000;,
  36;1.000000;1.000000;1.000000;1.000000;,
  37;1.000000;1.000000;1.000000;1.000000;,
  38;1.000000;1.000000;1.000000;1.000000;,
  39;1.000000;1.000000;1.000000;1.000000;,
  40;1.000000;1.000000;1.000000;1.000000;,
  41;1.000000;1.000000;1.000000;1.000000;,
  42;1.000000;1.000000;1.000000;1.000000;,
  43;1.000000;1.000000;1.000000;1.000000;,
  44;1.000000;1.000000;1.000000;1.000000;,
  45;1.000000;1.000000;1.000000;1.000000;,
  46;1.000000;1.000000;1.000000;1.000000;,
  47;1.000000;1.000000;1.000000;1.000000;,
  48;1.000000;1.000000;1.000000;1.000000;,
  49;1.000000;1.000000;1.000000;1.000000;,
  50;1.000000;1.000000;1.000000;1.000000;,
  51;1.000000;1.000000;1.000000;1.000000;,
  52;1.000000;1.000000;1.000000;1.000000;,
  53;1.000000;1.000000;1.000000;1.000000;,
  54;1.000000;1.000000;1.000000;1.000000;,
  55;1.000000;1.000000;1.000000;1.000000;,
  56;1.000000;1.000000;1.000000;1.000000;,
  57;1.000000;1.000000;1.000000;1.000000;,
  58;1.000000;1.000000;1.000000;1.000000;,
  59;1.000000;1.000000;1.000000;1.000000;,
  60;1.000000;1.000000;1.000000;1.000000;,
  61;1.000000;1.000000;1.000000;1.000000;,
  62;1.000000;1.000000;1.000000;1.000000;,
  63;1.000000;1.000000;1.000000;1.000000;,
  64;1.000000;1.000000;1.000000;1.000000;,
  65;1.000000;1.000000;1.000000;1.000000;,
  66;1.000000;1.000000;1.000000;1.000000;,
  67;1.000000;1.000000;1.000000;1.000000;,
  68;1.000000;1.000000;1.000000;1.000000;,
  69;1.000000;1.000000;1.000000;1.000000;,
  70;1.000000;1.000000;1.000000;1.000000;,
  71;1.000000;1.000000;1.000000;1.000000;,
  72;1.000000;1.000000;1.000000;1.000000;,
  73;1.000000;1.000000;1.000000;1.000000;,
  74;1.000000;1.000000;1.000000;1.000000;,
  75;1.000000;1.000000;1.000000;1.000000;,
  76;1.000000;1.000000;1.000000;1.000000;,
  77;1.000000;1.000000;1.000000;1.000000;,
  78;1.000000;1.000000;1.000000;1.000000;,
  79;1.000000;1.000000;1.000000;1.000000;,
  80;1.000000;1.000000;1.000000;1.000000;,
  81;1.000000;1.000000;1.000000;1.000000;,
  82;1.000000;1.000000;1.000000;1.000000;,
  83;1.000000;1.000000;1.000000;1.000000;,
  84;1.000000;1.000000;1.000000;1.000000;,
  85;1.000000;1.000000;1.000000;1.000000;,
  86;1.000000;1.000000;1.000000;1.000000;,
  87;1.000000;1.000000;1.000000;1.000000;,
  88;1.000000;1.000000;1.000000;1.000000;,
  89;1.000000;1.000000;1.000000;1.000000;,
  90;1.000000;1.000000;1.000000;1.000000;,
  91;1.000000;1.000000;1.000000;1.000000;,
  92;1.000000;1.000000;1.000000;1.000000;,
  93;1.000000;1.000000;1.000000;1.000000;,
  94;1.000000;1.000000;1.000000;1.000000;,
  95;1.000000;1.000000;1.000000;1.000000;,
  96;1.000000;1.000000;1.000000;1.000000;,
  97;1.000000;1.000000;1.000000;1.000000;,
  98;1.000000;1.000000;1.000000;1.000000;,
  99;1.000000;1.000000;1.000000;1.000000;,
  100;1.000000;1.000000;1.000000;1.000000;,
  101;1.000000;1.000000;1.000000;1.000000;,
  102;1.000000;1.000000;1.000000;1.000000;,
  103;1.000000;1.000000;1.000000;1.000000;,
  104;1.000000;1.000000;1.000000;1.000000;,
  105;1.000000;1.000000;1.000000;1.000000;,
  106;1.000000;1.000000;1.000000;1.000000;,
  107;1.000000;1.000000;1.000000;1.000000;,
  108;1.000000;1.000000;1.000000;1.000000;,
  109;1.000000;1.000000;1.000000;1.000000;,
  110;1.000000;1.000000;1.000000;1.000000;,
  111;1.000000;1.000000;1.000000;1.000000;,
  112;1.000000;1.000000;1.000000;1.000000;,
  113;1.000000;1.000000;1.000000;1.000000;,
  114;1.000000;1.000000;1.000000;1.000000;,
  115;1.000000;1.000000;1.000000;1.000000;,
  116;1.000000;1.000000;1.000000;1.000000;,
  117;1.000000;1.000000;1.000000;1.000000;,
  118;1.000000;1.000000;1.000000;1.000000;,
  119;1.000000;1.000000;1.000000;1.000000;,
  120;1.000000;1.000000;1.000000;1.000000;,
  121;1.000000;1.000000;1.000000;1.000000;,
  122;1.000000;1.000000;1.000000;1.000000;,
  123;1.000000;1.000000;1.000000;1.000000;,
  124;1.000000;1.000000;1.000000;1.000000;,
  125;1.000000;1.000000;1.000000;1.000000;,
  126;1.000000;1.000000;1.000000;1.000000;,
  127;1.000000;1.000000;1.000000;1.000000;,
  128;1.000000;1.000000;1.000000;1.000000;,
  129;1.000000;1.000000;1.000000;1.000000;,
  130;1.000000;1.000000;1.000000;1.000000;,
  131;1.000000;1.000000;1.000000;1.000000;,
  132;1.000000;1.000000;1.000000;1.000000;,
  133;1.000000;1.000000;1.000000;1.000000;,
  134;1.000000;1.000000;1.000000;1.000000;,
  135;1.000000;1.000000;1.000000;1.000000;,
  136;1.000000;1.000000;1.000000;1.000000;,
  137;1.000000;1.000000;1.000000;1.000000;,
  138;1.000000;1.000000;1.000000;1.000000;,
  139;1.000000;1.000000;1.000000;1.000000;,
  140;1.000000;1.000000;1.000000;1.000000;,
  141;1.000000;1.000000;1.000000;1.000000;,
  142;1.000000;1.000000;1.000000;1.000000;,
  143;1.000000;1.000000;1.000000;1.000000;,
  144;1.000000;1.000000;1.000000;1.000000;,
  145;1.000000;1.000000;1.000000;1.000000;,
  146;1.000000;1.000000;1.000000;1.000000;,
  147;1.000000;1.000000;1.000000;1.000000;,
  148;1.000000;1.000000;1.000000;1.000000;,
  149;1.000000;1.000000;1.000000;1.000000;,
  150;1.000000;1.000000;1.000000;1.000000;,
  151;1.000000;1.000000;1.000000;1.000000;,
  152;1.000000;1.000000;1.000000;1.000000;,
  153;1.000000;1.000000;1.000000;1.000000;,
  154;1.000000;1.000000;1.000000;1.000000;,
  155;1.000000;1.000000;1.000000;1.000000;,
  156;1.000000;1.000000;1.000000;1.000000;,
  157;1.000000;1.000000;1.000000;1.000000;,
  158;1.000000;1.000000;1.000000;1.000000;,
  159;1.000000;1.000000;1.000000;1.000000;,
  160;1.000000;1.000000;1.000000;1.000000;,
  161;1.000000;1.000000;1.000000;1.000000;,
  162;1.000000;1.000000;1.000000;1.000000;,
  163;1.000000;1.000000;1.000000;1.000000;,
  164;1.000000;1.000000;1.000000;1.000000;,
  165;1.000000;1.000000;1.000000;1.000000;,
  166;1.000000;1.000000;1.000000;1.000000;,
  167;1.000000;1.000000;1.000000;1.000000;,
  168;1.000000;1.000000;1.000000;1.000000;,
  169;1.000000;1.000000;1.000000;1.000000;,
  170;1.000000;1.000000;1.000000;1.000000;,
  171;1.000000;1.000000;1.000000;1.000000;,
  172;1.000000;1.000000;1.000000;1.000000;,
  173;1.000000;1.000000;1.000000;1.000000;,
  174;1.000000;1.000000;1.000000;1.000000;,
  175;1.000000;1.000000;1.000000;1.000000;,
  176;1.000000;1.000000;1.000000;1.000000;,
  177;1.000000;1.000000;1.000000;1.000000;,
  178;1.000000;1.000000;1.000000;1.000000;,
  179;1.000000;1.000000;1.000000;1.000000;,
  180;1.000000;1.000000;1.000000;1.000000;,
  181;1.000000;1.000000;1.000000;1.000000;,
  182;1.000000;1.000000;1.000000;1.000000;,
  183;1.000000;1.000000;1.000000;1.000000;,
  184;1.000000;1.000000;1.000000;1.000000;,
  185;1.000000;1.000000;1.000000;1.000000;,
  186;1.000000;1.000000;1.000000;1.000000;,
  187;1.000000;1.000000;1.000000;1.000000;,
  188;1.000000;1.000000;1.000000;1.000000;,
  189;1.000000;1.000000;1.000000;1.000000;,
  190;1.000000;1.000000;1.000000;1.000000;,
  191;1.000000;1.000000;1.000000;1.000000;,
  192;1.000000;1.000000;1.000000;1.000000;,
  193;1.000000;1.000000;1.000000;1.000000;,
  194;1.000000;1.000000;1.000000;1.000000;,
  195;1.000000;1.000000;1.000000;1.000000;;
 }
}
