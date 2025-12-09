# 🎮 Sudoku Master 🧩

![Language](https://img.shields.io/badge/language-C-blue?style=for-the-badge&logo=c)
![GUI](https://img.shields.io/badge/GUI-GTK%2B3-green?style=for-the-badge&logo=gtk)
![School](https://img.shields.io/badge/School-EILCO-red?style=for-the-badge)

> Projet réalisé dans le cadre de la formation **ING1 Informatique** à l'**EILCO Calais**.

## 📝 Description
Ce projet est une implémentation complète du jeu de **Sudoku** en langage C, dotée d'une interface graphique intuitive utilisant la bibliothèque **GTK**.

✨ **Fonctionnalités principales :**
*   🔐 Authentification des joueurs
*   💾 Sauvegarde et chargement de partie
*   ✅ Validation automatique de la grille
*   🎨 Interface graphique conviviale

## 👥 Auteurs
Ce projet a été réalisé par :
*   👤 **EL MAHDI ALOUI**
*   👤 **Akram Ouadghiri Bencherif**
*   👤 **Hajar Brech**

## 🙏 Remerciements
Nous tenons à exprimer notre gratitude envers l'ensemble de l'**équipe pédagogique** et nos **enseignants** pour leur encadrement, leur soutien et les connaissances transmises tout au long de ce projet.

---

## 🛠️ Installation & Compilation

### 🚀 Option 1: CLion (Recommandé)
1.  Ouvrez **CLion**.
2.  Sélectionnez **Open** et choisissez le dossier `sudoku_game`.
3.  CLion détectera automatiquement le `CMakeLists.txt`.
4.  Cliquez sur le bouton **Run** (▶️).

### ⚙️ Option 2: Code::Blocks
1.  Ouvrez **Code::Blocks**.
2.  Allez dans **File > New > Project**.
3.  Sélectionnez **Empty Project**.
4.  Nommez le projet et sauvegardez-le dans le dossier `sudoku_game`.
5.  Ajoutez tous les fichiers `.c` et `.h` du dossier `src`.
6.  **Build options** > **Linker settings** > Ajoutez : `` `pkg-config --libs gtk+-3.0` ``
7.  **Compiler settings** > **Other options** > Ajoutez : `` `pkg-config --cflags gtk+-3.0` ``
8.  Compilez et lancez !

### 💻 Option 3: Ligne de commande
Vous pouvez compiler manuellement avec `gcc` :
```bash
gcc -o sudoku_master.exe src/main.c src/gui.c src/auth.c src/storage.c src/sudoku.c src/sha256.c $(pkg-config --cflags --libs gtk+-3.0)
```
