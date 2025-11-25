# 📺 Commandes GUI du serveur Zappy

## 🎮 Interface graphique

Le GUI Zappy est un client graphique qui se connecte au serveur pour visualiser l'état du jeu en temps réel. Il utilise SFML pour l'affichage et communique avec le serveur via TCP.

### Connexion
- **Format** : `./zappy_gui -p port -h hostname`
- **Exemple** : `./zappy_gui -p 4242 -h localhost`

---

## 📡 Commandes GUI → Serveur

### 🗺️ **Commandes d'information du monde**

#### **1. msz (Map Size)**
- **GUI → Serveur** : `msz\n`
- **Serveur → GUI** : `msz X Y\n`
- **Fonction** : Demande la taille de la carte
- **Paramètres** :
  - `X` : Largeur de la carte
  - `Y` : Hauteur de la carte

#### **2. bct (Board Content at)**
- **GUI → Serveur** : `bct X Y\n`
- **Serveur → GUI** : `bct X Y q0 q1 q2 q3 q4 q5 q6\n`
- **Fonction** : Demande le contenu d'une case spécifique
- **Paramètres** :
  - `X Y` : Coordonnées de la case
  - `q0` à `q6` : Quantités des ressources (food, linemate, deraumere, sibur, mendiane, phiras, thystame)

#### **3. mct (Map Content)**
- **GUI → Serveur** : `mct\n`
- **Serveur → GUI** : `bct X Y q0 q1 q2 q3 q4 q5 q6\n` (pour chaque case)
- **Fonction** : Demande le contenu complet de la carte
- **Particularités** : Retourne autant de messages `bct` qu'il y a de cases

### 👥 **Commandes d'information des équipes**

#### **4. tna (Team Names)**
- **GUI → Serveur** : `tna\n`
- **Serveur → GUI** : `tna N\n` (pour chaque équipe)
- **Fonction** : Demande les noms de toutes les équipes
- **Paramètres** : `N` = Nom de l'équipe

---

## 📨 Notifications automatiques Serveur → GUI

Le serveur envoie automatiquement des notifications au GUI lors d'événements du jeu :

### 🧑‍🤝‍🧑 **Gestion des joueurs**

#### **pnw (Player New)**
- **Format** : `pnw #n X Y O L N\n`
- **Événement** : Connexion d'un nouveau joueur
- **Paramètres** :
  - `#n` : ID du joueur
  - `X Y` : Position initiale
  - `O` : Orientation (1=Nord, 2=Est, 3=Sud, 4=Ouest)
  - `L` : Niveau du joueur
  - `N` : Nom de l'équipe

#### **ppo (Player Position)**
- **Format** : `ppo #n X Y O\n`
- **Événement** : Changement de position/orientation d'un joueur
- **Paramètres** :
  - `#n` : ID du joueur
  - `X Y` : Nouvelle position
  - `O` : Nouvelle orientation

#### **plv (Player Level)**
- **Format** : `plv #n L\n`
- **Événement** : Changement de niveau d'un joueur
- **Paramètres** :
  - `#n` : ID du joueur
  - `L` : Nouveau niveau

#### **pin (Player Inventory)**
- **Format** : `pin #n X Y q0 q1 q2 q3 q4 q5 q6\n`
- **Événement** : Information sur l'inventaire d'un joueur
- **Paramètres** :
  - `#n` : ID du joueur
  - `X Y` : Position du joueur
  - `q0` à `q6` : Quantités dans l'inventaire

#### **pdi (Player Death)**
- **Format** : `pdi #n\n`
- **Événement** : Mort d'un joueur
- **Paramètres** : `#n` = ID du joueur mort

### 📦 **Gestion des ressources**

#### **pgt (Player Get)**
- **Format** : `pgt #n i\n`
- **Événement** : Un joueur ramasse une ressource
- **Paramètres** :
  - `#n` : ID du joueur
  - `i` : Type de ressource (0=food, 1=linemate, etc.)

#### **pdr (Player Drop)**
- **Format** : `pdr #n i\n`
- **Événement** : Un joueur dépose une ressource
- **Paramètres** :
  - `#n` : ID du joueur
  - `i` : Type de ressource

### 🥚 **Gestion des œufs**

#### **pfk (Player Fork)**
- **Format** : `pfk #n\n`
- **Événement** : Un joueur pond un œuf
- **Paramètres** : `#n` = ID du joueur

#### **enw (Egg New)**
- **Format** : `enw #e #n X Y\n`
- **Événement** : Création d'un nouvel œuf
- **Paramètres** :
  - `#e` : ID de l'œuf
  - `#n` : ID du joueur qui l'a pondu
  - `X Y` : Position de l'œuf

#### **ebo (Egg Born)**
- **Format** : `ebo #e\n`
- **Événement** : Connexion d'un joueur depuis un œuf
- **Paramètres** : `#e` = ID de l'œuf utilisé

#### **edi (Egg Death)**
- **Format** : `edi #e\n`
- **Événement** : Destruction d'un œuf
- **Paramètres** : `#e` = ID de l'œuf détruit

### 🔮 **Incantations**

#### **pic (Player Incantation)**
- **Format** : `pic X Y L #n #n...\n`
- **Événement** : Début d'une incantation
- **Paramètres** :
  - `X Y` : Position de l'incantation
  - `L` : Niveau de l'incantation
  - `#n #n...` : IDs des joueurs participants

#### **pie (Player Incantation End)**
- **Format** : `pie X Y R\n`
- **Événement** : Fin d'une incantation
- **Paramètres** :
  - `X Y` : Position de l'incantation
  - `R` : Résultat (1=succès, 0=échec)

### 💬 **Communication**

#### **pbc (Player Broadcast)**
- **Format** : `pbc #n M\n`
- **Événement** : Message diffusé par un joueur
- **Paramètres** :
  - `#n` : ID du joueur émetteur
  - `M` : Message diffusé

#### **pex (Player Expulsion)**
- **Format** : `pex #n\n`
- **Événement** : Expulsion d'un joueur
- **Paramètres** : `#n` = ID du joueur expulsé

### ⏱️ **Gestion du temps**

#### **sgt (Server Get Time)**
- **Format** : `sgt T\n`
- **Événement** : Information sur l'unité de temps
- **Paramètres** : `T` = Unité de temps actuelle

#### **sst (Server Set Time)**
- **Format** : `sst T\n`
- **Événement** : Modification de l'unité de temps
- **Paramètres** : `T` = Nouvelle unité de temps

### 🏆 **Fin de partie**

#### **seg (Server End Game)**
- **Format** : `seg N\n`
- **Événement** : Fin de partie
- **Paramètres** : `N` = Nom de l'équipe gagnante

### ⚠️ **Messages d'erreur**

#### **suc (Server Unknown Command)**
- **Format** : `suc\n`
- **Événement** : Commande inconnue

#### **sbp (Server Bad Parameters)**
- **Format** : `sbp\n`
- **Événement** : Paramètres invalides

#### **smg (Server Message)**
- **Format** : `smg M\n`
- **Événement** : Message du serveur
- **Paramètres** : `M` = Message

---

## 🎯 **Séquence de connexion type**

1. **Connexion** : Le GUI se connecte au serveur
2. **Accueil** : Le serveur envoie `WELCOME`
3. **Initialisation** : Le GUI demande automatiquement :
   - `tna` → Récupère les noms des équipes
   - `msz` → Récupère la taille de la carte
   - `mct` → Récupère le contenu complet de la carte
4. **Temps réel** : Le serveur envoie automatiquement toutes les notifications d'événements

---

## 🛠️ **Interface utilisateur**

### **Fonctionnalités**
- **Visualisation en temps réel** du monde Zappy
- **Affichage des joueurs** avec leurs positions et orientations
- **Affichage des ressources** sur chaque case
- **Menu de configuration** (résolution, volume, thème)
- **Support multi-résolution** (720p, 900p, 1080p)

### **Contrôles**
- **Échap** : Fermer l'application
- **Interface graphique** : Interaction via souris et clavier
- **Menu settings** : Configuration des paramètres

---

## 📋 **Types de ressources**

| ID | Nom | Description |
|----|-----|-------------|
| 0 | food | Nourriture |
| 1 | linemate | Pierre 1 |
| 2 | deraumere | Pierre 2 |
| 3 | sibur | Pierre 3 |
| 4 | mendiane | Pierre 4 |
| 5 | phiras | Pierre 5 |
| 6 | thystame | Pierre 6 |

---

## 📡 **Protocole de communication**

Le GUI utilise un **protocole TCP** avec des messages terminés par `\n`. Chaque message est traité de manière asynchrone, permettant une mise à jour fluide de l'interface en temps réel.

**Exemple de flux :**
```
GUI → Serveur : msz
Serveur → GUI : msz 10 10
GUI → Serveur : mct
Serveur → GUI : bct 0 0 1 0 0 0 0 0 0
Serveur → GUI : bct 0 1 0 1 0 0 0 0 0
...
```