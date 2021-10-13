# Tripodes Firmware (esp32 t-display)
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


### Televersement du projet

Cliquer sur "Platformio > PROJET TAKS > Default > General > Upload"

![alt text](https://github.com/juthomas/Chemical_Orca/blob/master/README_images/Upload_Project.png)


Cliquer sur "Platformio > PROJET TAKS > Default > Platform > Build Filesystem Image"

![alt text](https://github.com/juthomas/Tripodes-firmware/blob/master/README_images/BuildFilesystemImage.png)


Cliquer sur "Platformio > PROJET TAKS > Default > Platform > Upload Filesystem Image"

![alt text](https://github.com/juthomas/Tripodes-firmware/blob/master/README_images/UploadFilesystemImage.png)


### Configuration WIFI, UDP et OSC

Allumer le tripode, appuyer sur le bouton de gauche,

Connecter votre ordinateur sur le reseau wifi du tripode (SSID: tripodesAP, MDP: 44448888)

Ouvrir un navigateur WEB a l'adresse "http://192.168.4.1"

Changer les Ports et Adresses sur l'interface WEB puis cliquer sur "Update"

### Activation de l'UDP et de l'OSC

Une fois en mode "Station" ou en mode "Access Point" (Appui sur le bouton gauche ou droit au demarage du tripode),<br/>
Cliquer une fois pour le passage en mode "Envoi UDP" (barre rouge)<br/>
Double cliquer pour le passage en mode "Envoi OSC" (barre bleue)<br/>
=> Combinaison des envois UPD & OSC = barre violette
