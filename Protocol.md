# 📡 Protocole de Communication Zappy

Ce document décrit en détail le protocole de communication TCP utilisé dans le projet Zappy entre les différents composants : Serveur, Interface Graphique (GUI) et Clients IA.

## 📋 Table des Matières

- [Vue d'ensemble](#vue-densemble)
- [Symboles et Conventions](#symboles-et-conventions)
- [Commandes Client IA → Serveur](#commandes-client-ia--serveur)
- [Commandes GUI → Serveur](#commandes-gui--serveur)
- [Notifications Serveur → GUI](#notifications-serveur--gui)
- [Messages d'Erreur](#messages-derreur)
- [Système de Files d'Attente](#système-de-files-dattente)

## 🔍 Vue d'ensemble

Le protocole Zappy utilise TCP pour la communication entre les composants. Chaque message se termine par `\n` et est traité de manière asynchrone avec un système de files d'attente pour les commandes IA.

### Types de Clients
- **CLIENT_TYPE_AI** : Bots IA contrôlant les joueurs
- **CLIENT_TYPE_GUI** : Interface graphique pour la visualisation
- **GRAPHIC** : Connexion spéciale pour l'interface graphique

## 🔤 Symboles et Conventions

| Symbole | Signification | Symbole | Signification |
|---------|---------------|---------|---------------|
| `X` | Position horizontale (largeur) | `n` | Numéro de joueur |
| `Y` | Position verticale (hauteur) | `O` | Orientation : 1(N), 2(E), 3(S), 4(W) |
| `q0` | Quantité ressource 0 (food) | `L` | Niveau du joueur/incantation |
| `q1` | Quantité ressource 1 (linemate) | `e` | Numéro d'œuf |
| `q2` | Quantité ressource 2 (deraumere) | `T` | Unité de temps |
| `q3` | Quantité ressource 3 (sibur) | `N` | Nom de l'équipe |
| `q4` | Quantité ressource 4 (mendiane) | `R` | Résultat d'incantation |
| `q5` | Quantité ressource 5 (phiras) | `M` | Message |
| `q6` | Quantité ressource 6 (thystame) | `i` | Numéro de ressource |

### Ressources du Jeu
0. **food** - Nourriture (survie)
1. **linemate** - Pierre précieuse
2. **deraumere** - Pierre précieuse
3. **sibur** - Pierre précieuse
4. **mendiane** - Pierre précieuse
5. **phiras** - Pierre précieuse
6. **thystame** - Pierre précieuse

## 🤖 Commandes Client IA → Serveur

### 🚀 Commandes de Déplacement

#### `Forward`
```
Client → Serveur : Forward\n
Serveur → Client : ok\n | ko\n
Serveur → GUI : ppo #n X Y O\n
```
- **Fonction** : Fait avancer le joueur d'une case dans sa direction actuelle
- **Délai** : 7/freq unités de temps
- **Carte toroïdale** : Les coordonnées utilisent le modulo (wraparound)

#### `Right`
```
Client → Serveur : Right\n
Serveur → Client : ok\n | ko\n
Serveur → GUI : ppo #n X Y O\n
```
- **Fonction** : Fait tourner le joueur de 90° vers la droite
- **Délai** : 7/freq unités de temps
- **Orientations** : 1→2→3→4→1 (N→E→S→W→N)

#### `Left`
```
Client → Serveur : Left\n
Serveur → Client : ok\n | ko\n
Serveur → GUI : ppo #n X Y O\n
```
- **Fonction** : Fait tourner le joueur de 90° vers la gauche
- **Délai** : 7/freq unités de temps
- **Orientations** : 1→4→3→2→1 (N→W→S→E→N)

### 👁️ Commandes d'Observation

#### `Look`
```
Client → Serveur : Look\n
Serveur → Client : [case0,case1,case2,...]\n
```
- **Fonction** : Vision autour du joueur selon son niveau
- **Délai** : 7/freq unités de temps
- **Portée** : Niveau du joueur (niveau 1 = 1 case, niveau 2 = 2 cases, etc.)
- **Format réponse** : Cases dans l'ordre de distance croissante
- **Contenu cases** : Ressources et joueurs présents

#### `Inventory`
```
Client → Serveur : Inventory\n
Serveur → Client : [food: X, linemate: Y, deraumere: Z, sibur: A, mendiane: B, phiras: C, thystame: D]\n
```
- **Fonction** : Affiche l'inventaire complet du joueur
- **Délai** : 1/freq unités de temps
- **Format** : Quantités de toutes les ressources

### 📦 Commandes de Gestion des Ressources

#### `Take [ressource]`
```
Client → Serveur : Take food\n | Take linemate\n | Take deraumere\n | etc.
Serveur → Client : ok\n | ko\n
Serveur → GUI : pgt #n i\n
Serveur → GUI : bct X Y q0 q1 q2 q3 q4 q5 q6\n
```
- **Fonction** : Ramasse une ressource spécifique sur la case actuelle
- **Délai** : 7/freq unités de temps
- **Conditions** : Ressource doit être disponible sur la case
- **Effet** : Ajoute à l'inventaire, met à jour la case

#### `Set [ressource]`
```
Client → Serveur : Set food\n | Set linemate\n | Set deraumere\n | etc.
Serveur → Client : ok\n | ko\n
Serveur → GUI : pdr #n i\n
Serveur → GUI : bct X Y q0 q1 q2 q3 q4 q5 q6\n
```
- **Fonction** : Dépose une ressource de l'inventaire sur la case actuelle
- **Délai** : 7/freq unités de temps
- **Conditions** : Ressource doit être dans l'inventaire
- **Effet** : Retire de l'inventaire, ajoute à la case

### 📊 Commandes d'Information

#### `Connect_nbr`
```
Client → Serveur : Connect_nbr\n
Serveur → Client : X\n
```
- **Fonction** : Nombre d'œufs disponibles pour l'équipe
- **Délai** : 1/freq unités de temps
- **Utilité** : Planification de nouvelles connexions

#### `Player_nbr`
```
Client → Serveur : Player_nbr\n
Serveur → Client : X\n
```
- **Fonction** : Nombre de joueurs connectés dans l'équipe
- **Délai** : 1/freq unités de temps

#### `Player_pos`
```
Client → Serveur : Player_pos\n
Serveur → Client : X,Y\n
```
- **Fonction** : Position actuelle du joueur
- **Délai** : 1/freq unités de temps
- **Format** : Coordonnées séparées par une virgule

### 🔮 Commandes Spéciales

#### `Incantation`
```
Client → Serveur : Incantation\n
Serveur → Client : Elevation underway Current level: L\n | ko\n
Serveur → GUI : pic X Y L #n #n...\n
Serveur → GUI : pie X Y R\n
```
- **Fonction** : Tente une montée de niveau
- **Délai** : 300/freq unités de temps
- **Conditions** :
  - Nombre requis de joueurs du même niveau sur la case
  - Ressources requises sur la case
- **Effet** : Monte tous les participants d'un niveau en cas de succès

**Conditions par niveau :**
- Niveau 1→2 : 1 joueur, 1 linemate
- Niveau 2→3 : 2 joueurs, 1 linemate, 1 deraumere, 1 sibur
- Niveau 3→4 : 2 joueurs, 2 linemate, 1 sibur, 2 phiras
- Niveau 4→5 : 4 joueurs, 1 linemate, 1 deraumere, 2 sibur, 1 phiras
- Niveau 5→6 : 4 joueurs, 1 linemate, 2 deraumere, 1 sibur, 3 mendiane
- Niveau 6→7 : 6 joueurs, 1 linemate, 2 deraumere, 3 sibur, 1 phiras
- Niveau 7→8 : 6 joueurs, 2 linemate, 2 deraumere, 2 sibur, 2 mendiane, 2 phiras, 1 thystame

#### `Fork`
```
Client → Serveur : Fork\n
Serveur → Client : ok\n | ko\n
Serveur → GUI : pfk #n\n
Serveur → GUI : enw #e #n X Y\n
```
- **Fonction** : Pond un œuf pour permettre une nouvelle connexion
- **Délai** : 42/freq unités de temps
- **Effet** : Crée un œuf à la position actuelle

#### `Eject`
```
Client → Serveur : Eject\n
Serveur → Client : ok\n | ko\n
Serveur → Clients éjectés : eject: direction\n
Serveur → GUI : pex #n\n
Serveur → GUI : ppo #n X Y O\n (pour chaque joueur déplacé)
Serveur → GUI : edi #e\n (pour chaque œuf détruit)
```
- **Fonction** : Éjecte tous les joueurs et détruit tous les œufs de la case
- **Délai** : 7/freq unités de temps
- **Effet** : Déplace les joueurs d'une case dans la direction de l'éjecteur

#### `Broadcast [message]`
```
Client → Serveur : Broadcast Hello world\n
Serveur → Client : ok\n | ko\n
Serveur → Tous les clients IA : message direction, Hello world\n
Serveur → GUI : pbc #n Hello world\n
```
- **Fonction** : Diffuse un message à tous les joueurs avec indication de direction
- **Délai** : 7/freq unités de temps
- **Direction** : Calculée selon la position relative (1-8, 0=même case)

## 📺 Commandes GUI → Serveur

### 🗺️ Commandes d'Information du Monde

#### `msz`
```
GUI → Serveur : msz\n
Serveur → GUI : msz X Y\n
```
- **Fonction** : Demande la taille de la carte

#### `bct X Y`
```
GUI → Serveur : bct X Y\n
Serveur → GUI : bct X Y q0 q1 q2 q3 q4 q5 q6\n
```
- **Fonction** : Demande le contenu d'une case spécifique

#### `mct`
```
GUI → Serveur : mct\n
Serveur → GUI : bct X Y q0 q1 q2 q3 q4 q5 q6\n (pour chaque case)
```
- **Fonction** : Demande le contenu de toute la carte

#### `tna`
```
GUI → Serveur : tna\n
Serveur → GUI : tna N\n (pour chaque équipe)
```
- **Fonction** : Demande les noms de toutes les équipes

### 👤 Commandes d'Information des Joueurs

#### `ppo #n`
```
GUI → Serveur : ppo #42\n
Serveur → GUI : ppo #42 X Y O\n
```
- **Fonction** : Demande la position d'un joueur spécifique

#### `plv #n`
```
GUI → Serveur : plv #42\n
Serveur → GUI : plv #42 L\n
```
- **Fonction** : Demande le niveau d'un joueur spécifique

#### `pin #n`
```
GUI → Serveur : pin #42\n
Serveur → GUI : pin #42 X Y q0 q1 q2 q3 q4 q5 q6\n
```
- **Fonction** : Demande l'inventaire d'un joueur spécifique

### ⏰ Commandes de Gestion du Temps

#### `sgt`
```
GUI → Serveur : sgt\n
Serveur → GUI : sgt T\n
```
- **Fonction** : Demande la fréquence actuelle du serveur

#### `sst T`
```
GUI → Serveur : sst 100\n
Serveur → GUI : sst 100\n
```
- **Fonction** : Modifie la fréquence du serveur

## 📨 Notifications Serveur → GUI

### 👤 Gestion des Joueurs

#### `pnw` - Nouvelle Connexion
```
pnw #n X Y O L N\n
```
- **Événement** : Connexion d'un nouveau joueur
- **Paramètres** : ID, position, orientation, niveau, équipe

#### `ppo` - Position Joueur
```
ppo #n X Y O\n
```
- **Événement** : Déplacement ou rotation d'un joueur
- **Déclencheurs** : Forward, Right, Left, Eject

#### `plv` - Niveau Joueur
```
plv #n L\n
```
- **Événement** : Montée de niveau d'un joueur
- **Déclencheur** : Incantation réussie

#### `pin` - Inventaire Joueur
```
pin #n X Y q0 q1 q2 q3 q4 q5 q6\n
```
- **Événement** : Modification de l'inventaire
- **Déclencheurs** : Take, Set, connexion initiale

#### `pdi` - Mort Joueur
```
pdi #n\n
```
- **Événement** : Mort d'un joueur par manque de nourriture

### 🎬 Actions et Interactions

#### `pex` - Expulsion
```
pex #n\n
```
- **Événement** : Un joueur utilise la commande Eject

#### `pbc` - Broadcast
```
pbc #n M\n
```
- **Événement** : Un joueur diffuse un message

#### `pgt` - Collecte Ressource
```
pgt #n i\n
```
- **Événement** : Un joueur ramasse une ressource
- **Déclencheur** : Take réussi

#### `pdr` - Dépôt Ressource
```
pdr #n i\n
```
- **Événement** : Un joueur dépose une ressource
- **Déclencheur** : Set réussi

### 🔮 Incantations

#### `pic` - Début d'Incantation
```
pic X Y L #n #n...\n
```
- **Événement** : Début d'une incantation
- **Paramètres** : Position, niveau, IDs des participants

#### `pie` - Fin d'Incantation
```
pie X Y R\n
```
- **Événement** : Fin d'une incantation
- **Résultat** : 1=succès, 0=échec

### 🥚 Gestion des Œufs

#### `pfk` - Ponte
```
pfk #n\n
```
- **Événement** : Un joueur pond un œuf

#### `enw` - Nouvel Œuf
```
enw #e #n X Y\n
```
- **Événement** : Un œuf est créé
- **Paramètres** : ID œuf, ID joueur parent, position

#### `ebo` - Éclosion
```
ebo #e\n
```
- **Événement** : Un œuf éclot (nouvelle connexion)

#### `edi` - Destruction Œuf
```
edi #e\n
```
- **Événement** : Un œuf est détruit (par Eject)

### 🎮 Gestion du Jeu

#### `seg` - Fin de Jeu
```
seg N\n
```
- **Événement** : Fin de partie
- **Paramètre** : Nom de l'équipe gagnante

#### `smg` - Message Serveur
```
smg M\n
```
- **Événement** : Message informatif du serveur

## ❌ Messages d'Erreur

#### `suc` - Commande Inconnue
```
suc\n
```
- **Cause** : Commande non reconnue par le serveur

#### `sbp` - Paramètres Incorrects
```
sbp\n
```
- **Cause** : Paramètres de commande invalides

#### `ko` - Échec Général
```
ko\n
```
- **Causes** :
  - Ressource indisponible
  - Action impossible
  - Conditions non remplies

## ⚡ Système de Files d'Attente

### Fonctionnement
- **Capacité** : 10 commandes maximum par joueur
- **Exécution** : Séquentielle selon les délais
- **Rejet** : Si file pleine, commande rejetée avec `ko\n`

### Délais d'Exécution (en unités de temps / fréquence)
- **Mouvement** (Forward, Right, Left) : 7/freq
- **Interaction** (Look, Take, Set, Eject, Broadcast) : 7/freq
- **Information** (Inventory, Connect_nbr, Player_nbr, Player_pos) : 1/freq
- **Spécial** (Fork) : 42/freq
- **Incantation** : 300/freq

### Exemple de Calcul
Avec une fréquence de 100 Hz :
- Forward : 7/100 = 0.07 secondes
- Incantation : 300/100 = 3 secondes

## 🔄 Flux de Communication Typique

### Connexion d'un Client IA
1. Client se connecte au serveur
2. Serveur envoie `WELCOME\n`
3. Client envoie le nom de l'équipe
4. Serveur répond avec le nombre de places disponibles et la taille de la carte
5. Serveur notifie la GUI avec `pnw`

### Action de Mouvement
1. Client IA : `Forward\n`
2. Serveur : Ajoute à la file d'attente
3. Serveur : Exécute après délai
4. Serveur → Client : `ok\n`
5. Serveur → GUI : `ppo #n X Y O\n`

### Incantation Complète
1. Client IA : `Incantation\n`
2. Serveur : Vérifie les conditions
3. Serveur → GUI : `pic X Y L #n...\n`
4. Serveur : Attend 300/freq unités de temps
5. Serveur : Exécute ou échoue
6. Serveur → Participants : `Elevation underway Current level: L\n` ou `ko\n`
7. Serveur → GUI : `pie X Y R\n`

Ce protocole assure une communication robuste et synchronisée entre tous les composants du système Zappy.
