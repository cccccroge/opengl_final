# Another weird openGL application demo

This is our final project for opengl introduction course at school.



### Screen Shot

![](/home/cccccroge/.var/app/com.github.marktext.marktext/config/marktext/images/2020-01-23-11-37-21-image.png)

![](/home/cccccroge/.var/app/com.github.marktext.marktext/config/marktext/images/2020-01-23-11-37-41-image.png)

![](/home/cccccroge/.var/app/com.github.marktext.marktext/config/marktext/images/2020-01-23-11-38-13-image.png)

![](/home/cccccroge/.var/app/com.github.marktext.marktext/config/marktext/images/2020-01-23-11-38-45-image.png)

### Implement Items

- Main Scene
  
  - [x] Skybox
  
  - [x] Assimp scene/model loading

- Lighting, Matetial & Rendering
  
  - [x] Use basic forward rendering with Blinn-Phong shading
  
  - [x] Directional light/ Point light / Spot light and their shadow mapping
- [ ] 
- Other effects
  
  - [x] Particle system (snow)
  
  - [x] Fog
  
  - [x] Ocean

- UI & Navigation
  
  - [x] SkyboxTrack ball navigation
  
  - [x] Path record/navigation
  
  - [x] Rich GUI to control lighting/weather/camera
    
    ![](/home/cccccroge/.var/app/com.github.marktext.marktext/config/marktext/images/2020-01-23-12-10-33-image.png)



### Dependencies

- GLEW 2.1.0

- Freeglut 3.2.1

- glm 0.9.9.6

- Assimp 3.3.1

- stb image loader 2.23

- ImGui 1.74



### Development

- OS：Windows 10, 64 bit

- IDE：Visual Studio 2017
  (Build setting: Debug, x64)

- Project Structure
  
  - assets/            3d models, shader code, images...
  
  - src/                  all source code
  
  - include/          library headers to include
  
  - lib/                   library binaries that header files will look at
  
  - bin/                 executable and intermediate object files 
    


