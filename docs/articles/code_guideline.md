# Guide des Bonnes Pratiques de Programmation

Ce guide est une copie du guide provenant de [Exo_Control](https://github.com/robotique-udes/Exo_Control) écrit par Gabriel Desrochers.

Ce guide rassemble les conventions et les règles d'écriture de notre projet. Son but est de rendre notre code propre, facile à lire et simple à comprendre pour tout le monde.

---

## 1. Conventions d'Écriture 

### Le Style

Pour éviter que chacun écrive à sa manière, nous utilisons trois styles précis :
*   **camelCase** : La première lettre est en minuscule, et chaque nouveau mot commence par une majuscule (ex: `rightMotor`).
*   **PascalCase** : Chaque mot commence par une majuscule, même le premier (ex: `RightMotor`).
*   **UPPER_SNAKE_CASE** : Tout est en majuscules, et les mots sont séparés par un tiret bas `_` (ex: `MAX_SPEED`).

Voici ce que vous devez appliquer pour chaque élément :

| Élément de code | Style à utiliser | Exemple |
| :--- | :--- | :--- |
| **Variables** | camelCase | `motorID`, `currentTemperature` |
| **Fonctions** | camelCase | `getTorque()`, `printData()` |
| **Classes & Structures** | PascalCase | `Motor`, `Logic` |
| **Noms de fichiers** | PascalCase | `Logic.hpp`, `Motor.cpp` |
| **Enums (Types)** | PascalCase | `MotorStatus`, `BNOValues` |
| **Énumérateurs (Valeurs)**| UPPER_SNAKE_CASE | `HIP_LEFT`, `KNEE_LEFT` |
| **Constantes & #define** | UPPER_SNAKE_CASE | `MAX_TEMPERATURE`, `PI` |

### Les Préfixes

Les préfixes sont de petites lettres ajoutées au tout début du nom d'une variable. Ils sont très utiles, car ils permettent de comprendre instantanément d'où vient une variable (son origine et sa portée) sans avoir à chercher sa définition dans le code.

Voici les trois règles simples que nous appliquons pour les variables :

| Préfixe | Signification | Quand l'utiliser ? | Exemple |
| :--- | :--- | :--- | :--- |
| **Aucun** | Variable locale | Variable temporaire créée à l'intérieur d'une fonction ou d'un bloc de code. | `bool overheating = isOverheating();` |
| **`m_`** | Membre de classe | Variable globale à une classe (Attribut). Le `m` signifie *Member variable*. | `float m_temperature;` (dans la classe Motor) |
| **`p_`** | Paramètre | Variable reçue comme argument à l'entrée d'une fonction. Le `p` signifie *Parameter*. | `bool isOverheating(float p_temp);` |

#### Exemple concret d'utilisation :

```cpp
// Le p_ montre que les valeurs viennent de l'extérieur de la fonction
bool Motor::isOverheating(float p_temp)
{
    // Le m_ montre que nous modifions les variables internes de notre classe
    m_temp = p_temp;

    // Aucun préfixe ici car "overheating" est une variable locale qui disparait à la fin de la fonction
    bool overheating = m_temp > MAX_TEMP;
    return overheating;
}
```


---

## 2. Les Variables

Une variable sert à stocker une information. Son nom doit être **descriptif** (on doit deviner ce qu'elle contient) et représenter un **objet** ou un **concept** (pas une action).

*   **Mauvais** : `int i = 0;` ou `int faireCalcul = 5;` (trop vague, ou utilise un verbe d'action)
*   **Bon** : `int motorID = 0;` ou `int speedLimit = 25;`

---

## 3. Les Fonctions

Une fonction exécute une tâche. Son nom doit **toujours commencer par un verbe** au présent ou à l'infinitif en anglais pour indiquer l'action qu'elle accomplit.

**Mauvais (Pas de verbe) :**
```cpp
// On ne sait pas si la fonction calcule, affiche ou modifie la torque
float torque(float angle)
{
    // ...
}
```

**Bon (Action claire) :**
```cpp
// Action : Récupérer la valeur
float getTorque(float angle)
{
    // ...
}

// Action : Faire le calcul
float calculateTorque(float angle)
{
    // ...
}
```

---

## 4. Les Commentaires

Les commentaires explicatifs doivent être **très rares** dans vos fichiers. Si vous ressentez le besoin d'expliquer votre code avec du texte, c'est souvent parce que votre code est mal nommé. Le code doit être **"self-documenting"** (auto-explicatif).

**Mauvais (Code mystérieux qui demande des explications) :**
```cpp
// Si la température du moteur est plus haute que 80 degrés Celsius
if (canData[2] > 80)
{
    // Fermer le moteur du genou gauche
    motors[0].setTorque(0.0);
}
```

**Bon (Le code se lit comme une phrase, aucun commentaire requis) :**
```cpp
float temperature = motors[LEFT_KNEE].getTemperature();

if (temperature > MAX_TEMPERATURE)
{
    motors[LEFT_KNEE].setTorque(0.0);
}
```

---

## 5. La Documentation (Doxygen)

Attention à ne pas confondre *commenter* et *documenter* ! 
*   On n'écrit aucun commentaire inutile dans les fichiers de code (`.cpp`).
*   En revanche, **on documente obligatoirement toutes les entêtes** dans les fichiers de configuration (`.hpp` et `.h`).

Pour cela, on utilise le format **Doxygen**. Un bloc Doxygen commence toujours par `/**` et se place **juste au-dessus** de l'élément.

### Exemple Standard de Documentation

```cpp
/**
 * @brief Vérifie si la température actuelle du moteur dépasse le seuil critique.
 * 
 * @details Compare la valeur lue par le capteur avec la limite maximale autorisée. 
 *          Cette fonction est utilisée pour déclencher les procédures de sécurité 
 *          en cas de surchauffe.
 * 
 * @param[in] p_currentTemperature La température actuelle mesurée (en degrés Celsius).
 * @param[in] p_maxThreshold Le seuil critique à ne pas dépasser (en degrés Celsius).
 * 
 * @retval true Si le moteur est en surchauffe et nécessite un arrêt d'urgence.
 * @retval false Si la température est sécuritaire et sous le seuil critique.
 */
bool isOverheating(float p_currentTemperature, float p_maxThreshold);

```

### Aide-mémoire des Balises Doxygen

| Balise | Où l'utiliser ? | À quoi ça sert ? |
| :--- | :--- | :--- |
| **@file** | Tout en haut d'un fichier | Identifie le fichier (Obligatoire dans chaque fichier `.h`/`.hpp`). |
| **@brief** | Partout | Une description courte et simple en une seule phrase. |
| **@details** | Partout | (Optionnel) Explications plus poussées, détails techniques. |
| **@param[in]** | Fonctions | Décrit une variable reçue en entrée par la fonction. |
| **@param[out]**| Fonctions | Décrit une variable modifiée par la fonction pour renvoyer un résultat. |
| **@return** | Fonctions | Décrit ce que la fonction renvoie à la fin (ex: un nombre, un texte). |
| **@retval** | Fonctions | Décrit le résultat pour une valeur précise (ex: `@retval true si succès`). |
| **@code** | Partout | Permet d'intégrer un exemple de code dans votre texte. |
| **@endcode** | Partout | Ferme la zone d'exemple de code. |

---

## 6. Structure des Fichiers d'Entête (.hpp / .h)

Dans nos fichiers d'entête, les classes doivent toujours suivre une structure stricte et organisée du haut vers le bas.

### Règle d'or de l'Encapsulation
**Aucun attribut (variable de classe) ne doit être public !** Si quelqu'un à l'extérieur de la classe a besoin de lire ou modifier une variable, créez des fonctions d'accès appelées **Getter** (pour lire) et **Setter** (pour modifier). Cela protège notre code contre les modifications accidentelles.

### Structure à respecter :
```text
MaClasse
  ├── 1. Fonctions et Méthodes PUBLIQUES (Accessibles par tout le monde)
  ├── 2. Fonctions et Méthodes PRIVÉES (Utilisées seulement à l'intérieur)
  └── 3. Attributs/Variables PRIVÉS (Les données de la classe)
```

### Exemple de classe :

```cpp
/**
 * @file Motor.hpp
 * @brief Définition de la classe Motor pour la gestion des couples des membres de l'exo.
 */

class Motor
{
public:
    // --- 1. MÉTHODES PUBLIQUES ---
    
    /**
     * @brief Récupère la température actuelle du moteur.
     * @return La température mesurée en degrés Celsius.
     */
    float getTemp();

    /**
     * @brief Calcule le couple (torque) instantané du moteur.
     * @return Le couple estimé ou calculé en Newton-mètres (Nm).
     */
    float calculateTorque();

private:
    // --- 2. MÉTHODES PRIVÉES ---
    
    /**
     * @brief Affiche les données brutes du bus CAN dans la console pour le débogage.
     */
    void printCanValues();

    // --- 3. ATTRIBUTS PRIVÉS ---
    float m_temp;   ///>la température du moteur [Celsius]
    float m_currentTorque; ///> la torque que le moteur output [Nm]
};
```



## 7. Garder le Code DRY (Don't Repeat Yourself)

Le principe **DRY**  est une règle d'or en programmation. Si vous écrivez deux fois ou plus la même logique de code, il y a très souvent une meilleure façon de faire. 

Répéter du code rend les fichiers inutilement longs et difficiles à corriger. 

### Exemple : Application d'un couple (torque) sur 4 moteurs



**Mauvais : Copier-coller le même code (Non-DRY)**
Si nous voulons modifier la formule mathématique plus tard, nous devrons modifier les 4 blocs de code un par un. C'est une source fréquente d'erreurs d'inattention !

```cpp
// Application de la force sur le moteur Avant-Gauche
float torqueFL = p_torque[FRONT_LEFT] * torque_multiplier;
if (torqueFL > MAX_TORQUE) 
    torqueFL = MAX_TORQUE; 
m_motors[FRONT_LEFT].setTorque(torqueFL);

// Application de la force sur le moteur Avant-Droit
float torqueFR = p_torque[FRONT_RIGHT] * torque_multiplier;
if (torqueFR > MAX_TORQUE) 
    torqueFR = MAX_TORQUE; 
m_motors[FRONT_RIGHT].setTorque(torqueFR);

// Application de la force sur le moteur Arrière-Gauche
float torqueBL = p_torque[BACK_LEFT] * torque_multiplier;
if (torqueBL > MAX_TORQUE) 
    torqueBL = MAX_TORQUE;
m_motors[BACK_LEFT].setTorque(torqueBL);

// Application de la force sur le moteur Arrière-Droit
float torqueBR = p_torque[BACK_RIGHT] * torque_multiplier;
if (torqueBR > MAX_TORQUE)
    torqueBR = MAX_TORQUE; 
m_motors[BACK_RIGHT].setTorque(torqueBR);
```

• **Bon : Utiliser une boucle `for` (Respect de la règle DRY)**
Puisque l'action est identique pour chaque moteur, nous regroupons les identifiants de nos moteurs dans un tableau et appliquons la logique une seule fois à l'aide d'une boucle. 

```cpp

// La boucle applique la logique à chaque moteur de la liste de manière automatique
for (int motorID = 0; i < NB_MOTOR; i++)
{
    float calculatedTorque = p_torque[motorID] * torque_multiplier;
    if (calculatedTorque > MAX_TORQUE)
    {
        calculatedTorque = MAX_TORQUE;
    }
    m_motors[activeMotors[motorID]].setTorque(calculatedTorque);
}
```

### Pourquoi c'est beaucoup mieux ?
1. **Lisibilité** : Le fichier passe de 20 lignes confuses à moins de 10 lignes claires.
2. **Maintenance** : Si la formule de calcul ou la limite change, il n'y a **qu'une seule ligne** à modifier dans tout le projet.
