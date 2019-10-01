# Idées à implémenter/faire/essayer...
- Fichier à accès aléatoire
- Recherche en triant les tail et en faisant une recherche dicotomique
- Vérifier que la "reverse" fonction est bien répartie (actuellement cela ne semble pas le cas)

# TODO
- Boucle pour hash et reverse plusieurs fois
- Mettre en place le sytème de tri
- Passer d'un string à un binaire de manière plus efficace


## Méthode de tri

Il n'est pas possible de trier directement tous les mot de passes "tail".  Ils ne rentrent pas tous dans la mémoire (max 12Go).   
**Solution:**    
On garde en mémoire un nombre raisonnable de résultats (disons 1Go). Ces données peuvent être trié facilement. Ils sont ensuite stocké dans un fichier.
Une fois le résultat sauvegardé, on supprime les données présentes en mémoire et on recommence.   
Dans un second temps, on va ouvrir tous les fichiers (si 1Go par fichier, max 12 fichiers) et lire dans l'ordre les éléments. On fera une comparaison des 12 éléments pour prendre le premier et le stocker dans le fichier final.


Exemple concrêt avec 3 fichiers:

```
    FICHIER_1           FICHIER_2           FICHIER_3
    aabcd   <-          bacdd   <-          admbx   <-
    abcdm               duxva               cuamd   
                                                    
                                                    
                        FINAL                       
                                                    
```

--------------

```
    FICHIER_1           FICHIER_2           FICHIER_3
    aabcd               bacdd   <-          admbx   <-
    abcdm   <-          duxva               cuamd   
                                                    
                                                    
                        FINAL                       
                        aabcd                       
```

--------------

```
    FICHIER_1           FICHIER_2           FICHIER_3
    aabcd               bacdd   <-          admbx   <-
    abcdm               duxva               cuamd   
    <-                                              
                                                    
                        FINAL                       
                        aabcd                       
                        abcdm                       
```

--------------

```
    FICHIER_1           FICHIER_2           FICHIER_3
    aabcd               bacdd   <-          admbx   
    abcdm               duxva               cuamd   <-
    <-                                              
                                                    
                        FINAL                       
                        aabcd                       
                        abcdm                       
                        admbx                       
```

--------------

```
    FICHIER_1           FICHIER_2           FICHIER_3
    aabcd               bacdd               admbx   
    abcdm               duxva   <-          cuamd   <-
    <-                                              
                                                    
                        FINAL                       
                        aabcd                       
                        abcdm                       
                        admbx                       
                        bacdd                       
```

--------------

```
    FICHIER_1           FICHIER_2           FICHIER_3
    aabcd               bacdd               admbx   
    abcdm               duxva   <-          cuamd   
    <-                                      <-      
                                                    
                        FINAL                       
                        aabcd                       
                        abcdm                       
                        admbx                       
                        bacdd                       
                        cuamd                       
```

--------------

```
    FICHIER_1           FICHIER_2           FICHIER_3
    aabcd               bacdd               admbx   
    abcdm               duxva               cuamd   
    <-                  <-                  <-      
                                                    
                        FINAL                       
                        aabcd                       
                        abcdm                       
                        admbx                       
                        bacdd                       
                        cuamd                       
                        duxva                       
```
