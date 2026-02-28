# ft_vox
ft_vox est un projet de moteur graphique 3D, conçu pour explorer
les concepts avancés du rendu bas niveau et de la programmation GPU moderne.

*Réalisé par [Gaël Cannaud](https://github.com/Helco18) et [Sébastien Craeymeersch](https://github.com/AgarOther).*

## Hot-swappable rendering backend

Le moteur repose sur un **RHI (Rendering Hardware Interface)** permettant une abstraction complète des APIs graphiques. Notre architecture permet un rendu multi-backend avec bascule d’API graphique à chaud entre OpenGL et Vulkan, à tout moment de l’exécution.

## Contrôles

- **TAB** : changer dynamiquement l’API graphique (OpenGL ↔ Vulkan)
- **1 / 2 / 3** : changer le type de caméra  
  - **1** : caméra classique (Euler)  
  - **2** : caméra 6-DOF  
  - **3** : caméra FPS
- **W / A / S / D** : déplacement de la caméra
- **SPACE / CTRL** : contrôle de la hauteur
- **SHIFT** : accélérer la caméra
- **Molette haut / bas** : ajuster la vitesse de déplacement
- **Q / E** : contrôle du tangage *(uniquement en mode 6-DOF)*
- **LEFT CLIC** : casse les blocks
- **F6** : affiche la bordure des chunks
- **F7** : active le wireframe

## Flags de l’exécutable

L’exécutable supporte plusieurs flags d’exécution :
- `-gl` `-opengl` : force l’utilisation de l’API **OpenGL** au lancement. *(Vulkan est utilisé par défaut)*
- `-f` / `-fov` : initialise le **FOV** à la valeur renseignée. *(Valeurs par défaut : 90)*
- `-p` : active le **profiler intégré**.
- `-d` : active les **messages de debug**.
- `-v` / `-vsync` : active la **synchronisation verticale**.
- `-r=<distance>` : initialise la limite de **render distance** à la valeur renseignée. *(La valeur par défaut est 10 chunks de 32×32×32)*
- `-x=<coordX>` / `-y=<coordY>` /  `-z=<coordZ>` : définit la **position de spawn** par défaut. *(Valeurs par défaut : 0, 0, 0)*

## Stack technique

Ce projet utilise la bibliothèque [GLFW](https://www.glfw.org) pour la création de la fenêtre et la gestion des entrées, ainsi que [OpenGL](https://www.opengl.org/) et [Vulkan](https://www.vulkan.org) pour le rendu graphique. Les shaders sont en [Slang](https://shader-slang.org/) pour assurer la compatibilité multi-backend. La caméra est entièrement gérée avec des **quaternions** pour des rotations précises et fluides, évitant ainsi le gimbal lock. Nous utilisons également notre **Simplex Noise** pour l’optimisation de la génération procédurale. Le projet est entièrement en **C++** et s’appuie sur un **Makefile**. L’exécutable gère différents **flags de compilation et d’exécution** pour faciliter le build, les tests et la configuration.  
  
Pour les tests et l’optimisation, nous disposons d’outils comme un **profiler intégré** fournissant des statistiques détaillées sur le nombre d’appels et les temps d’exécution moyens et médians des fonctions, tandis qu’un **logger** permet de suivre en temps réel les différentes étapes du programme. L’exécutable retourne **0 en cas de réussite** et **1 en cas d’erreur**.

## Librairies utilisées

- [GLFW](https://www.glfw.org) : pour la création de la fenêtre et la gestion des entrées.
- [Vulkan](https://www.vulkan.org) : pour le rendu graphique performant.
- [OpenGL](https://www.opengl.org/) : pour le rendu graphique rétrocompatible.
- [GLM](https://github.com/g-truc/glm) : pour les mathématiques de l'API graphique.
- [stb_image](https://github.com/nothings/stb) : pour le chargement d'images.
- [GLAD](https://glad.dav1d.de/) : pour le chargement des extensions OpenGL.

## Liens utiles

- [GLFW Documentation](https://www.glfw.org)  
- [Vulkan Documentation](https://docs.vulkan.org)  
- [Vulkan tutorial 1](https://docs.vulkan.org/tutorial/latest/00_Introduction.html)  
- [Vulkan Tutorial 2](https://vulkan-tutorial.com)  
- [OpenGL docs](https://docs.gl/)
- [Slang](https://shader-slang.org/)
- [Frustum Culling](https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling)
