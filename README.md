# TinyEngine
A tiny game engine based on opengl (Not Finished)

## Main Feature
- Now
	- Forward Rendering
	- Shadow mapping
	- PCF/PCSS
	- Deferred Rendering
	- PBR
	- Tonemapping
	- SSR
	- FXAA (Console)
	- ImGUI
	- Skeletal Animation 
 - Later
	- Cloth Simulation
	- SPH/PBD

## Environment
 - VS2019

## User Guide
Camera Control: W/S/A/D/Q/E  forward / back / left / right / up / down
H -> hide mouse cursor
If you want to show different pass texture, please define DEBUG in GuiManager.cpp Draw function

## Problem encountered:
 - PCSS may light leaking, we should avoid higher object hiding lower object in shadow map space. 
Otherwise, bigger filtersize will be choose

![PCSS_Light_Leaking](./image/PCSS_light_leaking.jpg)


 - Dear IMGUI TreeNode's input name should be different, otherwise they are the same, 
you will control all at the same time
 - Circular Include
 - Skeleton Animation, pass int (when pass BoneID) Should use "glVertexAttribIPointer" not "glVertexAttribPointer"