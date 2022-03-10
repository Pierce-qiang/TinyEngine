# TinyEngine
A tiny game engine based on opengl (building framework)

Problem encountered:
 - PCSS may light leaking, we should avoid higher object hiding lower object in shadow map space
 - otherwise, bigger filtersize will be choose

![PCSS_Light_Leaking](./image/PCSS_light_leaking.jpg)