# 📋 Commandes IA du serveur Zappy

## 🚀 Commandes de déplacement

### 1. Forward
- **Format** : `Forward\n`
- **Fonction** : Fait avancer le joueur d'une case dans la direction où il regarde
- **Réponse serveur** : `ok\n` (succès) ou `ko\n` (échec)
- **Notification GUI** : `ppo #n X Y O\n` (nouvelle position du joueur)
- **Particularités** : La carte est toroïdale (wraparound), gère les coordonnées avec modulo

### 2. Right
- **Format** : `Right\n`
- **Fonction** : Fait tourner le joueur de 90° vers la droite
- **Réponse serveur** : `ok\n` (succès) ou `ko\n` (échec)
- **Notification GUI** : `ppo #n X Y O\n` (nouvelle orientation)
- **Orientations** : 1(Nord), 2(Est), 3(Sud), 4(Ouest)

### 3. Left
- **Format** : `Left\n`
- **Fonction** : Fait tourner le joueur de 90° vers la gauche
- **Réponse serveur** : `ok\n` (succès) ou `ko\n` (échec)
- **Notification GUI** : `ppo #n X Y O\n` (nouvelle orientation)

---

## 👁️ Commandes d'observation

### 4. Look
- **Format** : `Look\n`
- **Fonction** : Permet au joueur de voir autour de lui selon son niveau
- **Réponse serveur** : `[case0,case1,case2,...]\n`
- **Portée** : Vision dépendante du niveau du joueur (niveau 1 = 1 case, etc.)
- **Contenu des cases** : Liste des ressources et joueurs présents

### 5. Inventory
- **Format** : `Inventory\n`
- **Fonction** : Affiche le contenu de l'inventaire du joueur
- **Réponse serveur** : `[food: X, linemate: Y, deraumere: Z, sibur: A, mendiane: B, phiras: C, thystame: D]\n`

---

## 📦 Commandes de gestion des ressources

### 6. Take [ressource]
- **Format** : `Take food\n`, `Take linemate\n`, `Take deraumere\n`, etc.
- **Fonction** : Ramasse une ressource spécifique sur la case actuelle
- **Réponse serveur** : `ok\n` (succès) ou `ko\n` (ressource non disponible)
- **Notifications GUI** :
  - `pgt #n i\n` (joueur ramasse la ressource)
  - `bct X Y q0 q1 q2 q3 q4 q5 q6\n` (contenu case mis à jour)
- **Ressources supportées** : food, linemate, deraumere, sibur, mendiane, phiras, thystame

### 7. Set [ressource]
- **Format** : `Set food\n`, `Set linemate\n`, `Set deraumere\n`, etc.
- **Fonction** : Dépose une ressource spécifique de l'inventaire sur la case actuelle
- **Réponse serveur** : `ok\n` (succès) ou `ko\n` (ressource non disponible dans l'inventaire)
- **Notifications GUI** :
  - `pdr #n i\n` (joueur dépose la ressource)
  - `bct X Y q0 q1 q2 q3 q4 q5 q6\n` (contenu case mis à jour)

---

## 📊 Commandes d'information

### 8. Connect_nbr
- **Format** : `Connect_nbr\n`
- **Fonction** : Indique le nombre de slots de connexion disponibles dans l'équipe du joueur
- **Réponse serveur** : `value\n` (où value = max_clients - current_clients)
- **Utilité** : Permet à l'IA de connaître combien de coéquipiers peuvent encore se connecter

### 9. Fork
- **Format** : `Fork\n`
- **Fonction** : Permet au joueur de pondre un œuf à sa position actuelle
- **Réponse serveur** : `ok\n` (succès) ou `ko\n` (pas de slot libre)
- **Délai d'exécution** : 42/f unités de temps
- **Notifications GUI** :
  - `pfk #n\n` (joueur pond un œuf)
  - `enw #e #n X Y\n` (nouvel œuf créé)
- **Utilité** : Crée un nouveau point d'apparition pour l'équipe

---

## ⚡ Système de gestion des commandes

### File d'attente
- Toutes les commandes IA sont ajoutées à une **file d'attente** par joueur
- **Capacité** : 10 commandes maximum en attente
- **Exécution** : Les commandes sont exécutées selon leur délai et la fréquence du serveur

### Délais d'exécution
- Chaque commande a un **délai d'exécution** calculé selon la fréquence du serveur (`-f` paramètre)
- Les commandes ne sont pas instantanées mais respectent le rythme du jeu