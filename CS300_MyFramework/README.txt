/* Start Header -------------------------------------------------------
Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: README.txt
Purpose: describe the use / materials of the cs_300 assignment
Language: English
Platform: N/A
Project: t.naselow_CS300_3
Author: Trevor Naselow / t.naselow / id:180002215
Creation date: 1/27/2017
End Header --------------------------------------------------------*/ 

INFO: 
This is a custom framework written entirely by Trevor Naselow in DirectX11 for use in the class CS300
- NOTE: all face and vertex normals were drawn using the geometry shader in hlsl
- NOTE: doing lighting calculations in pixel shader for per pixel lighting

a.
- Use Model text box to enter the name of the model (ex: cube.obj)
- Use Rotation / Scale / Position boxes to edit transform of model
- Use the materail drop down to edit the material values of the object
- Use the Light property drop downs to modify light properties
- Switch normal drawing modes using the dropdown

b. Only enter valid object names : cube.obj, horse.obj, bunny.obj

c. All
d. N/A

e.
- OBJ loading: ResourceManager.cpp - line 17 loadMesh
- Rendering Geometry: Renderer_D3D.cpp - line 191 EndFrame (more rendering functions found in renderer_d3d)
	- Color_VS / Color_G / Color_PS Shaders for Rendering (geometry shader used to render normals)
- Specified GUI for lights/stuff: Sandbox.cpp - line 54 update

f. Lab Machine Edison: DIT2835US
g. 15-20 :  1/16/17 - 1/27/17
h. N/A