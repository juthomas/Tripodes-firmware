# Tripdes Firmware (esp32 t-display)
![alt text](https://github.com/juthomas/Tripodes-firmware/blob/master/README_images/1620348658624.jpg)
<br/>

## Upload du Firmware
<br/>

### Installation de platformio (Sur VScode)

Dans le Menu "Extensions" (Ctrl+Shift+X)
taper "platformio", puis installer

![alt text](https://github.com/juthomas/Tripodes-firmware/blob/master/README_images/Install_Platformio.jpg)

### Ouverture du projet (Sur platformio)
  Dezipper le projet (Ce repo)  
  Cliquer sur le menu de platformio (Icon a gauche)  
  Puis sur le bouton "Open Project"  
  Selectionner le projet  

![alt text](https://github.com/juthomas/Tripodes-firmware/blob/master/README_images/Projet_Open_1.jpg)

![alt text](https://github.com/juthomas/Tripodes-firmware/blob/master/README_images/Projet_Open_2.jpg)


### Mise a jour des librairies 

Cliquer sur "Platformio > PROJET TAKS > Default > Miscellaneous > Update All"

![alt text](https://github.com/juthomas/Chemical_Orca/blob/master/README_images/Update_Project.png)

### Mise a jour des identifiants WIFI

dans "src" > "main.cpp", modifier  
  
  `[76] const char* ssid = "tripodesAP"; `  
  `[78] const char* password =  "44448888"; `  
   

### Televersement du projet

Cliquer sur "Platformio > PROJET TAKS > Default > General > Upload"

![alt text](https://github.com/juthomas/Chemical_Orca/blob/master/README_images/Upload_Project.png)
