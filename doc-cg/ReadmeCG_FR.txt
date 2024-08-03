Veuillez consulter ce site créé par Krtyski. (Administrateur du gadget électronique)
http://egadget2.web.fc2.com/CBasic/Interpreter/CBasic_interpreter.html
Ce site et le texte suivant ont été traduits par Krtyski (administrateur du gadget électronique) & Lephenixnoir (administrateur de Planet-Casio).
Merci beaucoup pour votre aide.

===============================================================================

Complément Casio Basic Interpreter (&Compiler) Version 1.x beta

copyright (c) 2015-2017-2020 de sentaro21
courriel sentaro21@pm.matrix.jp

derničre mise ŕ jour par sentaro21 le 9/2/2020
===============================================================================

** Si vous pensez que vos programmes Casio Basic ne tournent pas assez vite, C.Basic peut vous libérer de cette frustration! **

Commencez avec les programmes Casio Basic authentiques, puis profitez de commandes étendues que Casio Basic seul ne peut pas exécuter.

- Actuellement, **C.Basic** (interpréteur de base) fonctionne sur les modčles fx-9860G (SH3), fx-9860GII (SH3) et fx-9860GII (SH4A), mais la prise en charge de Prizm fx-CG10 et fx-CG20 est planifiée.
- **C.Basic** deviendra un compilateur Casio Basic "**C:Basic**", qui exécutera des programmes beaucoup plus rapidement que C.Basic (10 fois son apparence).

Toutes les instructions prises en charge par C.Basic sont répertoriées dans les fichiers `" Command_List.txt "` et `" Manual_EN.txt "` inclus dans le package de distribution.


## Avantages

- C.Basic est principalement compatible avec les produits Casio Basic authentiques.
- Les programmes sont exécutés au moins 10 fois plus rapidement dans C.Basic.
- Les éditeurs de listes et de fichiers de C.Basic sont plus faciles ŕ utiliser que les habituels.
- Peut exécuter des programmes ŕ partir de la mémoire de stockage / mémoire principale et supporte les structures de sous-dossiers.
- Certaines commandes Casio Basic sont étendues pour davantage de fonctionnalités.
- Commandes nouvellement implémentées pour une programmation plus puissante et plus flexible.

## Les inconvénients

- C.Basic ne couvre pas toutes les commandes de base d'origine.
- Les calculs ne donnent pas les męmes résultats que l'interpréteur habituel car C.Basic utilise une virgule flottante double précision au lieu de BCD ŕ virgule fixe.
- C.Basic stocke les fichiers PICT dans la mémoire de stockage, ce qui les ralentit (du moins jusqu'ŕ la version 0.87, qui utilise la zone de mémoire de la mémoire principale).
- Il doit y avoir des bugs, malheureusement ^^ Merci de nous fournir un rapport de bogue si vous en trouvez.


# # Presque compatible avec la vanille Casio Basic

Notre objectif est une compatibilité sans faille, mais certaines différences sont intentionnelles:

- Les commandes d'entrée `?` et `??` `sont affichées sur une seule ligne. L'affichage d'une chaîne de plus de 21 caractčres entraînera un défilement horizontal au lieu d'un retour ŕ la ligne.
- Quand une commande d'esquisse graphique est suivie d'une commande ŕ instructions multiples `:`, l'écran n'est pas actualisé.
- La durée du programme est affichée ŕ la fin du programme. Cette fonction peut ętre déclenchée dans la page de configuration.
- Les délimiteurs de commentaires `'` sont autorisés avant les retours ŕ la ligne et Disps (sortie), et `:` peuvent ętre inclus dans la chaîne de commentaire.

C.Basic intčgre également les fonctionnalités conviviales de fx-5800P:

- C.Basic supporte une commande `"?A"` comme fx-5800P, par opposition ŕ fx-9860G / fx-9860GII qui ne supporte que `"?->A"`.
- La commande de sortie de chaîne `" "` est entičrement compatible avec le fx-5800P, ce qui en fait un différent du fx-9860G / fx-9860GII. Voir [Sujet] (http://egadget.blog.fc2.com/blog-entry-520.html) pour plus de détails.


======= ATTENTION !!! =========
Nous ne pensons pas que C.Basic pourrait endommager votre calculatrice, mais la mémoire principale pourrait tout de męme ętre endommagée ou détruite de maničre inattendue, ce qui nécessiterait une réinitialisation complčte de la mémoire. Par conséquent, il est fortement recommandé de sauvegarder vos données de mémoire principale et de mémoire de stockage. Sachez que vous ne devez utiliser C.Basic qu'ŕ vos risques et périls.
===============================

======= Décharge de garantie =========
L’utilisation de C.Basic et C: Basic (ci-aprčs SOFTWARE) est ŕ la seule risque de l’utilisateur. Tous les matériaux, informations, produits, logiciels, programmes et services fournis sont fournis «en l’état», sans aucune garantie. Les créateurs de LOGICIEL et d’équipe d’assistance fournissant des documents d’assistance, un site Web d’assistance, des programmes d’information et des exemples de programmes (ci-aprčs «ÉQUIPE DE DÉVELOPPEMENT») déclinent expressément toutes les garanties expresses, implicites, statutaires ou autres, y compris, sans limitation, les garanties de qualité marchande, d'adéquation ŕ un usage particulier et de non-violation des droits de propriété et des droits de propriété intellectuelle. Sans aucune limitation, DEVELOPMENT TEAM ne garantit pas que le développement du LOGICIEL sera ininterrompu, rapide, sécurisé ou sans erreur.
======================================

-------------------------------------------------------------------------------
# Démarrer C.Basic

Pour démarrer ** C.Basic **, appuyez sur `[MENU]`, sélectionnez l'icône C.Basic, puis appuyez sur `[EXE]`.

Il existe deux maničres de copier un fichier de programme (fichier `g1m`) développé pour l'interpréteur Casio Basic d'origine dans la liste des fichiers en C.Basic:

## Utilisation du gestionnaire de mémoire pré-installé

- Pour démarrer le gestionnaire de mémoire, appuyez sur `[MENU]`, sélectionnez l'icône du gestionnaire de mémoire et appuyez sur `[EXE]`.
- Appuyez sur `[F1]` pour afficher le contenu de la mémoire principale.
- Déplacez le curseur vers le bas sur `<PROGRAM>` et appuyez sur `[EXE]`. Vous devriez maintenant voir la liste de tous les programmes Basic Casio actuellement installés pour l'interpréteur habituel.
- Sélectionnez le fichier programme que vous souhaitez copier.
- Appuyez sur `[F1] (SEL)` puis `[F2] (COPY)`.
- Sélectionnez `" ROOT "`, appuyez sur `[EXE]`; la calculatrice vous demandera un nom de fichier.
- Entrez un nom de fichier approprié ou simplement «A», puis appuyez sur «[EXE]».
- Appuyez sur la touche `[MENU]` pour revenir au menu principal.
- Démarrer C.Basic.
- Dans la liste des fichiers, sélectionnez le nom que vous venez d'entrer, appuyez sur `[F6]` puis `[F2] (REN)`.
- On vous demandera `[Renommer le nom du fichier?]` Et le nom du fichier d'origine sera affiché (męme si vous avez tapé le signe "" A "`). Appuyez sur `[EXE]`.
- Maintenant, le nom du fichier a été corrigé.

## Utilisation du logiciel PC Link (FA-124)

FA-124 est un logiciel de liaison PC inclus dans les packages de fx-9860G et fx-9860G II. Pour des instructions détaillées, veuillez vous reporter ŕ son manuel.

Une fois que vous avez téléchargé le fichier source d'un programme (fichier `g1m`), vous pouvez utiliser le logiciel de liaison PC pour le copier dans la liste des fichiers de C.Basic.

=================================================================================
Guide rapide - Environnement de développement de C.Basic
=================================================================================
---------------------------------------------------------------------------------
Page de liste de fichiers
---------------------------------------------------------------------------------
- [UP] Déplace le curseur vers le fichier précédent.
- [DOWN] Déplace le curseur sur le fichier suivant.
En appuyant sur A ŕ Z, le curseur passe ŕ un nom de fichier commençant par l'alphabet enfoncé.
Cette fonctionnalité est compatible avec fx-5800P mais pas avec fx-9860G / fx-9860GII.
En appuyant sur [.] passez ŕ un nom de fichier commençant par  "~".
En appuyant sur [EXP(x10)] la touche pour accéder au haut de la liste des dossiers.

- [F1] (EXE) Exécuter le fichier sélectionné.
- [F2] (EDIT) Édite le fichier sélectionné.
- [F3] (NEW) Créer un nouveau fichier.
- [F4] (COPY) Copie le fichier sélectionné.
- [F5] (DEL) Supprimez le fichier sélectionné.
- [F6] ( > ) Permet de passer au menu suivant.
  [F1] (Texte) Convertit le fichier source sélectionné (fichier g1m) en fichier texte.
  [F2] (REN) Renomme le fichier.
  [F3] (Fav.) Définissez / réinitialisez le fichier sélectionné dans / ŕ partir de "Favori".
  [F4] (Fv.up) Déplace le fichier favori sélectionné vers le haut.
  [F5] (Fv.dw) Déplace le fichier favori sélectionné vers le bas.
  [F6] ( > ) Revient au menu précédent.
  [F1] (MK.F) Créer un nouveau dossier
  [F6] ( > ) Revient au menu précédent.

- [EXE] Exécuter le fichier sélectionné.

- [SHIFT] +
  [EXIT] (QUIT) Revenez ŕ la racine du sous-dossier.
  [GAUCHE] Contraste (ps) (* fonction interne du systčme d'exploitation)
  [RIGHT] Contraste (up) (* fonction interne du systčme d'exploitation)
  [F1] (Var) Aller ŕ la page de révision des variables.
  [F2] (Mat) Aller ŕ la page de révision des variables de mat.
  [F3] (V-W) Aller ŕ la page de révision de la valeur du paramčtre ViewWindow.
  [F4] (Pass) Définir / réinitialiser le mot de passe.
  [F6] (Debg) Démarrer le mode Debug & ouvrir la page de l'éditeur.

- [VARS] (page de révision des variables)
  [F1] (A<>a) Basculer l'affichage des variables majuscules et minuscules.
  [F2] (Init) Initialise toutes les variables affichées.
  [F3] (D<>I) Siwitch variables entičres et doubles.
  [F6] (>Hex/>Dec) Commute l'affichage des valeurs en Hex et Dec.

- [SHIFT] [MENU] (SETUP) Réglages initiaux
  [MENU] Aller ŕ la page de configuration.
  [MENU] [F6] Pop up Information sur la version.

- [OPTN] Définir / Réinitialiser le fichier sélectionné dans / depuis "Favori".

- [EXIT] Déplace le curseur vers le premier fichier de la liste.
         Lorsque le curseur est sur le premier fichier du sous-dossier, revenez ŕ la racine.

-------------------------------------------------------------------------------
Page de l'éditeur
-------------------------------------------------------------------------------
Le repčre de guide affiché ŕ l'extręme droite de la page d'édition indique la position du curseur dans le fichier source.

- [F1] (JUMP) Affiche le sous-menu JUMP.
  [F1] (TOP) Aller ŕ la premičre ligne du fichier source.
  [F2] (BTM) Aller ŕ la derničre ligne du fichier source.
  [F3] (GOTO) Passez ŕ une ligne oů vous voulez aller.
  [F5] (SkipUp) Ignore les lignes (le nombre de lignes prédéfini est disponible dans la page de configuration) vers le haut.
  [F6] (SkipDw) Ignore les lignes (le nombre de lignes prédéfini est disponible dans la page de configuration) vers la fin.

- [F2] (SRC) Aller ŕ la page de recherche / remplacement de chaînes.
- [F3] (CMD) Allez ŕ la page de sélection de commande. (c'est comme la liste des commandes fx-5800P) (mode fx-5800P)
      (MENU)) SelectCommand (mode fx-9860G)
- [F4] (A<>a) Commutez majuscule et minuscule.
- [F5] (CHAR) Aller ŕ la page de sélection de personnage.
- [F6] (EXE) Exécuter un programme ou le redémarrer lorsqu'il est en pause.

- [SHIFT] +
  [EXIT] (QUIT) Retournez ŕ la page de la liste des fichiers.
  [GAUCHE] Contraste (ps) (* fonction interne du systčme d'exploitation)
  [RIGHT] Contraste (up) (* fonction interne du systčme d'exploitation)
  [UP] Faites défiler 1 page (6 lignes) vers le haut.
  [BAS] Faites défiler 1 page (6 lignes) jusqu'ŕ la fin.
  [F1] (Var) Aller ŕ la page Variables.
  [F2] (Mat) Aller ŕ la page Variables Mat.
  [F3] (V-W) Allez ŕ la page ViewWindow.
       (VWIN) Sélectionnez Command. (mode fx-9860G)
     [F5] (Dump) / (List) Bascule entre les affichages "Basic List" et "Hex Dump".
     [F6] (G <> T) Permet de changer d’écran de graphiques et de texte.

  [F4] (SKTCH) Sélectionnez Command. (mode fx-9860G)
  [F4] (Dump) / (List) Affiche les affichages "Basic List" et "Hex Dump" (mode fx-5800P).
  [F5] (Dump) / (List) Bascule entre les affichages "Basic List" et "Hex Dump".
  [F6] (CHAR) Aller ŕ la page de sélection de personnage. (mode fx-9860G)
  [F6] (G <> T) Permet de changer d’écran de texte et graphique (mode fx-5800P).

- [OPTN] Liste de commandes contextuelle prise en charge par [OPTN] de fx-9860G / GII (mode fx-5800P)
         Sélectionnez Commande. (mode fx-9860G)
- [VARS] Liste déroulante de liste déroulante prise en charge par [VARS] de fx-9860G / GII (mode fx-5800P)
         Sélectionnez Commande. (mode fx-9860G)

- [MENU] Historique de saisie de commande (ordre chronologique / ordre de fréquence d'entrée modifié par les touches gauche et droite.)
  [AC/ON] Effacer l'historique

- [SHIFT] [VARS] (PRGM) Liste de commande contextuelle prise en charge par [PRGM] de fx-9860G / GII.
- [SHIFT] [MENU] (SETUP) Allez ŕ la page Setup (mode fx-5800P)
               (SETUP) Sélectionnez Command. (mode fx-9860G)

- [ALPH] [DEL] (UNDO) Supprimer ou coller les opérations sont restaurées dans l'état précédent. (une seule fois)

- [EXIT] Revenir ŕ la page précédente, annuler le mode Clip ou revenir ŕ la page de la liste des fichiers.

-------------------------------------------------------------------------------
Page de l'éditeur de mode de débogage
-------------------------------------------------------------------------------
- [F1] (Cont) Continue le programme ŕ partir de la position du curseur.
- [F2] (Trce) Tracez le programme ŕ partir de la position du curseur et revenez ŕ la page Editeur en mode débogage juste aprčs l'exécution de la commande tracée,.
- [F3] (Step) Trace and Step Over (Commencer et suivre) ŕ partir de la position du curseur et revenir ŕ la page de l'éditeur en mode débogage juste aprčs l'exécution de la commande. Lorsque la sous-routine est exécutée, la sous-routine est exécutée jusqu'ŕ la fin.
- [F4] (S.Out) Sortez de la position du curseur et revenez ŕ la page Editeur en mode Debug juste aprčs l'exécution de la commande. Lorsque le sursor est en sous-routine, la sous-routine est exécutée jusqu'ŕ la fin.
- [F5] (L<>S) Basculez la page de l'éditeur et l'écran du programme en cours. Lorsque l'écran d'exécution du programme est affiché, les opérations de débogage avec [F1] - [F4] sont toujours disponibles.
- [F6] (>) Basculez le menu Debug manu and Editor. Lorsque l'écran d'exécution du programme est affiché, appuyez sur [F6] (>) pour afficher le menu Débogage.
- [EXIT] Revenez ŕ la page précédente, annulez le mode Clip ou revenez ŕ la page d'édition normale (pas en mode débogage).

-------------------------------------------------------------------------------
Écran de fonctionnement du programme
-------------------------------------------------------------------------------
Pendant le programme est en cours d'exécution;

- [AC] Arręte le programme.
   [EXIT] Aller ŕ l'éditeur en mode débogage. Le curseur se situe au point de rupture.
   [Gauche] Allez ŕ l'éditeur en mode débogage. Le curseur se situe au point de rupture.
   [F1] Reprendre le programme.
   [EXE] Reprendre le programme.
Quand le programme est arręté par la commande Disps (- Disp - est affiché ŕ l’écran);

- [SHIFT] +
   [F1] (Var) Aller ŕ la page des variables
   [F2] (Mat) Aller ŕ la page des variables de tapis
   [F3] (V-W) Aller ŕ la page ViewWindow
   [F6] (G <> T) Écran Changer graphisme et texte
- [EXE] Reprendre le programme.

-------------------------------------------------------------------------------
Écran Mat / List Editor
-------------------------------------------------------------------------------
[Écran Mat / List / Vct (la méthode d’opération est commune.)
(Le tapis que vous avez obtenu en adressant la marque * en le listant.)

- [F1] (DEL) Supprimer un tapis / une liste.
- [F2] (DEL-ALL) Supprimer tout Mat / Liste.
- [F3] (DIM) Définissez la dimension, la taille de l'élément, une valeur initiale d'index du Mat / List.
- [F4] (INITIAL) Initialise tous les éléments du Mat / List of selected.
- [F5] (Mat#/Lst#/Vct#) Sélectionnez un numéro et sautez.
- [F6] (A<>a) Commutez majuscule et minuscule.

- [EXE] Entrez dans l'écran d'édition Mat / List. (détaillé plus tard)

- [SHIFT][F1] (Mat)  Sélectionnez Mat
- [SHIFT][F2] (Liste) Sélectionnez Liste
- [SHIFT][F3] (Vct)  Sélectionnez  Vct

- [OPTN] Commutateur s’affichant entre Mat et List.
- [VARS] Commutateur montrant entre Mat et Vct.

- [LEFT] backword 26.
- [RIGHT] mot d’ordre 26.

- [SHIFT]+[UP/DOWN] défilement de page

[Écran d'édition Mat / List]
[SHIFT]+[8] (CLIP) Vous pouvez extraire toutes les données Mat / List dans le tampon de clips.
[SHIFT]+[9] (PASTE) Vous pouvez coller des données Mat / List ŕ partir du tampon de clips.


(mode d'édition de valeur numérique)
 - [F1] (EDIT) Modifie l'élément du Mat / List.
 - [F2] (GO) Aller ŕ l'élément.
 - [F3] (INITIAL) Initialise tous les éléments.
 - [F4] (>x,y / >m,n) Changement d'indication [m,n]/[x,y].

 - [SHIFT]+[F5] ou [F5] Modifier l'affichage décimal <> binaire. (uniquement pour nybble (4 bits), octet (1 octet), le mot (2 octets))
 - [SHIFT]+[F6] ou [F6] Change l’affichage décimal <> hexadécimal. (autre que 1 bit et un nombre complexe (16 octets), le double (8 octets) affiche uniquement les données internes)

 - [SHIFT]+[F2] ou [OPTN] Passer ŕ l'éditeur de points.
 - [SHIFT]+[F3] ou [VARS] Passer ŕ l'affichage de la chaîne de caractčres.

 Seulement comme pour ce qui suit, mode g3m (sauf le bit / liste 1 bit)
 -[DÉCALAGE]
    [5] Saisissez une couleur de 16 bits différente de celle avec une couleur de 4 bits dans le quartet (4 bits).
    [6] Entrez une couleur de 16 bits. (sauf le nibble (4 bits) Mat / List)


(éditeur de points)
 traiter la valeur de chaque élément du tapis comme un code de couleur.
 blanc = 1, noir = 0 avec 1 bit Mat et le mode g1m.
 C'est une couleur de 4 bits dans le nibble (4 bits Mat).

 - [F1] (0<>1) Modifiez la valeur de l'élément avec 0 <> 1. (Mode 1 bit Mat et g1m)
 - [F1] Passez ŕ la couleur affichée par le menu de fonctions. (autre que le bit 1 Mat du mode g3m)
   * Il change en noir comme męme couleur

 Seulement comme pour ce qui suit, mode g3m (sauf le bit / liste 1 bit)
 -[DÉCALAGE]
    [5] Saisissez une couleur de 16 bits différente de celle avec une couleur de 4 bits dans le quartet (4 bits).
    [6] Entrez une couleur de 16 bits. (sauf le nibble (4 bits) Mat / List)

-------------------------------------------------------------------------------
Rechercher / Remplacer du texte
-------------------------------------------------------------------------------
[Recherche chaîne de caractčres]
-Vous entrez une chaîne de caractčres, puis appuyez sur [F1] (SEARCH) ou sur [EXE].
-Vous appuyez ŕ nouveau sur [F1] (SEARCH) ou [EXE] pour rechercher ensuite.
- [F6] (RETRY) Permet de revenir ŕ l'écran de recherche.

[Remplacer la chaîne de caractčres]
-Vous saisissez une chaîne de caractčres avant le remplacement, puis appuyez sur [F2] (REPL).
-Alors vous entrez une chaîne de caractčres aprčs le remplacement et appuyez sur [F1] (SEARCH), [F2] (REPL) ou [EXE].
-Vous appuyez ŕ nouveau sur [F1] (SEARCH) ou [EXE] pour rechercher ensuite.
-Vous appuyez sur [F2] (REPL) pour le remplacer.
-Vous appuyez sur [F3] (ALL) ou [F4] (ALL +) pour tout remplacer. Je pousse [AC] pour l'arręter.
-Vous n’affichez pas la progression en cours de remplacement dans [F4] (ALL+).

-------------------------------------------------------------------------------
Spécification incompatible avec l'original CasioBasic
-------------------------------------------------------------------------------
Notre objectif est la compatibilité avec les produits Casio Basic authentiques, mais pas ŕ 100%.

Une ligne de saisie par la commande "?" et "? ->" ne sont que sur une seule ligne. Une chaîne de caractčres de plus de 21 colonnes est toujours affichée sur la ligne horizontale unique par défilement et non sur plusieurs lignes.
Lorsque ":" (commande ŕ instructions multiples) vient juste aprčs la commande de croquis, l'écran graphique n'est pas redessiné.
La durée du programme est affichée aprčs la fermeture du programme. Cette fonction peut ętre activée ou désactivée dans la page de configuration.
Un séparateur de texte de commentaire 'est disponible pour les commentaires avant Carriage Return ou Disps (sortie), mais ":" (commande ŕ instructions multiples) peut ętre inclus dans la chaîne commentée.

C.Basic intčgre également la fonctionnalité conviviale de fx-5800P;

C.Basic supporte un script "? A" supporté par fx-5800P. Ceci n'est pas supporté par fx-9860G / fx-9860GII (le script "? -> A" seulement est supporté).
La fonction de sortie de chaîne "" est totalement compatible avec fx-5800P dans les détails mais pas exactement compatible avec fx-9860G / fx-986GII. Voir les sujets en détail.

===============================================================================
Menu de Configuration
===============================================================================
Options compatibles avec le Basic Casio :

Angle       :Deg/Rad/Gra
Complex Mode:Real/a+bi/rtheta
Draw Type   :Connect/Plot
Coord       :on/off
Grid        :on/off/Line
Axes        :on/off/Scale
Label       :on/off
Derivative  :on/off
Background  :None / Pict1-20
Plot/LineCol:Black etc.
Sketch Line :Normal/Thick/Broken/Dot/Thin


Nouvelles options fournies par C.Basic :
-------------------------------------------------------------------------------
Display: Fix / Sci / Nrm / Eng
  Le nombre maximal de chiffres possibles est augmenté ŕ 15. Indiquer 0 le pousse ŕ 16.
  Nrm1 et Nrm2 sont identiques au Basic Casio et affichent jusqu'ŕ 10 chiffres.
    Nrm1: -0.01<x<0.01
    Nrm2: -0.000000001<x<0.000000001
    Indiquer un autre paramčtre (0 ou 3..15) modifie le nombre maximum de chiffres.
    Other number (0 or 3 - 15) is to set max number of digit.
  Eng peut ętre configuré ŕ Eng Off, Eng On (/E) et séparateur de milliers (/3)
  Commandes liées : Eng, Norm, Fix, Sci
-------------------------------------------------------------------------------
Exp Display  : E / Stdx10 / Allx10
	sélectionnez le caractčre de l'exposant.
	E:      -1.23e99    caractčre précédent.
	Stdx10: -1.23(x10)99 nouveau caractčre uniquement taille de police standard.
	Allx10: -1.23(x10)99 nouveau caractčre de toute la taille de la police.

- Commandes lieés : '#exp 0	// caractčre précédent.
		    '#exp 1	// nouveau caractčre uniquement taille de police standard.
		    '#exp 2	// new character all font size.
-------------------------------------------------------------------------------
Syntax Help   : On/Off
  Aide ŕ la commande dans l'éditeur
-------------------------------------------------------------------------------
SetupRecover  : On/Off
  Activer cette option restaure automatiquement, aprčs la sortie d'un programme, tous les paramčtres du Basic Casio ŕ leur valeur d'avant l'exécution.
-------------------------------------------------------------------------------
Command Input : C.Basic/Standard/>5800/>9800
  Choisit le mode d'entrée de commandes : Standard (fx-9860G) ou C.Basic (fx-5800P).
  Vous pouvez choisir le fonctionnement de "" : soit fx-5800P, soit fx-CG.
 - Commandes lieés : '#58
                     '#98
-------------------------------------------------------------------------------
[X]character : [0x90]/[X]
  Pour sélectionner le caractčre de la touche [X/Theta/T].
-------------------------------------------------------------------------------
Max Mem Mode : on/off
  Utiliser toute la mémoire disponible.
-------------------------------------------------------------------------------
EnableExFont : on/off
  Activer l'utilisation de la police externe.
-------------------------------------------------------------------------------
Edit ExtFont : On/Off
  Activer l'éditeur de polices externes.
-------------------------------------------------------------------------------
Edit GB Font  : On/Off/Full
  Pour sélectionner la police GB affichée sur l’éditeur.
  Off : normal. pas afficher la police GB.
  On  : affichage de la police de caractčres sans F7xx, F9xx, E5xx, E6xx, E7xx
  Full: affiche toutes les polices GB.
-------------------------------------------------------------------------------
EditFontSize  : Standard/Mini/MiniMini/MiniMiniFX /+Fixed pitch/+Gap
  Sélectionne la taille de police de l'éditeur.
-------------------------------------------------------------------------------
EditFontSize : Standard/Mini/Mini_rev/Mini UnderCursor/Mini_rev_UnderCSR
  Sélectionne la taille de police de l'éditeur.
-------------------------------------------------------------------------------
Edit Indent+  : Off/1/2/4 Save-
activer l'indent automatique dans l'éditeur.
 Off: diable auto indent.
  1 : Définir la largeur de l'endent eden à 1.
  2 : Définir la largeur de l'endente à 1.
  4 : Définir la largeur de l'endent eden à 1.
 Enregistrer : supprimez les espaces vides, y compris les indents lors de l'enregistrement du programme. (à compatible avec Basic Casio)
-------------------------------------------------------------------------------
Edit LineNum :on/off
  Configure l'affichage des numéros de ligne.
-------------------------------------------------------------------------------
EditListChar : List / reverseL / Thick L
  Sélectionne le mode d'affichage des listes.
-------------------------------------------------------------------------------
Edit-backCol  : Black/Blue/Red/Magenta/Green/Cyan/Yellow/White
  Sélectionne le couleur de fond de l'éditeur.
-------------------------------------------------------------------------------
Edit-baseCol  : Black/Blue/Red/Magenta/Green/Cyan/Yellow/White
  Sélectionne le couleur de base de l'éditeur.
-------------------------------------------------------------------------------
E-NumericCol  : Black/Blue/Red/Magenta/Green/Cyan/Yellow/White
  Sélectionne le couleur numérique de l'éditeur.
-------------------------------------------------------------------------------
E-CommandCol  : Black/Blue/Red/Magenta/Green/Cyan/Yellow/White
 select command color of editor.
  Sélectionne le couleur de commande de l'éditeur.
-------------------------------------------------------------------------------
Ed-QuotColor  : Black/Blue/Red/Magenta/Green/Cyan/Yellow/White
  Sélectionne le couleur des chaînes de l'éditeur.
-------------------------------------------------------------------------------
E-CommentCol  : Black/Blue/Red/Magenta/Green/Cyan/Yellow/White
  Sélectionne le couleur des commentaires de l'éditeur.
-------------------------------------------------------------------------------
E-LineNumCol  : Black/Blue/Red/Magenta/Green/Cyan/Yellow/White
 select line number color of editor.
  Sélectionne le couleur du numéro de ligne de l'éditeur.
-------------------------------------------------------------------------------
Auto OverClk  : on/off
  Régler l'overclockage automatique pour CG10/20.
-------------------------------------------------------------------------------
Heap RAM Size : 96KB/117KB/127KB/SIZE
  Définissez la taille de segment maximale de l'utilisation C.Basic.
  (* Vous ne pouvez pas le changer au moment de l'édition du programme.)
-------------------------------------------------------------------------------
Use Hidden RAM : on / off
  Utiliser la mémoire supplémentaire cachée.
-------------------------------------------------------------------------------
HiddenRAM Init : on / off
  Initialiser la mémoire supplémentaire cachée avant l'exécution.
-------------------------------------------------------------------------------
Max Pict No : 20~99
  Lorsque la mémoire cachée est utilisée, le nombre de Picture peut ętre augmenté.
  (*) Changer cette valeur réinitialise les Picture, matrices et listes !
  (*) Ce paramčtre n'est pas valide.
-------------------------------------------------------------------------------
Max List No : 52~1040
  Lorsque la mémoire cachée est utilisée, le nombre de listes peut ętre augmenté.
  (*) Changer cette valeur réinitialise les Picture, matrices et listes !
-------------------------------------------------------------------------------
AT DebugMode : on / off
  Active le debugger lorsque [AC/ON] est utilisé pour interrompre le programme.
-------------------------------------------------------------------------------
ExitDM Popup : on / off
  Afficher une popup quand le debugger se ferme.
-------------------------------------------------------------------------------
Break Stop : on / off
  Accepte ou refuse les interruptions par la touche [AC/ON].
  Quand cette option est ŕ off, vous ne pouvez pas quitter un programme en appuyant sur [AC/ON].
  - Commandes liées : '#Break 0    '#Break 1
-------------------------------------------------------------------------------
Exec TimeDsp : on / off  /reset  /%HR
  Affiche le temps d'exécution du programme une fois le programme terminé.
  Le timer est lancé ŕ la premičre ligne du programme. Les commandes ? et Disps le réinitialisent.
  Les fonctions Getkey1/2 mettent le timer en pause et le redémarrent ensuite.
  En cas de reset, le timer est réinitialisé est reprend au premier appel de Getkey1/2.
  %HR: Utilise le timer ŕ 1/32768s (SH4A uniquement)
  - Commandes liées : '#GetKeyC  '#GetKeyR
-------------------------------------------------------------------------------
IfEnd Check : on / off
  Vérifie strictement la correspondance entre chaque "If" et chaque "IfEnd".
-------------------------------------------------------------------------------
ACBreak : on / off
  Active ou désactive la commande ACBreak.
  - Commandes liées : ACBreak
-------------------------------------------------------------------------------
Force Return : None/ F1/ EXE/ F1&EXE
  Force le retour ŕ l'éditeur lors d'une interruption avec [AC/ON].
  (Exemple) Retour de force réglé à [F1]key
  -Liste de fichiers→[F1](EXE)→[AC]→[EXIT]... Retourne à la liste de fichiers.
  -Liste de fichiers→[EXE][AC]→[EXIT] ...... Retourne à l'éditeur.
  -Liste de fichiers→[F1](EXE]→[AC]→[EXE](restart)→[AC]→[EXIT] ...... Retourne à la liste de fichiers.
  (L'opération dépend de la clé qui est pressée en premier.)
-------------------------------------------------------------------------------
Key 1st time : 25 ms - 1000 ms (valeur par défaut : 500 ms)
  Durée avant qu'une touche soit répétée pour la premičre fois (multiple de 25 ms)
-------------------------------------------------------------------------------
Key Rep time : 25 ms - 500 ms (valeur par défaut : 125 ms)
  Durée avant qu'une touche qui a déjŕ été répétée soit répétée de nouveau (multiple de 25 ms)
-------------------------------------------------------------------------------
SkipUp/Down : 1 - 9999
  Nombre d'écrans parcourus pour chaque pression de SkipUp ([SHIFT][UP]) ou SkipDown ([SHIFT][DOWN]) dans l'éditeur.
-------------------------------------------------------------------------------
Mat Dsp mode : [m,n] / [X,Y]
  Sélectionne le format d'affichage de matrices dans l'éditeur de matrices.
  Les formats [m,n] et [X,Y] sont transposés l'un de l'autre. Ce n'est qu'un paramčtre d'affichage, les données de la matrice restent stockées dans l'ordre original.
-------------------------------------------------------------------------------
Matrix base : 0 / 1
  Indice de début pour la numérotation des éléments de matrices. Il faut ętre en mode 0 pour utiliser la premičre ligne et la premičre colonne de la VRAM.
  - Commandes liées : '#Mat 0, '#Mat 1
-------------------------------------------------------------------------------
DATE : 2017/01/17 TUE
  Change la date de la RTC (horloge du systčme). Le jour de la semaine est calculé automatiquement.
-------------------------------------------------------------------------------
TIME : 23:59:59
  Change l'heure de la RTC.
  - Commandes liées : DATE, TIME
-------------------------------------------------------------------------------
Root folder:/			(Seul le Graph 35+E II est valide.)
  [F1]: pour définir le dossier racine.
  [F2]: pour définir le dossier actuel.
  - Commandes liées :
   '#R/ 	// pour définir le dossier racine.
   '#R.		// pour définir le dossier actuel.
-------------------------------------------------------------------------------
Favorite Col : Black/Blue/Red/Magenta/Green/Cyan/Yellow/White/16bit Color
  Sélectionne le couleur de fichier préférée de l'éditeur.
-------------------------------------------------------------------------------
Auto file Save : on/off
  Désactive la popup de confirmation ŕ la sauvegarde dans l'éditeur.
-------------------------------------------------------------------------------
Force g1m save : on / off
  Permet de sauvegarder automatiquement les fichiers g1m quand les programmes textes sont édités.
-------------------------------------------------------------------------------
Direct GB cnvt : on/off
  Définir le code GB sans conversion de texte.
  (* conversion supporte le mode GB de l'éditeur.)
-------------------------------------------------------------------------------
Pict mode: S.Mem / Heap / Both
  - S.Mem : Les Picture sont créées dans la mémoire de stockage (petit surcoűt temporel)
  - Heap  : Les Picture ne sont pas vraiment créées, juste stockée temporairement dans la RAM
  - Both  : Les deux.
  - Clear : Supprime le fichier compatible Pict créé dans la mémoire de tas au démarrage du programme ŕ chaque fois.
-------------------------------------------------------------------------------
Storage mode: S.Mem / SD / MainMem
  Sélectionne le stockage temporaire pour le code pendant l'édition et l'exécution.
  S.Mem    : Mémoire de stockage
  SD       : Carte SD (Graph 95 SD uniquement)
  Main mem : Mémoire principale
-------------------------------------------------------------------------------
RefrshCtl DD: off / Grp / All
  Sélectionne le niveau de restriction du rafraîchissement automatique de l'écran.
  Off  : Aucune restriction particuličre (comme en Basic Casio).
  Grp  : Seules les commandes graphiques et ClrText, Locate, Text, LocateYX et "" provoquent le rafraîchissement de l'écran (ces exceptions sont en rétro-compatibilité avec les précédentes versions de C.Basic).
  All  : Aucune commande ne rafraîchit l'écran automatiquement. PutDispDD doit ętre utilisé.
  Time : Sélectionne la durée de rafraîchissement automatiquement en unités de 1/128 s. La valeur par défaut est 3, ce qui donne environ 1/42 s de fréquence de rafraîchissement.
  Par défaut, la valeur est 0.
  - Commandes liées : RefrshCtrl, RefrshTime
-------------------------------------------------------------------------------
Wait count : 0~9999
  Délai contrôlant la vitesse d'exécution globale.
  - Commandes liées : Wait
-------------------------------------------------------------------------------
G1M/G3M mode : auto/g3m/g1m
 Définir pour exécuter le mode par défaut.
 - Commandes liées : '#G1M, '#g1m, '#G3M, '#g3m
-------------------------------------------------------------------------------
Execute mode: Dbl# / Int% / CPLX
  Définit le mode de variables numériques par défaut.
  - Commandes liées: '#CBasic, '#CBASIC, '#CBINT, '#CBint '#CBCPLX '#CBcplx
-------------------------------------------------------------------------------

===============================================================================
Reconnaissance
===============================================================================

Pour étudier et mettre en śuvre la spécification interne,
"Calculatrices fx basées sur SuperH (version 20)"
a donné des informations utiles.

Pour la gestion des fichiers dans C.Basic,
"https://community.casiocalc.org/topic/6836-wsc-fvm-v12-casio-fx-9860-series-on-calc-c-compiler/
Wsc & Fvm V1.2 - Compilateur C sur calcul série Casio Fx-9860 "
a donné des informations utiles.

Pour les polices Kana japonaises, les données de police et la routine de sortie du site suivant sont utilisées;
"http://www.d-b.ne.jp/~k-kita/fx_sdk_005.html"


Pour les fonctionnalités détaillées de Casio Basic d’origine, le site suivant est référencé;
Gadget électronique - Calculatrice de programmation de Krtyski
"https://egadget.blog.fc2.com/"
(presque le contenu est en japonais).

https://community.casiocalc.org/topic/7637-cbasic-international-release/
https://codewalr.us/index.php?topic=1818.0
https://www.planet-casio.com/Fr/forums/topic14738-1-C.Basic.html
Leurs avis ont été trčs utiles.

Je suis reconnaissant pour la coopération de tous.

par sentaro21

===============================================================================
lien connexe
===============================================================================
https://egadget2.web.fc2.com/CBasic/Interpreter/CBasic_interpreter.html
https://egadget.blog.fc2.com/blog-entry-630.html (part1)
https://egadget.blog.fc2.com/blog-entry-658.html (part2)
https://egadget.blog.fc2.com/blog-entry-664.html (part3)

http://community.casiocalc.org/topic/7637-cbasic-international-release/
https://codewalr.us/index.php?topic=1818.0
https://www.planet-casio.com/Fr/forums/topic14738-1-C.Basic.html
https://www.planet-casio.com/Fr/forums/lecture_sujet.php?id=15240

===============================================================================
Licence
===============================================================================
Ce logiciel est un logiciel libre, conforme ŕ GPLv2.