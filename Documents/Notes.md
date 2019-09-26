# Idées à implémenter/faire/essayer...
- Fichier à accès aléatoire
- Recherche en triant les tail et en faisant une recherche dicotomique


## Méthode de tri

Il n'est pas possible de trier directement tous les mot de passes "tail".  Ils ne rentrent pas tous dans la mémoire (max 12Go).   
**Solution:**    
On garde en mémoire un nombre raisonnable de résultats (disons 1Go). Ces données peuvent être trié facilement. Ils sont ensuite stocké dans un fichier.
Une fois le résultat sauvegardé, on supprimé les données mémoire et on recommence.   
Dans un second temps, on va ouvrir tous les fichiers (si 1Go par fichier, max 12 fichiers) et lire dans l'ordre les éléments. On fera une comparaison des 12 éléments pour préndre le premier et le stocker dans le fichier final.


Exemple concrêt avec 3 fichiers:

```
FICHIER_1
aabcd   <-
abcdm

FICHIER_2
bacdd   <-
duxva

FICHIER_3
admbx   <-
cuamd

FINAL

```

--------------

```
FICHIER_1
aabcd
abcdm   <-

FICHIER_2
bacdd   <-
duxva

FICHIER_3
admbx   <-
cuamd

FINAL
aabcd
```

--------------

```
FICHIER_1
aabcd
abcdm
<-

FICHIER_2
bacdd   <-
duxva

FICHIER_3
admbx   <-
cuamd

FINAL
aabcd
abcdm
```

--------------

```
FICHIER_1
aabcd
abcdm
<-

FICHIER_2
bacdd   <-
duxva

FICHIER_3
admbx
cuamd   <-

FINAL
aabcd
abcdm
admbx
```

--------------

```
FICHIER_1
aabcd
abcdm
<-

FICHIER_2
bacdd
duxva   <-

FICHIER_3
admbx
cuamd   <-

FINAL
aabcd
abcdm
admbx
bacdd
```

--------------

```
FICHIER_1
aabcd
abcdm
<-

FICHIER_2
bacdd
duxva   <-

FICHIER_3
admbx
cuamd
<-

FINAL
aabcd
abcdm
admbx
bacdd
cuamd
```

--------------

```
FICHIER_1
aabcd
abcdm
<-

FICHIER_2
bacdd
duxva
<-

FICHIER_3
admbx
cuamd
<-

FINAL
aabcd
abcdm
admbx
bacdd
cuamd
duxva
```
