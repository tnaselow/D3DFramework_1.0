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
Creation date: 2/19/2017
End Header --------------------------------------------------------*/ 

INFO: 
This is a custom framework written entirely by Trevor Naselow in DirectX11 for use in the class CS300

a.
- Use Model text box to enter the name of the model (ex: cube.obj)
- Use Rotation / Scale / Position boxes to edit transform of model
- Use the materail drop down to edit the material values of the object
- Use the Light property drop downs to modify light properties
- Turn on / off blinn and texturing using buttons in ui
- Can set the number of lights in the num lights box - can support max of 8
- All lights start as point lights to change type change in drop down on selected lights
- Direction of spot / directinoal lights always points towards the object in the middle

b. Only enter valid object names : cube.obj, horse.obj, bunny.obj

c. All
d. N/A

e.
- Loading Shaders: Shader.cpp - line 50 loadShader
- Materials / light data loaded into constant buffer: Renderer_D3D.cpp - line 162 mapCBuffer
- Texture Sampler setup gpu side only in PhongPix_PS.hlsl
- Phong illumination: PhongPix_PS.hlsl mirrored PhongVert_VS.hlsl

f. Lab Machine Edison: DIT2835US
g. 15 :  2/9/17 - 2/19/17
h. N/A