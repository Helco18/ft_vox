# ft_vox
ft_vox est un projet de voxel utilisant un moteur graphique 3D, conçu pour explorer
les concepts avancés du rendu bas niveau et de la programmation GPU moderne.

*Réalisé par [Gaël Cannaud](https://github.com/Helco18) et [Sébastien Craeymeersch](https://github.com/AgarOther).*

## Build

Assurez-vous que les dépendances suivantes sont présentes :  

- **Vulkan SDK** (version minimale recommandée : 1.3.x)  
- Compilateur **C++17** ou supérieur  
- Make et outils de build standards (cmake, wget, tar, git)  

Toutes les librairies nécessaires (**GLFW**, **GLM**, **stb_image**, **GLAD**) sont automatiquement téléchargées et installées via le Makefile.

Pour compiler le projet :  

```bash
git clone https://github.com/Helco18/ft_vox.git
cd ft_vox/rendu
make
```

## Run

Pour lancer l’exécutable :
```Bash
./ft_vox [flags]
```

## Build / Run rapide en une seule commande
```Bash
make && ./ft_vox
```

## Hot-swappable RHI

Le moteur repose sur un **RHI (Rendering Hardware Interface)** permettant une abstraction complète des APIs graphiques. Notre architecture permet un rendu multi-backend avec bascule d’API graphique à chaud entre OpenGL et Vulkan, à tout moment de l’exécution. Les deux engines reposent sur un principe **agnostique** : ils ne connaissent pas le reste du programme et renvoient un ID (pipeline/asset) lorsqu'une ressource est uploadée. La complexité de notre RHI est principalement de faire fonctionner OpenGL et Vulkan à partir d'une **classe abstraite** contenant des fonctions communes aux deux engines de manière à ce que **le reste du code n'ait pas besoin de savoir sur quel engine nous sommes**.

## Contrôles

- **TAB** : changer dynamiquement l’API graphique (OpenGL ↔ Vulkan)
- **1 / 2 / 3** : changer le type de caméra  
  - **1** : caméra classique (Euler)  
  - **2** : caméra 6-DOF  
  - **3** : caméra FPS
- **W / A / S / D** : déplacement de la caméra
- **SPACE / SHIFT** : contrôle de la hauteur
- **CTRL GAUCHE** : accélérer la caméra
- **Molette haut / bas** : ajuster la vitesse de déplacement
- **Q / E** : contrôle du tangage *(uniquement en mode 6-DOF)*
- **Clic droit** : casse les blocks
- **F6** : affiche la bordure des chunks
- **F7** : active le wireframe

## Flags de l’exécutable

L’exécutable supporte plusieurs flags d’exécution :
- `-gl` `-opengl` : force l’utilisation de l’API **OpenGL** au lancement. *(Vulkan est utilisé par défaut)*
- `-fov` : initialise le **FOV** à la valeur renseignée. *(Valeurs par défaut : 80)*
- `-p` : active le **profiler intégré**.
- `-d` : active les 
**messages de debug du programme et de l'engine**.
- `-v` : active seulement les **messages de debug de l'engine**.
- `-vsync` : active la **synchronisation verticale**.
- `-r <distance>` : initialise la limite de **render distance** à la valeur renseignée. *(La valeur par défaut est 8 chunks de 32×32×32)*
- `-x <coordX>` / `-y <coordY>` /  `-z <coordZ>` : définit la **position de spawn** par défaut. *(Valeurs par défaut : 0, 0, 0)*
- `-s` : définit la **seed utilisée pour la génération**. *(Seed par défaut : 42)*

## Stack technique

Ce projet utilise la bibliothèque [GLFW](https://www.glfw.org) pour la création de la fenêtre et la gestion des entrées, ainsi que [OpenGL](https://www.opengl.org/) et [Vulkan](https://www.vulkan.org) pour le rendu graphique. Les shaders sont en [Slang](https://shader-slang.org/) pour assurer la compatibilité multi-backend : c'est une langue de programmation de shaders qui permet de compiler sur OpenGL et Vulkan. La caméra est entièrement gérée avec des **quaternions** pour des rotations précises et fluides, évitant ainsi le gimbal lock. Nous utilisons également notre **Simplex Noise** pour l’optimisation de la génération procédurale. Le projet est entièrement en **C++** et en **Slang**. L’exécutable gère différents **flags de compilation et d’exécution** pour faciliter le build, les tests et la configuration.
  
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
